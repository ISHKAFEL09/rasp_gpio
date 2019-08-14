#ifndef _RASP_GPIO_H
#define _RASP_GPIO_H

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/cdev.h>


#define BCM2837_GPIO_BASE   0x3F200000
#define GPIO_SET_OFFSET		7
#define GPIO_CLEAR_OFFSET	10
#define MAX_PIN				54
#define MODE_INPUT			0x0
#define MODE_OUTPUT			0x1
#define HIGH				1
#define	LOW					0
#define RASP_MAJOR			0
#define GPIO_PIN            4
#define MAX_SONG_CHAR       16

#undef PDEBUG
#ifdef RASP_DEBUG
#   define PDEBUG(fmt, args...) printk(KERN_DEBUG "rasp_gpio: " fmt, ##args)
#else
#   define PDEBUG(fmt, args...)
#endif

struct rasp_dev {
	u32 *gpio_base;
	struct cdev cdev;
	char song[MAX_SONG_CHAR];
};

#endif
