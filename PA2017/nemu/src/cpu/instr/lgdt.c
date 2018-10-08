#include "cpu/instr.h"

make_instr_func(lgdt_m_v) {
	OPERAND m;
	m.data_size = data_size;
	int len = 1;
	len += modrm_rm(eip + 1, &m);

	cpu.gdtr.limit = laddr_read(m.addr, 2);
	cpu.gdtr.base = laddr_read(m.addr + 2, 4);

	return len;
}
