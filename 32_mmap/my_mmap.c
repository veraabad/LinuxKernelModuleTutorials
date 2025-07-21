#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/page.h>
#include <asm/io.h>

#define DEVNAME "mydev"

static int major;
static void *my_data;

static ssize_t my_read(struct file *file, char __user *user_buffer, size_t len, loff_t *offs)
{
    int not_copied, to_copy = (len > PAGE_SIZE) ? PAGE_SIZE : len;
    not_copied = copy_to_user(user_buffer, my_data, to_copy);
    return to_copy - not_copied;
}

static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *offs)
{
    int not_copied, to_copy = (len > PAGE_SIZE) ? PAGE_SIZE : len;
    not_copied = copy_from_user(my_data, user_buffer, to_copy);
    return to_copy - not_copied;
}

static int my_mmap(struct file *file, struct vm_area_struct *vma)
{
    int status;
    vma->vm_pgoff = virt_to_phys(my_data) >> PAGE_SHIFT;
    status = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, vma->vm_end - vma->vm_start, vma->vm_page_prot);
    if (status) {
        pr_err("my_mmap - Error remap_pfn range: %d\n", status);
        return -EAGAIN;
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
    .mmap = my_mmap,
};

static int __init my_init(void)
{
    pr_info("my_mmap - Hello!\n");
    my_data = kzalloc(PAGE_SIZE, GFP_DMA);
    if (!my_data) {
        return -ENOMEM;
    }
    pr_info("my_mmap - I have allocated a page (%ld Bytes)\n", PAGE_SIZE);
    pr_info("my_mmap - PAGESHIFT: %d\n", PAGE_SHIFT);

    major = register_chrdev(0, DEVNAME, &fops);
    if (major < 0) {
        pr_err("my_mmap - Error registering device number!\n");
        kfree(my_data);
        return major;
    }
    pr_info("my_mmap - Major Device Number: %d\n", major);

    return 0;
}

static void __exit my_exit(void)
{
  if (my_data) {
    kfree(my_data);
  }
  unregister_chrdev(major, DEVNAME);
  pr_info("my_mmap - Goodbye\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A simple implementation for the mmap syscall");
