CC = arm-linux-androideabi-g++
ADB=adb

OPENCL_PATH =/home/mpclass/Downloads/OpenCL_lib_and_include
CFLAG =  -I$(OPENCL_PATH)/include -g
LDFLAGS = -l$(OPENCL_PATH)/lib/libGLES_mali.so -lm

TARGET=GaussianBlurGPU
TARGET_SRC =$(TARGET).cpp bmp.c

all: $(TARGET)

$(TARGET): $(TARGET_SRC)
	$(CC) -static $(TARGET_SRC) $(CFLAG) $(LDFLAGS) -fpermissive -o $(TARGET)
	echo
	echo "**** Install:" /data/local/tmp/$(TARGET)"****"
	$(ADB) push $(TARGET) /data/local/tmp
	$(ADB) push lena.bmp /data/local/tmp
	$(ADB) push Blur.cl /data/local/tmp
	$(ADB) shell chmod 755 /data/local/tmp/$(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)
