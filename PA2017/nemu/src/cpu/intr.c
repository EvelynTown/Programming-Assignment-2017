#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"


void raise_intr(uint8_t intr_no) {
#ifdef IA32_INTR
	//printf("Please implement raise_intr()");
	//assert(0);
	
	//push eflags
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.eflags.val);

	//push CS
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.cs.val);

	//push eip
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.eip);
	
	uint32_t addr_gate = cpu.idtr.base + sizeof(GateDesc) * intr_no;
	GateDesc gateDesc;
	gateDesc.val[0] = laddr_read(addr_gate, 4);
	gateDesc.val[1] = laddr_read(addr_gate + 4, 4);

	//clear the IF if it's an interrupt
	if(gateDesc.type == 14)
		cpu.eflags.IF = 0;

	cpu.cs.val = gateDesc.selector;
	cpu.eip = (gateDesc.offset_31_16 << 16) + gateDesc.offset_15_0;
	load_sreg(SREG_CS);
#endif
}

void raise_sw_intr(uint8_t intr_no) {
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
