#include <linux/module.h>       // 模块相关的宏和函数
#include <linux/fs.h>           // 文件系统相关的宏和函数
#include <linux/uaccess.h>      // 用户空间和内核空间之间的数据拷贝
#include <linux/init.h>         // 模块初始化和退出宏
#include <linux/kernel.h>       // 内核相关的宏和函数
#include <linux/cdev.h>         // 字符设备相关的宏和结构

#define DEVICE_NAME "simple_char"  // 设备名称
#define BUFFER_SIZE 1024               // 设备缓冲区大小

static dev_t dev_num;                  // 设备号
static struct cdev simple_char_cdev;        // 字符设备结构
static char buffer[BUFFER_SIZE]="simple driver";       // 设备缓冲区
static int buffer_offset = 0;          // 缓冲区当前偏移量

// 设备打开操作
static int simple_char_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Simple char device opened\n");
    return 0;
}

// 设备释放操作
static int simple_char_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Simple char device released\n");
    return 0;
}

// 设备读取操作
static ssize_t simple_char_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {


    
    printk(KERN_INFO "reading data buf: pos=%lld,buffer_offset=%lld,\n", *pos,buffer_offset);

    if (*pos >= buffer_offset){
        printk(KERN_INFO "*pos >= buffer_offset reading data\n");
        return 0; // 已经读取完所有数据
    }
    
    

    if (count > buffer_offset - *pos)
        count = buffer_offset - *pos;

    if (copy_to_user(buf, buffer + *pos, count)){
        printk(KERN_INFO "-EFAULT copy_to_user(buf, buffer + *pos, count)\n");
        return -EFAULT;
    }
       
    printk(KERN_INFO "reading\n");
    *pos += count;
    return count;
}

// 设备写入操作
static ssize_t simple_char_write(struct file *file, const char __user *buf, size_t count, loff_t *pos) {
    if (count > BUFFER_SIZE - buffer_offset)
        count = BUFFER_SIZE - buffer_offset;

    if (copy_from_user(buffer + buffer_offset, buf, count))
        return -EFAULT;

    buffer_offset += count;
    return count;
}

// 文件操作结构体
static struct file_operations simple_char_fops = {
    .owner = THIS_MODULE,
    .open = simple_char_open,
    .release = simple_char_release,
    .read = simple_char_read,
    .write = simple_char_write,
};

// 模块初始化函数
static int __init simple_char_init(void) {
    int ret;

    // 动态分配设备号
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "Failed to allocate device number\n");
        return ret;
    }

    // 初始化字符设备
    cdev_init(&simple_char_cdev, &simple_char_fops);
    simple_char_cdev.owner = THIS_MODULE;

    // 注册字符设备
    ret = cdev_add(&simple_char_cdev, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "Failed to add cdev\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    printk(KERN_INFO "Simple char device registered with major number %d\n", MAJOR(dev_num));
    return 0;
}

// 模块退出函数
static void __exit simple_char_exit(void) {
    // 注销字符设备
    cdev_del(&simple_char_cdev);

    // 释放设备号
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "Simple char device unregistered\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple character device driver");
