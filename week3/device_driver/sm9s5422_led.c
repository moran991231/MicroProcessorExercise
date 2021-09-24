#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <mach/regs-gpio.h>
#include <linux/of_gpio.h>
#include <hanback/gpios.h>
#include <linux/delay.h>
	
static int sm9s5422_led_open(struct inode * inode, struct file * file){
	int err=0,i=0;

	printk(KERN_CRIT"sm9s5422_led_open, \n");
	printk("2018440059 Park Jaesun \n");


	for (i=0; i<8; i++)
	{
		err = gpio_request(gpy7(i), "Led");
		if(err)
			printk("led.c failed to request gpy7(%d) \n", i);
		s3c_gpio_setpull(gpy7(i), S3C_GPIO_PULL_NONE);
		gpio_direction_output(gpy7(i), 0);
	}

	return 0;
}

static int sm9s5422_led_release(struct inode * inode, struct file * file){
	int i=0;
	printk("sm9s5422_led_release, \n");

	for(i=0; i<8; i++)
		gpio_free(gpy7(i));

	return 0;
}



static ssize_t sm9s5422_led_write(struct file * file, const char * buf, size_t length, loff_t * ofs){
	int ret=0, i=0;
	printk("sm9s5422_led_write, \n");
	unsigned char cbuf[8];
	ret = copy_from_user(cbuf, buf, length);
	for(i=0; i<8; i++)
		gpio_direction_output(gpy7(i), (unsigned int) cbuf[i]);
	return 0;
}










static struct file_operations sm9s5422_led_fops = {
	.owner = THIS_MODULE,
	.open = sm9s5422_led_open,
	.release = sm9s5422_led_release,
	.write = sm9s5422_led_write,
};

static struct miscdevice sm9s5422_led_driver = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "sm9s5422_led",
	.fops = &sm9s5422_led_fops,
};

static int sm9s5422_led_init(void){
	printk("sm9s5422_led_init, \n");
	return misc_register(&sm9s5422_led_driver);
}

static void sm9s5422_led_exit(void){
	printk("sm9s5422_led_exit, \n");
	misc_deregister(&sm9s5422_led_driver);
}

module_init(sm9s5422_led_init);
module_exit(sm9s5422_led_exit);

MODULE_AUTHOR("MPCLASS");
MODULE_DESCRIPTION("Led Control");
MODULE_LICENSE("Dual BSD/GPL");
