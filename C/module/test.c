#include <linux/init.h>
#include <linux/module.h> 
#include <linux/kernel.h>

MODULE_LICENSE("Dual BSD/GPL");  

extern int bar(void);

static int hello_init(void)  
{  
    bar();
    printk(KERN_ALERT "hello,world.\n");  
    return 0;  
}  

int helloworld_test(void)
{
    return 0;
}

EXPORT_SYMBOL(helloworld_test);

static void hello_exit(void)  
{  
    printk(KERN_ALERT "goodbye.\n");  
}  

module_init(hello_init);  
module_exit(hello_exit);  
MODULE_AUTHOR("robertsong");  
