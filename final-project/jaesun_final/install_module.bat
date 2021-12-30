adb shell insmod /system/lib/modules/sm9s5422_segment_js.ko
adb shell chmod 777 /dev/sm9s5422_segment_js
adb shell lsmod | findstr sm9s5422_segment_js 