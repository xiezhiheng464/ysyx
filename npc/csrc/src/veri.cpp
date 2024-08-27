#include "veri.h"
#include "init.h"
#include "disasm.h"
#include "difftest.h"
#include "init.h"
#include "state.h"
#include "log.h"
int exec_step = 0;
char last_inst[100]={};
VerilatedContext* contextp = new VerilatedContext;
VerilatedVcdC* tfp = new VerilatedVcdC; //初始化VCD对象指针
Vtop* top = new Vtop{contextp};
extern "C" void ebreak(){
    //tfp->dump(contextp->time()); //dump wave
    printf("ebreak\n");
    //delete top;
    //delete contextp;
    tfp->close();
    npc_state = NPC_END;
    // exit(0);
}
void init_verilator(int argc, char** argv){
    contextp->commandArgs(argc, argv);
    contextp->traceEverOn(true); //打开追踪功能
    top->trace(tfp, 0); //
    tfp->open("wave.vcd"); //设置输出的文件wave.vcd
}
void rst(int n,int argc,char** argv){
    top->rst_n = 0;
    top->clk = 0;
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/2); //推动仿真时间
    top->rst_n = 0;
    top->clk = 1;
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/2); //推动仿真时间
    init_monitor(argc, argv);
    top->rst_n = 0;
    top->clk = 0;
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/2); //推动仿真时间
    top->rst_n = 0;
    top->clk = 1;
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/2); //推动仿真时间
    top->rst_n = 0;
    top->clk = 0;
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/2); //推动仿真时间
}
void Dis_inst(){
   char p[100];
   uint8_t *pt = guest_to_host(cpu_pc[0]);
   printf("0x%08x:", cpu_pc[0]);
   for (int i = 3; i >= 0; i--) {
      printf(" %02x", *(pt+i));
   }
   printf("\t");
   disassemble(p, 100, cpu_pc[0], pt, 4); 
   printf("%s\n",p);
   sprintf(last_inst,"0x%08x %02x %02x %02x %02x\t", \
           cpu_pc[0],*(pt+3),*(pt+2),*(pt+1),*pt);
   strcat(last_inst,p);
};

void clk_cycle(int n){ //n需要是个偶数
    Dis_inst();
    top->clk = 1;
    top->rst_n = 1;
   // top->inst = pmem_read(0x80000000,4);
    //top->inst = pmem_read(top->pc,4);
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/2); //推动仿真时间
    
    top->clk = 0;
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/2); //推动仿真时间
}
void free(){
    delete top;
    delete contextp;
    tfp->close();
}

void exec_times(unsigned int times,int n)
{
    switch (npc_state) {
    case NPC_END: case NPC_ABORT:
      printf("Program execution has ended. To restart the program, exit NPC and run again.\n");
      return;
    default: npc_state = NPC_RUNNING;
  }

    for(unsigned int i = 0;i < times;i++){
        clk_cycle(n);
        exec_step++;
    #ifdef CONFIG_DIFFTEST
        difftest_exec_once();
    #endif 
        if (npc_state != NPC_RUNNING) break;
    }
    switch (npc_state) {
     case NPC_RUNNING: npc_state = NPC_STOP; break;

     case NPC_END: case NPC_ABORT:
       Log("NPC: %s" ,
           (npc_state == NPC_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
            (cpu_gpr[10] == 0? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
             ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED)))
           );
       // fall through
     //case NPC_QUIT: statistic();
   }
};
