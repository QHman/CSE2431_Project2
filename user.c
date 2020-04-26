#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

char proc_buffer[1000];
int proc_buffer_size = 0

static int proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "Hello proc!\n");
  return 0;
}

static int proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, proc_show, NULL);
}

int proc_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data){
	int len;

	if (offset > 0) {
		/* we have finished to read, return 0 */
		len  = 0;
	} else {
		/* fill the buffer, return the buffer size */
		memcpy(buffer, proc_buffer, proc_buffer_size);
		len = procfs_buffer_size;
	}

	return ret;
}
static const struct file_operations hello_proc_fops = {
  .owner = THIS_MODULE,
  .open = proc_open,
  .read = proc_read,
  .release = single_release,
};

static int __init proc_init(void) {
  proc_create("hello_proc", 0, NULL, &hello_proc_fops);
  return 0;
}

static void __exit proc_exit(void) {
  remove_proc_entry("hello_proc", NULL);
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_exit);
