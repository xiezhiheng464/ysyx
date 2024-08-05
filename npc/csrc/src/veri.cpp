#include "veri.h"
VerilatedContext* contextp = new VerilatedContext;
VerilatedVcdC* tfp = new VerilatedVcdC; //初始化VCD对象指针
Vtop* top = new Vtop{contextp};
extern "C" void ebreak(){
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
    top->clk = 0;
    contextp->timeInc(n); //推动仿真时间
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
}
unsigned int inst_read(unsigned int pc){
    if(contextp->time() > 50)
        return 0b00000000000100000000000001110011;
    return 0b00000011000000000000000010010011;
} 
void clk_cycle(int n){ //n需要是个偶数
    top->clk = 1;
    top->rst = 0;
    top->inst = inst_read(top->pc);
    top->eval();
    if (tfp != nullptr)
    contextp->timeInc(n/2); //推动仿真时间
      tfp->dump(contextp->time()); //dump wave
    
    top->clk = 0;
    top->rst = 0;
    top->inst = inst_read(top->pc);
    top->eval();
    contextp->timeInc(n/2); //推动仿真时间
    if (tfp != nullptr)
      tfp->dump(contextp->time()); //dump wave
}
void free(){
    delete top;
    delete contextp;
    tfp->close();
}
