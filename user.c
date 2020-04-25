#include <linux/module.h>
#include <linux/init.h>
#include <linux/kermal.h>

// Initialization of module
int __init init_MyKernelModule(void)
{
  printk("Process List Lookup Module Init.\n");
  print_task_by_name();
  return 0;
}

// Exit of module
void __exit exit_MyKernelModule(void)
{
  printk("Process List Lookup Module Exit.\n");
  return;
}
