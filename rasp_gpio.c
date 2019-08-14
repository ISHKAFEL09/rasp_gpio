#include "rasp_gpio.h"

int rasp_major = RASP_MAJOR;
int rasp_minor = 0;

module_param(rasp_major, int, S_IRUGO);
module_param(rasp_minor, int, S_IRUGO);

MODULE_AUTHOR("Yin.Wang");
MODULE_LICENSE("Dual BSD/GPL");

struct rasp_dev *rasp_device;

static int gpio_set_mode(u8 pin, u8 value) {
    u32 mode, selval, mask;
	u32 *addr;
	int ret = -1;

	if(pin > MAX_PIN) 
		return ret;

	ret = 0;
	mode = (u32)value << (3 * (pin % 10));
	mask = 7 << (3 * (pin % 10));
	addr = rasp_device->gpio_base + pin / 10;

	selval = ioread32(addr);
	selval &= ~mask;
	selval |= mode;
	iowrite32(selval, addr);
	return ret;
}


static int gpio_write_pin(u8 pin, u8 value) {
	u32 *base;
	u8 offset;
	u32 selval;
	int ret = -1;

	if(pin > MAX_PIN)
		return ret;

	ret = 0;
	offset = pin % 32;
	if(value == HIGH)
		base = rasp_device->gpio_base + GPIO_SET_OFFSET + (pin >> 5);
	else if(value == LOW)
		base = rasp_device->gpio_base + GPIO_CLEAR_OFFSET + (pin >> 5);
	else 
		return -1;
	selval = 1 << offset;
	iowrite32(selval, base);

	return ret;
}


static void gpio_write(char *song, size_t count)
{
    size_t i = 0;
    char c;

    while(i < count) {
        c = *(song + i);
        if(c == '1') {
            gpio_write_pin(GPIO_PIN, HIGH);
        }
        else if(c == '0') {
            gpio_write_pin(GPIO_PIN, LOW);
        }
        else {
            PDEBUG("unknown char\n");
            gpio_write_pin(GPIO_PIN, HIGH);
        }
        i++;
    }
}


/*
static void _gpio_test(void) {
    gpio_set_mode(4, MODE_OUTPUT);
    gpio_write_pin(4, 0);
    mdelay(1000);
    gpio_write_pin(4, 1);
}
*/


int rasp_open(struct inode *inode, struct file *filp)
{
    struct rasp_dev *raspDev;

    raspDev = container_of(inode->i_cdev, struct rasp_dev, cdev);
    filp->private_data = raspDev;
    raspDev->gpio_base = ioremap(BCM2837_GPIO_BASE, 0x80);
    return 0;
}


int rasp_release(struct inode *inode, struct file *filp)
{
    return 0;
}


ssize_t rasp_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
    return 0;
}


ssize_t rasp_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
    struct rasp_dev *raspDev = filp->private_data;
    ssize_t ret = -ENOMEM;

    gpio_set_mode(GPIO_PIN, MODE_OUTPUT);

    if(count > MAX_SONG_CHAR)
        count = MAX_SONG_CHAR;

    if(copy_from_user(raspDev->song, buf, count)) {
        ret = -EFAULT;
        goto out;
    }

    ret = count;

    gpio_write(raspDev->song, count);
    out:
    return ret;
}


struct file_operations rasp_fops = {
	.owner = THIS_MODULE,
	.open = rasp_open,
	.release = rasp_release,
	.write = rasp_write,
	.read = rasp_read,
};


static void rasp_setup_cdev(struct rasp_dev *dev, int index)
{
	int err, devno = MKDEV(rasp_major, rasp_minor + index);

	cdev_init(&dev->cdev, &rasp_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &rasp_fops;
	err = cdev_add(&dev->cdev, devno, 1);

	if(err)
		PDEBUG(KERN_NOTICE "Error %d adding rasp_gpio%d", err, index);
}


static void rasp_cleanup_module(void) {
    dev_t devno = MKDEV(rasp_major, rasp_minor);
    cdev_del(&rasp_device->cdev);
    iounmap(rasp_device->gpio_base);
    kfree(rasp_device);
    unregister_chrdev_region(devno, 1);
}


static int __init rasp_gpio_init(void)
{
	u32 ret;
	dev_t dev=0;

	PDEBUG("rasp init\n");
	rasp_device = kmalloc(sizeof(struct rasp_dev), GFP_KERNEL);
	if(!rasp_device) {
		ret = -ENOMEM;
		goto fail;
	}
	memset(rasp_device, 0, sizeof(struct rasp_dev));
    rasp_device->gpio_base = ioremap(BCM2837_GPIO_BASE, 0x80);

	if(rasp_major) {
		dev = MKDEV(rasp_major, rasp_minor);
		ret = register_chrdev_region(dev, 1, "rasp");
	} else {	
		ret = alloc_chrdev_region(&dev, 0, 1, "rasp");
		rasp_major = MAJOR(dev);
	}
	if(ret < 0) {
		PDEBUG("rasp: can't get major %d\n", rasp_major);
		return ret;
	}

	rasp_setup_cdev(rasp_device, 0);
	return 0;

fail:
	rasp_cleanup_module();
	return ret;	
}


static void __exit rasp_gpio_exit(void)
{
	rasp_cleanup_module();
    PDEBUG("rasp exit\n");
}


module_init(rasp_gpio_init);
module_exit(rasp_gpio_exit);
