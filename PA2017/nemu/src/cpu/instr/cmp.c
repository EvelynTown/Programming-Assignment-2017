#include "cpu/instr.h"

static void instr_execute_2op(){
	operand_read(&opr_src);
	operand_read(&opr_dest);
	alu_sub(opr_src.val,opr_dest.val);
}

make_instr_impl_2op(cmp,i,rm,b)
make_instr_impl_2op(cmp,i,rm,v)
make_instr_impl_2op(cmp,i,a,v)
make_instr_impl_2op(cmp,i,a,b)
make_instr_impl_2op(cmp,r,rm,v)
make_instr_impl_2op(cmp,r,rm,b)
make_instr_impl_2op(cmp,rm,r,v)
make_instr_impl_2op(cmp,rm,r,b)

make_instr_func(cmp_i2rm_bv) {
	OPERAND rm,imm;

	rm.data_size = data_size;
	int len = 1;
	len += modrm_rm(eip+1,&rm);

	imm.type = OPR_IMM;
	imm.addr = eip + len;
	imm.data_size = 8;
	
	operand_read(&rm);
	operand_read(&imm);
	imm.val = sign_ext(imm.val,8);
	alu_sub(imm.val,rm.val);
	return len + 1;
}
