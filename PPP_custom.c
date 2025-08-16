#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "PPP_custom.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))


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
int8_t ppp_encode(
    const uint8_t* payload,
    size_t payload_len,
    uint8_t* out_buf,
    size_t out_buf_len,
    size_t* frame_size,
    uint8_t address,
    uint8_t control,
    uint16_t protocol)

{
    size_t out_len = 0;
    uint8_t header[PPP_HEADER_SIZE];
    header[0] = address;
    header[1] = control;
    header[2] = (protocol >> 8) & 0xFF;
    header[3] = (protocol) & 0xFF;

    uint16_t fcs = ppp_crc16_ccitt_start(header, sizeof(header));
    fcs = crc16_ccitt_continue(fcs, payload, payload_len);

    out_buf[out_len] = PPP_FLAG;
    out_len++;

    for (int i = 0; i < sizeof(header); i++) {
        if (out_buf_len < out_len + 2) {
            return PPP_ERR_OUTPUT_BUFFER_TOO_SMALL;
        }
        ppp_write_escaped(header[i], out_buf, &out_len);
    }
    for (size_t i = 0; i < payload_len; i++) {
        if (out_buf_len < out_len + 2) {
            return PPP_ERR_OUTPUT_BUFFER_TOO_SMALL;
        }
        ppp_write_escaped(payload[i], out_buf, &out_len);
    }
    if (out_buf_len < out_len + 2) {
        return PPP_ERR_OUTPUT_BUFFER_TOO_SMALL;
    }
    ppp_write_escaped((fcs >> 8) & 0xFF, out_buf, &out_len);
    if (out_buf_len < out_len + 2) {
        return PPP_ERR_OUTPUT_BUFFER_TOO_SMALL;
    }
    ppp_write_escaped(fcs & 0xFF, out_buf, &out_len);
    if (out_buf_len < out_len + 1) {
        return PPP_ERR_OUTPUT_BUFFER_TOO_SMALL;
    }
    out_buf[out_len] = PPP_FLAG;
    out_len++;
	*frame_size = out_len;
    return PPP_SUCCESS;
}

// Decodes a PPP frame (assumes full frame with flags is given)
int8_t ppp_decode(uint8_t *frame, size_t frame_len, uint8_t *decoded_buffer, ppp_decoded_frame_t* message) {
    if (frame_len < PPP_PROTOCOL_OVERHEAD_BYTE_SIZE || frame[0] != PPP_FLAG || frame[frame_len - 1] != PPP_FLAG)
        return PPP_ERR_MESSAGE_TOO_SHORT;
    message->frame = decoded_buffer;
    uint8_t *temp = &decoded_buffer[1];
    decoded_buffer[0] = frame[0]; // Placeholder for sequence number

    size_t temp_len = ppp_unescape(&frame[1], frame_len - PPP_START_FLAG_SIZE - 1, temp, frame_len-1); // remove 0x7E

    if (temp_len < (PPP_PROTOCOL_OVERHEAD_BYTE_SIZE - (2 * PPP_START_FLAG_SIZE))) return PPP_ERR_FRAME_TOO_SMALL;
    
    uint16_t recv_crc = ((uint16_t)temp[temp_len - 2] << 8) | temp[temp_len-1];
    uint16_t calc_crc = ppp_crc16_ccitt_start(temp, temp_len - PPP_CRC_BYTE_SIZE);
    if (recv_crc != calc_crc) return PPP_ERR_INVALID_CRC;

    message->address = temp[0];
    message->control = temp[1];
	message->protocol = (uint16_t)(((uint16_t)temp[2] << 8) | temp[3]);
    message->payload = &temp[PPP_HEADER_SIZE];
    message->frame_size = temp_len + (PPP_START_FLAG_SIZE * 2);
    message->payload_size = temp_len - PPP_HEADER_SIZE - PPP_CRC_BYTE_SIZE;
    message->CRC = calc_crc;

    return PPP_SUCCESS;
}



static void ppp_on_rx_frame(ppp_stream_t* stream, ppp_decoded_frame_t* message) {
    stream->last_recv_frame_timestamp_ms = stream->get_system_millis ? stream->get_system_millis() : 0;
    if (stream->packet_rx_callback) {
        stream->packet_rx_callback(stream, message, stream->packet_rx_callback_context);
    }
    ppp_stream_drop_recv_frame(stream);
}

static void ppp_on_tx_frame(ppp_stream_t* stream, ppp_encoded_frame_t* message) {
    stream->last_sent_frame_timestamp_ms = stream->get_system_millis ? stream->get_system_millis() : 0;
    if (stream->packet_tx_callback) {
        stream->packet_tx_callback(stream, &(stream->tx_message), stream->packet_tx_callback_context);
    }
	ppp_stream_drop_send_frame(stream);
}

uint8_t ppp_stream_init(
    ppp_stream_t* stream,
    uint8_t* rx_buffer,
    uint8_t* tx_buffer,
    uint16_t buffer_capacity,
    uint16_t mtu,
    ppp_frame_rx_callback_t rx_callback,
    void* rx_context,
    ppp_frame_tx_callback_t tx_callback,
    void* tx_context,
    ppp_get_system_millis get_system_millis,
    ppp_stream_status_callback_t stream_status_callback,
    void* stream_status_callback_context,
    ppp_send_buffer_fcn_t send_buffer_fcn,
    void* send_buffer_fcn_context,
    ppp_recv_buffer_fcn_t recv_buffer_fcn,
    void* recv_buffer_fcn_context
) {
    if (!stream || !rx_buffer || !tx_buffer) return;
	memset(stream, 0, sizeof(ppp_stream_t));

    if (buffer_capacity < PPP_ENCODED_FRAME_MAX_LEN(mtu)) {
        return PPP_ERR_BUFFER_TOO_SMALL;
    }

    if (rx_buffer == NULL ||
        tx_buffer == NULL
        )
    {
        return PPP_ERR_INVALID_PARAMETERS;
    }

    //stream->rx_buffer = rx_buffer;
    //stream->tx_buffer = tx_buffer;

    rxtxbuffer_init(&stream->rx_buffer, rx_buffer, buffer_capacity);
    rxtxbuffer_init(&stream->tx_buffer, tx_buffer, buffer_capacity);

    stream->buffer_capacity = buffer_capacity;
    stream->MTU_size = mtu;

    //stream->rx_frame_received_size = 0;
    //stream->tx_frame_sent_size = 0;

    stream->rx_state = PPP_RX_FRAME_PARSING_STATE_WAIT_FOR_START_FLAG;
    stream->tx_state = PPP_RX_FRAME_PARSING_STATE_WAIT_FOR_START_FLAG;

    stream->packet_rx_callback = rx_callback;
    stream->packet_tx_callback = tx_callback;

    stream->packet_rx_callback_context = rx_context;
    stream->packet_tx_callback_context = tx_context;

	stream->get_system_millis = get_system_millis;

    stream->stream_status_callback = stream_status_callback;
    stream->stream_status_callback_context = stream_status_callback_context;

    stream->send_buffer_fcn = send_buffer_fcn;
    stream->send_buffer_fcn_context = send_buffer_fcn_context;

    stream->recv_buffer_fcn = recv_buffer_fcn;
	stream->recv_buffer_fcn_context = recv_buffer_fcn_context;

	return PPP_SUCCESS;
}


int8_t ppp_stream_parse_byte(ppp_stream_t* stream, uint8_t byte) {
    if (!stream) return PPP_ERR_INVALID_PARAMETERS;

ppp_stream_parse_byte_start_lable:
    switch (stream->rx_state) {
    case PPP_RX_FRAME_PARSING_STATE_WAIT_FOR_START_FLAG:
        if (byte == PPP_FLAG) {
			rxtxbuffer_clear(&stream->rx_buffer);
            rxtxbuffer_push_arr(&stream->rx_buffer, &byte, 1);
            stream->rx_state = PPP_RX_FRAME_PARSING_STATE_RECEIVING_BODY;
        }
        break;

    case PPP_RX_FRAME_PARSING_STATE_RECEIVING_BODY:
        if (rxtxbuffer_data_size(&stream->rx_buffer) >= stream->buffer_capacity) {
            ppp_stream_drop_recv_frame(stream);
            goto ppp_stream_parse_byte_start_lable;
            //return PPP_ERR_OUTPUT_BUFFER_TOO_SMALL;
        }
        rxtxbuffer_push_arr(&stream->rx_buffer, &byte, 1);

        if (byte == PPP_FLAG) {
            if (rxtxbuffer_data_size(&stream->rx_buffer) < PPP_PROTOCOL_OVERHEAD_BYTE_SIZE) {
                ppp_stream_drop_recv_frame(stream);
                goto ppp_stream_parse_byte_start_lable;
            }
            // We have a full frame
            ppp_decoded_frame_t* message = &stream->rx_message;
            int8_t result = ppp_decode(rxtxbuffer_buffer(&stream->rx_buffer), rxtxbuffer_data_size(&stream->rx_buffer), rxtxbuffer_buffer(&stream->rx_buffer), message);

            if (result == PPP_SUCCESS) {
                if (message->payload_size > stream->MTU_size)
                {
                    ppp_stream_drop_recv_frame(stream);
                    goto ppp_stream_parse_byte_start_lable;
                    //return PPP_ERR_MTU_EXCEDEED;
                }
                stream->rx_state = PPP_RX_FRAME_PARSING_STATE_VALID_FRAME;
				ppp_on_rx_frame(stream, message);
            }
            else {
                ppp_stream_drop_recv_frame(stream);
                goto ppp_stream_parse_byte_start_lable;
            }
        }
        break;
    default:
        ppp_stream_drop_recv_frame(stream);
        goto ppp_stream_parse_byte_start_lable;
        break;
    }

    return PPP_STATUS_PENDING;
}

int8_t ppp_stream_send_routine(ppp_stream_t* stream, ppp_send_buffer_fcn_t send_fcn, void* context) {
    if (!stream || !send_fcn) return PPP_ERR_INVALID_PARAMETERS;
    int sent_now = 0;

    if (stream->tx_state == PPP_TX_FRAME_PARSING_STATE_SENDING) {
        
        sent_now = send_fcn(rxtxbuffer_data_ptr(&stream->tx_buffer), stream->tx_message.frame_size - rxtxbuffer_sent_size(&stream->tx_buffer), context);
        if (sent_now < 0) {
            return PPP_ERR_SENDING_OPERATION_ERROR;
        }
		rxtxbuffer_sent_data_increase_size(&stream->tx_buffer, sent_now);
        if (rxtxbuffer_sent_size(&stream->tx_buffer) >= stream->tx_message.frame_size) {
            stream->tx_state = PPP_TX_FRAME_PARSING_STATE_SENDING_COMPLETED;
			ppp_on_tx_frame(stream, &(stream->tx_message));
        }
    }
    return PPP_SUCCESS;
}

int8_t ppp_stream_send_frame(ppp_stream_t* stream, const ppp_decoded_frame_t message) {
    if (!stream) {
        return PPP_ERR_INVALID_PARAMETERS;
    }

    if (message.payload_size > stream->MTU_size) {
        return PPP_ERR_MTU_EXCEDEED;
    }

    if (stream->tx_state == PPP_TX_FRAME_PARSING_STATE_SENDING) {
        return PPP_ERR_ALREADY_A_FRAME_IN_QUEUE;
    }
    else {
        size_t enc_len;
		rxtxbuffer_clear(&stream->tx_buffer);
        int8_t result = ppp_encode(message.payload, message.payload_size, rxtxbuffer_buffer(&stream->tx_buffer), stream->buffer_capacity, &enc_len, message.address, message.control, message.protocol);
		

		if (result == PPP_SUCCESS) {
            rxtxbuffer_data_increase_size(&stream->tx_buffer, enc_len);
            ppp_encoded_frame_t encoded_message;
			encoded_message.address = message.address;
			encoded_message.control = message.control;
			encoded_message.protocol = message.protocol;
			encoded_message.frame = rxtxbuffer_buffer(&stream->tx_buffer);
			encoded_message.frame_size = enc_len;
            encoded_message.payload_size = message.payload_size;

            
			stream->tx_message = encoded_message;
			stream->tx_state = PPP_TX_FRAME_PARSING_STATE_SENDING;
	    }

        return result;
    }
}

void ppp_stream_drop_recv_frame(ppp_stream_t* stream) {
    stream->rx_state = PPP_RX_FRAME_PARSING_STATE_WAIT_FOR_START_FLAG;
    rxtxbuffer_clear(&stream->rx_buffer);
}

void ppp_stream_drop_send_frame(ppp_stream_t* stream) {
    stream->tx_state = PPP_TX_FRAME_PARSING_STATE_NOTHING;
	rxtxbuffer_reset_sent_size(&stream->tx_buffer);
}

void ppp_stream_tick(ppp_stream_t* stream) {
    int result = ppp_stream_send_routine(stream, stream->send_buffer_fcn, stream->send_buffer_fcn_context);
    if (result != PPP_SUCCESS) {
        printf("Error: ppp_stream_send_routine {%d}\n", result);
        stream->stream_status = PPP_STREAM_STATUS_CONNECTION_ERROR;
        if (stream->stream_status_callback) {
            stream->stream_status_callback(stream, stream->stream_status, stream->stream_status_callback_context);
        }
        //return;
    }

    // recv routine
    uint8_t byte;
    do
    {
        result = stream->recv_buffer_fcn(&byte, 1, stream->recv_buffer_fcn_context);
		ppp_stream_parse_byte(stream, byte);
    } while (result > 0);
    if (result < 0) {
        printf("Error: ppp_stream_recv_routine {%d}\n", result);
        stream->stream_status = PPP_STREAM_STATUS_CONNECTION_ERROR;
        if (stream->stream_status_callback) {
            stream->stream_status_callback(stream, stream->stream_status, stream->stream_status_callback_context);
        }
        //return;
    }
    // add echo reply commands
}



/* Returns positive distance (mod 65536) from seq_b to seq_a */
#define SEQ_DISTANCE(seq_a, seq_b)  ((uint16_t)((seq_a) - (seq_b)))

/* True if seq_a comes before seq_b in the sequence space (handles wrap-around) */
#define SEQ_IS_BEFORE(seq_a, seq_b) (SEQ_DISTANCE((seq_a),(seq_b)) & (1 << ((TCP_SEQUENCE_NUMBER_SIZE * 8)-1)))

/* True if seq_a is before or equal to seq_b in the sequence space */
#define SEQ_IS_BEFORE_OR_EQUAL(seq_a, seq_b) (!SEQ_IS_BEFORE((seq_b),(seq_a)))


static void tcp_ppp_rx_callback(
    struct ppp_stream* stream,
    ppp_decoded_frame_t* message,
    tcp_t* context
) {
    ppp_decoded_packet_t pkt;

    if (message->payload_size < TCP_HEADER_SIZE) return; // malformed

    context->last_recv_packet_timestamp_ms = stream->get_system_millis ? stream->get_system_millis() : 0;

	pkt.packet = message->payload;
	pkt.payload_size = message->payload_size;
    pkt.sequence_number = (((uint16_t)message->payload[0]) << 8) | (uint16_t)(message->payload[1]);
    pkt.ack_number = (((uint16_t)message->payload[TCP_SEQUENCE_NUMBER_SIZE]) << 8) | (uint16_t)(message->payload[TCP_SEQUENCE_NUMBER_SIZE+1]);

    pkt.payload = message->payload + TCP_HEADER_SIZE;
    pkt.payload_size = message->payload_size - TCP_HEADER_SIZE;


    /* 2) Slide our send window using peer's cumulative ACK
          Only accept ACKs in [send_base .. sequence_number] */
    if (SEQ_IS_BEFORE_OR_EQUAL(context->send_base, pkt.ack_number) && SEQ_IS_BEFORE_OR_EQUAL(pkt.ack_number, context->sequence_number)) {
        uint16_t in_flight = SEQ_DISTANCE(context->sequence_number, context->send_base);
        uint16_t want_advance = SEQ_DISTANCE(pkt.ack_number, context->send_base);
        uint16_t advance = (want_advance <= in_flight) ? want_advance : in_flight;

        if (advance) {
            ring_buffer_advance_tail(&context->send_buffer, advance);
            context->send_base = (uint16_t)(context->send_base + advance);
        }
    }
    /* else: ignore bogus/out-of-window ACKs */

    /* 3) Data handling (does NOT change our sequence_number) */
    if (pkt.payload_size == 0) {
        /* Pure ACK; nothing to deliver; do not change ack_number */
        return;
    }

    /* We accept only contiguous bytes to ack_number (recv_next). */
    if (pkt.sequence_number == context->ack_number) {

		size_t free_space = ring_buffer_free_space(&context->recv_buffer);
		size_t write_size = MIN(free_space, pkt.payload_size);

        /* In-order: take all and advance ack_number by pl_len */
        ring_buffer_enqueue_arr(&context->recv_buffer, pkt.payload, write_size);
        context->ack_number = (uint16_t)(context->ack_number + write_size);
    }
    else if (SEQ_IS_BEFORE(pkt.sequence_number, context->ack_number)) {
        /* Overlap: part of this payload is duplicate; trim the prefix */
        uint16_t overlap = SEQ_DISTANCE(context->ack_number, pkt.sequence_number);
        if (overlap < pkt.payload_size) {
            uint16_t new_bytes = (uint16_t)(pkt.payload_size - overlap);

            size_t free_space = ring_buffer_free_space(&context->recv_buffer);
            size_t write_size = MIN(free_space, new_bytes);

            ring_buffer_enqueue_arr(&context->recv_buffer, pkt.payload + overlap, write_size);
            context->ack_number = (uint16_t)(context->ack_number + write_size);
        }
        /* else fully duplicate → ignore */
    }
    else {
        /* Gap (out-of-order). Do NOT advance ack_number.
           Optionally buffer OOO segments here if you want reassembly. */
    }
}

static inline void be16wr(uint8_t* p, uint16_t v) { p[0] = (uint8_t)(v >> 8); p[1] = (uint8_t)v; }

static inline size_t tcp_time_since_ms(const tcp_t* t, size_t old_timestamp_ms)
{
    size_t now = t->stream.get_system_millis ? t->stream.get_system_millis() : 0;
    return now - old_timestamp_ms; // wraps correctly for unsigned
}

/* Peek from send ring and send one PPP frame (seq/ack header + chunk).
   Returns 0 if nothing sent, >0 if a frame was queued, <0 on error. */
static int tcp_try_send_next(tcp_t* ctx)
{
    /* Nothing queued? */
    size_t used = ring_buffer_used_space(&ctx->send_buffer);
    if (used == 0) {
        /* send an ACK-only frame if last ack was not sent*/
        if (ctx->last_ack_number_sent != ctx->ack_number &&
            tcp_time_since_ms(ctx, ctx->last_sent_packet_timestamp_ms) >= TCP_ACK_SEND_TIMEOUT_MS
			
            )
        {
   //         size_t a = tcp_time_since_ms(ctx, ctx->last_sent_packet_timestamp_ms);

   //         int ggg = 2;
			//printf("%lld >= %d -> %d\n", a, ctx->keep_alive_interval_ms, a >= ctx->keep_alive_interval_ms);
        }
        else if (ctx->keep_alive_interval_ms > 0 &&
            tcp_time_since_ms(ctx, ctx->last_sent_packet_timestamp_ms) >= ctx->keep_alive_interval_ms)
        {

        }
        else {
            return 0;
        }
    }

    /* Max payload per frame = MTU - header(4). */
    uint16_t max_payload = (ctx->stream.MTU_size > TCP_HEADER_SIZE) ? (uint16_t)(ctx->stream.MTU_size - TCP_HEADER_SIZE) : 0;
    if (max_payload == 0) return -1;

    uint16_t chunk = (uint16_t)MIN(used, (size_t)max_payload);

    /* Build header + payload into a temporary linear buffer. */
    /* Stack buffer is fine because ppp_stream_send_frame() encodes into stream->tx_buffer. */
    uint8_t *frame_buf = ctx->send_packet_buffer; /* adjust 1500 to your worst-case MTU */
	size_t frame_buf_size = ctx->send_packet_buffer_capacity;
    if ((size_t)(4 + chunk) > frame_buf_size) return -2;

    /* Header */
    be16wr(&frame_buf[0], ctx->send_base);  /* seq for this chunk */
	
    be16wr(&frame_buf[2], ctx->ack_number);       /* piggyback latest cumulative ACK */

    /* Payload: peek from ring without consuming (we consume on ACK) */
    /* You need a ring "peek" API. If you don't have one, expose it or copy via two linear segments. */
    size_t copied = ring_buffer_peek_arr(&ctx->send_buffer, &frame_buf[4], chunk);
    if (copied != chunk) {
        /* Fallback: if no peek API exists, implement a two-chunk copy using your ring’s linear read ptrs. */
        return -3;
    }

    /* Queue frame into PPP */
    ppp_decoded_frame_t out = { 0 };
    out.address = PPP_DEFAULT_ADDRESS;
    out.control = PPP_DEFAULT_CONTROL;
    out.protocol = TCP_PPP_PROTOCOL; /* your custom protocol ID */
    out.payload = frame_buf;
    out.payload_size = (uint16_t)(TCP_HEADER_SIZE + chunk);

    int8_t r = ppp_stream_send_frame(&ctx->stream, out);
    if (r != PPP_SUCCESS) return r;
    ctx->last_ack_number_sent = ctx->ack_number; /* remember last ack sent */
    return (int)chunk; /* bytes scheduled (payload size) */
}


static void tcp_ppp_tx_callback(
    struct ppp_stream* stream,
    ppp_encoded_frame_t* message,
    tcp_t* ctx
) {
    /* message->payload_size is the raw PPP payload length we constructed earlier.
       It includes 4 bytes header + payload. */
    if (message == NULL) return;
    if (message->payload_size < TCP_HEADER_SIZE) return;

	ctx->last_sent_packet_timestamp_ms = stream->get_system_millis ? stream->get_system_millis() : 0;
    uint16_t just_sent_payload = (uint16_t)(message->payload_size - TCP_HEADER_SIZE);

    /* Advance our send_next (sequence_number) by the number of payload bytes we just sent. */
    ctx->sequence_number = (uint16_t)(ctx->sequence_number + just_sent_payload);

    /* Do NOT pop data from the send ring here.
       We only drop bytes from the ring when the peer ACKs them (RX path). */


       /* Try to send the next chunk (if any). */
    tcp_try_send_next(ctx);
}


uint8_t tcp_init(tcp_t* t,
    uint8_t* rx_ring_mem, size_t rx_ring_size,
    uint8_t* tx_ring_mem, size_t tx_ring_size,
    uint8_t* send_pkt_buf, size_t send_pkt_buf_cap,
    uint8_t* ppp_recv_buffer, uint8_t* ppp_send_buffer,
    size_t ppp_buffer_size,
    uint16_t mtu,
    size_t keep_alive_interval_ms,
    ppp_send_buffer_fcn_t send_fn, void* send_ctx,
    ppp_recv_buffer_fcn_t recv_fn, void* recv_ctx,
    ppp_get_system_millis get_ms)
{
    memset(t, 0, sizeof(*t));

    if (send_pkt_buf_cap < TCP_MAX_PACKET_SIZE(mtu)) {
		return TCP_ERR_BUFFER_TOO_SMALL;
    }

    if (rx_ring_size < 2 ||
        tx_ring_size < 2)
    {
        return TCP_ERR_BUFFER_TOO_SMALL;
    }

    if (send_pkt_buf == NULL ||
        rx_ring_mem == NULL ||
        tx_ring_mem == NULL)
    {
        return PPP_ERR_INVALID_PARAMETERS;
    }

    t->send_packet_buffer = send_pkt_buf;
    t->send_packet_buffer_capacity = send_pkt_buf_cap;

    ring_buffer_init(&t->recv_buffer, rx_ring_mem, rx_ring_size);
    ring_buffer_init(&t->send_buffer, tx_ring_mem, tx_ring_size);

    t->send_base = 0;
    t->sequence_number = 0;
    t->ack_number = 0;

    t->keep_alive_interval_ms = keep_alive_interval_ms;

    uint8_t res = ppp_stream_init(&t->stream,
        /* rx_buffer: */ ppp_recv_buffer,         /* used as scratch for PPP rx */
        /* tx_buffer: */ ppp_send_buffer,        /* PPP encodes here before send_fn */
        /* buffer_capacity */ (uint16_t)ppp_buffer_size,
        /* mtu */ TCP_MAX_PACKET_SIZE(mtu),
        /* rx_cb */ tcp_ppp_rx_callback, t,
        /* tx_cb */ tcp_ppp_tx_callback, t,
        /* get_ms */ get_ms,
        /* status_cb */ NULL, NULL,
        /* send_fn */ send_fn, send_ctx,
        /* recv_fn */ recv_fn, recv_ctx);

	if (res != PPP_SUCCESS) {
		return res; // PPP stream init failed
	}

}

int tcp_send(tcp_t* t, uint8_t* buf, int len) {
    if (!t || !buf || len <= 0) return 0;
    size_t free = ring_buffer_free_space(&t->send_buffer);
    size_t to_copy = free < (size_t)len ? free : (size_t)len;
    size_t wrote = ring_buffer_enqueue_arr(&t->send_buffer, buf, to_copy);
    //if (wrote) (void)tcp_try_send_next(t);
    return (int)wrote;
}

int tcp_recv(tcp_t* t, uint8_t* buf, int len) {
    if (!t || !buf || len <= 0) return 0;
    size_t used = ring_buffer_used_space(&t->recv_buffer);
    size_t to_copy = used < (size_t)len ? used : (size_t)len;
    return (int)ring_buffer_dequeue_arr(&t->recv_buffer, buf, to_copy);
}

int tcp_available_to_send(tcp_t* t) {
    if (!t) return 0;
    return (int)ring_buffer_free_space(&t->send_buffer);
}

int tcp_available_to_recv(tcp_t* t) {
    if (!t) return 0;
    return (int)ring_buffer_used_space(&t->recv_buffer);
}

uint8_t tcp_get_connection_state(tcp_t* tcp) {
	if (!tcp) return TCP_CONNECTION_STATE_DISCONNECTED;
	return tcp->connection_state;
}



void tcp_tick(tcp_t* tcp) {
    tcp_try_send_next(tcp);
	ppp_stream_tick(&tcp->stream);

	/* Check if we need to send a keep-alive packet */
	if (TCP_CONNECTION_TIMEOUT_MS > 0 &&
        tcp_time_since_ms(tcp, tcp->last_sent_packet_timestamp_ms) >= TCP_CONNECTION_TIMEOUT_MS ||
        tcp_time_since_ms(tcp, tcp->last_recv_packet_timestamp_ms) >= TCP_CONNECTION_TIMEOUT_MS)
    {
		tcp->connection_state = TCP_CONNECTION_STATE_DISCONNECTED;
	}
    else
    {
		tcp->connection_state = TCP_CONNECTION_STATE_ESTABLISHED;
    }
}

#if PPP_TCP_WIDNOWS_DEBUG != 0
/*========================================================================================================================*/


#define PPP_MAX_PAYLOAD_LEN 32
void ppp_test_encode_decode(void) {
    uint8_t encoded[PPP_ENCODED_FRAME_MAX_LEN(PPP_MAX_PAYLOAD_LEN)];
    uint8_t decoded[PPP_ENCODED_FRAME_MAX_LEN(PPP_MAX_PAYLOAD_LEN)];
    size_t encoded_max_size = sizeof(encoded);
    ppp_decoded_frame_t decoded_message;

    for (uint16_t i = 0; i <= 255; i++) {
        const uint8_t base_payload[] = { 'H', PPP_ESCAPE, 'l', 'l', 'o', PPP_FLAG, PPP_ESCAPE, PPP_ESC_MASK, 'o', 'r', 'l', 'd' };
        uint8_t test_payload[sizeof(base_payload) + 1];
        size_t test_payload_size = sizeof(test_payload);
        memcpy(test_payload, base_payload, sizeof(base_payload));
        test_payload[sizeof(base_payload)] = (uint8_t)i;  // Append counter

        size_t enc_len;
        ppp_encode(test_payload, test_payload_size, encoded, encoded_max_size, &enc_len, PPP_DEFAULT_ADDRESS, PPP_DEFAULT_CONTROL, i);

        int status = ppp_decode(encoded, enc_len, decoded, &decoded_message);
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

/*========================================================================================================================*/


#define TEST_MTU_SIZE    256
#define TEST_BUFFER_SIZE PPP_ENCODED_FRAME_MAX_LEN(TEST_MTU_SIZE)

static void on_rx_frame(ppp_stream_t* stream, ppp_decoded_frame_t* message, void* context) {
    printf("RX CALLBACK: Received frame with payload size = %u\n", message->payload_size);
    printf("Payload: ");
    for (uint16_t i = 0; i < message->payload_size; i++) {
        printf("%c", message->payload[i]);
    }
    printf("\n");
}

static void on_tx_frame(ppp_stream_t* stream, ppp_encoded_frame_t* message, void* context) {
    printf("TX CALLBACK: Frame sent with size = %u\n", rxtxbuffer_sent_size(&stream->tx_buffer));
}

int test_send_function(void* buffer, int buffer_size, void* context) {
    ppp_stream_t* stream = (ppp_stream_t*)context;
    uint8_t* bytes = (uint8_t*)buffer;

    for (size_t i = 0; i < rxtxbuffer_data_remaining(&stream->tx_buffer); i++)
    {
        printf("%c", ((char*)rxtxbuffer_buffer(&stream->tx_buffer))[i]);
    }printf("\n");

    for (size_t i = 0; i < buffer_size; i++)
    {
        printf("%c", bytes[i]);
    }printf("\n");

    for (int i = 0; i < buffer_size; i++) {
        ppp_stream_parse_byte(stream, bytes[i]);
    }

    return buffer_size;
}

#include <stdio.h>
void ppp_stream_test(void) {
    uint8_t rx_buf[TEST_BUFFER_SIZE];
    uint8_t tx_buf[TEST_BUFFER_SIZE];

    ppp_stream_t stream;
    ppp_stream_init(&stream,
        rx_buf,
        tx_buf,
        TEST_BUFFER_SIZE,
        TEST_MTU_SIZE,
        on_rx_frame,
        NULL,
        on_tx_frame,
        NULL,
        NULL,
        NULL,
        NULL,
        test_send_function,
        NULL,
        NULL,
        NULL);


    // Prepare a test payload
    for (size_t i = 0; i < 3; i++)
    {
        const char test_payload[100];
        snprintf(test_payload,sizeof(test_payload), "ID: %d", i);
        ppp_decoded_frame_t out_frame;
        out_frame.payload = (uint8_t*)test_payload;
        out_frame.payload_size = (uint16_t)strlen(test_payload);
        out_frame.address = PPP_DEFAULT_ADDRESS;
        out_frame.control = PPP_DEFAULT_CONTROL;
        out_frame.protocol = 0x02;

        if (ppp_stream_send_frame(&stream, out_frame) != PPP_SUCCESS) {
            printf("Error: Failed to queue frame for sending\n");
            //continue;
        }

        //if (i % 2) {
        //    stream.tx_buffer[1]++;
        //}

        if (i == 1) {
			//((uint8_t*)rxtxbuffer_data_ptr(&stream.tx_buffer))[0] = 0; // Simulate an error in the first byte
            //stream.tx_buffer.[0] = 0;
        }

        // Send data and parse back as if it was received byte-by-byte
        int result = ppp_stream_send_routine(&stream, test_send_function, &stream);
        if (result != PPP_SUCCESS) {
            printf("Error: ppp_stream_send_routine {%d}\n", result);
        }
    }

}
/*========================================================================================================================*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

typedef struct {
    ring_buffer_t* tx_to_peer;    // wire we write to
    ring_buffer_t* rx_from_peer;  // wire we read from
} link_end_t;

/* ppp_stream send-buffer callback: push a bulk of bytes onto the wire */
static int wire_send_fn(void* buffer, int buffer_size, void* ctx) {
	static int inserted_error = 0; // for debugging
    link_end_t* end = (link_end_t*)ctx;
    if (!end || buffer_size <= 0) return -1;

    if (inserted_error <= 0 && buffer_size > 1) {
        ((uint8_t*)buffer)[1] += 1;
        inserted_error = 1; // for debugging, to see if we can handle errors
    }

    int pushed = ring_buffer_enqueue_arr(end->tx_to_peer, (const uint8_t*)buffer, (size_t)buffer_size);

    return pushed; // return how many bytes we "sent"
}

/* ppp_stream recv-buffer callback: pop ONE byte (ppp_stream_tick loops until zero) */
static int wire_recv_fn(uint8_t* buffer, int buffer_size, void* ctx) {
    (void)buffer_size; // we only read 1 byte as ppp_stream_tick calls with len=1
    link_end_t* end = (link_end_t*)ctx;
    if (!end) return -1;
    int got = ring_buffer_dequeue_arr(end->rx_from_peer, buffer, 1); // read one byte
    return got; // 1 if a byte, 0 if none
}



static size_t get_ms(void) {
    static LARGE_INTEGER freq = { 0 };
    if (freq.QuadPart == 0) {
        QueryPerformanceFrequency(&freq);
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (size_t)((now.QuadPart * 1000) / freq.QuadPart);
}

/* ==== demo ==== */
#define BUF_SIZE 512
#define MTU_SIZE 2
#define KEEP_ALIVE_INTERVAL_MS 1000
int tcp_test_1(void) {
    /* Two wire directions */
    ring_buffer_t a2b, b2a;

	uint8_t a2b_buf[TCP_PPP_ENCODED_FRAME_MAX_LEN(MTU_SIZE)], b2a_buf[TCP_PPP_ENCODED_FRAME_MAX_LEN(MTU_SIZE)];
	ring_buffer_init(&a2b, a2b_buf, sizeof(a2b_buf)); // A to B
	ring_buffer_init(&b2a, b2a_buf, sizeof(b2a_buf)); // B to A

    link_end_t linkA = { .tx_to_peer = &a2b, .rx_from_peer = &b2a };
    link_end_t linkB = { .tx_to_peer = &b2a, .rx_from_peer = &a2b };

    /* Buffers for both endpoints */
    uint8_t A_rx_ring[BUF_SIZE], A_tx_ring[BUF_SIZE], A_stage[TCP_MAX_PACKET_SIZE(MTU_SIZE)], A_ppp_rx_buf[TCP_PPP_ENCODED_FRAME_MAX_LEN(MTU_SIZE)], A_ppp_tx_buf[TCP_PPP_ENCODED_FRAME_MAX_LEN(MTU_SIZE)];   // A
    uint8_t B_rx_ring[BUF_SIZE], B_tx_ring[BUF_SIZE], B_stage[TCP_MAX_PACKET_SIZE(MTU_SIZE)], B_ppp_rx_buf[TCP_PPP_ENCODED_FRAME_MAX_LEN(MTU_SIZE)], B_ppp_tx_buf[TCP_PPP_ENCODED_FRAME_MAX_LEN(MTU_SIZE)];   // B

    tcp_t A = { 0 }, B = { 0 };

    /* Init both TCP-over-PPP endpoints */
    tcp_init(&A, A_rx_ring, sizeof(A_rx_ring),
        A_tx_ring, sizeof(A_tx_ring),
        A_stage, sizeof(A_stage),
        A_ppp_rx_buf, A_ppp_tx_buf, 
		sizeof(A_ppp_rx_buf),
        MTU_SIZE,
        KEEP_ALIVE_INTERVAL_MS,
        wire_send_fn, &linkA,
        wire_recv_fn, &linkA,
        get_ms);

    tcp_init(&B, B_rx_ring, sizeof(B_rx_ring),
        B_tx_ring, sizeof(B_tx_ring),
        B_stage, sizeof(B_stage),
        B_ppp_rx_buf, B_ppp_tx_buf,
        sizeof(B_ppp_rx_buf),
        MTU_SIZE,
        KEEP_ALIVE_INTERVAL_MS,
        wire_send_fn, &linkB,
        wire_recv_fn, &linkB,
        get_ms);

    /* Send a message from A to B */
    const char* msg = "Hello from A -> B!";
    int queued = tcp_send(&A, (uint8_t*)msg, (int)strlen(msg));
    printf("A queued %d bytes\n", queued);

    /* Pump both stacks until B has data (simple bounded loop) */
    uint32_t start = get_ms();
  //  for (size_t i = 0; i < 100; i++)
  //  {
  //      tcp_tick(&A.stream);
  //      tcp_tick(&B.stream);
		////printf("A: %d bytes to send, %d bytes to recv\n", ring_buffer_used_space(&(A.send_buffer)), ring_buffer_used_space(&(A.recv_buffer)));
		////printf("B: %d bytes to send, %d bytes to recv\n", ring_buffer_used_space(&(B.send_buffer)), ring_buffer_used_space(&(B.recv_buffer)));

  //  }
    /* Read at B */
    uint8_t buf[128] = { 0 };
    size_t data_len = 0;

    while (1) {
        tcp_tick(&A.stream);
		Sleep(1); // simulate some delay, e.g. network latency
        tcp_tick(&B.stream);
        Sleep(1); // simulate some delay, e.g. network latency
        int got = tcp_recv(&B, &(buf[data_len]), sizeof(buf) - data_len);

		data_len += got;
        printf("B recv %d bytes: \"%.*s\"\n", got, data_len, buf);
        if ((get_ms() - start) > 200000) { // 2s safety timeout
            printf("Timeout waiting for delivery\n");
            break;
        }
    }



    return 0;
}


/*========================================================================================================================*/

#endif // PPP_TCP_WIDNOWS_DEBUG != 0