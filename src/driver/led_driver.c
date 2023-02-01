#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4GNU/linux");
MODULE_DESCRIPTION("A simple gpio driver for led");

static dev_t my_device_nr;
static struct class* my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_led"
#define DRIVER_CLASS "MyModuleClass_led"

unsigned short map[2][8] = {
	{8,22,10,16,11,12,24,23},  // row
	{9,27,5,17,7,6,25,20}      // col
};

static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
	int to_copy, not_copied, delta;
	unsigned short value;
	unsigned short pin;
	unsigned short rowcol;
	unsigned short highlow;

	to_copy = min(count, sizeof(value));
	not_copied = copy_from_user(&value, user_buffer, to_copy);

	pin = value % 8;                     // pin number
	rowcol = (value & (1 << 3)) >> 3;    // row = 0, col = 1
	highlow = (value & (1 << 4)) >> 4;   // high, low

	gpio_set_value(map[rowcol][pin], highlow);

	delta = to_copy - not_copied;
	return delta;

}
static int driver_open(struct inode* device_file, struct file* instance)
{
	printk("led - open was called!\n");
	return 0;
}
static int driver_close(struct inode* device_file, struct file* instance)
{
	printk("led - close was called!\n");
	return 0;
}
static struct file_operations fops = {
   .owner = THIS_MODULE,
   .open = driver_open,
   .release = driver_close,
   .write = driver_write
};
static int __init ModuleInit(void)
{
	printk("Hello, Kernel!\n");

	// Allocate a device nr
	if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0)
	{
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major : %d, Minor : %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

	// Create device class
	if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL)
	{
		printk("Device class can not a created!\n");
		goto ClassError;
	}

	// Create device file
	if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL)
	{
		printk("Can not create device file!\n");
		goto FileError;
	}

	// Initialize device file
	cdev_init(&my_device, &fops);

	// Registering device to kernel
	if (cdev_add(&my_device, my_device_nr, 1) == -1)
	{
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	if (gpio_request(17, "rpi-gpio-17"))
	{
		printk("Can not allocate GPIO 17\n");
		goto AddError;
	}
	if (gpio_direction_output(17, 0))
	{
		printk("Can not set GPIO 17 to output!\n");
		goto Gpio17Error;
	}

	if (gpio_request(27, "rpi-gpio-27")) {
		printk("Can not allocate GPIO 27\n");
		goto AddError;
	}
	if (gpio_direction_output(27, 0)) {
		printk("Can not set  GPIO 27 to output\n");
		goto Gpio27Error;
	}

	if (gpio_request(22, "rpi-gpio-22")) {
		printk("Can not allocate GPIO 22\n");
		goto AddError;
	}
	if (gpio_direction_output(22, 0)) {
		printk("Can not set  GPIO 22 to output\n");
		goto Gpio22Error;
	}

	if (gpio_request(10, "rpi-gpio-10")) {
		printk("Can not allocate GPIO 10\n");
		goto AddError;
	}
	if (gpio_direction_output(10, 0)) {
		printk("Can not set  GPIO 10 to output\n");
		goto Gpio10Error;
	}

	if (gpio_request(9, "rpi-gpio-9")) {
		printk("Can not allocate GPIO 9\n");
		goto AddError;
	}
	if (gpio_direction_output(9, 0)) {
		printk("Can not set  GPIO 9 to output\n");
		goto Gpio9Error;
	}

	if (gpio_request(11, "rpi-gpio-11")) {
		printk("Can not allocate GPIO 11\n");
		goto AddError;
	}
	if (gpio_direction_output(11, 0)) {
		printk("Can not set  GPIO 11 to output\n");
		goto Gpio11Error;
	}

	if (gpio_request(5, "rpi-gpio-5")) {
		printk("Can not allocate GPIO 5\n");
		goto AddError;
	}
	if (gpio_direction_output(5, 0)) {
		printk("Can not set  GPIO 5 to output\n");
		goto Gpio5Error;
	}

	if (gpio_request(23, "rpi-gpio-23")) {
		printk("Can not allocate GPIO 23\n");
		goto AddError;
	}
	if (gpio_direction_output(23, 0)) {
		printk("Can not set  GPIO 23 to output\n");
		goto Gpio23Error;
	}

	if (gpio_request(24, "rpi-gpio-24")) {
		printk("Can not allocate GPIO 24\n");
		goto AddError;
	}
	if (gpio_direction_output(24, 0)) {
		printk("Can not set  GPIO 24 to output\n");
		goto Gpio24Error;
	}

	if (gpio_request(25, "rpi-gpio-25")) {
		printk("Can not allocate GPIO 25\n");
		goto AddError;
	}
	if (gpio_direction_output(25, 0)) {
		printk("Can not set  GPIO 25 to output\n");
		goto Gpio25Error;
	}

	if (gpio_request(8, "rpi-gpio-8")) {
		printk("Can not allocate GPIO 8\n");
		goto AddError;
	}
	if (gpio_direction_output(8, 0)) {
		printk("Can not set  GPIO 8 to output\n");
		goto Gpio8Error;
	}

	if (gpio_request(7, "rpi-gpio-7")) {
		printk("Can not allocate GPIO 7\n");
		goto AddError;
	}
	if (gpio_direction_output(7, 0)) {
		printk("Can not set  GPIO 7 to output\n");
		goto Gpio7Error;
	}

	if (gpio_request(12, "rpi-gpio-12")) {
		printk("Can not allocate GPIO 12\n");
		goto AddError;
	}
	if (gpio_direction_output(12, 0)) {
		printk("Can not set  GPIO 12 to output\n");
		goto Gpio12Error;
	}

	if (gpio_request(16, "rpi-gpio-16")) {
		printk("Can not allocate GPIO 16\n");
		goto AddError;
	}
	if (gpio_direction_output(16, 0)) {
		printk("Can not set  GPIO 16 to output\n");
		goto Gpio16Error;
	}

	if (gpio_request(6, "rpi-gpio-6")) {
		printk("Can not allocate GPIO 6\n");
		goto AddError;
	}
	if (gpio_direction_output(6, 0)) {
		printk("Can not set  GPIO 6 to output\n");
		goto Gpio6Error;
	}

	if (gpio_request(20, "rpi-gpio-20")) {
		printk("Can not allocate GPIO 20\n");
		goto AddError;
	}
	if (gpio_direction_output(20, 0)) {
		printk("Can not set  GPIO 20 to output\n");
		goto Gpio20Error;
	}

	return 0;

Gpio5Error:
	gpio_free(5);
Gpio6Error:
	gpio_free(6);
Gpio7Error:
	gpio_free(7);
Gpio8Error:
	gpio_free(8);
Gpio9Error:
	gpio_free(9);
Gpio10Error:
	gpio_free(10);
Gpio11Error:
	gpio_free(11);
Gpio12Error:
	gpio_free(12);
Gpio16Error:
	gpio_free(16);
Gpio17Error:
	gpio_free(17);
Gpio20Error:
	gpio_free(20);
Gpio22Error:
	gpio_free(22);
Gpio23Error:
	gpio_free(23);
Gpio24Error:
	gpio_free(24);
Gpio25Error:
	gpio_free(25);
Gpio27Error:
	gpio_free(27);
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}
static void __exit ModuleExit(void) {
	gpio_set_value(23, 0);
	gpio_set_value(24, 0);
	gpio_set_value(25, 0);
	gpio_set_value(8, 0);
	gpio_set_value(7, 0);
	gpio_set_value(12, 0);
	gpio_set_value(16, 0);
	gpio_set_value(20, 0);
	gpio_set_value(17, 0);
	gpio_set_value(27, 0);
	gpio_set_value(22, 0);
	gpio_set_value(10, 0);
	gpio_set_value(9, 0);
	gpio_set_value(11, 0);
	gpio_set_value(5, 0);
	gpio_set_value(6, 0);

	gpio_free(23);
	gpio_free(24);
	gpio_free(25);
	gpio_free(8);
	gpio_free(7);
	gpio_free(12);
	gpio_free(16);
	gpio_free(20);
	gpio_free(17);
	gpio_free(27);
	gpio_free(22);
	gpio_free(10);
	gpio_free(9);
	gpio_free(11);
	gpio_free(5);
	gpio_free(6);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye,Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);