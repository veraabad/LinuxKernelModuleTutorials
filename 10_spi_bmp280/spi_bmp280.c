#include <linux/module.h>
#include <linux/init.h>
#include <linux/spi/spi.h>

#define MY_BUS_NUM 0

static struct spi_device *bmp280_dev;

static int __init my_init(void)
{
    struct spi_controller *master;
    uint8_t id;
    uint8_t val[2] = {0x75, 0x40};
    struct spi_board_info spi_device_info = {
        .modalias = "bmp280",
        .max_speed_hz = 1000000,
        .bus_num = MY_BUS_NUM,
        .chip_select = 0,
        .mode = 3,
    };
    master = spi_busnum_to_controller(MY_BUS_NUM);
    if (!master) {
        pr_warn("There is no spi bus with number: %d\n", MY_BUS_NUM);
        return -1;
    }
    bmp280_dev = spi_new_device(master, &spi_device_info);
    if (!bmp280_dev) {
        pr_warn("Could not create device!\n");
        return -1;
    }
    bmp280_dev->bits_per_word = 8;

    if(spi_setup(bmp280_dev) != 0) {
        pr_warn("Could not change bus setup!\n");
        spi_unregister_device(bmp280_dev);
        return -1;
    }

    id = spi_w8r8(bmp280_dev, 0xD0);
    pr_info("Chip ID: 0x%x\n", id);

    spi_write(bmp280_dev, val, sizeof(val));
    id = spi_w8r8(bmp280_dev, 0xF5);
    pr_info("Config Reg. value: 0x%x\n", id);


    pr_info("hello - Hello, Kernel!\n");
    return 0;
}

static void __exit my_exit(void)
{
    if(bmp280_dev) {
        spi_unregister_device(bmp280_dev);
    }
    pr_info("hello - Goodbye, Kernel!\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A simple LKM to read and write some registers of a BMP280 sensor");
