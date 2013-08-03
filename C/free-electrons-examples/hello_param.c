#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

/* A couple of parameters that can be passed in: how many times and to whom */

static char *whom = "world";
module_param(whom, charp, 0);

static int howmany = 1;
module_param(howmany, int, 0);

static int __init hello_init(void)
{
  int i;
  for (i=0; i < howmany; i++)
    printk(KERN_ALERT "(%d) hello, %s\n", i, whom);
  
  return 0;
}

static void __exit hello_exit(void)
{
  printk(KERN_ALERT "Goodbye %s!\n", whom);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Un modulo che saluta");
MODULE_AUTHOR("Fabio Pozzi");

