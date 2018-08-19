#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

// __class_create is available only with GPL license
MODULE_LICENSE("GPL");

int major_number = 0;
const char device_name[] = "indicator";
const char class_name[] = "ind_class";
static struct class*  indicator_class  = NULL; ///< The device-driver class struct pointer
static struct device* indicator_device = NULL; ///< The device-driver device struct pointer

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init hello_init(void)
{

    // dinamicly allocate major number
    major_number = register_chrdev(0, device_name, &fops);
    if( major_number < 0 )
    {
        printk(KERN_ALERT "Failed to register\n");
        return major_number;

    }
    printk(KERN_INFO "Registered successful with major number: %d\n", major_number);

    // register the device class
    indicator_class = class_create(THIS_MODULE, class_name);
    if (IS_ERR(indicator_class))
    {
        unregister_chrdev(major_number, device_name);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(indicator_class); // Correct way to return an error on a pointer
    }
    printk(KERN_INFO "Device class registered correctly\n");

    // Register the device driver
    indicator_device = device_create(indicator_class, NULL, MKDEV(major_number, 0), NULL, device_name);
    if (IS_ERR(indicator_device))
    {
        class_destroy(indicator_class);
        // Repeated code but the alternative is goto statements
        unregister_chrdev(major_number, device_name);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(indicator_device);
    }
    printk(KERN_INFO "Device class created correctly\n"); // Made it! device was initialized

    return 0;
}

static void __exit hello_exit(void)
{
    device_destroy(indicator_class, MKDEV(major_number, 0));    // remove the device
    class_unregister(indicator_class);                          // unregister the device class
    class_destroy(indicator_class);                             // remove the device class
    unregister_chrdev(major_number, device_name);               // unregister the major number
    printk(KERN_INFO "Goodbye from the indicator driver!\n");
}


static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "Device relesed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offst)
{
    printk(KERN_INFO "\n");
    return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offst)
{
    printk(KERN_INFO "Received message: %s\n", buffer);
    return 0;
}

module_init(hello_init);
module_exit(hello_exit);
