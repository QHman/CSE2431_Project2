#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/proc_fs.h>

struct proc_dir_entry *tlc_proc_a;
char proc_buffer[100];
int proc_buffer_size = 0;

static ssize_t proc_read(struct file *fp, char *buf, size_t len, loff_t * off)
{
  static int finished=0;
  if(finished) {
    finished = 0;
    return 0;
  }
  finished = 1;
		memcpy(buffer, proc_buffer, proc_buffer_size);

	return proc_buffer_size;
}

static struct file_operations proc_fops = {
  .owner= THIS_MODULE,
  .read= proc_read
};

static int hello_init(void)
{
  proc_create("tlc_proc_a", 0444, NULL, &proc_fops);
  proc_buffer[proc_buffer_size] = '!';
  proc_buffer_size += 1;
  return 0;
}

static void hello_exit(void)
{
  remove_proc_entry("tlc_proc_a", NULL);
}

module_init(hello_init);
module_exit(hello_exit);
