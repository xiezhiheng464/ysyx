#include "veri.h"
#include "init.h"
#include "sdb.h"
int main(int argc, char** argv) {
    init_verilator(argc, argv);
    rst(20);
    init_monitor(argc, argv);
        //clk_cycle(10);
        //isa_reg_display();
    sdb_mainloop();
    free();
    return 0;
}

