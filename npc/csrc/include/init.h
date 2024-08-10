#include <stdint.h>
void init_monitor(int argc, char *argv[]);
uint32_t pmem_read(uint32_t addr, int len);
void pmem_write(uint32_t addr, int len, uint32_t data);
uint32_t paddr_read(uint32_t addr,int len);
void paddr_write(uint32_t addr,int len,uint32_t data);
