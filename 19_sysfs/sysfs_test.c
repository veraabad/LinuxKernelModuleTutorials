#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/string.h>

static struct kobject *dummy_kobj;

// Read callback for hello/dummy
static ssize_t dummy_show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    return sprintf(buffer, "You have read from /sys/kernel/%s%s\n", kobj->name, attr->attr.name);
}

// Write callback for hello/dummy
static ssize_t dummy_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    pr_info("sysfs_test - You wrote '%s' to /sys/kernel/%s%s\n", buffer, kobj->name, attr->attr.name);
    return count;
}

static struct kobj_attribute dummy_attr = __ATTR(dummy, 0660, dummy_show, dummy_store);

static int __init my_init(void)
{
    pr_info("sysfs_test - Creating /sys/kernel/hello/dummy\n");

    dummy_kobj = kobject_create_and_add("hello", kernel_kobj);
    if (!dummy_kobj) {
        pr_err("sysfs_test - Error creating /sys/kernel/hello\n");
        return -ENOMEM;
    }

    if (sysfs_create_file(dummy_kobj, &dummy_attr.attr)) {
        pr_err("sysfs_test - Error creating /sys/kernel/hello/dummy\n");
        kobject_put(dummy_kobj);
        return -ENOMEM;
    }

    return 0;
}

static void __exit my_exit(void)
{
    pr_info("sysfs_test - Deleting /sys/kernel/hello/dummy\n");
    sysfs_remove_file(dummy_kobj, &dummy_attr.attr);
    kobject_put(dummy_kobj);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("Creating a folder and a file in sysfs");
