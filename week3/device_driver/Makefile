export ARCH=arm
export CROSS_COMPILE=arm-eabi-

CC=arm-eabi-gcc
ADB=adb

ifneq ($(KERNELRELEASE),)
obj-m := sm9s5422_led.o
else
KDIR := /home/moran/linux
all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

	echo
	echo "**** Install: " /system/lib/modules/sm9s5422_led.ko
	$(ADB) push sm9s5422_led.ko /system/lib/modules/
	$(ADB) shell chmod 644 /system/lib/modules/sm9s5422_led.ko
	echo
	echo "**** Load Module: " /system/lib/modules/sm9s5422_led.ko
	$(ADB) shell toolbox rmmod sm9s5422_led > /dev/null
	$(ADB) shell insmod /system/lib/modules/sm9s5422_led.ko
	$(ADB) shell lsmod | grep sm9s5422_led
	echo
endif

clean:
	rm -f *symvers
	rm -f *.ko
	rm -f *.obj
	rm -f *.mod.c 
	rm -f *.cmd 
	rm -rf .tmp_versions



