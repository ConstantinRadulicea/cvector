#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "PPP_custom.h"


// Polynomial: 0x1021, Init: 0xFFFF, No reflection, No final XOR
uint16_t crc16_ccitt_continue(uint16_t crc, const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
        }
    }
    return crc;
}

// Computes CRC-16-CCITT (poly 0x1021, init 0xFFFF)
uint16_t ppp_crc16_ccitt_start(const uint8_t* data, size_t len) {
    uint16_t crc = 0xFFFF;
    return crc16_ccitt_continue(crc, data, len);
}

// Escapes a byte and writes to output buffer
void ppp_write_escaped(uint8_t byte, uint8_t* out_buf, size_t* out_len) {
    if (byte == PPP_FLAG || byte == PPP_ESCAPE) {
        out_buf[(*out_len)++] = PPP_ESCAPE;
        out_buf[(*out_len)++] = byte ^ PPP_ESC_MASK;
    }
    else {
        out_buf[(*out_len)++] = byte;
    }
}

// Removes escaping from a PPP frame body (excluding flag bytes)
// Returns length of unescaped data, or 0 on error (e.g., dangling 0x7D)
size_t ppp_unescape(const uint8_t* in_buf, size_t in_len, uint8_t* out_buf, size_t max_out_len) {
    size_t out_len = 0;
    int escaping = 0;

    for (size_t i = 0; i < in_len; i++) {
        uint8_t byte = in_buf[i];

        if (escaping) {
            byte ^= PPP_ESC_MASK;
            escaping = 0;
        }
        else if (byte == PPP_ESCAPE) {
            escaping = 1;
            continue;
        }

        if (out_len >= max_out_len) return 0;

        out_buf[out_len] = byte;
        out_len++;
    }

    if (escaping) return 0;

    return out_len;
}



// Encodes a PPP frame
size_t ppp_custom_encode(const uint8_t* payload, size_t payload_len, uint8_t* out_buf, size_t out_buf_len, uint8_t sequence_counter, uint8_t control) {
    size_t out_len = 0;
    uint8_t header[HEADER_SIZE];
    header[0] = sequence_counter;
    header[1] = control;

    uint16_t fcs = ppp_crc16_ccitt_start(header, sizeof(header));
    fcs = crc16_ccitt_continue(fcs, payload, payload_len);

    out_buf[out_len] = PPP_FLAG;
    out_len++;

    for (int i = 0; i < sizeof(header); i++) {
        if (out_buf_len < out_len + 2) {
            return 0;
        }
        ppp_write_escaped(header[i], out_buf, &out_len);
    }
    for (size_t i = 0; i < payload_len; i++) {
        if (out_buf_len < out_len + 2) {
            return 0;
        }
        ppp_write_escaped(payload[i], out_buf, &out_len);
    }
    if (out_buf_len < out_len + 2) {
        return 0;
    }
    ppp_write_escaped((fcs >> 8) & 0xFF, out_buf, &out_len);
    if (out_buf_len < out_len + 2) {
        return 0;
    }
    ppp_write_escaped(fcs & 0xFF, out_buf, &out_len);
    if (out_buf_len < out_len + 1) {
        return 0;
    }
    out_buf[out_len] = PPP_FLAG;
    out_len++;
    return out_len;
}

// Decodes a PPP frame (assumes full frame with flags is given)
int8_t ppp_custom_decode(uint8_t *frame, size_t frame_len, uint8_t *decoded_buffer, ppp_custom_decoded_message_t* message) {
    if (frame_len < PROTOCOL_OVERHEAD_BYTE_SIZE || frame[0] != PPP_FLAG || frame[frame_len - 1] != PPP_FLAG)
        return -1;
    message->buffer = decoded_buffer;
    uint8_t *temp = &decoded_buffer[1];
    decoded_buffer[0] = frame[0]; // Placeholder for sequence number

    size_t temp_len = ppp_unescape(&frame[1], frame_len - START_FLAG_SIZE - 1, temp, frame_len-1); // remove 0x7E

    if (temp_len < (PROTOCOL_OVERHEAD_BYTE_SIZE - (2 * START_FLAG_SIZE))) return PPP_ERR_MESSAGE_TOO_SMALL;
    
    uint16_t recv_crc = ((uint16_t)temp[temp_len - 2] << 8) | temp[temp_len-1];
    uint16_t calc_crc = ppp_crc16_ccitt_start(temp, temp_len - CRC_BYTE_SIZE);
    if (recv_crc != calc_crc) return PPP_ERR_INVALID_CRC;

    message->sequence_counter = &temp[0];
    message->control = &temp[1];
    message->payload = &temp[HEADER_SIZE];
    message->buffer_size = temp_len + (START_FLAG_SIZE * 2);
    message->payload_size = temp_len - HEADER_SIZE - CRC_BYTE_SIZE;
    message->CRC = calc_crc;

    return PPP_SUCCESS;
}


int8_t ppp_custom_rx_append_byte(ppp_custom_rx_message_t* rx_msg, uint8_t byte) {

    switch (rx_msg->rx_state) {
    case PPP_RX_MSG_WAIT_START_FLAG:
        if (byte == PPP_FLAG) {
            rx_msg->buffer_size = 0;
            rx_msg->rx_state = PPP_RX_MSG_RECEIVING_BODY;
            rx_msg->buffer[rx_msg->buffer_size] = byte; // Store start flag
            rx_msg->buffer_size++;
        }
        break;

    case PPP_RX_MSG_RECEIVING_BODY:
        rx_msg->buffer[rx_msg->buffer_size] = byte; // Store start flag
        rx_msg->buffer_size++;
		if (rx_msg->buffer_size >= rx_msg->buffer_capacity) {
			rx_msg->rx_state = PPP_RX_MSG_ERROR; // Buffer overflow
			return PPP_ERR_OUTPUT_BUFFER_TOO_SMALL;
		}
        if (byte == PPP_FLAG) {
            rx_msg->rx_state = PPP_RX_MSG_RECEIVED;
        }
        break;
    default:
        return PPP_WARNING_INVALID_PARSING_STATE;
        break;
    }

    return PPP_SUCCESS;
}


int8_t ppp_custom_rx_process_byte(ppp_custom_rx_message_t* rx_msg, uint8_t byte) {
    if (rx_msg->rx_state == PPP_RX_MSG_EMPTY || rx_msg->rx_state == PPP_RX_MSG_ERROR || rx_msg->rx_state == PPP_RX_MSG_DECODED) {
        rx_msg->rx_state = PPP_RX_MSG_WAIT_START_FLAG;
        rx_msg->buffer_size = 0;
    }
    else if (rx_msg->rx_state == PPP_RX_MSG_RECEIVED || rx_msg->rx_state == PPP_RX_MSG_DECODED || rx_msg->rx_state == PPP_RX_MSG_WAIT_START_FLAG || rx_msg->rx_state == PPP_RX_MSG_RECEIVING_BODY)
    {
    }
    else {
        rx_msg->rx_state = PPP_RX_MSG_WAIT_START_FLAG;
        rx_msg->buffer_size = 0;
    }

	int8_t result = ppp_custom_rx_append_byte(rx_msg, byte);

    if (rx_msg->rx_state == PPP_RX_MSG_ERROR) {
        return result;
    }

    if (rx_msg->rx_state == PPP_RX_MSG_RECEIVED) {
        result = ppp_custom_decode(rx_msg->buffer, rx_msg->buffer_size, rx_msg->buffer, &(rx_msg->decoded_message));
        if (result == PPP_SUCCESS) {
			rx_msg->rx_state = PPP_RX_MSG_DECODED;
        }
        else {
            rx_msg->rx_state = PPP_RX_MSG_ERROR;
        }
        return result;
    }
    return result;
}


int8_t ppp_custom_prepare_tx_message(ppp_custom_tx_message_t *tx_message, uint8_t sequence, uint8_t control, uint8_t* payload, size_t payload_size) {
    size_t enc_len = ppp_custom_encode(payload, payload_size, tx_message->buffer, tx_message->buffer_capacity, sequence, control);
	tx_message->buffer_size = enc_len;
	tx_message->sent_size = 0;
    tx_message->tx_state = PPP_TX_MSG_EMPTY;
    if (enc_len == 0) {
        tx_message->tx_state = PPP_TX_MSG_ERROR;
		return PPP_ERR_OUTPUT_BUFFER_TOO_SMALL;
	}
    tx_message->tx_state = PPP_TX_MSG_ENCODED;
	return PPP_SUCCESS;
}

void ppp_custom_tx_stream_send_message_routine(ppp_custom_stream_t* stream, ppp_custom_tx_message_t* tx_message) {
    if (tx_message->tx_state == PPP_TX_MSG_ENCODED || tx_message->tx_state == PPP_TX_MSG_SENDING) {
		int result = stream->send_fcn(tx_message->buffer + tx_message->sent_size,
			tx_message->buffer_size - tx_message->sent_size);
        if (result >= 0) {
            tx_message->sent_size += result;
        }
        if (tx_message->sent_size == tx_message->buffer_size)
        {
            tx_message->tx_state == PPP_TX_MSG_SENDT;
        }
        else {
			tx_message->tx_state = PPP_TX_MSG_SENDING;
        }
    }
}

int8_t ppp_custom_tx_stream_routine(ppp_custom_stream_t* stream) {
    if (stream->tx_message_control.tx_state == PPP_TX_MSG_SENDT) {
        stream->tx_message_control.tx_state = PPP_TX_MSG_EMPTY;
    }
    else if (stream->tx_message_control.tx_state == PPP_TX_MSG_ENCODED || stream->tx_message_control.tx_state == PPP_TX_MSG_SENDING)
    {
        ppp_custom_tx_stream_send_message_routine(stream, &stream->tx_message_control);
    }
}

int8_t ppp_custom_rx_stream_routine(ppp_custom_stream_t* stream) {
    uint8_t byte;
    int result = stream->recv_fcn(&byte, 1);
    if (result <= 0)
    {
        return PPP_WAIT_FOR_DATA;
    }
    ppp_custom_rx_process_byte(&(stream->rx_message), byte);

    if (stream->rx_message.rx_state == PPP_RX_MSG_DECODED)
    {   
        if (*(stream->rx_message.decoded_message.control) == PPP_DATA_PROTOCOL)
        {
            if (*(stream->rx_message.decoded_message.sequence_counter) == stream->rx_sequence_counter)
            {
                if (ring_buffer_free_space(&(stream->receive_buffer)) >= stream->rx_message.decoded_message.payload_size) {
                    ring_buffer_enqueue_arr(&(stream->receive_buffer), stream->rx_message.decoded_message.payload, stream->rx_message.decoded_message.payload_size);
                    stream->rx_message.rx_state = PPP_RX_MSG_EMPTY; // Reset state after processing
				    // send ack for the current sequence number
                    ppp_custom_prepare_tx_message(stream, stream->rx_sequence_counter, PPP_ACK_PROTOCOL, NULL, 0);
                    stream->rx_sequence_counter++;
                }
                else {
                    // drop the message
                    stream->rx_message.rx_state = PPP_RX_MSG_EMPTY;
                }
		    }
		    else if(*(stream->rx_message.decoded_message.sequence_counter) == (stream->rx_sequence_counter - 1)){
                // send ack for the last sequence number because the sender didnt receive any
                ppp_custom_prepare_tx_message(stream, stream->rx_sequence_counter - 1, PPP_ACK_PROTOCOL, NULL, 0);
			    // drop the message
                stream->rx_message.rx_state = PPP_RX_MSG_EMPTY;
		    }
            else {
				// sender is out of sync and is sending other messages
				// drop the message
                stream->rx_message.rx_state = PPP_RX_MSG_EMPTY;
            }
        }
        else if (*(stream->rx_message.decoded_message.control) == PPP_ACK_PROTOCOL)
        {
            if (stream->tx_message.tx_state == PPP_TX_MSG_WAIT_ACK && *(stream->rx_message.decoded_message.sequence_counter) == stream->tx_sequence_counter) {
                stream->tx_message.tx_state = PPP_TX_MSG_COMPLETED;
                stream->tx_sequence_counter++;
            }
            else {
                // vary bad, the receiver is acknologing other messages
            }
            stream->rx_message.rx_state = PPP_RX_MSG_EMPTY;
        }
        else
        {
            // drop the message, protocol unknown
            stream->rx_message.rx_state = PPP_RX_MSG_EMPTY;
        }
    }

    return 0;
}



void ppp_custom_test_encode_decode(void) {
    uint8_t encoded[ENCODED_FRAME_MAX_LEN(PPP_MAX_PAYLOAD_LEN)];
    uint8_t decoded[ENCODED_FRAME_MAX_LEN(PPP_MAX_PAYLOAD_LEN)];
	size_t encoded_max_size = sizeof(encoded);
	ppp_custom_decoded_message_t decoded_message;

    for (uint16_t i = 0; i <= 255; i++) {
        const uint8_t base_payload[] = { 'H', PPP_ESCAPE, 'l', 'l', 'o', PPP_FLAG, PPP_ESCAPE, PPP_ESC_MASK, 'o', 'r', 'l', 'd' };
        uint8_t test_payload[sizeof(base_payload) + 1];
        size_t test_payload_size = sizeof(test_payload);
        memcpy(test_payload, base_payload, sizeof(base_payload));
        test_payload[sizeof(base_payload)] = (uint8_t)i;  // Append counter

        size_t enc_len = ppp_custom_encode(test_payload, test_payload_size, encoded, encoded_max_size, i, 1);

        int status = ppp_custom_decode(encoded, enc_len, decoded, &decoded_message);
        if (status == PPP_SUCCESS) {
            printf("Test %u passed.\n", i);
        }
        else {
            printf("PPP decode failed at counter %u with code %d\n", i, status);
        }
        if (decoded_message.payload_size != test_payload_size || memcmp(decoded_message.payload, test_payload, decoded_message.payload_size) != 0) {
            printf("Payload mismatch at counter %u\n", i);
        }
    }
}


void ppp_custom_test_receive(void) {
    ppp_custom_stream_t stream;
    stream.rx_sequence_counter = 0;
    stream.tx_sequence_counter = 0;
    uint8_t encoded[ENCODED_FRAME_MAX_LEN(PPP_MAX_PAYLOAD_LEN)];
    size_t encoded_max_size = sizeof(encoded);

    const uint8_t test_payload[] = { 'A' };//{ 'H', PPP_ESCAPE, 'l', 'l', 'o', PPP_FLAG, PPP_ESCAPE, PPP_ESC_MASK, 'o', 'r', 'l', 'd' };
    size_t test_payload_size = sizeof(test_payload);
    size_t enc_len = ppp_custom_encode(test_payload, test_payload_size, encoded, encoded_max_size, 0, PPP_DATA_PROTOCOL);

    for (size_t i = 0; i < enc_len; i++)
    {
        ppp_custom_rx_stream_routine(&stream);
    }
}

