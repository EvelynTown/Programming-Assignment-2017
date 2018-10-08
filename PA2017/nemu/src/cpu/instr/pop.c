#include "cpu/instr.h"

make_instr_func(pop) {
	OPERAND r, m;
	r.data_size = m.data_size = data_size;

	m.type = OPR_MEM;
	m.addr = cpu.esp;
	m.sreg = SREG_SS;
	r.type = OPR_REG;
	r.addr = opcode & 0x7;

	operand_read(&m);
	r.val = m.val;
	operand_write(&r);
	
	cpu.esp += data_size/8;
	return 1;
}

make_instr_func(popa) {
	cpu.edi = vaddr_read(cpu.esp, SREG_SS, 4);
	cpu.esp += 4;

	cpu.esi = vaddr_read(cpu.esp, SREG_SS, 4);
	cpu.esp += 4;

	cpu.ebp = vaddr_read(cpu.esp, SREG_SS, 4);
	cpu.esp += 4;
	
	//skip esp
	cpu.esp += 4;
	
	cpu.ebx = vaddr_read(cpu.esp, SREG_SS, 4);
	cpu.esp += 4;
	
	cpu.edx = vaddr_read(cpu.esp, SREG_SS, 4);
	cpu.esp += 4;
	
	cpu.ecx = vaddr_read(cpu.esp, SREG_SS, 4);
	cpu.esp += 4;
	
	cpu.eax = vaddr_read(cpu.esp, SREG_SS, 4);
	cpu.esp += 4;

	return 1;
}	
