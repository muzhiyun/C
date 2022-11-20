#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

struct func_t {
    void (*init)(void);
    void (*invoke)(int data);
    int  (*set)(int level);
    int  (*get)(void);
    void (*deinit)(void);
};

extern struct func_t func;
static int glevel = 1;

void func_invoke(int data)
{
    glevel = data;
    printk(KERN_INFO "func invoke\n");
}

int func_get(void)
{
    printk(KERN_INFO "func get %d\n",glevel);
    return glevel;
}

int func_set(int level)
{
    printk(KERN_INFO "func set %d\n",level);
    glevel = level;
    return glevel;
}

void func_deinit(void)
{
    printk(KERN_INFO "func deinit\n");
}

void func_init(void)
{
    printk(KERN_INFO "func init\n");
}

static int __init func_enter(void)
{
    printk(KERN_INFO "func_enter\n");
    func.init = func_init;
    func.invoke = func_invoke;
    func.set = func_set;
    func.get = func_get;
    func.deinit = func_deinit;
    return 0;
}
module_init(func_enter);


static void __exit func_exit(void)
{
    printk(KERN_INFO "func_exit\n");
    
}
module_exit(func_exit);

MODULE_AUTHOR("Meng Li <my616422@126.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("An func point module");
MODULE_ALIAS("func");
