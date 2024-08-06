#include "veri.h"
#include "init.h"
int main(int argc, char** argv) {
    init_verilator(argc, argv);
    init_monitor(argc, argv);
    rst(20);
    int i=10;
    int count=0;
    while (count<i) {
        clk_cycle(10);
        count++;
        //isa_reg_display();
    }
    free();
    return 0;
}

