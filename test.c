
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char* argv[])
{
  write(1, "This will be output to standard out\n", 36);
  int file_ex = open("exfile.txt", O_WRONLY | O_APPEND);
  FILE *filecat;
  char data[150];
  filecat = popen("cat /proc/proc_mal_write", "r");
  if (filecat == NULL){
    printf("Read file error \n");
    exit(1);
  }
  printf("Stolen Data Write: ");
  while(fgets(data, 150, filecat) != NULL){
    printf("%s", data);
  }
  if (pclose(filecat) == -1){
    printf("Close file error \n");
    exit(1);
  }

  filecat = popen("cat /proc/proc_mal_open", "r");
  if (filecat == NULL){
    printf("Read file error \n");
    exit(1);
  }
  printf("Stolen Data Open: ");
  while(fgets(data, 150, filecat) != NULL){
    printf("%s", data);
  }
  if (pclose(filecat) == -1){
    printf("Close file error \n");
    exit(1);
  }
  close(file_ex);
  return 0;

}
