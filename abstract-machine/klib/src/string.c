#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    size_t len=0;
    while(*s!='\0'){
        s++;
        len++;
    }
    return len;
}

char *strcpy(char *dst, const char *src) {
    char *d1=dst;
    char *s1=(char *)src;
    while(true){
        *d1=*s1;
        if(*s1=='\0')
            break;
        d1++;
        s1++;
    }
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
    char *d1=dst;
    char *s1=(char *)src;
    for(int i=0;i<n;i++){
        *d1=*s1;
        d1++;
        s1++;
    }
    return dst;
}

char *strcat(char *dst, const char *src) {
    char *d1=dst;
    char *s1=(char *)src;
    while(*d1!='\0')d1++;
    while(true){
        *d1=*s1;
        if(*s1=='\0')
            break;
        d1++;
        s1++;
    }
    return dst;
}

int strcmp(const char *s1, const char *s2) {
    char *d=(char *)s1;
    char *s=(char *)s2;
    while(true){
        if(*d== *s && *d =='\0')
            return 0;
        else if(*d==*s){
            d++;s++;
        }
        else{
            if(*d>*s)return 1;
            else return -1;
        }
    }
}

int strncmp(const char *s1, const char *s2, size_t n) {
    char *d=(char *)s1;
    char *s=(char *)s2;
    int i=0;
    while(i<n){
        if(*d== *s && *d =='\0')
            return 0;
        else if(*d==*s){
            d++;s++;i++;
        }
        else{
            if(*d>*s)return 1;
            else return -1;
        }
    }
    return 0;
}
void *memset(void *s, int c, size_t n) {
    unsigned char* p = s;
    while(n--){
        *p++ = (unsigned char)c;
    }
    return s;
    //  panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
    assert(dst != NULL || src != NULL);
    if(dst < src){
        char *p = (char *)dst;
        char *q = (char *)src;
        while(n--)
        {
            *p++ = *q++;
        }
    }
    else{
        char *p = (char *)dst + n;
        char *q = (char *)src + n;
        while(n--)
        {
            *--p = *--q;
        }
    }
    return dst;
//  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
    assert(out != NULL && in != NULL);
    char *p = (char*)out;
    char *q = (char*)in;
    if(p == q)
        return p;
    while(n--){
        *p++ = *q++;
    }
    return out;
    //  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const char *p = (const char *)s1;
    const char *q = (const char *)s2;
    for(int i = 0;i < n;i++){
        if(*(p+i) > *(q+i))
            return 1;
        else if(*(p+i) < *(q+i))
            return -1;
    }
    return 0;
    //  panic("Not implemented");
}
#endif
