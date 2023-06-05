/*
 *  chardev.c: Creates a read-only char device that says how many times
 *  you've read from the dev file. Write operation will be denied. 
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/poll.h>
#include <linux/cdev.h>


/*
 *  Prototypes - this would normally go in a .h file
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "qn1"   /* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80              /* Max length of the message from the device */

/*
 * Global variables are declared as static, so are global within the file.
 */

static int Major;               /* Major number assigned to our device driver */
static int Device_Open = 0;     /* Is device open?
                                 * Used to prevent multiple access to device */
static char msg[BUF_LEN];       /* The msg the device will give when asked */
static char *msg_Ptr;
static int msg_size;

static struct class *cls;

static struct file_operations qn1_fops = 
{
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

// This function is called when the module is loaded
int init_module(void)
{
    Major = register_chrdev(0, DEVICE_NAME, &qn1_fops);

    if (Major < 0) 
    {
        // pr_alert: printk with KERN_ALERT an alert-level message; 
        pr_alert("Registering char device failed with %d\n", Major);
        return Major;
    }

    //pr_info: printk with KERN_INFO loglevel, an info-level message
    pr_info("I was assigned major number %d.\n", Major);
    printk(KERN_INFO "I was assigned major number %d.\n", Major);

    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);

    //pr_info: printk with KERN_INFO loglevel, an info-level message
    pr_info("Device created on /dev/%s\n", DEVICE_NAME);

    return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
    device_destroy(cls, MKDEV(Major, 0));
    class_destroy(cls);

    /*
     * Unregister the device
     */
    unregister_chrdev(Major, DEVICE_NAME);
}

/*
 * Methods
 */

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
    if (Device_Open)
        return -EBUSY;

    Device_Open++;

    // sprintf(msg, "It is %d:%d:%d on %d %d %d now.\n", hr, min, sec, d, m, y);
    // sprintf(msg, "%d\n", total_secs);
    // printk(KERN_INFO "%s\n",msg);
    msg_Ptr = msg;

    try_module_get(THIS_MODULE);

    return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
    Device_Open--;          /* We're now ready for our next caller */

    /*
     * Decrement the usage count, or else once you opened the file, you'll
     * never get get rid of the module.
     */
    module_put(THIS_MODULE);

    return SUCCESS;
}

/*
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp,   /* see include/linux/fs.h   */
                           char *buffer,        /* buffer to fill with data */
                           size_t length,       /* length of the buffer     */
                           loff_t * offset)
{


    /*
     * Number of bytes actually written to the buffer
     */
    int bytes_read = 0;

    /*
     * If we're at the end of the message,
     * return 0 signifying end of file
     */
    if (*msg_Ptr == 0)
        return 0;

    /*
     * Actually put the data into the buffer
     */
    while (length && *msg_Ptr) {

        /*
         * The buffer is in the user data segment, not the kernel
         * segment so "*" assignment won't work.  We have to use
         * put_user which copies data from the kernel data segment to
         * the user data segment.
         */
        put_user(*(msg_Ptr++), buffer++);

        length--;
        bytes_read++;
    }

    /*
     * Most read functions return the number of bytes put into the buffer
     */
    return bytes_read;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/hello
 */
static ssize_t device_write(struct file *filp,
                            const char *buff,
                            size_t len,
                            loff_t * off)
{
    // Clear the message buffer before writing
    memset(msg, 0, BUF_LEN);

    // Copy the data from user space to kernel space
    if (copy_from_user(msg, buff, len) != 0) {
        return -EFAULT;  // Error occurred while copying data
    }

    // Store the size of the message
    msg_size = strlen(buff);

    // Print the received message in the kernel log
    printk(KERN_INFO "Received message from user space: %s\n", msg);

    return len;  // Return the number of bytes written
}

MODULE_LICENSE("GPL");                  	// The license type 
MODULE_DESCRIPTION("Read qn1 Module"); // The description of the module
MODULE_AUTHOR("week2");                     // The author of the module
MODULE_VERSION("0.1a");                  	// The version of the module
