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

#include "sdb.h"
#include "veri.h"
#include "reg.h"
#include "init.h"
#include "state.h"
static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(npc) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  exec_times(-1,10);
  return 0;
}


static int cmd_q(char *args) {
  npc_state = NPC_QUIT;
  return -1;
}
static int cmd_x(char *args){
  char *arg=strtok(NULL," ");
  if(arg==NULL){printf("you need to give a number and the beginning address\n");return 0;}
  int number=atoi(arg);
  char *arg2=strtok(NULL," ");
  if(arg2==NULL){printf("you need to give the beginning address\n");return 0;}
  unsigned int address;
  sscanf(arg2,"%x",&address);
  for(int i=0;i<number;i++){
	printf("0x%x:   %x\n",address+4*i,paddr_read(address+4*i,4));
  }
  return 0;
}
static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_d(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_info(char *args);
static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute the given steps",cmd_si },
  { "info", "Print the needed data, r for register and w for watchpoints",cmd_info},
  { "x", "Check the given memory, you need to given a number and the beginning address",cmd_x},
  { "p","Calculate the expression",cmd_p },
  { "w","add the expr to the watchpoints",cmd_w },
  { "d","delete the watchpoint according to the NO",cmd_d }
  /* TODO: Add more commands */
};

#define NR_CMD (int)(sizeof(cmd_table) / sizeof(cmd_table[0]))//ARRLEN(cmd_table)
static int cmd_si(char *args){
  char *arg=strtok(NULL," ");
  if(arg==NULL){
	exec_times(1,10);
	return 0;
  }
  int step=atoi(arg);
  exec_times(step,10);
  return 0;
}
static int cmd_d(char *args){
  char *arg=strtok(NULL," ");
  if(arg==NULL){
	printf("you need to give the NO of the watchpoint you want to delete\n");
	return 0;
  }
  int NO=atoi(arg);
  free_wp(NO);
  return 0;
}

static int cmd_info(char *args){
  char *arg=strtok(NULL," ");
  if(arg==NULL){
	  printf("you need to give r or w option,r for register,w for watchpoints\n");
	  return 0;
  }
  bool is_r=strcmp(arg, "r");
  bool is_w=strcmp(arg, "w");
  if(is_r==0){isa_reg_display();}
  else if(is_w==0){wp_display();} 	
  else printf("you need to give r or w option,r for register,w for watchpoints\n");
  return 0;
}
static int cmd_w(char *args){
  if(args==NULL){
	  printf("you need to give the expression\n");
	  return 0;
  } 
  new_wp(args);  
  return 0;
}

static int cmd_p(char *args){
  if(args==NULL){
	  printf("you need to give the calculated expression\n");
	  return 0;
  } 
  //char test[]={"50 + 4 * 3 /( 1 - 2)"};
  bool k;
  unsigned int res=expr(args,&k);
  if(!k)  printf("you need to give the valid expression\n");
  else printf("%u\n",res);
  return 0;
}
static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  //sdb_set_batch_mode();
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }
    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();
  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
