#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/utsname.h>
#include <linux/jiffies.h>
#include <linux/cpumask.h>
#include <linux/string.h>

#define DEVICE_NAME "sysinfo"
#define BUF_LEN 256

static int major_number;
static int display_mode = 0; // 0: all, 1: kernel, 2: cpu, 3: time

// Función que se ejecuta cuando se lee el dispositivo (cat /dev/sysinfo)
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    char message[BUF_LEN];
    short message_len = 0;


    if (*offset > 0) {
        return 0; 
    }

    // Seleccionamos el mensaje basado en el modo actual
    switch (display_mode) {
        case 1:
            message_len = snprintf(message, BUF_LEN, "kernel release: %s\n", utsname()->release);
            break;
        case 2:
            message_len = snprintf(message, BUF_LEN, "cpus: %d\n", num_online_cpus());
            break;
        case 3:
            message_len = snprintf(message, BUF_LEN, "jiffies: %lu\n", jiffies);
            break;
        default:
            message_len = snprintf(message, BUF_LEN, "kernel release: %s\njiffies: %lu\ncpus: %d\n",
                                   utsname()->release, jiffies, num_online_cpus());
            break;
    }

    // Enviamos la cadena al espacio de usuario
    if (copy_to_user(buffer, message, message_len)) {
        return -EFAULT;
    }

    *offset = message_len;
    return message_len;
}

// Función que se ejecuta cuando se escribe en el dispositivo (echo "..." > /dev/sysinfo)
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    char command[16];
    size_t copy_len = len < (sizeof(command) - 1) ? len : (sizeof(command) - 1);

    // Traemos el comando desde el espacio de usuario
    if (copy_from_user(command, buffer, copy_len)) {
        return -EFAULT;
    }
    command[copy_len] = '\0';

    // Usamos strncmp para ignorar el salto de línea (\n) que 'echo' añade por defecto
    if (strncmp(command, "all", 3) == 0) {
        display_mode = 0;
    } else if (strncmp(command, "kernel", 6) == 0) {
        display_mode = 1;
    } else if (strncmp(command, "cpu", 3) == 0) {
        display_mode = 2;
    } else if (strncmp(command, "time", 4) == 0) {
        display_mode = 3;
    } else {
        pr_warn("sysinfo: Comando no reconocido\n");
    }

    return len;
}

static struct file_operations fops = {
    .read = dev_read,
    .write = dev_write,
};

static int __init sysinfo_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_alert("sysinfo failed to register a major number\n");
        return major_number;
    }
    pr_info("sysinfo: registered correctly with major number %d\n", major_number);
    return 0;
}

static void __exit sysinfo_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("sysinfo: unloaded successfully\n");
}

module_init(sysinfo_init);
module_exit(sysinfo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lander");
MODULE_DESCRIPTION("Character device for system information control");
