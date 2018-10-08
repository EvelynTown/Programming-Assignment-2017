#include "cpu/instr.h"
#include "device/port_io.h"

make_instr_func(in_dx2al) {
	uint32_t temp = 0;
	temp = pio_read(cpu.edx & 0xffff,1);
	cpu.eax &= (~0xff);
	cpu.eax += temp;
	return 1;
}

make_instr_func(in_dx2ax) {
	uint32_t temp = 0;
	if(data_size == 16) {
		temp = pio_read(cpu.edx & 0xffff, 2);
		cpu.eax &= (~0xffff);
		cpu.eax += temp;
	}
	else if(data_size == 32) {
		temp = pio_read(cpu.edx & 0xffff, 4);
		cpu.eax = temp;
	}
	return 1;
}
