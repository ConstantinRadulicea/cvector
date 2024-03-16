#ifndef __SENDBUFFER_H__
#define __SENDBUFFER_H__

#include <stdint.h>

#define RXTXBUFFER_SUCCESS						0
#define RXTXBUFFER_ERROR_INVALID_PARAMETERS		1

#define RXTXBUFFER_DEFAULT(vec) vec.data=NULL; vec.capacity=(size_t)0; vec.sent_size=(size_t)0; vec.recved_size=(size_t)0

#define RXTXBUFFER_CAST(ptr, type) ((type)(ptr))

typedef struct rxtxbuffer {
    void* data;
    //size_t size;
    size_t capacity;
    size_t sent_size;
    size_t recved_size;
}rxtxbuffer;


void* rxtxbuffer_data(rxtxbuffer* pt);

size_t rxtxbuffer_sent_size(rxtxbuffer* pt);

size_t rxtxbuffer_recved_size(rxtxbuffer* pt);

size_t rxtxbuffer_capacity(rxtxbuffer* pt);

int rxtxbuffer_init(rxtxbuffer* _Cvector, void* buf, size_t _recved_size, size_t _sent_size, size_t _capacity);

size_t rxtxbuffer_rx_remaining(rxtxbuffer* sendbuf);

size_t rxtxbuffer_tx_remaining(rxtxbuffer* sendbuf);

void rxtxbuffer_tx_decrease_size(rxtxbuffer* pt, size_t sent_data_size);

void rxtxbuffer_rx_increase_size(rxtxbuffer* pt, size_t recved_data_size);

void* rxtxbuffer_tx_buf(rxtxbuffer* pt);

void* rxtxbuffer_rx_buf(rxtxbuffer* pt);




#endif // !__SENDBUFFER_H__
