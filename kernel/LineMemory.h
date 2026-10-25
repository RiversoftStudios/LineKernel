#ifndef __LINEMEMORY_H__
#define __LINEMEMORY_H__

#include <stdint.h>
#include <stddef.h>
#include "compilers.h"

void heap_init(void);
void kfree(void* ptr);
void* kmalloc(size_t size) ATTRIBUTE_MALLOC(kfree, 1);

#endif
