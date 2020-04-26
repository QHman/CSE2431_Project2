/*
** test.c for lkm-syscall
**
** Originally made by xsyann
** Contact <contact@xsyann.com>
**
** Current version built by Yuan Xiao
** Contact <xiao.465@osu.edu>
*/

#include <sys/syscall.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define HIJACKED_SYSCALL __NR_write


char buffer[40];
char *buf;
int foo = 43;

int main(int argc, char* argv[])
{
        printf("Call syscall at offset %d\n", HIJACKED_SYSCALL);
        printf("Virtual address @%p\n", &foo);
        snprintf(buffer, sizeof(buffer), "%lx", (size_t)(&foo));
        printf("Syscall return %lx\n", syscall(HIJACKED_SYSCALL, buffer));
        FILE *proc_file = fopen("/proc/syscall_mal","r"):
        if (proc_file == NULL)
        {
          printf("read file error\n");
          exit(1);
        }

        char *fgets(buf, 100, proc_file);
        printf("Stolen Data:%s", buf);
        close(proc_file);
        return 0;
}
