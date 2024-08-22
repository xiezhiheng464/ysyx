#include "veri.h"
#include "init.h"
#include "sdb.h"
int main(int argc, char** argv) {
    init_verilator(argc, argv);
    rst(10);
    rst(10);
    init_monitor(argc, argv);
    rst(10);
    sdb_mainloop();
    free();
    return 0;
}

