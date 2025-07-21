#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/sched/signal.h>
#include <linux/ioctl.h>

#define IO_BUTTON 17
#define IO_OFFSET 569

static struct gpio_desc *button;

static int major;
unsigned int irq_number;

#define REGISTER_UAPP _IO('R', 'g')
static struct task_struct *task = NULL;

#define SIGNR 44

static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
    struct siginfo info;
    pr_info("gpio_irq: Interrupt was triggered and ISR was called\n");
    if (task != NULL) {
        memset(&info, 0, sizeof(info));
        info.si_signo = SIGNR;
        info.si_code = SI_QUEUE;
        if (send_sig_info(SIGNR, (struct kernel_siginfo *) &info, task) < 0) {
            pr_err("gpio_irq: Error sending signal\n");
        }
    }
    return (irq_handler_t) IRQ_HANDLED;
}

static int my_close(struct inode *device_file, struct file *instance) 
{
    if (task != NULL) {
        task = NULL;
    }
    return 0;
}

static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg)
{
    if (cmd == REGISTER_UAPP) {
        task = get_current();
        pr_info("gpio_irq: Userspace app with PID %d is registered\n", task->pid);
    }
    return 0;

}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .release = my_close,
    .unlocked_ioctl = my_ioctl,
};

static int __init my_init(void)
{
    int status;
    pr_info("gpio_irq: Loading module...\n");

    button = gpio_to_desc(IO_BUTTON + IO_OFFSET);
    if (!button) {
        pr_err("gpio_irq: Error getting pin %d\n", IO_BUTTON);
        return -ENODEV;
    }

    status = gpiod_direction_input(button);
    if (status) {
      pr_err("gpio_irq: Error getting pin %d to input\n", IO_BUTTON);
      return status;
    }

    irq_number = gpiod_to_irq(button);

    if(request_irq(irq_number, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0) {
        pr_err("Can not request interrup number: %d\n", irq_number);
        gpio_free(17);
        return -1;
    }

    major = register_chrdev(0, "gpio_irq_signal", &fops);
    if (major < 0) {
        pr_err("gpio_irq - Error registering chrdev\n");
        free_irq(irq_number, NULL);
        return major;
    }
    pr_info("gpio_irq: Major number: %d\n", major);

    pr_info("gpio_irq: Done!\n");
    pr_info("GPIO %d is mapped to IRQ number: %d\n", IO_BUTTON, irq_number);
    return 0;
}

static void __exit my_exit(void)
{
    pr_info("gpio_irq - Unloading module...\n");
    free_irq(irq_number, NULL);
    unregister_chrdev(major, "gpio_irq_signal");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("LKM which sends a signal to a userpace app when GPIO 17 has a rising edge");
