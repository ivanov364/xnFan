/*
 * xnFan.c
*/

// change fan speed in terminal
// echo <0 to 255> > /sys/class/hwmon/hwmon2/pwm1



/*
 *
 *
 * pwm1_enable: pulse width modulation fan control method (0: no fan speed control,
 * 1: manual fan speed control using pwm interface, 2: automatic fan speed control)
    //  https://kernel.org/doc/html/latest/gpu/amdgpu/thermal.html
 1. echo "1" > /sys/class/hwmon/hwmon1/pwm1_enable

 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fcntl.h>
#include <linux/buffer_head.h>


#include <asm/uaccess.h>
#include <asm/segment.h>

#define DEVICE_NAME "case_fan_cooler"
#define CLASS_NAME "case_fan_cooler_class"
#define FAN_SPEED_FILE "/sys/class/hwmon/hwmon1/pwm1"
#define FAN_COLOR_FILE "/sys/class/hwmon/hwmon0/led1_color"

static int major_number, ret;
static struct class* case_fan_cooler_class = NULL;
static struct device* case_fan_cooler_device = NULL;

static int file_write(const char *file_path, const char *text)
{
    struct file *file;
    loff_t pos = 0;

    file = filp_open(file_path, O_WRONLY | O_CREAT, 0644);
    if (IS_ERR(file))
    {
        pr_err("Error opening file: %ld\n", PTR_ERR(file));
        return -1;
    }

    ret = kernel_write(file, text, strlen(text), &pos);
    if (ret < 0)
    {
        pr_err("Error writing to file: %d\n", ret);
        filp_close(file, NULL);
        return -1;
    }

    filp_close(file, NULL);
    return 0;
}

static int case_fan_cooler_open(struct inode* inodep, struct file* filep)
{
    printk(KERN_INFO "case_fan_cooler: Device has been opened\n");
    return 0;
}

static int case_fan_cooler_release(struct inode* inodep, struct file* filep)
{
    printk(KERN_INFO "case_fan_cooler: Device successfully closed\n");
    return 0;
}

static ssize_t case_fan_cooler_read(struct file* filep, char* buffer, size_t len, loff_t* offset)
{
    printk(KERN_INFO "case_fan_cooler: Read operation not supported\n");
    return -EINVAL; // returning negative errno
}

static ssize_t case_fan_cooler_write(struct file* filep, const char* buffer, size_t len, loff_t* offset)
{
    char command[10];
    int value; // 86 avg fan speed

    if (len > sizeof(command))
    {
        printk(KERN_INFO "case_fan_cooler: Invalid command\n");
        return -EINVAL;
    }

    ret = copy_from_user(command, buffer, len);
    if (ret)
    {
        printk(KERN_INFO "case_fan_cooler: Failed to copy command from user\n");
        return -EFAULT;
    }

    command[len] = '\0';

    if (strcmp(command, "speed") == 0) // speed
    {
        ret = kstrtoint(command, 10, &value); // string to int
        if (ret)
        {
            printk(KERN_INFO "case_fan_cooler: Failed to convert speed value to integer\n");
            return -EINVAL;
        }

        value += '0'; // converting integer to char
        ret = file_write(FAN_SPEED_FILE, value);
        if (ret)
        {
            printk(KERN_INFO "case_fan_cooler: Failed to write speed value to file\n");
            return -EFAULT;
        }
        printk(KERN_INFO "case_fan_cooler: Speed changed\n");
    }
    else if (strcmp(command, "color") == 0) // color
    {
        // https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
        ret = file_write(FAN_COLOR_FILE, "green"); // red??
        if (ret)
        {
            printk(KERN_INFO "case_fan_cooler: Failed to write color value to file\n");
            return -EFAULT;
        }
        printk(KERN_INFO "case_fan_cooler: Color changed\n");
    }
    else
    {
        printk(KERN_INFO "case_fan_cooler: Invalid command\n");
        return -EINVAL;
    }

    return len;
}

static struct file_operations fops = {
    .open       = case_fan_cooler_open,
    .release    = case_fan_cooler_release,
    .read       = case_fan_cooler_read,
    .write      = case_fan_cooler_write
};

static int __init case_fan_cooler_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
    {
        printk(KERN_ALERT "case_fan_cooler: Failed to register a major number\n");
        return major_number;
    }

    case_fan_cooler_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(case_fan_cooler_class))
    {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "case_fan_cooler: Failed to register device class\n");
        return PTR_ERR(case_fan_cooler_class);
    }

    case_fan_cooler_device = device_create(case_fan_cooler_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(case_fan_cooler_device))
    {
        class_destroy(case_fan_cooler_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "case_fan_cooler: Failed to create the device\n");
        return PTR_ERR(case_fan_cooler_device);
    }

    printk(KERN_INFO "case_fan_cooler: Device driver loaded successfully\n");


    return 0;
}

static void __exit case_fan_cooler_exit(void)
{
    device_destroy(case_fan_cooler_class, MKDEV(major_number, 0));
    class_unregister(case_fan_cooler_class);
    class_destroy(case_fan_cooler_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "case_fan_cooler: Device driver unloaded successfully\n");
}

module_init(case_fan_cooler_init);
module_exit(case_fan_cooler_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ernest Ivanov");
MODULE_DESCRIPTION("Linux kernel module  that control case fan cooler speed and color");
