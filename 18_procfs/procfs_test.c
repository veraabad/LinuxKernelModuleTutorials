#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

static int major;
static struct proc_dir_entry *proc_folder;
static struct proc_dir_entry *proc_file;

static ssize_t my_read(struct file *File, char *user_buffer, size_t count, loff_t *offs)
{
    char text[] = "Hello from a procfs file!\n";
    int to_copy, not_copied, delta;
    pr_info("procfs_test - read callback called\n");

    to_copy = min(count, sizeof(text));

    not_copied = copy_to_user(user_buffer, text, to_copy);

    delta = to_copy - not_copied;

    return delta;
}

static ssize_t my_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{
    char text[255];
    int to_copy, not_copied, delta;
    pr_info("procfs_test - write callback called\n");

    memset(text, 0, sizeof(text));

    to_copy = min(count, sizeof(text));

    not_copied = copy_from_user(text, user_buffer, to_copy);
    pr_info("procfs_test - You have written %s to me\n", text);

    delta = to_copy - not_copied;

    return delta;
}

static struct proc_ops fops = {
    .proc_write = my_write,
    .proc_read = my_read,
};

static int __init my_init(void)
{
    proc_folder = proc_mkdir("hello", NULL);
    if (proc_folder == NULL) {
        pr_err("procfs_test - Error creating /proc/hello\n");
        return -ENOMEM;
    }

    proc_file = proc_create("dummy", 0666, proc_folder, &fops);
    if (proc_file == NULL) {
        pr_err("procfs_test - Error creating /proc/hello/dummy\n");
        proc_remove(proc_folder);
        return -ENOMEM;
    }

    pr_info("procfs_test - Created /proc/hello/dummy\n");

    return 0;
}

static void __exit my_exit(void)
{
    pr_info("procfs_test - Removing /proc/hello/dummy\n");
    proc_remove(proc_file);
    proc_remove(proc_folder);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A simple example for threads in a LKM");
