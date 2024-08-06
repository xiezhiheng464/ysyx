#include "veri.h"
#include "init.h"
VerilatedContext* contextp = new VerilatedContext;
VerilatedVcdC* tfp = new VerilatedVcdC; //初始化VCD对象指针
Vtop* top = new Vtop{contextp};
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
};
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
void clk_cycle(int n){ //n需要是个偶数
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
