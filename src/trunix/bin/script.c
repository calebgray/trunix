#!/bin/cinit

#include <stdio.h>

int main(int argc, const char **argv, const char * const *envp) {
    printf("--------------------------------------------------------------------------------\n");
    printf("script.c\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("argv:\n");
    for (int i = 0; i < argc; ++i) {
        printf("%s\n", argv[i]);
    }
    printf("envp:\n");
    for (int i = 0; envp[i] != NULL; ++i) {
        printf("%s\n", envp[i]);
    }
    printf("--------------------------------------------------------------------------------\n");

    return 0;
}
