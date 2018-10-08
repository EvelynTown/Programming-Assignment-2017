#include "memory/cache.h"

void init_cache(){
	/*for(int i = 0;i < CACHE_LINE; i++) 
		cache[i].v = false;*/
	for(int i = 0; i < N;i++){
		for(int j = 0; j < M; j++)
			cache[i].ca[j].valid = 0;
	}
}

/*uint32_t cache_read(paddr_t paddr, size_t len, CacheLine *cache){
	uint32_t tag = paddr >> 13;
	uint32_t group = (paddr & 0x1fc0) >> 6;
	uint32_t addr = paddr & 0x3f;
	uint32_t line_beg = group*8;
	uint32_t data = 0;
	uint32_t addr_backup = addr, paddr_backup = paddr;
	bool another_line = 0;
	addr = addr + len - 1;

	if(addr_backup / 0x40 != addr / 0x40) //read another line
		another_line = 1;

	//if hit = 1, read it
	for(int i = 0; i < 8; line_beg++, i++) {
		if(tag == cache[line_beg].tag && cache[line_beg].v == true) {
			if(!another_line) {
				for(int j = 0; j < len; j++, addr--) {
					data <<= 8;
			       	data |= cache[line_beg].data[addr];
				}
			}
			else {
				uint32_t data_1 = 0, data_2 = 0;
				for(int j = 63-addr_backup; j >= 0;j--) {
					data_1 <<= 8;
					data_1 |= cache[line_beg].data[addr_backup + j];
					paddr++;
				}
				data_2 = cache_read(paddr, len + addr_backup - 64, cache);
				data_2 <<= (64-addr_backup)*8;
				data = data_2 + data_1;
			}
			return data;
		}
	}

	//random
	srand((uint32_t)time(NULL));
	line_beg = group*8 + (rand()%8);
	cache[line_beg].v = true;
	cache[line_beg].tag = tag;
	paddr &= 0xffffffc0;
	for(int i = 0;i < 64; i++) {
		cache[line_beg].data[i] = hw_mem_read(paddr, 1);
		paddr++;
	}

	if(another_line) {
		uint32_t data_1 = 0, data_2 = 0;
		data_1 = cache_read(paddr_backup, 64-addr_backup, cache);
		data_2 = cache_read(paddr, len+addr_backup-64, cache);
		data_2 <<= (64-addr_backup)*8;
		data = data_2 + data_1;
	}
	else {	
		for(int j = 0; j < len; j++, addr--) {
			data <<= 8;
	        data |= cache[line_beg].data[addr];
		}
	}
	return data;
}*/

int cache_ReadA(uint8_t addr,size_t len,struct c1 *C,uint32_t *d){
	int j = 0;
	for(;j < len && (addr+j) < 64; j++){
		*d+=(C->data[addr+j] << (j*8));
	}
	return j;
}

uint32_t cache_ReadB(uint32_t paddr,int j,size_t len,uint32_t d);
uint32_t cache_ReadB(uint32_t paddr,int j,size_t len,uint32_t d){
	if(j < len){
		paddr = paddr+64;
		len = len-j;
		d += cache_read(paddr,len) << j*8;
		return d;
	}
	else
		return d;
}

uint32_t cache_read(paddr_t paddr, size_t len) {
	uint8_t group = (paddr >> 6)&0x7f;
	uint32_t tag = (paddr >> 13)&0x7ffff;
	uint8_t addr = paddr&0x3f;
	bool hit = 0;
	uint32_t d = 0;

	paddr = paddr-addr;
	for(int i = 0; i < M; i++){
		if(cache[group].ca[i].Tag == tag && cache[group].ca[i].valid == 1)	{
			hit = 1;
			int j = 0;
			j = cache_ReadA(addr,len,&(cache[group].ca[i]),&d);
			return cache_ReadB(paddr,j,len,d);
		}
	}

	if(hit == 0){
		int j = 0,k = 0;
		for(;k < M; k++){
			if(cache[group].ca[k].valid == 0){
				for(int i = 0; i < 64; i++)
					cache[group].ca[k].data[i] = hw_mem_read(paddr+i,1);
				cache[group].ca[k].valid = 1;
				cache[group].ca[k].Tag = tag;
				j = cache_ReadA(addr,len,&cache[group].ca[k],&d);
				break;
			}
		}
		if(k < M){
			return cache_ReadB(paddr,j,len,d);
		}
	

		/*full set*/
		int number;
		srand((uint32_t)time(NULL));
		number = rand()%8;

		for(int i = 0; i < 64; i++){
			cache[group].ca[number].data[i] = hw_mem_read(paddr+i,1);
		}		
		cache[group].ca[number].valid = 1;
		cache[group].ca[number].Tag = tag;
		int i = 0;
		i = cache_ReadA(addr,len,&cache[group].ca[number],&d);
		
		return cache_ReadB(paddr,i,len,d);
	}
	return 0;
}

/*void cache_write(paddr_t paddr, size_t len, uint32_t data, CacheLine *cache) {
	uint32_t tag = paddr >> 13;
	uint32_t group = (paddr & 0x1fc0) >> 6;
	uint32_t addr = paddr & 0x3f;
	uint32_t line_beg = group*8;
	uint32_t paddr_backup = paddr;
	bool another_line = 0;

	hw_mem_write(paddr_backup, len, data);

	if(addr/0x40 != (addr+len)/0x40)
		another_line = 1;

	//if hit = 1, write it
	for(int i = 0;i < 8; i++, line_beg++) {
		if(tag == cache[line_beg].tag && cache[line_beg].v == true) {
			if(!another_line) {
				for(int j = 0; j < len; j++) {
					cache[line_beg].data[addr] = data & 0xff;
					data >>= 8;
					addr++;
				}	
			}
			else {
				for(int j = 0; j < 64-addr; j++) {
					cache[line_beg].data[addr+j] = data & 0xff;
					data >>= 8;
					paddr++;
				}
				cache_write(paddr, len+addr-64, data, cache);
			}
			break;
		}
	}
}*/

void cache_WriteA(uint32_t tag,uint8_t addr,size_t len,uint32_t data,struct CacheLine *c);
void cache_WriteA(uint32_t tag,uint8_t addr,size_t len,uint32_t data,struct CacheLine *C) {
	for(int i = 0; i < M; i++){
		if(C->ca[i].Tag == tag){
			for(int j = 0; j < len&&(addr+j)<64;j++)
				C->ca[i].data[addr+j] = ((data >> (j*8))&0xff);
			break;
		}
	}
}

void cache_write(paddr_t paddr,size_t len,uint32_t data){
	uint8_t group = (paddr >> 6)&0x7f;
	uint32_t tag = (paddr >> 13)&0x7ffff;
	uint8_t addr = paddr&0x3f;
	
	hw_mem_write(paddr,len,data);

	cache_WriteA(tag,addr,len,data,&cache[group]);

	paddr = paddr+64-addr;
	tag = (paddr >> 13)&0x7ffff;
	group = (paddr >> 6)&0x7f;
	
	int temp = addr+len -64;
	if(temp > 0) {
		data = (data >> (8*(len-temp)));
		cache_WriteA(tag,0,temp,data,&cache[group]);
	}
}
