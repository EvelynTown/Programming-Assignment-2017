#include "cpu/instr.h"

static void instr_execute_1op_cc_pass() {
	operand_read(&opr_src);
	cpu.eip += sign_ext(opr_src.val, opr_src.data_size);
}

static void instr_execute_1op_cc_fail() {/* Do nothing */}

static void instr_execute_2op_cc_pass() {
	operand_read(&opr_src);
	cpu.eip = alu_add(cpu.eip, opr_src.val);
}

static void instr_execute_2op_cc_fail() {/* Do nothing */}


#define jcc_helper(suffix) \
	make_instr_impl_1op_cc(j, i, suffix, e) \
	make_instr_impl_1op_cc(j, i, suffix, a) \
	make_instr_impl_1op_cc(j, i, suffix, b) \
	make_instr_impl_1op_cc(j, i, suffix, g) \
	make_instr_impl_1op_cc(j, i, suffix, l) \
	make_instr_impl_1op_cc(j, i, suffix, o) \
	make_instr_impl_1op_cc(j, i, suffix, p) \
	make_instr_impl_1op_cc(j, i, suffix, s) \
	make_instr_impl_1op_cc(j, i, suffix, ae) \
	make_instr_impl_1op_cc(j, i, suffix, be) \
	make_instr_impl_1op_cc(j, i, suffix, ge) \
	make_instr_impl_1op_cc(j, i, suffix, le) \
	make_instr_impl_1op_cc(j, i, suffix, na) \
	make_instr_impl_1op_cc(j, i, suffix, ne) \
	make_instr_impl_1op_cc(j, i, suffix, no) \
	make_instr_impl_1op_cc(j, i, suffix, np) \
	make_instr_impl_1op_cc(j, i, suffix, ns) \
	make_instr_impl_2op_cc(j, i, r, suffix, be) \
	make_instr_impl_2op_cc(j, i, r, suffix, le) \
	make_instr_impl_2op_cc(j, i, r, suffix, l) \
	make_instr_impl_2op_cc(j, i, r, suffix, e) \
	make_instr_impl_2op_cc(j, i, r, suffix, a) \
	make_instr_impl_2op_cc(j, i, r, suffix, s) \
	make_instr_impl_2op_cc(j, i, r, suffix, ne) \
	make_instr_impl_2op_cc(j, i, r, suffix, g) \
	make_instr_impl_2op_cc(j, i, r, suffix, ae) \
	make_instr_impl_2op_cc(j, i, r, suffix, ns) \
	make_instr_impl_2op_cc(j, i, r, suffix, b) \
	make_instr_impl_2op_cc(j, i, r, suffix, ge) \

jcc_helper(near)
jcc_helper(short_)
make_instr_impl_1op_cc(j, i, short_, ecxz)
