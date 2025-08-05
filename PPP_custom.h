#ifndef PPP_CUSTOM_H
#define PPP_CUSTOM_H


/*

Sender:        [Frame(Seq=0)] --->
Receiver:                        ---> [ACK(0)]
Sender:        (Next Frame) --->

If ACK is lost:

Sender:        [Frame(Seq=1)] --->       X (lost)
								(waits...)
Sender:        [Frame(Seq=1)] --->
Receiver:                        ---> [ACK(1)]


*/

#include "ringbuffer.h"
#include "staticcvector.h"


#ifdef __cplusplus
extern "C" {
#endif

#define PPP_FLAG       0x7E
#define PPP_CTRL       0x03
#define PPP_ESCAPE     0x7D
#define PPP_ESC_MASK   0x20

#define PPP_ACK_PROTOCOL 0x01
#define PPP_DATA_PROTOCOL 0x02

#define SEQUENCE_COUNTER_BYTE_SIZE 1
#define CONTROL_BYTE_SIZE 1
#define CRC_BYTE_SIZE 2
#define START_FLAG_SIZE 1
#define HEADER_SIZE (SEQUENCE_COUNTER_BYTE_SIZE + CONTROL_BYTE_SIZE)
#define PROTOCOL_OVERHEAD_BYTE_SIZE (HEADER_SIZE + CRC_BYTE_SIZE + (START_FLAG_SIZE * 2))

#define PPP_SUCCESS 0
#define PPP_ERR_OUTPUT_BUFFER_TOO_SMALL 1
#define PPP_ERR_MESSAGE_TOO_SMALL 2
#define PPP_ERR_INVALID_CRC 3
#define PPP_WARNING_INVALID_PARSING_STATE 4
#define PPP_WAIT_FOR_DATA 5

#define PPP_MAX_PAYLOAD_LEN 32
#define ENCODED_FRAME_MAX_LEN(payload_size) ((payload_size * 2) + PROTOCOL_OVERHEAD_BYTE_SIZE)


typedef enum {
	PPP_RX_MSG_EMPTY = 0,
	PPP_RX_MSG_WAIT_START_FLAG,
	PPP_RX_MSG_RECEIVING_BODY,
	PPP_RX_MSG_RECEIVED,
	PPP_RX_MSG_DECODED,
	PPP_RX_MSG_ERROR,
} ppp_custom_rx_message_state_t;

typedef enum {
	PPP_TX_MSG_EMPTY = 0,
	PPP_TX_MSG_SENDING,
	PPP_TX_MSG_ENCODED,
	PPP_TX_MSG_ERROR,
	PPP_TX_MSG_SENDT,
	PPP_TX_MSG_WAIT_ACK,
	PPP_TX_MSG_COMPLETED
} ppp_custom_tx_message_state_t;

typedef struct {
	uint8_t* sequence_counter;
	uint8_t* control;
	uint8_t* payload;
	uint16_t CRC;
	uint8_t* buffer;
	uint16_t buffer_size;
	uint16_t payload_size;
} ppp_custom_decoded_message_t;

typedef struct {
	uint8_t buffer[ENCODED_FRAME_MAX_LEN(PPP_MAX_PAYLOAD_LEN)];
	size_t buffer_capacity;
	size_t buffer_size;
	ppp_custom_decoded_message_t decoded_message;
	ppp_custom_rx_message_state_t rx_state;
}ppp_custom_rx_message_t;

typedef struct {
	uint8_t buffer[ENCODED_FRAME_MAX_LEN(PPP_MAX_PAYLOAD_LEN)];
	size_t buffer_capacity;
	size_t buffer_size;
	size_t sent_size;
	ppp_custom_tx_message_state_t tx_state;
}ppp_custom_tx_message_t;


typedef struct {
	ppp_custom_rx_message_t rx_message;
	uint8_t rx_sequence_counter; // 0-255, used to detect duplicates

	ppp_custom_tx_message_t tx_message;
	ppp_custom_tx_message_t tx_message_control;
	uint8_t tx_sequence_counter; // 0-255, used to detect duplicates

	uint8_t timeout;	// boolean if 1 restart connection

	ring_buffer_t send_buffer;
	ring_buffer_t receive_buffer;
	int (*send_fcn)(void* buf, size_t buf_len);
	int (*recv_fcn)(void* buf, size_t buf_len);

} ppp_custom_stream_t;

size_t ppp_custom_encode(const uint8_t* payload, size_t payload_len, uint8_t* out_buf, size_t out_buf_len, uint8_t sequence_counter, uint8_t control);

int8_t ppp_custom_decode(uint8_t* frame, size_t frame_len, uint8_t* decoded_buffer, ppp_custom_decoded_message_t* message);

int8_t ppp_custom_rx_stream_routine(ppp_custom_stream_t* stream);

void ppp_custom_test_encode_decode(void);

#ifdef __cplusplus
}
#endif

#endif // !PPP_CUSTOM_H

