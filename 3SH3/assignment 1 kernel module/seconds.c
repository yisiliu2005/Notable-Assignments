/**
 * hello.c
 *
 * Kernel module that communicates with /proc file system.
 * 
 * Samkith jain September 6, 2024
 * Comp Sci 3SH3, Fall 2024
 * Reference:  hello.c Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128

#define PROC_NAME "seconds"
//#define MESSAGE "Hello World\n"

/**
 * My Linux distribution information:
 * yisi@yisi:~/Desktop/newKernel/newKernel$ lsb_release -a
 * No LSB modules are available.
 * Distributor ID: Ubuntu
 * Description:    Ubuntu 24.04.3 LTS
 * Release:        24.04
 * Codename:       noble
 * yisi@yisi:~/Desktop/newKernel/newKernel$ uname -r
 * 6.8.0-79-generic
 */

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static const struct proc_ops my_proc_ops = {
        .proc_read = proc_read,
};

static unsigned long start_jiffies;

/* This function is called when the module is loaded. */
int proc_init(void)
{
        start_jiffies = jiffies;
        // creates the /proc/hello entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &my_proc_ops);

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {

        // removes the /proc/hello entry
        remove_proc_entry(PROC_NAME, NULL);

        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/hello is read.
 * 
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        unsigned long jiffies_since = jiffies - start_jiffies;
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;

        unsigned long seconds = jiffies_to_msecs(jiffies_since)/1000;
        rv = sprintf(buffer, "Number of seconds since module loaded: %ld\n", seconds);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Module");
MODULE_AUTHOR("SGG");

