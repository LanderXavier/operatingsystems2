
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

static char *who = "world";
module_param(who, charp, 0644);
MODULE_PARM_DESC(who, "Name to greet");

static int __init hello_init(void)
{
    pr_info("hello_param: loaded. Hello, %s!\n", who);
    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("hello_param: unloaded. Bye, %s!\n", who);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("OS Workshop");
MODULE_DESCRIPTION("Hello module with parameter");
