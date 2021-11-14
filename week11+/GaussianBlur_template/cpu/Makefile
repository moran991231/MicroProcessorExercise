CC = arm-linux-androideabi-gcc
ADB=adb

.SUFFIXS : .c .o

CFLAG =  -g -O3
LDFLAGS =  -lm

TARGET=GaussianBlur
TARGET_SRC =$(TARGET).c bmp.c 
TARGET_OBJ =$(TARGET).o bmp.o

all: $(TARGET)

$(TARGET): $(TARGET_OBJ)
	$(CC) -static $(TARGET_OBJ) $(LDFLAGS) -o $(TARGET) 
	echo 
	echo "**** Install:" /data/local/tmp/$(TARGET)"****"
	$(ADB) push $(TARGET) /data/local/tmp
	$(ADB) push lena.bmp /data/local/tmp
	$(ADB) shell chmod 755 /data/local/tmp/$(TARGET)

.c.o : 
	$(CC) $(CFLAG) -c $< -o $@ 

clean:
	rm -f *.o
	rm -f $(TARGET)

