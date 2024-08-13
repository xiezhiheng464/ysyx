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

#include "reg.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
//#include "assert.h"
//#include "string.h"
#include "sdb.h"
#include "init.h"
enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_NUMBER, TK_REM,
  TK_RES, TK_xNUMBER,
  TK_reg,
  TK_nEQ,
  TK_AND,
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   * 注意16进制得放在10进制前面，否则0x的0就会被匹配走
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"-",'-'},			// 
  {"\\*",'*'},
  {"/",'/'},
  {"\\(",'('},
  {"\\)",')'},
  {"0x[0-9]+",TK_xNUMBER},
  {"[0-9]+",TK_NUMBER},
  {"!=",TK_nEQ},
  {"\\$[a-zA-Z0-9]*",TK_reg},
  {"&&",TK_AND},
};

#define NR_REGEX (int)(sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
	  printf("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
	  fflush(stdout);assert(0);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[10240] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  int count_k=0;
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

	//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
	    //i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
	if(rules[i].token_type==TK_NOTYPE)break;
	Token a;
	memset(a.str, '\0', sizeof(a.str));
	a.type=rules[i].token_type;
	strncpy(a.str,substr_start,substr_len);
	tokens[nr_token]=a;
	nr_token++;
	
	switch (rules[i].token_type) {
	  case '(':count_k++;break;
	  case ')':count_k--;break;
          default: break;
        }
	if(count_k<0)return false;	
        break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  if(count_k!=0)return false;
  for(i=0;i<nr_token;i++){
	if (tokens[i].type == '*' && 
			(i == 0 || 
			!(tokens[i-1].type==')' || tokens[i-1].type==TK_NUMBER
				|| tokens[i-1].type==TK_xNUMBER || tokens[i-1].type==TK_reg)))tokens[i].type=TK_REM;
	if (tokens[i].type == '-' && 
			(i == 0 || 
			!(tokens[i-1].type==')' || tokens[i-1].type==TK_NUMBER  \
				|| tokens[i-1].type==TK_xNUMBER || tokens[i-1].type==TK_reg)))tokens[i].type=TK_RES;
	/* if (tokens[i].type ==TK_xNUMBER){ */
	/*         int num; */
	/*         sscanf(tokens[i].str,"%x",&num); */
	/*         memset(tokens[i].str, '\0', sizeof(tokens[i].str)); */
	/*         sprintf(tokens[i].str,"%d",num); */
	/* } */
	/* printf("%s\n",tokens[i].str); */
  }
  return true;
}
uint32_t eval(int left,int right){
  int count_k=0;
  int kh_mark=1;
 // 判断左右符号是否匹配
  for(int i=left;i<=right-1;i++)
  {	
	switch (tokens[i].type) {
	  case '(':count_k++;break;
	  case ')':count_k--;break;
          default: break;
        }
	if(count_k==0){kh_mark=0;break; }
  }
      	if (left > right) {
	  printf("An error occurs during the calculation, left index is larger than right index\n");
	  fflush(stdout);assert(0);
  }
  
  else if (left == right ) {
	  uint32_t k;
	  if(tokens[left].type==TK_NUMBER){
  	  	  sscanf(tokens[left].str,"%x",&k);
	  }
	  else if(tokens[left].type==TK_xNUMBER){
  	  	  sscanf(tokens[left].str,"%x",&k);
	  }
	  else if(tokens[left].type==TK_reg){
		  bool success;
		  uint32_t k;
		  k=isa_reg_str2val(tokens[left].str+1,&success);
		  if(success==0){
	  		printf("An error occurs during the calculation, the reg doesn't exist\n");
		  	fflush(stdout);assert(0);
		  }
		  return k;
	  }
	  else {  printf("An error occurs during the calculation, the token should be a number\n");
		  fflush(stdout);assert(0);
	  }
	  return k;
  }
  else if (tokens[left].type=='('   && tokens[right].type==')'&& kh_mark ==1) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(left + 1, right - 1);
  }
  else {
	  int count_k=0,index_f=-1,index_type=10;
	  /* index_type, the mark of priority, */
	  /* 1 for &&, */
	  /* 2 for == and !=, */
	  /* 3 for + and -, */
	  /* 4 for * and /, */
	  /* 5 for - and * functions as single number */
	  for(int i=left;i<right;i++){ 
		  if(tokens[i].type=='(')count_k++;
		  else if(tokens[i].type==')')count_k--;
		  else if(count_k!=0)continue;
		  else if(tokens[i].type=='+'  && index_type>=3){index_f=i;index_type=3;}
		  else if(tokens[i].type=='-'  && index_type>=3){index_f=i;index_type=3;}
		  else if(tokens[i].type=='*'  && index_type>=4){index_f=i;index_type=4;}
		  else if(tokens[i].type=='/'  && index_type>=4){index_f=i;index_type=4;}
		  else if(tokens[i].type==TK_EQ  && index_type>=2){index_f=i;index_type=2;}
		  else if(tokens[i].type==TK_nEQ  && index_type>=2){index_f=i;index_type=2;}
		  else if(tokens[i].type==TK_AND  && index_type>=1){index_f=i;index_type=1;}
		  else if(tokens[i].type==TK_RES && index_type>=5){index_f=i;index_type=5;}
		  else if(tokens[i].type==TK_REM && index_type>=5){index_f=i;index_type=5;}
		  
	  }
	  if(tokens[index_f].type=='+')return eval(left,index_f-1)+eval(index_f+1,right);
	  else if(tokens[index_f].type=='*')return eval(left,index_f-1)*eval(index_f+1,right);
	  else if(tokens[index_f].type=='/'){
		  uint32_t k=eval(index_f+1,right);
		  if(k==0){
			  for(int ii=left+1;ii<=right;ii++)
			  	printf("%s",tokens[ii].str);
		  	  printf("%d,%d",index_f+1,right);
			  printf("An error occurs during the calculation, the divisior is 0\n");
			  fflush(stdout);
			  assert(0);
		  }
		  return eval(left,index_f-1)/k;
	  }
	  else if(tokens[index_f].type=='-')return eval(left,index_f-1)-eval(index_f+1,right);
	  else if(tokens[index_f].type==TK_RES) return -eval(index_f+1,right);
	  else if(tokens[index_f].type==TK_REM) return eval(index_f+1,right);
	  else if(tokens[index_f].type==TK_AND) return eval(left,index_f-1)!=0&&eval(index_f+1,right)!=0;
	  else if(tokens[index_f].type==TK_EQ) return eval(left,index_f-1)==eval(index_f+1,right);
	  else if(tokens[index_f].type==TK_nEQ) return eval(left,index_f-1)!=eval(index_f+1,right);
	  else if(tokens[index_f].type==TK_REM) return paddr_read(eval(index_f+1,right),4);
  	  
 
	}
  return 0;
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  uint32_t k=eval(0,nr_token-1);
  //uint32_t kk=k;
  //printf("%ld\n",k);
  //printf("%lu\n",kk);
  *success = true;
  
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

  return k;
}
