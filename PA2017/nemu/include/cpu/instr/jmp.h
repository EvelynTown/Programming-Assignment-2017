#ifndef __INSTR_JMP_H__
#define __INSTR_JMP_H__

make_instr_func(jmp_near);
make_instr_func(jmp_short_);
make_instr_func(jmp_rm_v);
make_instr_func(jmp_ptr_v);

make_instr_func(je_short_);
make_instr_func(jg_short_);
make_instr_func(jle_short_);
make_instr_func(jne_short_);
make_instr_func(jbe_short_);
make_instr_func(jae_short_);
make_instr_func(jge_short_);
make_instr_func(ja_short_);
make_instr_func(jl_short_);
make_instr_func(js_short_);
make_instr_func(jb_short_);
make_instr_func(jns_short_);

make_instr_func(jbe_i2r_near);
make_instr_func(jle_i2r_near);
make_instr_func(jl_i2r_near);
make_instr_func(je_i2r_near);
make_instr_func(ja_i2r_near);
make_instr_func(js_i2r_near);
make_instr_func(jne_i2r_near);
make_instr_func(jg_i2r_near);
make_instr_func(jae_i2r_near);
make_instr_func(jns_i2r_near);
make_instr_func(jb_i2r_near);
make_instr_func(jge_i2r_near);

#endif
