#include "cpu/instr.h"

static void instr_execute_2op() {
	operand_read(&opr_src);
	operand_read(&opr_dest);
	opr_dest.val = alu_adc(opr_dest.val, opr_src.val);
	operand_write(&opr_dest);
}

make_instr_impl_2op(adc, r, rm, v)

make_instr_func(adc_i2rm_bv) {
	OPERAND rm, imm;

	rm.data_size = data_size;
	imm.data_size = 8;

	int len = 1;
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);

	imm.type = OPR_IMM;
	imm.addr = eip + len;
	operand_read(&imm);
	
	imm.val = sign_ext(imm.val, 8);
	rm.val = alu_adc(imm.val, rm.val);
	operand_write(&rm);
	
	return len + 1;
}	
