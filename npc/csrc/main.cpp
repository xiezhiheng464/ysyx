#include "veri.h"
#include "init.h"
#include "sdb.h"
#include "state.h"
int main(int argc, char** argv) {
    init_verilator(argc, argv);
    rst(10, argc, argv);
    sdb_mainloop();
    free();
    return is_exit_status_bad();
}

