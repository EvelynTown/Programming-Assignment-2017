#include "cpu/instr.h"

static void instr_execute_2op(){
	operand_read(&opr_src);
	operand_read(&opr_dest);
	opr_dest.val = alu_add(opr_src.val, opr_dest.val);
	operand_write(&opr_dest);
}

make_instr_impl_2op(add, i, rm, v)
make_instr_impl_2op(add, i, a, v)
make_instr_impl_2op(add, r, rm, v)
make_instr_impl_2op(add, rm, r, v)
make_instr_impl_2op(add, rm, r, b)

make_instr_func(add_i2rm_bv) {
	OPERAND imm, rm;

	rm.data_size = data_size;
	int len = 1;
	len += modrm_rm(eip + 1, &rm);
	
	operand_read(&rm);
	imm.addr = eip + len;
	imm.type = OPR_IMM;
	imm.data_size = 8;
	operand_read(&imm);
	
	imm.val = sign_ext(imm.val, 8);
	rm.val = alu_add(imm.val, rm.val);
	operand_write(&rm);

	return len + 1;
}
