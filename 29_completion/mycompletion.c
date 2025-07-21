#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list my_timer;
static struct completion comp;

void timer_callback(struct timer_list *data)
{
    pr_info("mycompletion - Timer expired\n");
    complete(&comp);
}

static int __init my_init(void)
{
    int status;
    pr_info("mycompletion - Hello, Kernel!\n");

    timer_setup(&my_timer, timer_callback, 0);

    init_completion(&comp);

    pr_info("mycompletion - Start the timer the first time. Time 40ms\n");
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(40));
    status = wait_for_completion_timeout(&comp, msecs_to_jiffies(100));
    if (!status) {
        pr_warn("mycompletion - Completion timed out!");
    } else {
        pr_info("mycompletion - Completion was successful\n");
    }

    reinit_completion(&comp);
    pr_info("mycompletion - Start the timer the first time. Time 400ms\n");
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(400));
    status = wait_for_completion_timeout(&comp, msecs_to_jiffies(100));
    if (!status) {
        pr_warn("mycompletion - Completion timed out!");
    } else {
        pr_info("mycompletion - Completion was successful\n");
    }

    return 0;
}

static void __exit my_exit(void)
{
    pr_info("mycompletion - Goodbye, Kernel!\n");
    del_timer(&my_timer);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A simple LKM for completions");
