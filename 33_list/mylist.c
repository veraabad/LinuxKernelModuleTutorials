#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>

struct my_data {
    struct list_head list;
    char text[64];
};

LIST_HEAD(my_list);

static int __init my_init(void)
{
    struct my_data *tmp, *next;
    struct list_head *ptr;

    pr_info("mylist - Hello, Kernel\n");

    tmp = kmalloc(sizeof(struct my_data), GFP_KERNEL);
    strcpy(tmp->text, "Hello World");
    list_add_tail(&tmp->list, &my_list);

    tmp = kmalloc(sizeof(struct my_data), GFP_KERNEL);
    strcpy(tmp->text, "Second Element");
    list_add_tail(&tmp->list, &my_list);

    tmp = kmalloc(sizeof(struct my_data), GFP_KERNEL);
    strcpy(tmp->text, "and the last element");
    list_add_tail(&tmp->list, &my_list);

    list_for_each_prev(ptr, &my_list) {
        tmp = list_entry(ptr, struct my_data, list);
        pr_info("mylist - Element text: %s\n", tmp->text);
    }

    list_for_each_entry_safe(tmp, next, &my_list, list) {
        list_del(&tmp->list);
        kfree(tmp);
    }
    pr_info("mylist - Freeing memory done!\n");
    return 0;
}

static void __exit my_exit(void)
{
    pr_info("mylist - Goodbye, Kernel\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("Demonstration for lists in the kernel");
