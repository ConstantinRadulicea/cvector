#ifndef PPP_CUSTOM_H
#define PPP_CUSTOM_H

/*
 * =============================================================================
 *                           PPP FRAME STRUCTURE (Standard)
 * =============================================================================
 *
 * | Flag | Address | Control | Protocol | Information (Payload) | CRC-16 | Flag |
 * |------|---------|---------|----------|------------------------|--------|------|
 * |0x7E  |  0xFF   |  0x03   |  2 bytes |   0 to N bytes         | 2 bytes|0x7E  |
 *
 * Field Breakdown:
 * -----------------------------------------------------------------------------
 * - FLAG (1 byte)          : 0x7E
 *   - Marks the beginning and end of a PPP frame.
 *
 * - ADDRESS (1 byte)       : 0xFF (Broadcast address)
 *   - Always set to 0xFF in PPP. Required unless ACFC is negotiated.
 *
 * - CONTROL (1 byte)       : 0x03 (Unnumbered Information - UI frame)
 *   - Always set to 0x03 in standard PPP. Required unless ACFC is negotiated.
 *
 * - PROTOCOL (2 bytes)     : e.g., 0x0021 (IPv4), 0xC021 (LCP)
 *   - Indicates the protocol type of the payload.
 *
 * - INFORMATION (0 to N bytes):
 *   - The payload (e.g., IP packet, authentication data, etc.).
 *
 * - CRC-16 (2 bytes)       : FCS (Frame Check Sequence)
 *   - CRC-16-CCITT (poly 0x1021, init 0xFFFF).
 *   - Used for error detection.
 *
 * - FLAG (1 byte)          : 0x7E
 *   - Marks the end of the PPP frame.
 *
 * =============================================================================
 * Notes:
 * - All bytes between flags are escaped if they equal 0x7E or 0x7D.
 * - The Address and Control fields can be compressed (ACFC) if negotiated.
 * - PPP is byte-oriented and designed for use over serial links like UART.
 * =============================================================================
 */

#include <stdint.h>
#include <stddef.h>
#include "ringbuffer.h"
#include "rxtxbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PPP_TCP_WIDNOWS_DEBUG 1

#define PPP_FLAG       0x7E
#define PPP_CTRL       0x03
#define PPP_ESCAPE     0x7D
#define PPP_ESC_MASK   0x20
#define PPP_DEFAULT_ADDRESS    0xFF
#define PPP_DEFAULT_CONTROL    0x03

#define PPP_SEQUENCE_ADDRESS_BYTE_SIZE 1
#define PPP_CONTROL_BYTE_SIZE 1
#define PPP_PROTOCOL_BYTE_SIZE 2
#define PPP_CRC_BYTE_SIZE 2
#define PPP_START_FLAG_SIZE 1
#define PPP_HEADER_SIZE (PPP_SEQUENCE_ADDRESS_BYTE_SIZE + PPP_CONTROL_BYTE_SIZE + PPP_PROTOCOL_BYTE_SIZE)
#define PPP_PROTOCOL_OVERHEAD_BYTE_SIZE (PPP_HEADER_SIZE + PPP_CRC_BYTE_SIZE + (PPP_START_FLAG_SIZE * 2))

#define PPP_ENCODED_FRAME_MAX_LEN(payload_size) ((payload_size * 2) + ((PPP_HEADER_SIZE + PPP_CRC_BYTE_SIZE + PPP_START_FLAG_SIZE) * 2))

#define PPP_SUCCESS 0
#define PPP_ERR_OUTPUT_BUFFER_TOO_SMALL 1
#define PPP_ERR_FRAME_TOO_SMALL 2
#define PPP_ERR_INVALID_CRC 3

#define PPP_STATUS_PENDING 4
#define PPP_ERR_MTU_EXCEDEED 5
#define PPP_ERR_ALREADY_A_FRAME_IN_QUEUE 6
#define PPP_ERR_INVALID_PARAMETERS 7
#define PPP_ERR_SENDING_OPERATION_ERROR 8
#define PPP_ERR_STREAM_UNINITIALIZED 9
#define PPP_ERR_MESSAGE_TOO_SHORT 10
#define PPP_ERR_BUFFER_TOO_SMALL 11
#define TCP_ERR_BUFFER_TOO_SMALL 12

#define TCP_SEQUENCE_NUMBER_TYPE uint16_t
#define TCP_ACK_NUMBER_TYPE uint16_t
#define TCP_SEQUENCE_NUMBER_SIZE sizeof(TCP_SEQUENCE_NUMBER_TYPE)
#define TCP_ACK_NUMBER_SIZE sizeof(TCP_ACK_NUMBER_TYPE)
#define TCP_HEADER_SIZE (TCP_SEQUENCE_NUMBER_SIZE + TCP_ACK_NUMBER_SIZE)
#define TCP_PROTOCOL_OVERHEAD_BYTE_SIZE TCP_HEADER_SIZE

#define TCP_MAX_PACKET_SIZE(payload_size) (payload_size + TCP_PROTOCOL_OVERHEAD_BYTE_SIZE)
#define TCP_PPP_ENCODED_FRAME_MAX_LEN(payload_size) PPP_ENCODED_FRAME_MAX_LEN(TCP_MAX_PACKET_SIZE(payload_size))

#define TCP_PPP_PROTOCOL 0x0021 // PPP Protocol for TCP packets
#define TCP_ACK_SEND_TIMEOUT_MS 0
#define TCP_CONNECTION_TIMEOUT_MS 2000 // 10 seconds

struct ppp_stream;

typedef struct {
	uint8_t address;
	uint8_t control;
	uint16_t protocol;
	uint8_t* payload;
	uint16_t CRC;
	uint8_t* frame;
	uint16_t frame_size;
	uint16_t payload_size;
} ppp_decoded_frame_t;

typedef struct {
	uint8_t address;
	uint8_t control;
	uint16_t protocol;
	uint8_t* frame;
	uint16_t frame_size;
	uint16_t payload_size;
} ppp_encoded_frame_t;

int8_t ppp_encode(
	const uint8_t* payload,
	size_t payload_len,
	uint8_t* out_buf,
	size_t out_buf_len,
	size_t* frame_size,
	uint8_t address,
	uint8_t control,
	uint16_t protocol);

int8_t ppp_decode(uint8_t* frame, size_t frame_len, uint8_t* decoded_buffer, ppp_decoded_frame_t* message);


/*========================================================================================================================*/


typedef enum {
	PPP_RX_FRAME_PARSING_STATE_WAIT_FOR_START_FLAG = 0,
	PPP_RX_FRAME_PARSING_STATE_RECEIVING_BODY,
	PPP_RX_FRAME_PARSING_STATE_VALID_FRAME
} ppp_rx_frame_parsing_state_t;

typedef enum {
	PPP_TX_FRAME_PARSING_STATE_NOTHING = 0,
	PPP_TX_FRAME_PARSING_STATE_SENDING_COMPLETED,
	PPP_TX_FRAME_PARSING_STATE_SENDING,
	PPP_TX_FRAME_PARSING_STATE_ERROR
} ppp_tx_frame_parsing_state_t;

typedef enum {
	PPP_STREAM_STATUS_OK = 0,
	PPP_STREAM_STATUS_CONNECTION_TIMEOUT,
	PPP_STREAM_STATUS_CONNECTION_ERROR
} ppp_stream_status_t;


typedef void (*ppp_frame_rx_callback_t)(struct ppp_stream* stream, ppp_decoded_frame_t* message, void* context);
typedef void (*ppp_frame_tx_callback_t)(struct ppp_stream* stream, ppp_encoded_frame_t* message, void* context);
typedef void (*ppp_stream_status_callback_t)(struct ppp_stream* stream, ppp_stream_status_t status, void* context);
typedef size_t (*ppp_get_system_millis)();

typedef int (*ppp_send_buffer_fcn_t)(void* buffer, int buffer_size, void* context);
typedef int (*ppp_recv_buffer_fcn_t)(void* buffer, int buffer_size, void* context);

typedef struct ppp_stream {
	rxtxbuffer_t rx_buffer;
	rxtxbuffer_t tx_buffer;
	uint16_t buffer_capacity;
	uint16_t MTU_size;
	ppp_decoded_frame_t rx_message;
	ppp_encoded_frame_t tx_message;
	ppp_rx_frame_parsing_state_t rx_state;
	ppp_tx_frame_parsing_state_t tx_state;
	ppp_frame_rx_callback_t packet_rx_callback;
	ppp_frame_tx_callback_t packet_tx_callback;
	ppp_get_system_millis get_system_millis;
	void* packet_rx_callback_context;
	void* packet_tx_callback_context;
	size_t last_sent_frame_timestamp_ms;
	size_t last_recv_frame_timestamp_ms;
	ppp_stream_status_t stream_status;
	ppp_stream_status_callback_t stream_status_callback;
	void* stream_status_callback_context;
	ppp_send_buffer_fcn_t send_buffer_fcn;
	void* send_buffer_fcn_context;
	ppp_recv_buffer_fcn_t recv_buffer_fcn;
	void* recv_buffer_fcn_context;
}ppp_stream_t;



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
);

// unencoded message is a structure and contains a pointer to a payload buffer stored by the user that will be encoded in the internal frame rx_buffer
int8_t ppp_stream_send_frame(ppp_stream_t* stream, const ppp_decoded_frame_t message);

int8_t ppp_stream_parse_byte(ppp_stream_t* stream, uint8_t byte);

int8_t ppp_stream_send_routine(ppp_stream_t* stream, ppp_send_buffer_fcn_t send_fcn, void* context);

void ppp_stream_drop_recv_frame(ppp_stream_t* stream);

void ppp_stream_drop_send_frame(ppp_stream_t* stream);

void ppp_stream_tick(ppp_stream_t* stream);


/*========================================================================================================================*/

#define TCP_CONNECTION_STATE_CLOSED ((uint8_t)0)
#define TCP_CONNECTION_STATE_DISCONNECTED ((uint8_t)1)
#define TCP_CONNECTION_STATE_ESTABLISHED ((uint8_t)2)

typedef struct {
	uint16_t sequence_number;
	uint16_t ack_number;
	uint8_t* payload;
	uint8_t* packet;
	uint16_t packet_size;
	uint16_t payload_size;
} ppp_decoded_packet_t;

typedef struct {
	ppp_stream_t stream;
	uint8_t* send_packet_buffer;
	size_t send_packet_buffer_capacity;
	TCP_SEQUENCE_NUMBER_TYPE send_base;     // first unacked byte we've sent
	TCP_SEQUENCE_NUMBER_TYPE sequence_number;
	TCP_ACK_NUMBER_TYPE ack_number;
	TCP_ACK_NUMBER_TYPE last_ack_number_sent;
	size_t last_sent_packet_timestamp_ms;
	size_t last_recv_packet_timestamp_ms;
	size_t keep_alive_interval_ms; // 0 = disabled
	ring_buffer_t recv_buffer;
	ring_buffer_t send_buffer;
	uint8_t connection_state;
} tcp_t;


/*
mtu -> Maximum Transmission Unit, the maximum size of payload that can fin in a packet.
rx_ring_mem -> size > 1
tx_ring_mem -> size > 1
send_pkt_buf -> size = TCP_MAX_PACKET_SIZE(mtu)
ppp_recv_buffer -> size = TCP_PPP_ENCODED_FRAME_MAX_LEN(mtu)
ppp_send_buffer -> size = TCP_PPP_ENCODED_FRAME_MAX_LEN(mtu)
*/
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
	ppp_get_system_millis get_ms);

// copy the provided buffer to the send buffer and returns the copyed size
int tcp_send(tcp_t *tcp, uint8_t* buffer, int buffer_size);

// copy the used space from the receive buffer to the provided buffer and returns the copyed size
int tcp_recv(tcp_t* tcp, uint8_t* buffer, int buffer_size);

// Returns the free space in the send buffer
int tcp_available_to_send(tcp_t* tcp);

// Returns the used space in the receive buffer
int tcp_available_to_recv(tcp_t* tcp);

uint8_t tcp_get_connection_state(tcp_t* tcp);


void tcp_tick(tcp_t* tcp);



#if PPP_TCP_WIDNOWS_DEBUG != 0

void ppp_test_encode_decode(void);
void ppp_stream_test(void);
int tcp_test_1(void);

#endif // PPP_TCP_WIDNOWS_DEBUG != 0




#ifdef __cplusplus
}
#endif

#endif // !PPP_CUSTOM_H

