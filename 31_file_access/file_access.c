#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

static int __init my_init(void)
{
    struct file *file;
    char data[128] = "\"Hey, careful, an, there's beverage here!\" - Dude\n";
    ssize_t len;

    file = filp_open("/tmp/dude", O_RDWR | O_CREAT, 0644);

    if (!file) {
        pr_err("file_access - Error opening file\n");
        return -1;
    }

    len = kernel_write(file, data, sizeof(data), &file->f_pos);
    if (len < 0) {
        pr_err("file_access - Error writing to file: %ld\n", len);
        filp_close(file, NULL);
        return len;
    }

    pr_info("file_access - Wrote %ld bytes to file\n", len);

    memset(data, 0, sizeof(data));
    file->f_pos = 0;

    len = kernel_read(file, data, sizeof(data), &file->f_pos);
    if (len < 0) {
        pr_err("file_access - Error reading the file: %ld\n", len);
        filp_close(file, NULL);
        return len;
    }

    pr_info("file_access - Read %ld bytes: '%s'\n", len, data);
    filp_close(file, NULL);

    return 0;
}

static void __exit my_exit(void)
{
    pr_info("file_access - Unloading driver\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("An example for reading and writing to a file in your filesystem from a driver");
