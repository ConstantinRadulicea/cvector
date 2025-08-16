#include "rxtxbuffer.h"
#include <memory.h>


#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

// Utility function to initialize the rxtxbuffer_t
int rxtxbuffer_init(rxtxbuffer_t* _Cvector, void* buf, size_t _capacity) {
	if (_Cvector == NULL) {
		return RXTXBUFFER_ERROR_INVALID_PARAMETERS;
	}

	if (buf == NULL && _capacity != (size_t)0) {
		return RXTXBUFFER_ERROR_INVALID_PARAMETERS;
	}


	_Cvector->buffer = buf;
	_Cvector->data_size = 0;
	_Cvector->sent_size = 0;
	_Cvector->capacity = _capacity;

	return RXTXBUFFER_SUCCESS;
}

void* rxtxbuffer_buffer(rxtxbuffer_t* pt) {
	return pt->buffer;
}

size_t rxtxbuffer_sent_size(rxtxbuffer_t* pt) {
	return pt->sent_size;
}

size_t rxtxbuffer_data_size(rxtxbuffer_t* pt) {
	return pt->data_size;
}

size_t rxtxbuffer_capacity(rxtxbuffer_t* pt) {
	return pt->capacity;
}

size_t rxtxbuffer_free_space(rxtxbuffer_t* sendbuf) {
	return sendbuf->capacity - sendbuf->data_size;
}

size_t rxtxbuffer_data_remaining(rxtxbuffer_t* sendbuf) {
	return sendbuf->data_size - sendbuf->sent_size;
}

void rxtxbuffer_sent_data_increase_size(rxtxbuffer_t* pt, size_t sent_data_size) {
	pt->sent_size = MIN(MIN(pt->data_size, pt->sent_size + sent_data_size), pt->capacity);
}

void rxtxbuffer_data_increase_size(rxtxbuffer_t* pt, size_t recved_data_size) {
	pt->data_size = MIN(pt->data_size + recved_data_size, pt->capacity);
}

void rxtxbuffer_data_decrease_size(rxtxbuffer_t* pt, size_t decreased_size) {
	if (decreased_size >= pt->data_size) {
		pt->data_size = 0;
		pt->sent_size = 0;
	}
	else
	{
		pt->data_size -= decreased_size;
		if (pt->sent_size > pt->data_size) {
			pt->sent_size = pt->data_size; // Ensure sent size does not exceed data size
		}
	}
}

void rxtxbuffer_sent_data_decrease_size(rxtxbuffer_t* pt, size_t decreased_size) {
	if (decreased_size >= pt->data_size) {
		pt->sent_size = 0;
	}
	else {
		pt->sent_size -= decreased_size;
	}
}

void* rxtxbuffer_data_ptr(rxtxbuffer_t* pt) {
	return ((char*)(pt->buffer)) + pt->sent_size;
}

void* rxtxbuffer_free_space_ptr(rxtxbuffer_t* pt) {
	return ((char*)(pt->buffer)) + pt->data_size;
}

void rxtxbuffer_shift_data_buf(rxtxbuffer_t* pt) {
	memmove(rxtxbuffer_buffer(pt), rxtxbuffer_data_ptr(pt), rxtxbuffer_data_remaining(pt));
	pt->data_size = rxtxbuffer_data_remaining(pt);
	pt->sent_size = (size_t)0;
}
