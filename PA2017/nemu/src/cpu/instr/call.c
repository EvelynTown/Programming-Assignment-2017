#include "cpu/instr.h"

make_instr_func(call_r_v) {
	OPERAND imm,rm;
	cpu.esp -= 4;
	imm.addr = eip + 1;
	imm.type = OPR_IMM;
	imm.data_size = data_size;
	imm.sreg = SREG_CS;

	operand_read(&imm);
	rm.addr = cpu.esp;
	rm.val = cpu.eip + 1 + data_size / 8;
	rm.type = OPR_MEM;
	rm.sreg = SREG_SS;
	rm.data_size = data_size;
	operand_write(&rm);

	cpu.eip += (imm.val + 1 + data_size/8);

	return 0;
}

make_instr_func(call_rm_v) {
	OPERAND r,rm;
	
	cpu.esp -= 4;
	r.data_size = data_size;
	int len = 1;
	len += modrm_rm(eip + 1, &r);
	operand_read(&r);

	rm.addr = cpu.esp;
	rm.val = cpu.eip + len;
	rm.type = OPR_MEM;
	rm.data_size = data_size;
	rm.sreg = SREG_SS;
	operand_write(&rm);

	cpu.eip = r.val;

	return 0;
}
