#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/kobject.h>    // Using kobjects for the sysfs bindings
#include <linux/kthread.h>    // Using kthreads for the flashing functionality
#include <linux/delay.h>      // Using this header for the msleep() functio

// __class_create is available only with GPL license
MODULE_LICENSE("GPL");

int major_number = 0;
const char device_name[] = "indicator";
const char class_name[] = "ind_class";
static struct class*  indicator_class  = NULL; ///< The device-driver class struct pointer
static struct device* indicator_device = NULL; ///< The device-driver device struct pointer

struct task_struct *flashing_thread;
const int flashing_interval = 10;

const int pins_number = 12;
const int gpio_pins[] = { 24, 25, 12, 13, 17, 18,
                          27, 22, 23, 05, 06, 19 };

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

static int flash(void* data)
{
    int i, j;

    while(1)
    {
        if(kthread_should_stop()) {
            do_exit(0);
        }

        for(i = 0; i < 4; ++i) {

            for(j = 0; j < 8; ++j) {
                 gpio_set_value(gpio_pins[j+4], pin[i][j]);
            }

            gpio_set_value(gpio_pins[i], 1);
            msleep(flashing_interval);
            gpio_set_value(gpio_pins[i], 0);

        }
    }

    return 0;
}

static int __init hello_init(void)
{
    int i = 0;

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
    
    // Gpio init
    for(i = 0; i < 12; ++i)
    {
        gpio_request(gpio_pins[i], "sysfs");
        gpio_export(gpio_pins[i], false);
        gpio_direction_output(gpio_pins[i], 0);
        gpio_set_value(gpio_pins[i], 0);
    }
    printk(KERN_INFO "GPIO pins are initialized\n");

    // init flashing thread
    flashing_thread = kthread_run(flash, NULL, "flashing_thread");
    if (IS_ERR(flashing_thread))
    {
        printk(KERN_ALERT "Failed to create flashing thread");
        return PTR_ERR(flashing_thread);
    }

    return 0;
}

static void __exit hello_exit(void)
{
    int i = 0;

    kthread_stop(flashing_thread);

    device_destroy(indicator_class, MKDEV(major_number, 0));    // remove the device
    class_unregister(indicator_class);                          // unregister the device class
    class_destroy(indicator_class);                             // remove the device class
    unregister_chrdev(major_number, device_name);               // unregister the major number

    for(i = 0; i < 12; ++i)
    {
        gpio_unexport(gpio_pins[i]);
        gpio_free(gpio_pins[i]);
    }

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
