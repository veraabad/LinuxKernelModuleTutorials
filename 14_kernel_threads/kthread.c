#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>

static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1 = 1, t2 = 2;

int thread_function(void *thread_nr)
{
    unsigned int i = 0;
    int t_nr = *(int *)thread_nr;

    while(!kthread_should_stop()) {
        pr_info("kthread - Thread %d is executed! Counter val: %d\n", t_nr, i++);
        msleep(t_nr * 1000);
    }

    pr_info("kthread - Thread %d finished execution!\n", t_nr);
    return 0;
}

static int __init my_init(void)
{
    pr_info("kthread - Init threads\n");
    kthread_1 = kthread_create(thread_function, &t1, "kthread_1");
    if (kthread_1 != NULL) {
        wake_up_process(kthread_1);
        pr_info("kthread - Thread 1 was created and is running now!\n");
    } else {
        pr_err("kthread - Thread 1 could not be created!\n");
        return -1;
    }
    kthread_2 = kthread_run(thread_function, &t2, "kthread_2");
    if (kthread_2 != NULL) {
        pr_info("kthread - Thread 2 was created and is running now!\n");
    } else {
        pr_err("kthread - Thread 2 could not be created!\n");
        kthread_stop(kthread_1);
        return -1;
    }

    pr_info("kthread - Both threads are running now\n");

    return 0;
}

static void __exit my_exit(void)
{
    pr_info("kthread - Stop both threads\n");
    kthread_stop(kthread_1);
    kthread_stop(kthread_2);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A simple example for threads in a LKM");
