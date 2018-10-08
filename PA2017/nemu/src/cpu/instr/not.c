#include "cpu/instr.h"

make_instr_func(not_v) {
	OPERAND rm;

	rm.data_size = data_size;
	int len = 1;
	len += modrm_rm(eip + 1, &rm);

	operand_read(&rm);

	rm.val = ~rm.val;
	operand_write(&rm);

	return len;
}
