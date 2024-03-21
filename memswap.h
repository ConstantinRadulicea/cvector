#ifndef __MEMSWAP_H__
#define __MEMSWAP_H__

#include <stddef.h>

// swap 2 blocks of memory
void memswap(register void* _a, register void* _b, register size_t _size);

#endif
