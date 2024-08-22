#include "log.h"
#include "init.h"
#include "sdb.h"
#include "disasm.h"
#include "difftest.h"
static int is_batch_mode = false;
static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;
uint8_t pmem[CONFIG_MSIZE] = {};
uint8_t* guest_to_host(uint32_t paddr) { return pmem + paddr - CONFIG_MBASE; }
uint32_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }
static bool in_pmem(uint32_t addr) {
  return addr - CONFIG_MBASE < CONFIG_MSIZE && addr >= CONFIG_MBASE;
}
static uint32_t host_read(void *addr, int len) {                                                                                       
  switch (len) {
    case 1: return *(uint8_t  *)addr;
    case 2: return *(uint16_t *)addr;
    case 4: return *(uint32_t *)addr;
    default: assert(0);
  }
}
static void host_write(void *addr, int len, uint32_t data) {
  switch (len) {
    case 1: *(uint8_t  *)addr = data; return;
    case 2: *(uint16_t *)addr = data; return;
    case 4: *(uint32_t *)addr = data; return;
    default: assert(0);
  }
}
uint32_t paddr_read(uint32_t addr,int len)
{
    return host_read(guest_to_host(addr),len);
}
void paddr_write(uint32_t addr,int len,uint32_t data)
{
    host_write(guest_to_host(addr),len,data);
}
extern "C" void pmem_read_inst(int raddr, int *rdata){
  if (!in_pmem(raddr)) return;
  uint8_t *pt = guest_to_host(raddr) + 3;
  int ret = 0;
  for (int i = 0; i < 4; ++i) {
    ret = (ret << 8) | (*pt--);
  }
  *rdata = ret;
};
extern "C" void pmem_read(int raddr, int *rdata, char mask)
{
  if (!in_pmem(raddr)) return;
  int bits = ( (mask & 3) == 2 )? 4 : ((mask & 3) == 1 ) ? 2 : 1;
  uint8_t *pt = guest_to_host(raddr) + bits - 1;
  int signed_extend = ((mask & 4) == 0);
  if ( bits == 1 && signed_extend){
    uint8_t ret = *pt;
    *rdata = ret;
    return;
  }
  if ( bits == 2 && signed_extend){
    uint16_t ret = *pt;
    ret = (ret << 8) | (*(pt-1));
    *rdata = ret;
    return;
  }
  int ret = 0;
  for (int i = 0; i < bits; ++i) {
    ret = (ret << 8) | (*pt--);
  }
  *rdata = ret;
}
extern "C" void pmem_write(int waddr, int wdata, char mask)
{
  if (!in_pmem(waddr)) return;
  uint8_t *pt = guest_to_host(waddr);
  int bits = ( (mask & 3) == 2 )? 4 : ((mask & 3) == 1 ) ? 2 : 1;
  for (int i = 0; i < bits; ++i) {
    *pt = (wdata & 0xff);
    wdata >>= 8, pt++;
  }
}
static void welcome() {
  Log("Build time: %s, %s", __TIME__, __DATE__);
}

static const uint32_t img [] = {
  0x00000297,  // auipc t0,0
  0x00028823,  // sb  zero,16(t0)
  0x0102c503,  // lbu a0,16(t0)
  0x00100073,  // ebreak (used as nemu_trap)
  0xdeadbeef,  // some data
};

static long load_img() {
  if (img_file == NULL) {
    printf("No image is given. Use the default image\n");
    return sizeof(img); // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp,"Can not open '%s'",img_file);
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  printf("The image is %s, size = %ld\n", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(CONFIG_MBASE), size, 1, fp);
  assert(ret == 1);
  fclose(fp);
  return size;
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 1: img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}


void load_default_img() {
  /* Load built-in image. */
  memcpy(guest_to_host(CONFIG_MBASE), img, sizeof(img));
  //printf("%ld",sizeof(img));
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */
  /* Parse arguments. */
  parse_args(argc, argv);
  /* Set random seed. */
  //init_rand();
  
  /* Open the log file. */
  init_log(log_file);

  /* Initialize memory. */
  //init_mem();

  /* Initialize devices. */
  //IFDEF(CONFIG_DEVICE, init_device());
  init_disasm("riscv32-pc-linux-gnu");
  /* Perform ISA dependent initialization. */
  load_default_img();
  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();
  /* Initialize the simple debugger. */
  init_sdb();
  welcome();
  /* Initialize differential testing. */
#ifdef CONFIG_DIFFTEST
  init_difftest(diff_so_file, img_size);//difftest_port
  Log("CONFIG_DIFFTEST open");
#endif
}

