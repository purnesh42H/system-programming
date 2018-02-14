#include "malloc.h"
#include <assert.h>


int main(int argc, char **argv)
{
  printf("sizeof(int) = %d bytes\n", (int) sizeof(int));
  //int i = 0;
  size_t size = 2048;
  size_t num = 2048*2048, ssize = 16;

  void *mem = malloc(size);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size, mem);
  assert(mem != NULL);

  void *mem2 = malloc(size);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size, mem2);
  assert(mem2 != NULL);
  free(mem);
  printf("Successfully free'd %zu bytes from addr %p\n", size, mem2);
  free(mem2);
  printf("Successfully free'd %zu bytes from addr %p\n", size, mem);

  void *mem3 = malloc(2*size);
  printf("Successfully malloc'd %zu bytes at addr %p\n", 2*size, mem3);
  assert(mem3 != NULL);

  void *mem4 = calloc(3, size);
  printf("Successfully malloc'd %zu bytes at addr %p\n", 3*size, mem4);
  assert(mem4 != NULL);
  free(mem4);
  printf("Successfully free'd %zu bytes from addr %p\n", size, mem3);

  void *mem5 = realloc(mem3, 3*size);
  printf("Successfully malloc'd %zu bytes at addr %p\n", 2*size, mem5);
  assert(mem5 != NULL);

  void *memptr = NULL;
  int pos = posix_memalign((void **)&memptr, num, size);
  if (pos == 0)
    printf("Successfully malloc'd %zu bytes at addr %p\n", num*size, memptr);

  void *ptr = memalign(num, ssize);
  printf("Successfully malloc'd %zu bytes at addr %p\n", num*size, ptr);

  void *ptr1 = memalign(num, ssize);
  printf("Successfully malloc'd %zu bytes at addr %p\n", num*size, ptr1);

  return 0;
}
