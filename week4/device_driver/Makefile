export ARCH=arm
export CROSS_COMPILE=arm-eabi-

CC = arm-eabi-gcc
ADB = adb 

ifneq ($(KERNELRELEASE),)
obj-m :=sm9s5422_segment.o
else
KDIR := ~/linux

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

	echo
	echo "**** Install: " /system/lib/modules/sm9s5422_segment.ko "****"
	$(ADB) push sm9s5422_segment.ko /system/lib/modules/
	$(ADB) shell chmod 644 /system/lib/modules/sm9s5422_segment.ko
	echo
	echo "**** Load Module: " /system/lib/modules/sm9s5422_segment.ko "****"
	$(ADB) shell toolbox rmmod sm9s5422_segment > /dev/null
	$(ADB) shell insmod /system/lib/modules/sm9s5422_segment.ko
	$(ADB) shell lsmod | grep sm9s5422_segment
	echo "Succeeded~~"
endif

clean:
	rm -f *.symvers
	rm -f *.ko
	rm -f *.o
	rm -f *.mod.c
	rm -f .*.cmd
	rm -rf .tmp_versions