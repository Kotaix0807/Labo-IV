#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

int largestStr(char *arr[], int n);
int largestStr_bra(char *arr[], int n);
int largestOpt(char *choices[], int n_choice, const char *title);
unsigned long fileLines(const char *file, int opt);
char **readText(const char *file);
size_t u8_len(const char *s);
int replace_fmt(char **arr, int idx, const char *arg);


#endif /* TOOLS_H */
