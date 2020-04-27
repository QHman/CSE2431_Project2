
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char* argv[])
{
  //Hijacked calls, ran to test
  write(1, "This will be output to standard out\n", 36);
  open("exfile.txt", O_WRONLY | O_APPEND);


  FILE *filecat;
  char data[150];
  //Open pipe to grab the data in write
  filecat = popen("cat /proc/proc_mal_write", "r");
  if (filecat == NULL){
    printf("Read file error \n");
    exit(1);
  }
  printf("Stolen Data Write: ");
  //This iterates through the data grabed in /proc/proc_mal_write
  //And just prints it out. Other more malisious things could be done
  while(fgets(data, 150, filecat) != NULL){
    printf("%s", data);
  }
  if (pclose(filecat) == -1){
    printf("Close file error \n");
    exit(1);
  }

  //Repeat for open
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
  return 0;

}
