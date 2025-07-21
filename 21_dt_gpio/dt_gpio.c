#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>
#include <linux/proc_fs.h>

static int dt_probe(struct platform_device *pdev);
static void dt_remove(struct platform_device *pdev);

static struct of_device_id my_driver_ids[] = {
    {
        .compatible = "brightlight,mydev",
    }, { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, my_driver_ids);

static struct platform_driver my_driver = {
    .probe = dt_probe,
    .remove = dt_remove,
    .driver = {
        .name = "my_device_driver",
        .of_match_table = my_driver_ids,
    },
};

static struct gpio_desc *my_led = NULL;
static struct proc_dir_entry *proc_file;

static ssize_t my_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{
    switch (user_buffer[0]) {
        case '0':
        case '1':
            gpiod_set_value(my_led, user_buffer[0] - '0');
        default:
            break;
    }
    return count;
}

static struct proc_ops fops = {
    .proc_write = my_write,
};

static int dt_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    const char *label;
    int my_value, ret;

    if (!device_property_present(dev, "label")) {
        pr_err("dt_gpio - Error: Device property 'label' not found!\n");
        return -1;
    }
    if (!device_property_present(dev, "my_value")) {
        pr_err("dt_gpio - Error: Device property 'my_value' not found!\n");
        return -1;
    }
    if (!device_property_present(dev, "green-led-gpio")) {
        pr_err("dt_gpio - Error: Device property 'green-led-gpio' not found!\n");
        return -1;
    }

    ret = device_property_read_string(dev, "label", &label);
    if (ret) {
        pr_err("dt_gpio - Error: Could not read 'label'\n");
    }
    pr_info("dt_gpio - label: %s\n", label);

    ret = device_property_read_u32(dev, "my_value", &my_value);
    if (ret) {
        pr_err("dt_gpio - Error: Could not read 'my_value'\n");
    }
    pr_info("dt_gpio - my_value: %d\n", my_value);

    my_led = gpiod_get(dev, "green-led", GPIOD_OUT_LOW);
    if (IS_ERR(my_led)) {
        pr_err("dt_gpio - Error: could not setup the GPIO\n");
        return -1 * IS_ERR(my_led);
    }

    proc_file = proc_create("my-led", 0666, NULL, &fops);
    if (proc_file == NULL) {
        pr_err("dt_gpio - Error creating /proc/my-led\n");
        gpiod_put(my_led);
        return -ENOMEM;
    }

    pr_info("dt_gpio - Created /proc/my-led\n");
    return 0;
}

static void dt_remove(struct platform_device *pdev)
{
    pr_info("dt_gpio - Now I am in the remove function\n");
    gpiod_put(my_led);
    proc_remove(proc_file);
    return;
}

static int __init my_init(void)
{
    pr_info("dt_gpio - Loading the driver...\n");
    if (platform_driver_register(&my_driver)) {
        pr_err("dt_gpio - Error: Could not load driver\n");
        return -1;
    }
    return 0;
}

static void __exit my_exit(void)
{
    pr_info("sysfs_test - Unload driver\n");
    platform_driver_unregister(&my_driver);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A simple LKM to parse teh device tree fora  specific device and its properties");
