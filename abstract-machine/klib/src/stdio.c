#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
    char p[2048];
    va_list args;
    int count=0;
    va_start(args,fmt);
    count = vsprintf(p,fmt,args);
    va_end(args);
    for(int i=0;i<count;i++){putch(p[i]);}
    return count;
}

int vsprintf(char *out, const char *fmt, va_list args) {
    int count=0;
    int i=0;
    char *out_s=out;
    while(fmt[i]){
	if(fmt[i]!='%'){	 /*只要不是 %，这跳过本次循环，进行下一次循环，直到找到下一个%*/
		*out_s=fmt[i];
        out_s++;
        count++;
        i++;
        continue;
    }
    switch(fmt[++i]){
        case 'd':
            int ival;
            ival=va_arg(args,int);
            char temp[32]={};
            int length=0;
            while(1){
                temp[length] = ival % 10 + '0';
                ival = ival / 10;
                length++;
                if(ival == 0)break;
            }
            length--;
            while(length >= 0){
                *out_s++ = temp[length--];
                count++;
            }
            break;
        case 's':
            char *sval; 
            for(sval=va_arg(args,char*);*sval;sval++){
                *out_s++ = *sval;
                count++;
            }
            break;
        case 'c':
            char cval=va_arg(args,int);
            *out_s++ = cval;
            count++;
            break;
        default:
            *out_s++='%';
            *out_s++=fmt[i];
            count+=2;
			break;
        }
        i++;
    }
    *out_s = '\0';
    return count;
}
int sprintf(char *out, const char *fmt, ...) {
    va_list args;
    int count=0;
    va_start(args,fmt);
    count = vsprintf(out,fmt,args);
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
