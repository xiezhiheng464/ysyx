#include "common.h"
void init_monitor(int argc, char *argv[]);
uint32_t pmem_read(uint32_t addr, int len);
void pmem_write(uint32_t addr, int len, uint32_t data);
uint32_t paddr_read(uint32_t addr,int len);
void paddr_write(uint32_t addr,int len,uint32_t data);
uint8_t* guest_to_host(uint32_t paddr);
uint32_t host_to_guest(uint8_t *haddr);
