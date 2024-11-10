#include <linux/module.h>
#include <linux/init.h>
#include <linux/uacess.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>

static struct proc_dir_entry *driver_proc = NULL;

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
        return copy_to_user(user_buffer, "Hello user\n",11)?0:11;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t size, loff_t *offs)
{
        char user_data[10];
        memset(user_data, 0, 10);
        if ( copy_from_user(user_data, user_buffer, size))    ;
                return 0;
        printk("user write : %s", user_data);
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
