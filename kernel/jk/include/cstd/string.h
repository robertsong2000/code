#ifndef __STRING_H
#define __STRING_H

#include <sys/types.h>

int sprintf(char* buffer, char* format, ...);

char* strcpy(char* str1, const char* str2);
char* strncpy(char* str1, const char* str2, size_t n);

char* strcat(char* str1, const char* str2);
char* strchr(const char* str, int c);

int strncmp(const char* str1, const char* str2, size_t n);
int strcmp(const char* str1, const char* str2);

char* strstr(const char* str1, const char* str2);

size_t strlen(const char* str);

void* memset(void* str, int c, size_t n);
void* memcpy(void* str1, const void* str2, size_t n);

char* strtok(char* str1, const char* str2);

#endif
