#ifndef __RXTXBUFFER_H__
#define __RXTXBUFFER_H__

/**
 * @file rxtxbuffer.h
 * @brief Linear FIFO-style transmit/receive buffer for UART with DMA support.
 *
 * This structure implements a linear buffer for managing TX/RX data queues, where:
 * - `recved_size` marks how much valid data has been written to the buffer.
 * - `sent_size` marks how much of that data has already been consumed/transmitted.
 * - The unread portion lies between `sent_size` and `recved_size`.
 * - New data is written after `recved_size`, and the buffer can be compacted with `rxtxbuffer_shift_tx_buf()`.
 *
 * The layout of the buffer is as follows:
 *
 *     +---------------------------------------------------------+
 *     | Sent     |  To be transmitted  | Free space             |
 *     | [0:sent) |  [sent:recved)      | [recved:capacity)      |
 *     +---------------------------------------------------------+
 *      ^         ^                    ^                         ^
 *     data     sent_size         recved_size                capacity
 *
 * Example usage for UART TX with DMA:
 * - Write new data at `rxtxbuffer_rx_buf()`, increase with `rxtxbuffer_rx_increase_size()`.
 * - Start DMA from `rxtxbuffer_tx_buf()` of length `rxtxbuffer_tx_remaining()`.
 * - On completion, call `rxtxbuffer_tx_decrease_size()` and optionally `rxtxbuffer_shift_tx_buf()` to compact the buffer.
 *
 * This approach simplifies chunked DMA transmission without needing a true circular buffer.
 */


#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RXTXBUFFER_SUCCESS						0
#define RXTXBUFFER_ERROR_INVALID_PARAMETERS		1

#define RXTXBUFFER_DEFAULT(vec) vec.data=NULL; vec.capacity=(size_t)0; vec.sent_size=(size_t)0; vec.recved_size=(size_t)0

#define RXTXBUFFER_CAST(ptr, type) ((type)(ptr))

typedef struct rxtxbuffer {
    void* data;
    size_t capacity;
    size_t sent_size;
    size_t recved_size;
}rxtxbuffer;


void* rxtxbuffer_data(rxtxbuffer* pt);

size_t rxtxbuffer_sent_size(rxtxbuffer* pt);

size_t rxtxbuffer_recved_size(rxtxbuffer* pt);

size_t rxtxbuffer_capacity(rxtxbuffer* pt);

int rxtxbuffer_init(rxtxbuffer* _Cvector, void* buf, size_t _recved_size, size_t _sent_size, size_t _capacity);

// total ammount of data that is free to be received
size_t rxtxbuffer_rx_remaining(rxtxbuffer* sendbuf);

// total ammount of tx data that remains to be sent
size_t rxtxbuffer_tx_remaining(rxtxbuffer* sendbuf);

// decrease the ammount of the tx data that remains to be sent
void rxtxbuffer_tx_decrease_size(rxtxbuffer* pt, size_t sent_data_size);

// increase the ammount of the rx data that was received
void rxtxbuffer_rx_increase_size(rxtxbuffer* pt, size_t recved_data_size);

void* rxtxbuffer_tx_buf(rxtxbuffer* pt);

void* rxtxbuffer_rx_buf(rxtxbuffer* pt);

void rxtxbuffer_shift_tx_buf(rxtxbuffer* pt);


#ifdef __cplusplus
}
#endif

#endif // !__RXTXBUFFER_H__
