#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int dev;

int main(int argc, char* argv[]) {
    char buff[100];
    dev = open("/dev/sm9s5422_interrupt", O_RDONLY);  // options ??

    if (dev < 0) {
        printf("device open error! \n");
        return -1;
    }

    printf("Please push the button \n");
    read(dev, buff, 100);
    printf("\n %s \r\n\n", buff);

    close(dev);
    return 0;
}