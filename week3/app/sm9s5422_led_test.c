#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int dev, i;
    char temp, buf[8] = {0};  // here
    unsigned char t = 0;

    if (argc <= 1) {  // get led states from parameters
        printf("please input the parameter! ex) ./test 0xff \n");
        return -1;
    }
    dev = open("/dev/sm9s5422_led", O_WRONLY);

    if (dev != -1) {
        if (argv[1][0] == '0' && (argv[1][1] == 'x' || argv[1][1] == 'X')) {
            // if input is hexa-decimal
            temp = (char)strtol(argv[1], NULL, 16);
        } else {
            // if input is decimal
            temp = atoi(argv[1]);
        }

        for (int i = 0; i < 8; i++)  // i-th bit in temp=> buf[i]
            buf[i] = (char)((temp >> i) & 1);
    } else {
        printf("Device Open Error \n");
        return -1;
    }
    write(dev, buf, 8);
    close(dev);
    return 0;
}
