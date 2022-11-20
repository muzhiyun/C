#include "Encode_Decode_Module.h"

//可以使用异或　Base64 SHA1等加密解密算法　为做测试　仅简单＋１
int encode_handle(void)
{
    char* p = NULL;
    p = coder_dev.buffer;
    while (*p)
    {
       *p = *p+1;
       p++;
    }
    *p= '\0';
    return 0;
}

int decode_handle(void)
{
    char* p = NULL;
    p = coder_dev.buffer;
    while (*p)
    {
       *p = *p-1;
       p++;
    }
    *p = '\0';
    return 0;
}

int coder_open(struct inode *inode,struct file *filp)
{
    filp->private_data = &coder_dev;
    return 0;
}

ssize_t coder_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
    int ret = 0;
    ret = copy_from_user(coder_dev.buffer,buf,count);
    if(ret)
    {
        return -EFAULT;
    }
    else
    {
        return count;
    }
}

ssize_t coder_read(struct file *filp,char __user *buf,size_t count,loff_t *f_pos)
{
    int ret = 0;
    if(strlen(coder_dev.buffer)==0)
	    return 0;
    if(type == ENCODE)
    {
        encode_handle();
    }
    else
    {
        decode_handle();
    }
    ret = copy_to_user(buf,coder_dev.buffer,strlen(coder_dev.buffer));
    if(ret)
    {
        return -EFAULT;
    }
    else
    {
        ssize_t len =strlen(coder_dev.buffer);
	printk("len=%ld",len);
	memset(coder_dev.buffer,0,512);
	return len;
    }

}

static int coder_release(struct inode* inode, struct file* filp) {
	 return 0;
}


long coder_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
    switch (cmd)
    {
    case ENCODE:
        type = ENCODE;
        break;
    case DECODE:
        type = DECODE;
        break;
    default:
        return -EINVAL;
    }
    return 0;
}

struct file_operations coder_fops = {
    .open = coder_open,
    .write = coder_write,
    .read = coder_read,
    .unlocked_ioctl = coder_ioctl,
    .release = coder_release,
};




/*初始化设备*/
static int __coder_setup_dev(void) {
    int ret = 0;
	coder_major = MAJOR(devno);
    coder_minor = MINOR(devno);
    MKDEV(coder_major,coder_minor);

    cdev_init(&coder_dev.cdev,&coder_fops);
    coder_dev.cdev.owner = THIS_MODULE;

    ret = cdev_add(&coder_dev.cdev,devno,1);
    if(ret)
        printk(KERN_NOTICE "Error %d \n",devno);
    
    printk(KERN_NOTICE "finish major:%d  devno:%d \n",coder_major,devno);
    
    /*在/sys/class/目录下创建设备类别目录hello*/
	coder_class = class_create(THIS_MODULE, HELLO_DEVICE_CLASS_NAME);

	/*在/dev/目录和/sys/class/hello 目录下分别创建设备文件hello*/
	coder_device = device_create(coder_class, NULL, devno, "%s", HELLO_DEVICE_FILE_NAME);

    memset(coder_dev.buffer,0x0,sizeof(coder_dev.buffer));
	 return 0; 
}


static int __init coder_init(void)
{
    int ret = 0;

    //devno = MKDEV(coder_major,0);
    if(coder_major){
        ret = register_chrdev_region(devno,1,"coder");
        printk(KERN_NOTICE "register");
    }else{
        ret = alloc_chrdev_region(&devno,0,1,"coder");
        printk(KERN_NOTICE "alloc");
    }
    if(ret)
        printk(KERN_NOTICE "Error alloc %d \n",devno);
    __coder_setup_dev();

    return 0;
}
module_init(coder_init);



static void __exit coder_exit(void)
{

    dev_t devno = MKDEV(coder_major, coder_minor);

    printk(KERN_ALERT "Destroy hello device./n");

    if(coder_class) {
    device_destroy(coder_class, devno);
    class_destroy(coder_class);
    }

    cdev_del(&coder_dev.cdev);
    unregister_chrdev_region(devno,1);
}

module_exit(coder_exit);


MODULE_AUTHOR("Meng Li <my616422@126.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("An encoding and decoding module");
MODULE_ALIAS("coder");
