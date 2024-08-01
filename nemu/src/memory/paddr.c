/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <memory/host.h>
#include <memory/paddr.h>
#include <device/mmio.h>
#include <isa.h>

#if   defined(CONFIG_PMEM_MALLOC)
static uint8_t *pmem = NULL;
#else // CONFIG_PMEM_GARRAY
static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};
#endif
#define read_type 1
#define write_type 2
uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }
typedef struct {
  size_t paddr ;
  size_t data;
  int len;
  uint8_t type;
  size_t pc;
}mtrace_type;
#ifdef CONFIG_ITRACE_COND
#define MAX_MTRACE_LOOP_DEPTH 20
mtrace_type mtrace_loop[MAX_MTRACE_LOOP_DEPTH] = {};
int mtrace_loop_index = 0;
int mtrace_loop_mark = 0;
static void mtrace_loop_push(mtrace_type * loop,mtrace_type instance , int* index, int *mtrace_loop_mark) {
  loop[*index] = instance;
  (*index) ++;
  if(*index >= MAX_MTRACE_LOOP_DEPTH) {
    *index = 0; *mtrace_loop_mark = 0;
  }
}
void mtrace_loop_print(mtrace_type * loop,int index, int mtrace_loop_mark) {
  int i = index;
  if(mtrace_loop_mark)
    for(;i < MAX_MTRACE_LOOP_DEPTH;i++) {
    printf("    pc -> %08lx mem_type ->",mtrace_loop[i].pc);
    if(mtrace_loop[i].type == read_type){
      printf("mem read  ");
    }else if(mtrace_loop[i].type == write_type) {
      printf("mem write ");
    }
    printf("mem_addr -> %lx  mem_data -> %016lx mem_len -> %x \n",mtrace_loop[i].paddr,mtrace_loop[i].data,mtrace_loop[i].len); 
    //  printf("     inst ->  %08x __ pc ->  ",loop[i].isa.inst.val);

  }
  for(i = 0;i < index - 1;i++) {
    printf("    pc -> %08lx mem_type ->",mtrace_loop[i].pc);
    if(mtrace_loop[i].type == read_type){
      printf("mem read  ");
    }else if(mtrace_loop[i].type == write_type) {
      printf("mem write ");
    }
    printf("mem_addr -> %lx  mem_data -> %016lx mem_len -> %x \n",mtrace_loop[i].paddr,mtrace_loop[i].data,mtrace_loop[i].len); 
  }
       printf("--->pc -> %08lx mem_type ->",mtrace_loop[i].pc);
    if(mtrace_loop[i].type == read_type){
      printf("mem read  ");
    }else if(mtrace_loop[i].type == write_type) {
      printf("mem write ");
    }
    printf("mem_addr -> %lx  mem_data -> %016lx mem_len -> %x \n",mtrace_loop[i].paddr,mtrace_loop[i].data,mtrace_loop[i].len); 
}

#endif
static word_t pmem_read(paddr_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);
  return ret;
}

static void pmem_write(paddr_t addr, int len, word_t data) {
  host_write(guest_to_host(addr), len, data);
}

static void out_of_bound(paddr_t addr) {
  #ifdef CONFIG_ITRACE_COND
  mtrace_loop_print(mtrace_loop,mtrace_loop_index,mtrace_loop_mark);
  #endif
  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
      addr, PMEM_LEFT, PMEM_RIGHT, cpu.pc);
}

void init_mem() {
#if   defined(CONFIG_PMEM_MALLOC)
  pmem = malloc(CONFIG_MSIZE);
  assert(pmem);
#endif
  IFDEF(CONFIG_MEM_RANDOM, memset(pmem, rand(), CONFIG_MSIZE));
  Log("physical memory area [" FMT_PADDR ", " FMT_PADDR "]", PMEM_LEFT, PMEM_RIGHT);
}

size_t mem_pc = 0;
word_t paddr_read(paddr_t addr, int len , uint8_t mem_type) {
  #ifdef CONFIG_ITRACE_COND
  mtrace_type mem_read_mtrace = {};
  mem_read_mtrace.type = read_type;
  mem_read_mtrace.len = len;
  mem_read_mtrace.paddr = addr;
  mem_read_mtrace.pc = mem_pc;

  #endif
  //如果在所设定的内存范围内
  if (likely(in_pmem(addr)))  {
      size_t pmem_data = pmem_read(addr, len);
    #ifdef CONFIG_ITRACE_COND  
      mem_read_mtrace.data = pmem_data;
      if(mem_type == mem_others) {
         mtrace_loop_push(mtrace_loop,mem_read_mtrace,&mtrace_loop_index,&mtrace_loop_index);
      }else if(mem_type == mem_inst_fec){
        mem_pc = addr;
      }
    #endif
      return pmem_data;//mem_read_mtrace.data;

  }
  //如果不在设定的内存地址范围内，则进行MMIO处理
  IFDEF(CONFIG_DEVICE, return mmio_read(addr, len));

  out_of_bound(addr);
  return 0;
}

void paddr_write(paddr_t addr, int len, word_t data) {
  #ifdef CONFIG_ITRACE_COND
  mtrace_type mem_write_mtrace = {};
  mem_write_mtrace.type = write_type;
  mem_write_mtrace.len  = len;
  mem_write_mtrace.data = data;
  mem_write_mtrace.paddr = addr;
  mem_write_mtrace.pc = mem_pc;
  mtrace_loop_push(mtrace_loop,mem_write_mtrace,&mtrace_loop_index,&mtrace_loop_index);
  #endif
  if (likely(in_pmem(addr))) { pmem_write(addr, len, data); return; }
  // printf("oh my god\n");
  IFDEF(CONFIG_DEVICE, mmio_write(addr, len, data); return);
  out_of_bound(addr);
}


void mtrace_printf(){
    #ifdef CONFIG_ITRACE_COND
  mtrace_loop_print(mtrace_loop,mtrace_loop_index,mtrace_loop_mark);
#endif
}
