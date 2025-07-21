#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include "ioctl_test.h"

static int major;

static int driver_open(struct inode *device_file, struct file *instance)
{
    pr_info("ioctl_example - open was called \n");
    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance)
{
    pr_info("ioctl_example - close was called\n");
    return 0;
}

int32_t answer = 42;

static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg)
{
    struct mystruct test;
    switch(cmd) {
        case WR_VALUE:
            if (copy_from_user(&answer, (int32_t *) arg, sizeof(answer))) {
                pr_err("ioctl_example - Error copying data from user!\n");
            }
            pr_info("ioctl_example - Update the answer to %d\n", answer);
            break;
        case RD_VALUE:
            if (copy_to_user((int32_t *) arg, &answer, sizeof(answer))) {
                pr_err("ioctl_example - Error copying data to user!\n");
            }
            pr_info("ioctl_example - The answer was copied\n");
            break;
        case GREETER:
            if (copy_from_user(&test, (struct mystruct *) arg, sizeof(test))) {
                pr_err("ioctl_example - Error copying data to user!\n");
            }
            pr_info("ioctl_example - %d greets to %s\n", test.repeat, test.name);
            break;
        default:
            break;
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .unlocked_ioctl = my_ioctl
};

static int __init my_init(void)
{
    major = register_chrdev(0, "hello_cdev", &fops);
    if (major < 0) {
      pr_err("hello_cdev - Error registering chrdev\n");
      return major;
    }
    pr_info("hello_cdev - Major Device Number: %d\n", major);
    return 0;
}

static void __exit my_exit(void)
{
  unregister_chrdev(major, "hello_cdev");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A simple example for ioctl in a LKM");
