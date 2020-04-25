/*
  A kernel module that monitors the OS processes and their system calls. Print when loading the module.
  In the terminal, compile this module with "make" and load it with "sudo insmod ptrace.ko". 
  Check the result of printk by “dmesg”
  Unload it with "sudo rmmod process.ko".
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h> // task_struct definition
#include <asm/unistd.h>
#include <linux/list.h>
#include <linux/init_task.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

#ifndef __KERNEL__
#define __KERNEL__
#endif

// Monitor system calls and print them for each process
static void monitor_syscalls(void) {

  // The struct used for info of a process
  struct task_struct* task;

  // Go over through the list of processes
  for_each_process(task) {
    
    printk("Current process: %s, PID: %d\n", task->comm, task->pid);
  
    pid_t child = fork();
    
    if (child == 0) {
      return execute_child(task->comm);
    } else {
      return trace_child(child);
    }  
  }

}

static int execute_child(char **process) {
  ptrace(PTRACE_TRACEME);
  kill(getpid(), SIGSTOP);
  return execvp(process[0], process);
}

static int wait_for_syscall(pid_t child) {
  int status;
 
  while(1) {
    
    ptrace(PTRACE_SYSCALL, child, 0, 0);
    waitpid(child, &status, 0);

    if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80)
      return 0;

    if (WIFECITED(status))
      return 1;
  }

}

static int trace_child(pid_t child) {
  int status, syscall;

  waitpid(child, &status, 0);
  ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);
  
  while(1) {

    if (wait_for_syscall(child) != 0)
       break;

    syscall = ptrace(PTRACE_PEEKUSER, child, sizeof(long) * ORIG_EAX);
    
    printk((KERN_INFO "System Call: %d\n", syscall);
  }

  return 0;
}


// Initialization of module
static int init_syscall_monitoring(void)
{
  printk(KERN_INFO "System call monitoring loaded.\n");
  monitor_syscalls();
  return 0;
}

// Exit of module
static void exit_syscall_monitoring(void)
{
  printk((KERN_INFO "System call monitoring exited.\n");
  return;
}

module_init(init_syscall_monitoring);
module_exit(exit_syscall_monitoring);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A kernel module to monitor system calls of all processes in OS");
