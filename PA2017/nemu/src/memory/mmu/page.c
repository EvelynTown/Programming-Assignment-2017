#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr) {
#ifndef TLB_ENABLED
	uint32_t dir = (laddr>>22)&0x3ff;
	uint32_t page = (laddr>>12)&0x3ff;
	uint32_t offset = laddr&0xfff;
	PDE pde;
	PTE pte;
	pde.val = paddr_read((cpu.cr3.pdbr<<12)+dir*sizeof(pde), 4);
	pte.val = paddr_read((pde.page_frame<<12)+page*sizeof(pte), 4);
	
	if(!(pde.present && pte.present))
		printf("laddr %x\n", laddr);
	assert(pde.present && pte.present);
	return offset+(pte.page_frame<<12);
#else	
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
