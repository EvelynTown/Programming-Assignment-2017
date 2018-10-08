#include "cpu/instr.h"

static void instr_execute_2op(){
	operand_read(&opr_src);
	operand_read(&opr_dest);
	uint32_t temp = alu_and(opr_src.val, opr_dest.val);
	if (opr_src.data_size == 8 || opr_dest.data_size == 8)
		cpu.eflags.SF = (temp & 0x80) ? 1 : 0;
}

make_instr_impl_2op(test,r,rm,v)
make_instr_impl_2op(test,r,rm,b)
make_instr_impl_2op(test,i,rm,v)
make_instr_impl_2op(test,i,a,v)
make_instr_impl_2op(test,i,a,b)
make_instr_impl_2op(test,i,rm,b)
