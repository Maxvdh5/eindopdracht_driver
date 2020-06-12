//
// Created by max on 6/12/20.
//

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h> 	     /* file stuff */
#include <linux/kernel.h>    /* printk() */
#include <linux/errno.h>     /* error codes */
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/mod_devicetable.h>
MODULE_LICENSE("Dual BSD/GPL");

#define sysfs_file_len  64

int major;
const int minor = 0;
const int amount = 0;
static const char device_name[] = "driver-van-max";
static struct class *cl;

static const struct of_device_id of_driver-van-max_match_tbl[] = {
        { .compatible = "maxinc,lightsensor", },
        { /* end */ }
};

MODULE_DEVICE_TABLE(of, of_driver-van-max_match_tbl);

static struct cdev device;
static dev_t dev_num;

char* sysfs_adc = "/sys/devices/ocp.2/helper.14/AIN0"

ssize_t dev_read(struct file *filp, char *buffer, size_t len, loff_t *offset){
    struct file     *data_fp;
    mm_segment_t    data_fs;

    if (*offset != 0) {
        printk(KERN_INFO "Message has already been read");
        return 0;
    }

    // check if the target fs of the device is set
    if (sysfs_adc == NULL) {
        printk(KERN_INFO "%s: sysfs target is not set", device_name);
        return -EFAULT;
    }

    // open the device and read it's data
    data_fp = filp_open(*sysfs_path, O_RDONLY, 0);
    if (data_fp == NULL) {
        printk(KERN_ALERT "%s: filp_open failed", DEVICE_NAME);
        return -EFAULT;
    }

    // Get current segment descriptor
    data_fs = get_fs();
    // Set segment descriptor associated to kernel space
    set_fs(KERNEL_DS);
    // Read the file
    data_fp->f_op->read(data_fp, buffer, sysfs_file_len, &data_fp->f_pos);
    // Restore segment descriptor
    set_fs(data_fs);

    // set offset so next read will return 0 (EOF)
    // *offset = size_of_message;
    *offset = sysfs_file_len;
    // return size_of_message;
    return sysfs_file_len;
}

ssize_t dev_write(struct file *filp, const char *buffer, size_t len, loff_t *offset){
    printk(KERN_ALERT "Write %ld?\n",len);
    return 1;
}

int dev_open(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "Opened device\n");

    return 0;
}

int dev_release(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "Opened device\n");

    return 0;
}

struct file_operations fileOps = {
        .owner = THIS_MODULE,
        .read = dev_read,
        .write = dev_write,
        .open = dev_open,
        .release = dev_release,
};

static int dev_init(void){

    if (alloc_chrdev_region(&dev_num, minor, amount, device_name ) < 0)
    {
        printk(KERN_ALERT "init failed!\n");
        return -1;
    }
    if ((cl = class_create(THIS_MODULE, device_name)) == NULL)
    {
        printk(KERN_ALERT "cannot create class\n");
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    if (device_create(cl, NULL, dev_num, NULL, "Character-driver-device") == NULL)
    {
        printk(KERN_ALERT "cannot create device\n");
        class_destroy(cl);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    major = MAJOR(dev_num);
    printk(KERN_ALERT "major number: %d!\n", major);
    cdev_init(&device, &fileOps);
    if (cdev_add(&device, dev_num, 1) < 0)
    {
        printk(KERN_ALERT "Error adding chdev");
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    return 0;
}
static void dev_exit(void){
    printk(KERN_ALERT "Goodbye, world\n");
    cdev_del(&device);
    device_destroy(cl, dev_num);
    class_destroy(cl);
    unregister_chrdev_region(dev_num, amount);
}


module_init(dev_init);
module_exit(dev_exit);