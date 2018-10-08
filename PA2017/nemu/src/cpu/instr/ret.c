#include "cpu/instr.h"

make_instr_func(ret_near) {
	OPERAND rm;
	
	rm.addr = cpu.esp;
	rm.type = OPR_MEM;
	rm.sreg = SREG_SS;
	rm.data_size = data_size;

	operand_read(&rm);
	cpu.eip = rm.val;

	cpu.esp += 4;
	return 0;
}

make_instr_func(ret_far) {
	OPERAND imm, rm;

	rm.addr = cpu.esp;
	rm.type = OPR_MEM;
	rm.data_size = data_size;
	rm.sreg = SREG_SS;

	imm.data_size = 16;
	imm.addr = eip + 1;
	imm.type = OPR_IMM;
	imm.sreg = SREG_CS;
	operand_read(&imm);

	operand_read(&rm);
	cpu.eip = rm.val;

	
	cpu.esp += (imm.val+4);
	return 0;
}

