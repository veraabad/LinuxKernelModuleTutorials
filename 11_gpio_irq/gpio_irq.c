#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

unsigned int irq_number;

static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
    pr_info("gpio_irq: Interrup was triggered and ISR was called\n");
    return (irq_handler_t) IRQ_HANDLED;
}

static int __init my_init(void)
{
    pr_info("gpio_irq: Loading module...\n");

    if(gpio_request(17, "rpi-gpio-17")) {
        pr_err("Can not allocate GPIO 17\n");
        return -1;
    }

    if(gpio_direction_input(17)) {
        pr_err("Can not set GPIO 17 to input\n");
        gpio_free(17);
        return -1;
    }

    irq_number = gpio_to_irq(17);

    if(request_irq(irq_number, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0) {
        pr_err("Can not request interrup number: %d\n", irq_number);
        gpio_free(17);
        return -1;
    }

    pr_info("gpio_irf: Done!\n");
    pr_info("GPIO 17 is mapped to IRQ number: %d\n", irq_number);
    return 0;
}

static void __exit my_exit(void)
{
    pr_info("gpio_irq - Unloading module...\n");
    free_irq(irq_number, NULL);
    gpio_free(17);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A simple LKM for a gpio interrup");
