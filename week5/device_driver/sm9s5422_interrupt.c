#include <asm/io.h>
#include <asm/uaccess.h>
#include <hanback/gpios.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <mach/irqs.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

static DECLARE_WAIT_QUEUE_HEAD(wait_queue);

int irq_num = 0;
int irqNum[5];
int int_gpios[5];

static irqreturn_t button_interrupt(int irq, void *dev_id) {
    irq_num = irq;
    wake_up_interruptible(&wait_queue);

    return IRQ_HANDLED;
}

static int sm9s5422_interrupt_open(struct inode *inode, struct file *file) {
    printk("sm9s5422_interrupt_open \n");

    return 0;
}

static int sm9s5422_interrupt_release(struct inode *inode, struct file *file) {
    printk("sm9s5422_interrupt_release \n");

    return 0;
}

static ssize_t sm9s5422_interrupt_read(struct file *file, char *buf, size_t length, loff_t *ofs) {
    printk("sm9s5422_interrupt_read, 2018440059 Jaesun Park \n");

    char msg[100];
    int ret = 0;

    interruptible_sleep_on(&wait_queue);

    if (irq_num == irqNum[0]) sprintf(msg, "Up");
    if (irq_num == irqNum[1]) sprintf(msg, "Down");
    if (irq_num == irqNum[2]) sprintf(msg, "Left");
    if (irq_num == irqNum[3]) sprintf(msg, "Right");
    if (irq_num == irqNum[4])
        sprintf(msg, "Center");
    else
        sprintf("??? %d", irq_num);

    ret = copy_to_user(buf, msg, 100);
    if (ret < 0) return -1;

    return 0;
}

static struct file_operations sm9s5422_interrupt_fops = {
    .owner = THIS_MODULE, .open = sm9s5422_interrupt_open, .release = sm9s5422_interrupt_release, .read = sm9s5422_interrupt_read};

static struct miscdevice sm9s5422_interrupt_driver = {.minor = MISC_DYNAMIC_MINOR, .name = "sm9s5422_interrupt", .fops = &sm9s5422_interrupt_fops};

static int sm9s5422_interrupt_init(void) {
    printk("sm9s5422_interrupt_init \n");
    int res, i;
    irqNum[0] = gpio_to_irq(gpx0(3));
    irqNum[1] = gpio_to_irq(gpx0(4));
    irqNum[2] = gpio_to_irq(gpx0(5));
    irqNum[3] = gpio_to_irq(gpx1(6));
    irqNum[4] = gpio_to_irq(gpx1(7));

    for (i = 0; i < 5; i++) {
        res = request_irq(irqNum[i], button_interrupt, IRQF_TRIGGER_FALLING, "GPIO", NULL);
        if (res < 0) printk(KERN_ERR "%s : Request for IRQ %d failed \n", __FUNCTION__, irqNum[0]);
    }
    return misc_register(&sm9s5422_interrupt_driver);
}

static void sm9s5422_interrupt_exit(void) {
    printk("sm9s5422_interrupt_exit \n");
    int i;
    for (i = 0; i < 5; i++) free_irq(irqNum[i], NULL);
    misc_deregister(&sm9s5422_interrupt_driver);
}

module_init(sm9s5422_interrupt_init);
module_exit(sm9s5422_interrupt_exit);

MODULE_AUTHOR("Jaesun Park 2018440059");
MODULE_DESCRIPTION("UOS Applied Microprocessor Lab 2021-Fall");
MODULE_LICENSE("Dual BSD/GPL");
