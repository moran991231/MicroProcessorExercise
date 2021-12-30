export ARCH=arm
export CROSS_COMPILE=arm-eabi-

CC = arm-eabi-gcc
ADB = adb 
TNAME=sm9s5422_segment_js

ifneq ($(KERNELRELEASE),)
obj-m :=$(TNAME).o
else
KDIR := ~/linux

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

	echo
	echo "**** Install: " /system/lib/modules/$(TNAME).ko "****"
	$(ADB) push $(TNAME).ko /system/lib/modules/
	$(ADB) shell chmod 644 /system/lib/modules/$(TNAME).ko
	echo
	echo "**** Load Module: " /system/lib/modules/$(TNAME).ko "****"
	$(ADB) shell toolbox rmmod sm9s5422_segment > /dev/null
	$(ADB) shell toolbox rmmod $(TNAME) > /dev/null
	$(ADB) shell insmod /system/lib/modules/$(TNAME).ko
	$(ADB) shell lsmod | grep $(TNAME)
	echo "Succeeded~~"
endif

clean:
	rm -f *.symvers
	rm -f *.ko
	rm -f *.o
	rm -f *.mod.c
	rm -f .*.cmd
	rm -rf .tmp_versions