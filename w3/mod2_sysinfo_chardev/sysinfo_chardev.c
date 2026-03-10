#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/utsname.h>
#include <linux/jiffies.h>
#include <linux/smp.h>

#define DEV_NAME "sysinfo"

static int major;
static char msg[256];
static size_t msg_len;

static ssize_t sysinfo_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    if (*off >= msg_len) return 0; // EOF
    if (len > msg_len - *off) len = msg_len - *off;
    if (copy_to_user(buf, msg + *off, len))
        return -EFAULT;
    *off += len;
    return len;
}

static int sysinfo_open(struct inode *i, struct file *f)
{
    msg_len = scnprintf(msg, sizeof(msg),
                        "release=%s\njiffies=%llu\ncpus=%u\n",
                        utsname()->release,
                        (unsigned long long)jiffies,
                        num_online_cpus());
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = sysinfo_open,
    .read = sysinfo_read,
};

static int __init sysinfo_init(void)
{
    major = register_chrdev(0, DEV_NAME, &fops);
    if (major < 0) return major;
    pr_info("sysinfo: registered major=%d\n", major);
    pr_info("sysinfo: create device: sudo mknod /dev/%s c %d 0\n", DEV_NAME, major);
    return 0;
}

static void __exit sysinfo_exit(void)
{
    unregister_chrdev(major, DEV_NAME);
    pr_info("sysinfo: unregistered\n");
}

module_init(sysinfo_init);
module_exit(sysinfo_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Char device that prints basic system info");
