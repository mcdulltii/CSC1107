#include "structs.h"

#ifndef CSC1107_7_KERNEL
#define CSC1107_7_KERNEL

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "qn1"   /* Dev name as it appears in /proc/devices   */
#define BUF_LEN 64              /* Max length of the message from the device */

/*
 * Global variables are declared as static, so are global within the file.
 */
static userspace_t userspace;
static hash_result_t hash_result;
static int Major;               /* Major number assigned to our device driver */
static int Device_Open = 0;     /* Is device open?
                                 * Used to prevent multiple access to device */

static struct class *cls;

static struct file_operations qn1_fops = 
{
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

#endif
