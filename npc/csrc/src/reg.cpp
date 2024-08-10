#include "reg.h"
uint32_t *cpu_gpr = NULL;
uint32_t *cpu_pc = NULL;
extern "C" void get_regs(const svOpenArrayHandle r)
{
  cpu_gpr = (uint32_t *)(((VerilatedDpiOpenVar*)r) -> datap());
}
extern "C" void get_pc(const svOpenArrayHandle r)
{
  cpu_pc = (uint32_t *)(((VerilatedDpiOpenVar*)r) -> datap());
}
const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
void isa_reg_display() {
  printf("isa_reg_display\n");
  printf("===============================================================================\n");
  for(int i = 0; i < 8; i++ ){
    printf("%-3s:0x%08x    %-3s:0x%08x     %-3s:0x%08x    %-3s:0x%08x\n", \
          regs[4*i],cpu_gpr[4*i],regs[4*i+1],cpu_gpr[4*i+1],regs[4*i+2],cpu_gpr[4*i+2],regs[4*i+3],cpu_gpr[4*i+3]);

  }
  printf("pc :0x%08x\n",cpu_pc[0]);
  printf("===============================================================================\n");
}
uint32_t isa_reg_str2val(const char *s, bool *success) {
   for (int i = 0; i < 32; i++)
   {
    if (!strcmp(regs[i], s))
     {
       *success = 1;
      return cpu_gpr[i];
     }
   }
  if (!strcmp("pc", s))
   {
     *success = 1;
    return cpu_pc[0];
   }
   *success = 0;
   return 0;
 } 




