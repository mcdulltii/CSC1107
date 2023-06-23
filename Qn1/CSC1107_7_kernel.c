/*
 *  CSC1107_7_kernel.c: Creates a read-only char device that says how many times
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
#include <crypto/hash.h>
#include <linux/string.h>
#include "CSC1107_7_kernel.h"

// convert_to_hex: convert a string to hex and store in hex_str
void convert_to_hex(const char *str, char *hex_str, size_t hex_str_size) {
    size_t i;

    for (i = 0; i < hex_str_size - 1; i += 2) {
        snprintf(hex_str + i, hex_str_size - i, "%02x", str[i / 2]);
    }

    hex_str[hex_str_size - 1] = '\0';
}


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
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
    if (Device_Open)
        return -EBUSY;

    Device_Open++;

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
    if (copy_to_user(buffer, &hash_result, sizeof(hash_result_t)))
    {
        return -EFAULT;
    }

    return sizeof(hash_result);
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/hello
 */
static ssize_t device_write(struct file *filp,
                            const char *buff,
                            size_t len,
                            loff_t * off)
{
    size_t bytes_not_copied = 0;
    char user_space_hex_string[BUF_LEN * 2 + 1];
    char kernel_space_hex_string[BUF_LEN * 2 + 1];
    struct shash_desc* desc;
    struct crypto_shash* algorithm;
    char* hash;
    int err;

    // Clear user_space_hex_string
    memset(user_space_hex_string, 0, BUF_LEN * 2 + 1);

    bytes_not_copied = copy_from_user(&userspace, buff, len);
    if(bytes_not_copied != 0){
        printk(KERN_INFO "Error copying from user space\n");
        return -EFAULT;
    }

    // Print the received message in the kernel log
    printk(KERN_INFO "Received message from user space: %s\n", userspace.original_sentence);

    // Convert the sentence to hex and store in user_space_hex_string
    convert_to_hex(userspace.hashed_sentence, user_space_hex_string, userspace.digest_length * 2 + 1);

    // Print hashed sentence as long in kernel log
    printk(KERN_INFO "Hashed sentence from user space: %s\n", user_space_hex_string);

    switch(userspace.hash_type) {
		case md5: hash = "md5"; break;
		case sha1: hash = "sha1"; break;
		case sha256: hash = "sha256"; break;
        case sha384: hash = "sha384"; break;
        case sha512: hash = "sha512"; break;
		default: pr_alert("%s: hashing algorithm not recognized\n", DEVICE_NAME); return -EFAULT;
	}

    printk(KERN_INFO "Hashing Function: %s\n", hash);

    // Hash the sentence
    algorithm = crypto_alloc_shash(hash, 0, 0);

    desc = kmalloc(sizeof(struct shash_desc) + crypto_shash_descsize(algorithm), GFP_KERNEL);
    desc->tfm = algorithm;
    err = crypto_shash_init(desc);
    err = crypto_shash_update(desc, userspace.original_sentence, strlen(userspace.original_sentence));
    
    // Clear hash_result.hashed_sentence and store the hashed sentence in hash_result struct
    memset(hash_result.hashed_sentence, 0, BUF_LEN);
    err = crypto_shash_final(desc, hash_result.hashed_sentence);

    crypto_free_shash(algorithm);
    kfree(desc);

    // clear kernel_space_hex_string
    memset(kernel_space_hex_string, 0, BUF_LEN * 2 + 1);

    convert_to_hex(hash_result.hashed_sentence, kernel_space_hex_string, userspace.digest_length * 2 + 1);

    // Print hashed sentence as long in kernel log
    printk(KERN_INFO "Hashed sentence from kernel space: %s\n", kernel_space_hex_string);

    if(strcmp(user_space_hex_string, kernel_space_hex_string) == 0){
        printk(KERN_INFO "Hashes match\n");
        hash_result.match = 1;
    } else {
        printk(KERN_INFO "Hashes do not match\n");
        hash_result.match = 0;
    }

    return len;  // Return the number of bytes written
}

MODULE_LICENSE("GPL");                  	// The license type 
MODULE_DESCRIPTION("qn1 Module"); // The description of the module
MODULE_AUTHOR("Group 7");                     // The author of the module
MODULE_VERSION("0.1a");                  	// The version of the module

