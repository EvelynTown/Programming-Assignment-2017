#include "cpu/instr.h"

make_instr_func(leave) {
	OPERAND S;
	
	cpu.esp = cpu.ebp;
	S.data_size = data_size;
	S.addr = cpu.esp;
	S.type = OPR_MEM;
	S.sreg = SREG_SS;
	
	operand_read(&S);
	cpu.ebp = S.val;
	cpu.esp += 4;
	return  1;
}
