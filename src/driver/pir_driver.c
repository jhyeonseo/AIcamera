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

#define DRIVER_NAME "my_pir"
#define DRIVER_CLASS "MyModuleClass_pir"

static ssize_t driver_read(struct file* File, char* user_buffer, size_t count, loff_t* offs)
{
    int to_copy, not_copied, delta;
    unsigned short tmp;

    to_copy = min(count, sizeof(tmp));

    tmp = gpio_get_value(18);
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
    printk("pir - open was called!\n");
    return 0;
}
static int driver_close(struct inode* device_file, struct file* instance)
{
    printk("pir - close was called!\n");
    return 0;
}
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    //.write = driver_write
};
static int __init ModuleInit(void)
{
    printk("Pir driver start!\n");

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
    if (gpio_request(18, "rpi-gpio-18"))
    {
        printk("Can not allocate GPIO 18\n");
        goto AddError;
    }
    if (gpio_direction_input(18))
    {
        printk("Can not set GPIO 18 to input!\n");
        goto Gpio18Error;
    }

    return 0;

Gpio18Error:
    gpio_free(18);
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
    gpio_free(18);
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr, 1);
    printk("Goodbye Kernel\n");
}
module_init(ModuleInit);
module_exit(ModuleExit);
