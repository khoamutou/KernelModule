#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DRIVER_NAME		"my-driver"
#define DRIVER_CLASS	"MyModuleClass"

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

static int driver_open(struct inode *device_file, struct file * instance)
{
        printk("device open was called\n");
        return 0;
}

static int driver_close(struct inode *device_file, struct file * instance)
{
        printk("device close was called\n");
        return 0;
}

static ssize_t driver_read(struct file *File, char *user_buffer, size_t size, loff_t *offs)
{
        return copy_to_user(user_buffer, "Hello user\n", 11) ? 0 : 11;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t size, loff_t *offs)
{
        char user_data[10];
        memset(user_data, 0, 10);
        if (copy_from_user(user_data, user_buffer, size))
                return 0;
        printk("user write : %s", user_data);
        return size;
        
}

static struct  file_operations fops = {
		.owner = THIS_MODULE,
        .open = driver_open,
        .release = driver_close,
        .read =  driver_read,
        .write = driver_write
};

static int my_driver_init(void)
{
    printk("Hello Device Driver\n");

	if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) 
	{
		printk("Device could not be allocated\n");
		return -1;
	}

	printk("my-driver: Major: %d, Minor: %d\n", my_device_nr >> 20, my_device_nr & 0xfffff);

	if ((my_class = class_create( DRIVER_CLASS)) == NULL)
	{
		printk("Device class can not be create\n");
		unregister_chrdev_region(my_device_nr, 1);
		return -1;
	}

	if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL)
	{
		printk("Can not be create device file\n");
		class_destroy(my_class);
		return -1;
	}

	cdev_init(&my_device, &fops);
	if (cdev_add(&my_device, my_device_nr, 1) == 1)
	{
		printk("Register device fail\n");
		device_destroy(my_class, my_device_nr);
		return -1;
	}

        return 0;
}

static void my_driver_exit(void)
{
    printk("Goodbye Kernel\n");

	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("trankhoa");
MODULE_VERSION("1.0");
