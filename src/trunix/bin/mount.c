#include <stdlib.h>
#include <sys/mount.h>

int main(int argc, char **argv) {
  if (argc < 4) return 1;
  if (argc == 4) return mount(argv[1], argv[2], argv[3], 0, 0);
  if (argc == 5) return mount(argv[1], argv[2], argv[3], atol(argv[4]), 0);
  return mount(argv[1], argv[2], argv[3], atol(argv[4]), argv[5]);
}
