#include "cpu/instr.h"
#include "device/port_io.h"

make_instr_func(out_al2dx) {
	pio_write(cpu.edx & 0xffff, 1, cpu.eax & 0xff);
	return 1;
}
