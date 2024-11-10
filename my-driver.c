#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <linux/string.h>

#define BCM2837_GPIO_ADDRESS		0x3F200000
#define GPIO_PIN_OUTPUT                 1U
#define GPIO_PIN_15_OUTPUT              (GPIO_PIN_OUTPUT << 15)

#define GPIO_PIN_15_OUTPUT_ON           (1U << 5)
#define GPIO_PIN_15_OUTPUT_OFF          (1U << 5)                    
static struct proc_dir_entry *driver_proc = NULL;

static unsigned int *gpio_register = NULL;

static void GPIO_On(void)
{
	unsigned int * gpio_on_register = (unsigned int *) ((char *) gpio_register + 0x1C);
	*gpio_on_register = GPIO_PIN_15_OUTPUT_ON;
}

static void GPIO_Off(void)
{
	unsigned int * gpio_off_register = (unsigned int *) ((char *) gpio_register + 0x28);
	*gpio_off_register = GPIO_PIN_15_OUTPUT_OFF;
}

static int driver_open(struct inode *device_file, struct file * instance)
{
        // printk("device open was called\n");
        return 0;
}

static int driver_close(struct inode *device_file, struct file * instance)
{
        // printk("device close was called\n");
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

		if ( strncmp(user_data, "on", 2) == 0)
		{
			printk("Write On\n");
			GPIO_On();
		}
		else if ( strncmp(user_data, "off", 3) == 0)
		{
			printk("Write Off\n");
			GPIO_Off();
		}

        return size;
        
}

static struct proc_ops fops = {
        .proc_open = driver_open,
        .proc_release = driver_close,
        .proc_read =  driver_read,
        .proc_write = driver_write
};

static int my_driver_init(void)
{
        printk("Hello Kernel\n");

	gpio_register = (int *) ioremap(BCM2837_GPIO_ADDRESS, PAGE_SIZE);
	if (gpio_register ==NULL)
		printk("Failed to map GPIO register\n");

        unsigned int * gpio_sel = (unsigned int *) ((char *) gpio_register + 0x00);
        *gpio_sel = GPIO_PIN_15_OUTPUT;

        driver_proc = proc_create("my-driver", 0666, NULL, &fops);
        if (driver_proc == NULL)
                return -1;
        else
                return 0;
}

static void my_driver_exit(void)
{
        printk("Goodbye Kernel\n");
        proc_remove(driver_proc);
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("trankhoa");
MODULE_VERSION("1.0");
