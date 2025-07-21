#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/mutex.h>

static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1 = 1, t2 = 2;
static struct mutex lock;

int thread_function(void *thread_nr)
{
    int delay[] = {0, 1000, 500};
    int t_nr = *(int *)thread_nr;

    pr_info("mymutex - Thread %d is executed!\n", t_nr);

    mutex_lock(&lock);

    pr_info("mymutex - Thread %d is in critical section!\n", t_nr);
    msleep(delay[t_nr]);
    pr_info("mymutex - Thread %d is leaving the critical section!\n", t_nr);

    mutex_unlock(&lock);

    pr_info("mymutex - Thread %d finished execution!\n", t_nr);
    return 0;
}

static int __init my_init(void)
{
    pr_info("mymutex - Init threads\n");

    mutex_init(&lock);

    kthread_1 = kthread_create(thread_function, &t1, "kthread_1");
    if (kthread_1 != NULL) {
        wake_up_process(kthread_1);
        pr_info("mymutex - Thread 1 was created and is running now!\n");
    } else {
        pr_err("mymutex - Thread 1 could not be created!\n");
        return -1;
    }
    kthread_2 = kthread_run(thread_function, &t2, "kthread_2");
    if (kthread_2 != NULL) {
        pr_info("mymutex - Thread 2 was created and is running now!\n");
    } else {
        pr_err("mymutex - Thread 2 could not be created!\n");
        kthread_stop(kthread_1);
        return -1;
    }

    pr_info("mymutex - Both threads are running now\n");

    return 0;
}

static void __exit my_exit(void)
{
    pr_info("mymutex - Stop both threads\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A simple example for threads in a LKM");
