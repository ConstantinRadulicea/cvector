#include "cvector.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))


/***********************************************cstack*************************************************************************/
typedef cvector cstack;

// Utility function to initialize the stack
int cstack_init(cstack* _Stack, size_t _NumberOfElements, size_t _SizeOfElement) {
    return cvector_init((cvector*)_Stack, _NumberOfElements, _SizeOfElement);
}

// Returns the number of elements in the stack
size_t cstack_size(cstack* pt) {
    return cvector_size((cvector*)pt);
}

// Returns the maximum number of elements that the stack can hold.
size_t cstack_capacity(cstack* pt) {
    return cvector_capacity((cvector*)pt);
}

// Requests that the stack capacity be at least enough to contain n elements
int cstack_reserve(cstack* pt, size_t _NumOfElements) {
    return cvector_reserve((cvector*)pt, _NumOfElements);
}

// Resizes the container so that it contains ‘n’  dropping the newest elements in the stack.
int cstack_resize(cstack* pt, size_t _NewSize) {
    return cvector_resize((cvector*)pt, _NewSize);
}

// Reduces the capacity of the stack to fit its size and destroys all elements beyond the capacity.
int cstack_shrink_to_fit(cstack* pt) {
    return cvector_shrink_to_fit((cvector*)pt);
}

// It is used to pop all the elements from the stack
void cstack_clear(cstack* pt) {
    cvector_clear((cvector*)pt);
}

// Utility function to check if the stack is empty or not
int cstack_empty(cstack* pt) {
    return cvector_empty((cvector*)pt);
}

// Utility function to check if the stack is full or not
int cstack_full(cstack* pt) {
    return cvector_full((cvector*)pt);
}

// Utility function to return the top element of the stack
void* cstack_top(cstack* pt) {
    return cvector_back((cvector*)pt);
}

// Utility function to add an element `x` to the stack
int cstack_push(cstack* pt, const void* _Element) {
    return cvector_push_back((cvector*)pt, _Element);
}

// Utility function to pop a top element from the stack
void cstack_pop(cstack* pt) {
    cvector_pop_back((cvector*)pt);
}

// utility to free the memory allocated
void cstack_free(cstack* pt) {
    cvector_free((cvector*)pt);
}
