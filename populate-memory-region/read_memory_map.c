/*
To run:
  gcc 1.c -o one
  sudo ./one <PID>
    where the sample <PID> is 1
  OR
  sudo ./one
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

struct MemoryRegion
{
  void *startAddr;
  void *endAddr;
  int isReadable;
  int isWriteable;
  int isExecutable;
};

void *rTotSize, *rwTotSize;

void getFileName(int argc, char const *argv[], char *filename)
{
  char pidStr[20];
  strcpy(filename, "/proc/");
  if(argc == 2)
  {
    strcat(filename, argv[1]);
  }
  else if(argc < 2)
  {
    pid_t pid = getpid();
    sprintf(pidStr, "%d", pid);
    strcat(filename, pidStr);
  }
  strcat(filename, "/maps");
}

void writeStructToFile(struct MemoryRegion mr)
{
  FILE *out;
  out = fopen ("MemoryRegion.dat", "ab+");
  fwrite (&mr, sizeof(struct MemoryRegion), 1, out);
  fclose(out);
}

void calcMR(struct MemoryRegion mr)
{
  if(mr.isReadable == 1 && mr.isWriteable == 0)
    rTotSize += (mr.endAddr - mr.startAddr);
  else if(mr.isReadable == 1 && mr.isWriteable == 1)
    rwTotSize += (mr.endAddr - mr.startAddr);
}

void populateStruct(char *filename)
{
  FILE *maps;
  char *line=0;
	size_t line_n=0;
  struct MemoryRegion mr;
  char rwe[5];

  maps = fopen(filename, "r");
  remove("MemoryRegion.dat");
  while (1) {
		int ret = getline(&line, &line_n, maps);
    if(ret == -1)
      break;
    sscanf(line, "%p-%p %s", &mr.startAddr, &mr.endAddr, rwe);
    if(rwe[0] == 'r')
      mr.isReadable = 1;
    else
      mr.isReadable = 0;
    if(rwe[1] == 'w')
      mr.isWriteable = 1;
    else
      mr.isWriteable = 0;
    if(rwe[2] == 'x')
      mr.isExecutable = 1;
    else
      mr.isExecutable = 0;
    writeStructToFile(mr);
    calcMR(mr);

  }
  printf("%p\t%p\n", rTotSize, rwTotSize);
  fclose(maps);
}

int main(int argc, char const *argv[]) {
  char filename[50];
  getFileName(argc, argv, filename);
  populateStruct(filename);
  return 0;
}
