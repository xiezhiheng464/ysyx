#include "common.h"
#include <verilated_dpi.h>
extern uint32_t *cpu_gpr;  
extern uint32_t *cpu_pc;  
uint32_t isa_reg_str2val(const char *s, bool *success);
extern const char *regs[];
void isa_reg_display();
