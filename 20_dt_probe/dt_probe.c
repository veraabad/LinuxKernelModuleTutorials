#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

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

static int dt_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    const char *label;
    int my_value, ret;

    if (!device_property_present(dev, "label")) {
        pr_err("dt_probe - Error: Device property 'label' not found!\n");
        return -1;
    }
    if (!device_property_present(dev, "my_value")) {
        pr_err("dt_probe - Error: Device property 'my_value' not found!\n");
        return -1;
    }

    ret = device_property_read_string(dev, "label", &label);
    if (ret) {
        pr_err("dt_probe - Error: Could not read 'label'\n");
    }
    pr_info("dt_probe - label: %s\n", label);

    ret = device_property_read_u32(dev, "my_value", &my_value);
    if (ret) {
        pr_err("dt_probe - Error: Could not read 'my_value'\n");
    }
    pr_info("dt_probe - my_value: %d\n", my_value);

    return 0;
}

static void dt_remove(struct platform_device *pdev)
{
    pr_info("dt_probe - Now I am in the remove function\n");
    return;
}

static int __init my_init(void)
{
    pr_info("dt_probe - Loading the driver...\n");
    if (platform_driver_register(&my_driver)) {
        pr_err("dt_probe - Error: Could not load driver\n");
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
