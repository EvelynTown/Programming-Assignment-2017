#ifndef _CACHE_H_
#define _CACHE_H_

#include "nemu.h"
#include "memory/memory.h"
#include "cpu/cpu.h"
#include <stdio.h>
#include "device/mm_io.h"
#include "time.h"
#include "stdlib.h"

//#define CACHE_BLOCK 64
//#define CACHE_LINE 1024
#define N 128
#define M 8

/*typedef struct{
	bool v;
	uint32_t tag;
	uint8_t data[CACHE_BLOCK];
} CacheLine;*/

struct c1{
	bool valid;
	uint8_t data[64];
	uint32_t Tag;
};

struct CacheLine{
	struct c1 ca[8];
};

//CacheLine cache[CACHE_LINE];
struct CacheLine cache[N];

//zero valid_bit
void init_cache();

//read the cache
//uint32_t cache_read(paddr_t paddr, size_t len, CacheLine *cache);
int cache_ReadA(uint8_t addr, size_t len, struct c1* c, uint32_t *d);
uint32_t cache_read(paddr_t paddr, size_t len);

//write the cache
//void cache_write(paddr_t oaddr, size_t len, uint32_t data, CacheLine *cache);
void cache_write(paddr_t paddr, size_t len, uint32_t data);
#endif
