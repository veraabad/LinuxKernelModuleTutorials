#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/jiffies.h>

static struct task_struct *kthread_1;
static struct task_struct *kthread_2;

static int major;
static long int watch_var = 0;

// static waitqueue
DECLARE_WAIT_QUEUE_HEAD(wq1);

// dynamic waitqueue
static wait_queue_head_t wq2;

int thread_function(void *wait_sel)
{
    int selection = *(int *) wait_sel;

    switch(selection) {
        case 1:
            wait_event(wq1, watch_var == 11);
            pr_info("waitqueue - watch_var is now 11!\n");
            break;
        case 2:
            while(wait_event_timeout(wq2, watch_var == 22, msecs_to_jiffies(5000)) == 0) {
                pr_info("waitqueue - watch_var is still not 22, but timeout elapesed!\n");
            }
            pr_info("waitqueue - watch_var is now 22!\n");
            break;
        default:
            break;
    }

    pr_info("waitqueue - Thread monitoring wq%d finished execution!\n", selection);
    return 0;
}

static ssize_t my_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{
    int to_copy, not_copied, delta;
    char buffer[16];
    pr_info("waitqueue - write callback called\n");

    memset(buffer, 0, sizeof(buffer));

    to_copy = min(count, sizeof(buffer));

    not_copied = copy_from_user(buffer, user_buffer, to_copy);

    delta = to_copy - not_copied;

    if (kstrtol(buffer, 10, &watch_var) == -EINVAL) {
        pr_err("waitqueue - Error converting input!\n");
    }
    pr_info("waitqueue - watch is now %ld\n", watch_var);

    wake_up(&wq1);
    wake_up(&wq2);

    return delta;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = my_write
};

static int __init my_init(void)
{
    int t1 = 1, t2 = 2;
    pr_info("waitqueue - Init threads\n");

    // init dynamically created waitqueue
    init_waitqueue_head(&wq2);

    major = register_chrdev(0, "my_dev_nr", &fops);
    if (major < 0) {
        pr_err("waitqueue - Error registering chrdev\n");
        return major;
    }
    pr_info("waitqueue: Major number: %d\n", major);

    kthread_1 = kthread_run(thread_function, &t1, "kthread_1");
    if (kthread_1 != NULL) {
        pr_info("waitqueue - Thread 1 was created and is running now!\n");
    } else {
        pr_err("waitqueue - Thread 1 could not be created!\n");
        unregister_chrdev(major, "my_dev_nr");
        return -1;
    }

    kthread_2 = kthread_run(thread_function, &t2, "kthread_1");
    if (kthread_2 != NULL) {
        pr_info("waitqueue - Thread 2 was created and is running now!\n");
    } else {
        pr_err("waitqueue - Thread 2 could not be created!\n");
        watch_var = 11;
        wake_up(&wq1);
        mdelay(10);
        unregister_chrdev(major, "my_dev_nr");
        return -1;
    }
    pr_info("waitqueue - Both threads are running now!\n");
    return 0;
}

static void __exit my_exit(void)
{
    pr_info("waitqueue - Stop both threads\n");
    watch_var = 11;
    wake_up(&wq1);
    mdelay(10);
    watch_var = 22;
    wake_up(&wq2);
    mdelay(10);
    unregister_chrdev(major, "my_dev_nr");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abad Vera");
MODULE_DESCRIPTION("A simple example for threads in a LKM");
