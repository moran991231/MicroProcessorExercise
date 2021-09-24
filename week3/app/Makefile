CC = arm-linux-gnueabi-gcc
ADB=adb
TARGET = sm9s5422_led_test
TARGET_SRC = $(TARGET).c

all: $(TARGET)
	$(CC) $(TARGET_SRC) -static -o $(TARGET)

	echo
	echo " **** Install: " /data/local/tmp/sm9s5422_led_test "****"
	$(ADB) push sm9s5422_led_test /data/local/tmp
	$(ADB) shell chmod 755 /data/local/tmp/sm9s5422_led_test

clean:
	rm -f *.o
	rm -f *.i
	rm -f *.s
	rm -f sm9s5422_led_test

