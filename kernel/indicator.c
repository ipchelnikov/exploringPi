#include "indicator.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/kobject.h>    // Using kobjects for the sysfs bindings
#include <linux/kthread.h>    // Using kthreads for the flashing functionality
#include <linux/delay.h>      // Using this header for the msleep() functio
#include <linux/mutex.h>
#include <linux/string.h>     // for char string to digit conversion


// __class_create is available only with GPL license
MODULE_LICENSE("GPL");

int major_number = 0;
const char device_name[] = "indicator";
const char class_name[] = "ind_class";
static struct class*  indicator_class  = NULL; ///< The device-driver class struct pointer
static struct device* indicator_device = NULL; ///< The device-driver device struct pointer

struct task_struct *flashing_thread;
const int flashing_interval = 1000;

const int pins_number = 12;
const int gpio_pins[] = { 25, 12, 13, 24, 17, 18,
                          27, 22, 23, 05, 06, 19 };

// GPIO_13 - low left
// GPIO_17 - bottom
// GPIO_18 - dot
// GPIO_27 - low right
// GPIO_22 - middle
// GPIO_23 - top
// GPIO_05 - hi left
// GPIO_06 - hi right

// first ten are digest, 0xFF - all disabled
const unsigned int digit_pin_values[12] = { 0x14, 0x77, 0x4c, 0x45, 0x27, 0x85, 0x84, 0x57, 0x4, 0x5, 0xFF, 0xE8 };

const int EMPTY     = 10;
const int c_symbol  = 11;

struct mutex digits_mutex;
int digits[4] = {10, 10, 10, 10};

static int flash(void* data)
{
    int i = 0, j =0, count = 0;

    while(1)
    {
        if(kthread_should_stop()) {
            do_exit(0);
        }

        for(i = 0; i < 4; ++i) {
            for(j = 0; j < 8; ++j) {
                mutex_lock(&digits_mutex);
                
                gpio_set_value(gpio_pins[j+4],
                        digit_pin_values[digits[i]] & ( 0x1 << j ));

                mutex_unlock(&digits_mutex);
            }

            // Blink the middle dot for time 
            // It is time if the first diget is not EMPTY
            if (digits[0] != EMPTY && i == 1)
            {
                // light / sleep interval - 100 cicles
                if(count < 200)
                    gpio_set_value(27, 0);
                else if(count == 400)
                    count = 0;

                ++count;
            }

            gpio_set_value(gpio_pins[i], 1);
            udelay(flashing_interval);
            gpio_set_value(gpio_pins[i], 0);
        }
    
    }

    return 0;
}

static int __init indicator_init(void)
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
    mutex_init(&digits_mutex);

    return 0;
}

static void __exit indicator_exit(void)
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
    int val = 0;

    printk(KERN_INFO "Received message: %s\n", buffer);

    if(buffer[0] == 't') {

        val = simple_strtol(buffer+1, NULL, 10);
        
        mutex_lock(&digits_mutex);
        
        digits[0] = val/1000;
        val -= 1000*(val/1000);
        
        digits[1] = val/100;
        val -= 100*(val/100);
        
        digits[2] = val/10;
        val -= 10*(val/10);
        
        digits[3] = val;
        
        mutex_unlock(&digits_mutex);
    }
    else if (buffer[0] == 'c') {

        val = simple_strtol(buffer+1, NULL, 10);

        mutex_lock(&digits_mutex);

        digits[0] = EMPTY;      // not used
        digits[1] = val/10;
        digits[2] = val - 10*(val/10);
        digits[3] = c_symbol;   // celsium

        mutex_unlock(&digits_mutex);
    }
    else {
        printk(KERN_ALERT "Message format error\n");
    }

    return 0;
}

module_init(indicator_init);
module_exit(indicator_exit);
