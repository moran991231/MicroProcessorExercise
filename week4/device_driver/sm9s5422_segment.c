#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/of_gpio.h>
#include <hanback/gpios.h>

unsigned int ibuf[8];

int Getsegmentcode_base (int x)
{
  unsigned int i;

  for (i = 0; i < 8; i++)
    ibuf[i] = 0;

  switch (x) {
    case 0 :
      for (i=0; i<6; i++) ibuf[i] = 1;
      break;

    case 1 : ibuf[1] = 1; ibuf[2] = 1; break;

    case 2 :
      for (i=0; i<2; i++) ibuf[i] = 1;
      for (i=3; i<5; i++) ibuf[i] = 1;
      ibuf[6] = 1;
      break;

    case 3 :
      for (i=0; i<4; i++) ibuf[i] = 1;
      ibuf[6] = 1;
      break;

    case 4 :
      for (i=1; i<3; i++) ibuf[i] = 1;
      for (i=5; i<7; i++) ibuf[i] = 1;
      break;

    case 5 :
      ibuf[0] = 1;
      for (i=2; i<4; i++) ibuf[i] = 1;
      for (i=5; i<7; i++) ibuf[i] = 1;
      break;

    case 6 :
      for (i=2; i<7; i++) ibuf[i] = 1;
      break;

    case 7 :
      for (i=0; i<3; i++) ibuf[i] = 1;
      ibuf[5] = 1;
      break;

    case 8 :
      for (i=0; i<7; i++) ibuf[i] = 1;
      break;

    case 9 :
      for (i=0; i<4; i++) ibuf[i] = 1;
      for (i=5; i<7; i++) ibuf[i] = 1;
      break;

    case 10 :
      for (i=0; i<3; i++) ibuf[i] = 1;
      for (i=4; i<8; i++) ibuf[i] = 1;
      break;
    case 11 :
      for (i=0; i<8; i++) ibuf[i] = 1;
      break;
    case 12 :
      for (i=3; i<6; i++) ibuf[i] = 1;
      ibuf[0] = 1;
      ibuf[7] = 1;
      break;
    case 13 :
      ibuf[7] = 1;
      for (i=0; i<6; i++) ibuf[i] = 1;
      break;
    case 14 :
      for (i=3; i<8; i++) ibuf[i] = 1;
      ibuf[0] = 1;
      break;
    case 15 :
      for (i=4; i<8; i++) ibuf[i] = 1;
      ibuf[0] = 1;
      break;

    default :
      for (i=0; i<8; i++) ibuf[i] = 1;
      break;
  }
  return 0;
}
	
static int sm9s5422_segment_open(struct inode * inode, struct file * file){
	printk("sm9s5422_segment_open, \n");

  int err, i;

  for(i=0; i<8; i++){
    err = gpio_request(gpe0(i), "GPE0");
    if(err) 
      printk("segment.c failed to request GPE0(%d) \n", i);
    s3c_gpio_setpull(gpe0(i), S3C_GPIO_PULL_NONE);
    gpio_direction_output(gpe0(i),0);

  }
    for(i=0; i<7; i++){
      if(i==5) continue;
      err = gpio_request(gpg1(i),"GPG1");
      if(err)
        printk("segment.c failed to request GPG1(%d) \n",i);
      s3c_gpio_setpull(gpg1(i), S3C_GPIO_PULL_UP);
      gpio_direction_output(gpg1(i), 1);

    }

  return 0;
}




static int sm9s5422_segment_release(struct inode * inode, struct file * file){
  printk("sm9s5422_segment_release, \n");
  
  int i;

  for(i=0; i<8; i++)
    gpio_free(gpe0(i));

  for(i=0; i<7; i++)
    if(i!=5)
      gpio_free(gpg1(i));
  
  return 0;
}



static ssize_t sm9s5422_segment_write(struct file * file, const char * buf,
		size_t length, loff_t * ofs) {
	printk("sm9s5422_segment_write, \n");

	unsigned int ret, i, j;
	unsigned char data[6];
	ret = copy_from_user(data, buf, 6);

  for(j=0; j<5; j++){
    Getsegmentcode_base((unsigned int) data[j]);
    for(i=0; i<8; i++)
      gpio_direction_output(gpe0(i),0);

    if(j>0)
      gpio_direction_output(gpg1(j-1),1);

    gpio_direction_output(gpg1(j),0);

    for(i=0; i<8; i++)
      gpio_direction_output(gpe0(i), (unsigned int) ibuf[i]);

    mdelay(2);
  }

  Getsegmentcode_base((unsigned int) data[5]);

  for(i=0; i<8; i++)
    gpio_direction_output(gpe0(i),0);

  gpio_direction_output(gpg1(4), 1);
  gpio_direction_output(gpg1(6),0);
  for(i=0; i<8; i++)
  gpio_direction_output(gpe0(i), (unsigned int) ibuf[i]);

  mdelay(2);

  gpio_direction_output(gpg1(6), 1);

  return length;

}

static struct file_operations sm9s5422_segment_fops = {
	.owner = THIS_MODULE,
	.open = sm9s5422_segment_open,
	.release = sm9s5422_segment_release,
	.write = sm9s5422_segment_write,
};

static struct miscdevice sm9s5422_segment_driver = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "sm9s5422_segment",
	.fops = &sm9s5422_segment_fops,
};

static int sm9s5422_segment_init(void){
	printk("sm9s5422_segment_init, \n");
	
	return misc_register(&sm9s5422_segment_driver);
}

static void sm9s5422_segment_exit(void){
	printk("sm9s5422_segment_exit, \n");

	misc_deregister(&sm9s5422_segment_driver);
	
}

module_init(sm9s5422_segment_init);
module_exit(sm9s5422_segment_exit);

MODULE_AUTHOR("Hanback");
MODULE_DESCRIPTION("Segment Control");
MODULE_LICENSE("Dual BSD/GPL");
