#include <stdio.h>
#include "Vtop.h"
#include "verilated.h"

#include "verilated_vcd_c.h"
int main(int argc, char** argv) {
    VerilatedContext* contextp = new VerilatedContext;
    contextp->commandArgs(argc, argv);
    Vtop* top = new Vtop{contextp};
    VerilatedVcdC* tfp = new VerilatedVcdC; //初始化VCD对象指针
    contextp->traceEverOn(true); //打开追踪功能
    top->trace(tfp, 0); //
    tfp->open("wave.vcd"); //设置输出的文件wave.vcd
    int count = 0;
    int i = 100;    
    while (count < i) {
      int a = rand() & 1;
      int b = rand() & 1;
      top->a = a;
      top->b = b;
      top->eval();
      printf("a = %d, b = %d, f = %d\n", a, b, top->f);
      assert(top->f == (a ^ b));
      tfp->dump(contextp->time()); //dump wave
      contextp->timeInc(1); //推动仿真时间
      count = count + 1;
    }
    delete top;
    delete contextp;
    tfp->close();
    return 0;
}

