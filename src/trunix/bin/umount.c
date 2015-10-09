#!/bin/cinit

#include <stdlib.h>
#include <sys/mount.h>

int main(int argc, char **argv) {
  if (argc < 2) return 1;
  if (argc == 2) return umount(argv[1]);
  return umount2(argv[1], atoi(argv[2]));
}
