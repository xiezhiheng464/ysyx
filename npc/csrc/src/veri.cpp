#include "veri.h"
#include "init.h"
#include "disasm.h"
VerilatedContext* contextp = new VerilatedContext;
VerilatedVcdC* tfp = new VerilatedVcdC; //初始化VCD对象指针
Vtop* top = new Vtop{contextp};
extern uint32_t *cpu_pc ;
extern "C" void ebreak(){
    tfp->dump(contextp->time()); //dump wave
    printf("ebreak\n");
    delete top;
    delete contextp;
    tfp->close();
    exit(0);
}
void init_verilator(int argc, char** argv){
    contextp->commandArgs(argc, argv);
    contextp->traceEverOn(true); //打开追踪功能
    top->trace(tfp, 0); //
    tfp->open("wave.vcd"); //设置输出的文件wave.vcd
}
void rst(int n){
    top->rst = 1;
    top->clk = 1;
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/4); //推动仿真时间
    top->clk = 0;
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/4); //推动仿真时间
    top->clk = 1;
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/4); //推动仿真时间
    top->clk = 0;
    top->rst = 0;
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/4); //推动仿真时间
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
};

void clk_cycle(int n){ //n需要是个偶数
    Dis_inst();
    top->clk = 1;
    top->rst = 0;
   // top->inst = pmem_read(0x80000000,4);
    //top->inst = pmem_read(top->pc,4);
    top->eval();
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
    contextp->timeInc(n/2); //推动仿真时间
    
    top->clk = 0;
    top->rst = 0;
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
    for(unsigned int i = 0;i < times;i++)
        clk_cycle(n);
};
