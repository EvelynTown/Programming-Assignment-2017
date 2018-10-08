#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "memory/cache.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

uint8_t hw_mem[MEM_SIZE_B];

uint32_t hw_mem_read(paddr_t paddr, size_t len) {
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data) {
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len) {
	uint32_t ret = 0;
	int map_NO = is_mmio(paddr);
	if(map_NO == -1) {
		//CacheLine *L1_dcache = cache;
		#ifdef CACHE_ENABLED
			ret = cache_read(paddr, len);
				//cache_read(paddr, len, L1_dcache);
		#else
			ret = hw_mem_read(paddr, len);
		#endif
	}
	else {
		ret = mmio_read(paddr, len, map_NO);
	}
		return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data) {
	int map_NO = is_mmio(paddr);
	if(map_NO == -1) {
		//CacheLine *L1_dcache = cache;
		#ifdef CACHE_ENABLED
			//cache_write(paddr, len, data, L1_dcache);
			cache_write(paddr, len, data);
		#else
			hw_mem_write(paddr, len, data);
		#endif
	}
	else {
		mmio_write(paddr, len, data, map_NO);
	}
}


uint32_t laddr_read(laddr_t laddr, size_t len) {
	assert(len == 1 || len == 2 || len ==4 );
	if(cpu.cr0.pg == 1 && cpu.cr0.pe == 1) {
		//data cross the page boundary
		if((laddr>>12) != ((laddr+len-1)>>12)){
			size_t temp = 0;
			while(((temp+laddr)%(0x1000) != 0xfff) && temp <= len)
				temp++;
			temp++;
			uint32_t hwaddr_1 = page_translate(laddr);
			uint32_t hwaddr_2 = page_translate(laddr+temp);
			uint32_t addr = (paddr_read(hwaddr_1, temp)) + (paddr_read(hwaddr_2, len-temp) << (temp * 8));
			return addr;
		}
		else {
			uint32_t hwaddr = page_translate(laddr);
			return paddr_read(hwaddr, len);
		}
	}
	else
		return paddr_read(laddr, len);
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data) {
	assert(len == 1 || len == 2 || len ==4 );
	if(cpu.cr0.pg == 1 && cpu.cr0.pe == 1) {
		//data cross the page boundary
		if((laddr>>12) != ((laddr+len-1)>>12)){
			//printf("write_twice laddr %x, len %x\n", laddr, len);
			size_t temp = 0;
			while(((temp+laddr)%(0x1000) != 0xfff) && temp <= len)
				temp++;
			temp++;
			uint32_t hwaddr_1 = page_translate(laddr);
			uint32_t hwaddr_2 = page_translate(laddr+temp);
			uint32_t data_2 = data >> (temp * 8);
			uint32_t data_1 = data & (~((~0x0) << (temp * 8)));
			paddr_write(hwaddr_1, temp, data_1);
			paddr_write(hwaddr_2, len-temp, data_2);
		}
		else {
			uint32_t hwaddr = page_translate(laddr);
			paddr_write(hwaddr, len, data);
		}
	}
	else
		paddr_write(laddr, len, data);
}


uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	return laddr_read(vaddr, len);
#else
	uint32_t laddr = vaddr;
	if(cpu.cr0.pe == 1)
		laddr = segment_translate(vaddr, sreg);
	return laddr_read(laddr, len);
#endif
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data) {
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	laddr_write(vaddr, len, data);
#else
	uint32_t laddr = vaddr;
	if(cpu.cr0.pe == 1)
		laddr = segment_translate(vaddr, sreg);
	laddr_write(laddr, len, data);
#endif
}

void init_mem() {
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);
#ifdef CACHE_ENABLED
	init_cache();
#endif

#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t * get_mem_addr() {
	return hw_mem;
}
