#include "common.h"
#define CONFIG_DIFFTEST
void init_difftest(char *diff_so_file,long img_size);
void checkregs(uint32_t *ref_regs);
void difftest_exec_once();
