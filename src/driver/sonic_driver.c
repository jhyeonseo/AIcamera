#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4GNU/linux");
MODULE_DESCRIPTION("A simple gpio driver for segments");

static dev_t my_device_nr;
static struct class* my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_sonic"
#define DRIVER_CLASS "MyModuleClass_sonic"


static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs)
{
    int to_copy, not_copied, delta;
    unsigned short value = 0;

    to_copy = min(count, sizeof(value));
    not_copied = copy_from_user(&value, user_buffer, to_copy);

    // High, Low 신호 Trig에 입력
    gpio_set_value(19, value);


    delta = to_copy - not_copied;
    return delta;
}
static ssize_t driver_read(struct file* File, char* user_buffer, size_t count, loff_t* offs)
{
    int to_copy, not_copied, delta;
    unsigned short tmp;

    to_copy = min(count, sizeof(tmp));

    // High, Low 신호 Echo에서 읽어옴
    tmp = gpio_get_value(26);
    if (tmp > 0)
        tmp = 1;
    else
        tmp = 0;

    not_copied = copy_to_user(user_buffer, &tmp, to_copy);
    delta = to_copy - not_copied;

    return delta;
}
static int driver_open(struct inode* device_file, struct file* instance)
{
    printk("sonic - open was called!\n");
    return 0;
}
static int driver_close(struct inode* device_file, struct file* instance)
{
    printk("sonic - close was called!\n");
    return 0;
}
static struct file_operations fops ={
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write
};
static int __init ModuleInit(void)
{
    printk("Sonic driver start!\n");

    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0)
    {
        printk("Dvice Nr. could not be allocated!\n");
        return -1;
    }
    printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL)
    {
        printk("Device class can not e created!\n");
        goto ClassError;
    }
    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL)
    {
        printk("Can not create device file!\n");
        goto FileError;
    }
    cdev_init(&my_device, &fops);

    if (cdev_add(&my_device, my_device_nr, 1) == -1)
    {
        printk("Registering of device to kernel failed!\n");
        goto AddError;
    }
    if (gpio_request(19, "rpi-gpio-19"))
    {
        printk("Can not allocate GPIO 19\n");
        goto AddError;
    }
    if (gpio_direction_output(19, 0))
    {
        printk("Can not set GPIO 19 to output!\n");
        goto Gpio19Error;
    }
    if (gpio_request(26, "rpi-gpio-26"))
    {
        printk("Can not allocate GPIO 26\n");
        goto AddError;
    }
    if (gpio_direction_input(26))
    {
        printk("Can not set GPIO 26 to input!\n");
        goto Gpio26Error;
    }

    return 0;

Gpio19Error:
    gpio_free(19);
Gpio26Error:
    gpio_free(26);
AddError:
    device_destroy(my_class, my_device_nr);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev_region(my_device_nr, 1);
    return -1;
}
static void __exit ModuleExit(void)
{
    gpio_set_value(19, 0);
    gpio_free(19);
    gpio_free(26);
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr, 1);
    printk("Goodbye Kernel\n");
}
module_init(ModuleInit);
module_exit(ModuleExit);
