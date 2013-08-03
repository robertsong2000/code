#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void)
{
  printk(KERN_ALERT "Dai cazzo!\n");
  return 0;
}

static void __exit hello_exit(void)
{
  printk(KERN_ALERT "Adieu!\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Un modulo che saluta");
MODULE_AUTHOR("Fabio Pozzi");

