#ifndef _Encode_Decode_Module_H_
#define _Encode_Decode_Module_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define MAX_LEN 512

#define ENCODE 1
#define DECODE 0
#define HELLO_DEVICE_FILE_NAME "coder"
#define HELLO_DEVICE_CLASS_NAME "coder"

struct coder_dev_t {
    struct cdev cdev;
    char buffer[MAX_LEN];
    
} coder_dev;


#endif