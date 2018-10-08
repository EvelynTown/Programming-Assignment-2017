#include "cpu/instr.h"

make_instr_func(push){
	OPERAND r,rm;
	
	cpu.esp -= 4;

	rm.data_size = r.data_size = data_size;
	rm.type = OPR_MEM;
	rm.addr = cpu.esp;
	rm.sreg = SREG_SS;

	r.type = OPR_REG;
	r.addr = opcode & 0x7;
	r.sreg = SREG_CS;
	operand_read(&r);
	rm.val = r.val;
	operand_write(&rm);

	return 1;
}

make_instr_func(push_m_v) {
	OPERAND r,rm;
	
	cpu.esp -= 4;

	rm.data_size = r.data_size = data_size;
	int len = 1;
	len += modrm_rm(eip + 1, &r);
	rm.type = OPR_MEM;
	rm.addr = cpu.esp;
	rm.sreg = SREG_SS;

	operand_read(&r);
	rm.val = r.val;
	operand_write(&rm);

	return len;
}

make_instr_func(push_i_b) {
	OPERAND imm,rm;
	
	cpu.esp -= 4;

	rm.data_size = data_size;
	imm.type = OPR_IMM;
	imm.data_size = 8;
	imm.addr = eip + 1;

	rm.type = OPR_MEM;
	rm.addr = cpu.esp;

	operand_read(&imm);
	rm.val = imm.val;
	rm.sreg = SREG_SS;
	operand_write(&rm);

	return 2;
}

make_instr_func(push_i_v) {
	OPERAND imm,rm;
	
	cpu.esp -= 4;

	rm.data_size = data_size;
	imm.type = OPR_IMM;
	imm.data_size = data_size;
	imm.addr = eip + 1;

	rm.type = OPR_MEM;
	rm.addr = cpu.esp;

	operand_read(&imm);
	rm.val = imm.val;
	rm.sreg = SREG_SS;
	operand_write(&rm);

	return 1 + data_size/8;
}

make_instr_func(pusha) {
	uint32_t temp = cpu.esp;

	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.eax);

	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.ecx);

	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.edx);
	
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.ebx);
	
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, temp);
	
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.ebp);
	
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.esi);

	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.edi);

	return 1;
}

make_instr_func(push_esp) {
	uint32_t temp = cpu.esp;
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, temp);
	return 1;
}
