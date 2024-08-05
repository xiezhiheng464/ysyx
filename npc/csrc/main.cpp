#include "veri.h"
int main(int argc, char** argv) {
    init_verilator(argc, argv);
    rst(10);
    int i=10;
    int count=0;
    while (count<i) {
        clk_cycle(10);
        count++;
        isa_reg_display();
        printf("hello\n");
    }
    free();
    return 0;
}

