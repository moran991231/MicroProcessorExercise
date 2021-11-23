#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd, i;
    unsigned short input;

    if (argc != 3)
    {
        printf("usage: %s [number(str)] [time]\n", argv[0]);
    }

    char temp_val[6] = {0};
    for (int i = 0; i < 6; i++)
    {
        char ch = argv[1][i];
        if (ch == '\0')
            break;
        else if ('0' <= ch && ch <= '9')
            temp_val[i] = ch - '0';
        else if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
            temp_val[i] = ch;
    }

    int time = atoi(argv[2]);
    if ((fd = open("/dev/sm9s5422_segment", O_RDWR | O_SYNC)) < 0)
    {
        printf("FND open failed\n");
        exit(1);
    }
    ioctl(fd, 0, NULL, NULL);

    for (i = 0; i < time; i++)
        write(fd, &temp_val, 6);

    close(fd);
    return 0;
}
