#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int fd, i;
    unsigned short input;
    struct timeval start, end, timer;

    if (argc != 3) {
        printf("usage: %s [number(str)] [time]\n", argv[0]);
    }
    // time: light_time = 1: 15.45ms

    char temp_val[6] = {0};
    for (int i = 0; i < 6; i++) {
        char ch = argv[1][i];
        if (ch == '\0')
            break;
        else if ('0' <= ch && ch <= '9')
            temp_val[i] = ch - '0';
        else if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
            temp_val[i] = ch;
        else
            temp_val[i] = ' ';
    }

    int time = atoi(argv[2]);
    if ((fd = open("/dev/sm9s5422_segment", O_RDWR | O_SYNC)) < 0) {
        printf("FND open failed\n");
        exit(1);
    }
    ioctl(fd, 0, NULL, NULL);

    gettimeofday(&start, NULL);
    for (i = 0; i < time; i++) write(fd, &temp_val, 6);
    gettimeofday(&end, NULL);
    timersub(&end, &start, &timer);
    double ms = timer.tv_usec / 1000.0 + timer.tv_sec * 1000.0;
    double ms_time = ms / time;
    printf(" ms / time = %lf / %lf = %lf\n", ms, (double)time, ms_time);

    close(fd);
    return 0;
}
