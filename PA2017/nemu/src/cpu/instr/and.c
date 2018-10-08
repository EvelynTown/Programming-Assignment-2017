#include "cpu/instr.h"

static void instr_execute_2op() {
	operand_read(&opr_src);
	operand_read(&opr_dest);
	opr_dest.val = alu_and(opr_src.val, opr_dest.val);
	operand_write(&opr_dest);
}

make_instr_impl_2op(and,rm,r,b)
make_instr_impl_2op(and,rm,r,v)
make_instr_impl_2op(and,r,rm,v)
make_instr_impl_2op(and,i,rm,v)
make_instr_impl_2op(and,i,rm,b)

make_instr_func(and_i2rm_bv) {
	OPERAND rm,imm;
	rm.data_size = data_size;
	imm.data_size = 8;

	int len = 1;
	len += modrm_rm(eip + 1, &rm);
	
	imm.type = OPR_IMM;
	imm.addr = eip + len;
	operand_read(&imm);
	operand_read(&rm);
	imm.val = sign_ext(imm.val, 8);

	rm.val = alu_and(imm.val, rm.val);
	operand_write(&rm);

	return len + 1;
}

make_instr_func(and_i2a_v) {
	OPERAND r,imm;
	r.data_size = imm.data_size = data_size;
	r.type = OPR_REG;
	r.addr = REG_AX;

	imm.type = OPR_IMM;
	imm.addr = eip + 1;
	operand_read(&imm);
	operand_read(&r);

	r.val = alu_and(imm.val, r.val);
	operand_write(&r);

	return 1 + data_size/8;
}

