#include "Vtop.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vtop__Dpi.h"
#include "reg.h"
#include "common.h"
extern VerilatedContext* contextp;
extern VerilatedVcdC* tfp; //初始化VCD对象指针
extern Vtop* top;
extern uint32_t *cpu_gpr;  
extern uint32_t *cpu_pc;  
void init_verilator(int argc, char** argv);
void rst(int n,int argc,char **argv);
unsigned int inst_read(unsigned int pc); 
void clk_cycle(int n);
void free();
void exec_times(unsigned int times,int n);
extern int exec_step;
extern char last_inst[100];
