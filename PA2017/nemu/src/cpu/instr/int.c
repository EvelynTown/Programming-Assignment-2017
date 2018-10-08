#include "cpu/instr.h"
#include "cpu/intr.h"

make_instr_func(INT) {
	OPERAND imm;
	imm.data_size = data_size;
	imm.type = OPR_IMM;
	imm.addr = eip + 1;
	operand_read(&imm);
	raise_sw_intr(imm.val);
	return 0;
}

