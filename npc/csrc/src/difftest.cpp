#include <dlfcn.h>
#include "common.h"
#include "reg.h"
#include "init.h"
#include "log.h"
#include "difftest.h"
enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF }; 
void (*ref_difftest_memcpy)(uint32_t addr, void *buf, size_t n, bool direction) = NULL; 
void (*ref_difftest_regcpy)(void *dut, bool direction, void *pc) = NULL; 
void (*ref_difftest_exec)(uint64_t n) = NULL; 
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;
extern uint32_t *cpu_gpr;
extern uint32_t *cpu_pc;
uint32_t ref_regs[33];
uint8_t ref_mems[CONFIG_MSIZE];
void checkregs(uint32_t *ref_regs)
{
    int error_flag = 0;
    for (int i = 0; i < 32; ++i) {
        if (ref_regs[i] != cpu_gpr[i]) {
            printf("Error: Difftest failed at reg %d (%3s)\n", i, regs[i]);
            error_flag = 1;
    }}
    if (ref_regs[32] != *cpu_pc) {
        printf("Error: Difftest failed at pc\n");
        error_flag = 1;
    }
    if(error_flag){
         for (int j = 0; j < 32; ++j) {
             if (cpu_gpr[j] != ref_regs[j]) printf(ANSI_FG_RED);
             printf("reg %02d (%3s): dut = 0x%08x, ref = 0x%08x\n", j, regs[j], cpu_gpr[j], ref_regs[j]);
             if (cpu_gpr[j] != ref_regs[j]) printf(ANSI_NONE);
        }
    if (*cpu_pc != ref_regs[32]) printf(ANSI_FG_RED);
    printf("pc : dut = 0x%08x, ref = 0x%08x\n", *cpu_pc, ref_regs[32]);
    if (*cpu_pc != ref_regs[32]) printf(ANSI_NONE);    
    }
}
void checkmems(uint8_t *ref_mems,long size){
    for(long i=0;i<size;i++){
        if(ref_mems[i]!=paddr_read(i+CONFIG_MBASE,1))
        {
           printf(ANSI_FG_RED);
           printf("mem differ at %08lx, ref:%02x, dut:%02x\n",i+CONFIG_MBASE,ref_mems[i],paddr_read(i+CONFIG_MBASE,1));
           printf(ANSI_NONE);
        }
        
    };
};

void init_difftest(char *diff_so_file,long img_size)
{
  //char ref_so_file[] = "/home/johnson/ysyx-workbench/nemu/build/riscv64-nemu-interpreter-so";
  char *ref_so_file = diff_so_file;
  printf("%s\n",ref_so_file);
  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);

  ref_difftest_memcpy = (void (*)(uint32_t addr, void *buf, size_t n, bool direction))(dlsym(handle, "difftest_memcpy"));
  assert(ref_difftest_memcpy);

  ref_difftest_regcpy = (void (*)(void *dut, bool direction, void *pc))(dlsym(handle, "difftest_regcpy"));
  assert(ref_difftest_regcpy);

  ref_difftest_exec = (void (*)(uint64_t n))(dlsym(handle, "difftest_exec"));
  assert(ref_difftest_exec);

  ref_difftest_raise_intr = (void (*)(uint64_t NO))(dlsym(handle, "difftest_raise_intr"));
  assert(ref_difftest_raise_intr);

  void (*ref_difftest_init)() = (void (*)())(dlsym(handle, "difftest_init"));
  assert(ref_difftest_init);
  ref_difftest_init();
  //ref_difftest_memcpy(CONFIG_MBASE, pmem, img_size, DIFFTEST_TO_REF);
  ref_difftest_memcpy(CONFIG_MBASE, pmem, CONFIG_MSIZE, DIFFTEST_TO_REF);
  ref_difftest_regcpy(cpu_gpr, DIFFTEST_TO_REF, cpu_pc);
}

void difftest_exec_once()
{
  ref_difftest_exec(1);
  // pc -> ref_regs[32]
  ref_difftest_regcpy(ref_regs, DIFFTEST_TO_DUT, NULL);
  ref_difftest_memcpy(CONFIG_MBASE,ref_mems,CONFIG_MSIZE,DIFFTEST_TO_DUT);
  checkregs(ref_regs);
  //checkmems(ref_mems,30);
  checkmems(ref_mems,CONFIG_MSIZE);
}

