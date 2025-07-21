#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>

struct driver_data {
    uint8_t version;
    char text[64];
};

uint32_t *ptr1;
struct driver_data *ptr2;

static int __init my_init(void)
{
    ptr1 = kmalloc(sizeof(uint32_t), GFP_KERNEL);
    if (ptr1 == NULL) {
        pr_err("alloc_test - Out of memory!\n");
        return -1;
    }
    pr_info("alloc_test - *ptr1: 0x%x\n", *ptr1);
    *ptr1 = 0xC001C0DE;
    pr_info("alloc_test - *ptr1: 0x%x\n", *ptr1);
    kfree(ptr1);

    ptr1 = kzalloc(sizeof(uint32_t), GFP_KERNEL);
    if (ptr1 == NULL) {
        pr_err("alloc_test - Out of memory!\n");
        return -1;
    }
    pr_info("alloc_test - *ptr1: 0x%x\n", *ptr1);
    *ptr1 = 0xC001C0DE;
    pr_info("alloc_test - *ptr1: 0x%x\n", *ptr1);
    kfree(ptr1);

    ptr2 = kzalloc(sizeof(struct driver_data), GFP_KERNEL);
    if (ptr2 == NULL) {
        pr_err("alloc_test - Out of memory!\n");
    }
    ptr2->version = 123;
    strcpy(ptr2->text, "This is a test string for my linux kernel module");

    pr_info("alloc_test - *ptr2->version: 0x%x\n", ptr2->version);
    pr_info("alloc_test - *ptr2->text: %s\n", ptr2->text);

    return 0;
}

static void __exit my_exit(void)
{
    pr_info("alloc_test - *ptr2->version: 0x%x\n", ptr2->version);
    pr_info("alloc_test - *ptr2->text: %s\n", ptr2->text);
    kfree(ptr2);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("Demonstration for dynamic memory management in a LKM");
