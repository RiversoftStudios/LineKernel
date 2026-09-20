#ifndef __LINEMEMORY_H__
#define __LINEMEMORY_H__

#include <stdint.h>
#include <stddef.h>

void heap_init(void);
void* kmalloc(size_t size);
void kfree(void* ptr);

#endif
