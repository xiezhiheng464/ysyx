#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}
void print_deci(int n, char **p, int *count){
    if(n >= 10) print_deci(n/10, p,count);
    **p = n % 10 + '0';
    (*p)++;(*count)++;
};
int sprintf(char *out, const char *fmt, ...) {
    char *s=out;
    char *p;
    char *sval;
    va_list args;
    int ival;
    int count=0;
    va_start(args,fmt);
    for(p=(char *)fmt;*p;p++){
		if(*p!='%'){	 /*只要不是 %，这跳过本次循环，进行下一次循环，直到找到下一个%*/
			*s++=*p;
            count++;
            continue;
		}
		switch(*++p){
		case 'd' :
			ival=va_arg(args,int);
            if(ival<0){
                ival=-ival;*s++='-';count++;
            }
            else
                print_deci(ival,&s,&count);
			break;
		case 's':
            for(sval=va_arg(args,char*);*sval;sval++){
            *s++=*sval;
            count++;
            }
			break;
		default:
            *s++='%';
            *s++=*p;
            count+=2;
			break;
		}
	}
    *s='\0';
    va_end(args);
    return count;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
