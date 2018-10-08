#include "cpu/instr.h"

static void instr_execute_2op() {
	operand_read(&opr_src);
	operand_read(&opr_dest);
	opr_dest.val = alu_or(opr_src.val, opr_dest.val);
	operand_write(&opr_dest);
}

make_instr_impl_2op(or, r, rm, v)
make_instr_impl_2op(or, rm, r, b)
make_instr_impl_2op(or, rm, r, v)
make_instr_impl_2op(or, i, rm, b)
make_instr_impl_2op(or, i, rm, v)

make_instr_func(or_i2rm_bv) {
	OPERAND rm, imm;
	rm.data_size = data_size;
	imm.data_size = 8;

	int len = 1;
	len += modrm_rm(eip + 1, &rm);

	imm.type =  OPR_IMM;
	imm.addr = eip + len;
	operand_read(&imm);
	operand_read(&rm);
	imm.val = sign_ext(imm.val, 8);

	rm.val = alu_or(imm.val, rm.val);
	operand_write(&rm);
	return len + 1;
}

