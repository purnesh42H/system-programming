#include <stdio.h>

struct MemoryRegion
{
  void *startAddr;
  void *endAddr;
  int isReadable;
  int isWriteable;
  int isExecutabl;
};
void *r2TotSize, *rw2TotSize;

void calcMR(struct MemoryRegion mr)
{
  if(mr.isReadable == 1 && mr.isWriteable == 0)
    r2TotSize += (mr.endAddr - mr.startAddr);
  else if(mr.isReadable == 1 && mr.isWriteable == 1)
    rw2TotSize += (mr.endAddr - mr.startAddr);
}

void readFile()
{
  FILE *in;
  struct MemoryRegion readMR;
  in = fopen ("MemoryRegion.dat", "rb");
  while(fread(&readMR, sizeof(struct MemoryRegion), 1, in))
  {
    printf("%p\t\t%p\t\t%d\t\t%d\t\t%d\n", readMR.startAddr, readMR.endAddr, readMR.isReadable,
  readMR.isWriteable, readMR.isExecutabl);
    calcMR(readMR);
  }
  fclose(in);
}

int main(int argc, char const *argv[]) {
  readFile();
  printf("Total Read-Only Memory Size: %p\nTotal Read-Write Memory Size: %p\n", r2TotSize, rw2TotSize);
}
