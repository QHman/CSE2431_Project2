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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrew Cantor & Quinton Hiler");
MODULE_DESCRIPTION("Loadable Kernel Module Syscall");
MODULE_VERSION("0.1");

#define SYS_CALL_TABLE "sys_call_table"
#define SYSCALL_NI __NR_write

static ulong *syscall_table = NULL;

static void *original_syscall = NULL;

asmlinkage int (*old_write)(int fildes, const void *buf, size_t nbytes);

static unsigned long new_write(int fildes, const void *buf, size_t nbytes)
{

	static char buffer[nbytes];	

        if (copy_from_user(buffer, buf, nbytes)) {
                return -EFAULT;
        }
        buffer[nbytes-1] = 0;

        return (*old_write)(int fildes, const void *buf, size_t nbytes);
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
        replace_syscall(SYSCALL_NI, (ulong)new_write);
        return 0;
}

static void cleanup_syscall(void)
{
        page_read_write((ulong)syscall_table);
        syscall_table[SYSCALL_NI] = (ulong)original_syscall;
        page_read_only((ulong)syscall_table);
}

module_init(init_syscall);
module_exit(cleanup_syscall);


MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A kernel module to list process by their names");
