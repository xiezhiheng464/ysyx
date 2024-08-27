#include "state.h"
#include "reg.h"
int npc_state = NPC_STOP;
int is_exit_status_bad() {
  int good = (npc_state == NPC_END && cpu_gpr[10] == 0) ||
    (npc_state == NPC_QUIT);
  return !good;                                                                                                                                                                  
}


