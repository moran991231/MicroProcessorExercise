#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

static struct termios initial_settings, new_settings;
static int peek_character = -1;

void init_keyboard() {
    tcgetattr(0, &initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
}

void close_keyboard() { tcsetattr(0, TCSANOW, &initial_settings); }

int kbhit() {
    char ch;
    int nread;

    if (peek_character != -1) return 1;

    new_settings.c_cc[VMIN] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0, &ch, 1);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings);

    if (nread == 1) {
        peek_character = ch;
        return 1;
    }
    return 0;
}

int readch() {
    char ch;

    if (peek_character != -1) {
        ch = peek_character;
        peek_character = -1;
        return ch;
    }
    read(0, &ch, 1);
    return ch;
}

void print_info() {
    printf(" --------------------\n");
    printf("   7 segment Program \n");
    printf(" --------------------\n");
    printf("    [c] counter      \n");
    printf("    [q] exit         \n");
    printf(" --------------------\n\n");
}
void print_info_command() {
    printf(" --------------------\n");
    printf("        Counter      \n");
    printf(" --------------------\n");
    printf("    [p] pause        \n");
    printf("    [c] continue     \n");
    printf("    [r] reset        \n");
    printf("    [q] exit         \n");
    printf(" --------------------\n\n");
}

#define NUM_WRITE 14

int counter_program(int fd) {
    int count, value, i;
    unsigned short dir;
    char temp_val[6] = {0};
    printf("Input counter value (0: exit program, 999999~01 ) :");
    close_keyboard();
    scanf("%d", &value);  // get number to count
    init_keyboard();
    if (value == 0) return -1;  // if 0, exit

    print_info_command();
    ioctl(fd, 0, NULL, NULL);
    dir = 'r';

    while (dir != 'q') {
        // get command continue(c), puase(p), reset(r), exit(q)
        if (kbhit()) dir = readch();

        if (dir == 'c') {
            if (count <= 0) return 0;  // count down ended
            count--;
        } else if (dir == 'r') {
            count = value;
            dir = 'c';
        }  // else if (dir = 'p') {} do nothing especially

        // write decimal digit on temp_val[]
        int temp = count;
        for (int i = 5; 0 <= i; i--) {
            temp_val[i] = temp % 10;
            temp /= 10;
        }
        // write segment
        for (i = 0; i < NUM_WRITE; i++) write(fd, &temp_val, 6);
    }
    return 0;
}
int main(int argc, char *argv[]) {
    int fd, value = 1, i, temp, ch;
    unsigned short input;

    if ((fd = open("/dev/sm9s5422_segment", O_RDWR | O_SYNC)) < 0) {
        printf("FND open failed\n");
        exit(1);
    }
    init_keyboard();
    print_info();

    while (input != 'q') {
        if (kbhit()) input = readch();

        switch (input) {
            case 'c':
                if (counter_program(fd) == -1) {
                    input = 'q';
                    break;
                }
                print_info();
                break;
        }
    }
    close_keyboard();
    close(fd);
    return 0;
}
