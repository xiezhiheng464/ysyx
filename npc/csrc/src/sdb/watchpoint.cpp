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
#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[30];
  uint32_t num;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
void wp_display(){
	if(!head){printf("there no watchpoints\n");}
	WP* p=head;
	while(p){
		printf("the watchpoint %d,hold the expr %s,the expr num is %u\n",p->NO,p->expr,p->num);
		p=p->next;
	}
}
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    memset(wp_pool[i].expr, '\0', sizeof(wp_pool[i].expr));
  }

  head = NULL;
  free_ = wp_pool;
}
void new_wp(char *exp){
	if(!free_){printf("there are no free watchpoint\n");return;}
	WP* fwp=free_;
	free_=free_->next;
	fwp->next=NULL;
	strcpy(fwp->expr,exp);
	bool success;
	fwp->num=expr(fwp->expr,&success);
	WP *temp=head;
	if(!head){head=fwp;return;}
	while(temp->next)temp=temp->next;
	temp->next=fwp;
	
}
void free_wp(int NO){
	WP* p=head;
	WP* q;
	if(!head){printf("not found the watchpoint\n");return;}
	if(p->NO==NO){head=head->next;}	
	else {
		while( p->next && p->next->NO!=NO)p=p->next;
		if(!p->next){printf("not found the watchpoint\n");return;}
		q=p;
		p=p->next;
		q->next=p->next;
	}
	p->next=NULL;
    	memset(p->expr, '\0', sizeof(p->expr));
	WP* last;
	last=free_;
	if(!free_){free_=p;return;}
	while(last->next)last=last->next;
	last->next=p;
}
bool watch_change(){
	if(!head){return false;printf("there no watchpoints\n");}
	WP* p=head;
	bool change=false;
	while(p){
		bool success;
		if(expr(p->expr,&success)!=p->num){
		printf("the watchpoint %d:the value of the expr %s has changed,original is %08x,now is %08x\n",\
				p->NO,p->expr,p->num,expr(p->expr,&success));
		change=true;
		p->num=expr(p->expr,&success);
		}
		p=p->next;
	}
	return change;
}
/* TODO: Implement the functionality of watchpoint */

