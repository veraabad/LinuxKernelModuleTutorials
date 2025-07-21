#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/i2c.h>

static struct i2c_client *adc_client;
static int my_adc_probe(struct i2c_client *client);
static void my_adc_remove(struct i2c_client *client);

static struct of_device_id my_driver_ids[] = {
    {
        .compatible = "brightlight,mydev",
    }, { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, my_driver_ids);

static struct i2c_device_id my_adc[] = {
    {"my_adc", 0},
    { },
};
MODULE_DEVICE_TABLE(i2c, my_adc);

static struct i2c_driver my_driver = {
    .probe = my_adc_probe,
    .remove = my_adc_remove,
    .id_table = my_adc,
    .driver = {
        .name = "my_adc",
        .of_match_table = my_driver_ids,
    },
};

static struct proc_dir_entry *proc_file;

static ssize_t my_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{
    long val;
    if (kstrtol(user_buffer, 0, &val) == 0) {
        i2c_smbus_write_byte(adc_client, (uint8_t)val);
    }
    return count;
}

static ssize_t my_read(struct file *File, char *user_buffer, size_t count, loff_t *offs)
{
    uint8_t adc;
    adc = i2c_smbus_read_byte(adc_client);
    return sprintf(user_buffer, "%d\n", adc);
}

static struct proc_ops fops = {
    .proc_write = my_write,
    .proc_read = my_read,
};

static int my_adc_probe(struct i2c_client *client)
{
    pr_info("dt_i2c - Now I am in the probe function!\n");
    if (client->addr != 0x10) {
        pr_err("dt_i2c - Wrong I2C address!\n");
        return -1;
    }

    adc_client = client;

    proc_file = proc_create("myadc", 0666, NULL, &fops);
    if (proc_file == NULL) {
        pr_err("dt_i2c - Error creating /proc/myadc\n");
        return -ENOMEM;
    }

    pr_info("dt_i2c - Created /proc/myadc\n");
    return 0;
}

static void my_adc_remove(struct i2c_client *client)
{
    pr_info("dt_i2c - Now I am in the remove function\n");
    proc_remove(proc_file);
    return;
}

module_i2c_driver(my_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A driver for my simple ATMEGA I2C ADC");
