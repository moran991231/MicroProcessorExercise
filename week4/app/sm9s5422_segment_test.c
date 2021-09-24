#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>

static struct termios initial_settings, new_settings;
static int peek_character = -1;

void init_keyboard() {
	tcgetattr(0,&initial_settings);
	new_settings = initial_settings;
	new_settings.c_lflag &= ~ICANON;
	new_settings.c_lflag &= ~ECHO;
	new_settings.c_lflag &= ~ISIG;
	new_settings.c_cc[VMIN] = 1;
	new_settings.c_cc[VTIME] = 0;
	tcsetattr(0,TCSANOW,&new_settings);
}

void close_keyboard() {
	tcsetattr(0,TCSANOW,&initial_settings);
}

int kbhit() {
	char ch;
	int nread;

	if(peek_character != -1) return 1;

	new_settings.c_cc[VMIN] = 0;
	tcsetattr(0,TCSANOW,&new_settings);
	nread = read(0,&ch,1);
	new_settings.c_cc[VMIN] = 1;
	tcsetattr(0,TCSANOW,&new_settings);

	if(nread == 1) {
		peek_character = ch;
		return 1;
	}
	return 0;
}

int readch() {
	char ch;

	if(peek_character != -1) {
		ch = peek_character;
		peek_character = -1;
		return ch;
	}
	read(0,&ch,1);
	return ch;
}

int main(int argc, char * argv[]) {
	int fd, value=1, i, count, temp ,ch;
	unsigned short input, dir =0;
	char temp_val[6] ={0};

	if((fd=open("/dev/sm9s5422_segment", O_RDWR|O_SYNC))<0){
		printf("FND open failed\n");
		exit(1);
	}

	init_keyboard();

	printf(" --------------------\n");
	printf("   7 segment Program \n");
	printf(" --------------------\n");
	printf("    [c] counter      \n");
	printf("    [q] exit         \n");
	printf(" --------------------\n\n");

	while( input !='q'){
		input ='r';
		if(kbhit()){
			ch = readch();
			switch (ch)
			{
			case 'c':
			case 'q':
				input=ch;
				break;			 
			}
		}

	switch(input){
		case 'c':
			printf("Input conter value (0: exit program, 999999~01 ) :");
			close_keyboard();
			scanf("%d", &value);
			init_keyboard();
			count = value;
			
			printf(" --------------------\n");
			printf("        Counter      \n");
			printf(" --------------------\n");
			printf("    [p] pause      \n");
			printf("    [c] continue    \n");
			printf("    [r] reset    \n");
			printf("    [q] exit    \n");
			printf(" --------------------\n\n");
			ioctl(fd, 0, NULL, NULL);
			dir='c';

			while(dir != 'q'){
				int temp = count;
				for(int i=5; 0<=i; i--){
					temp_val[i] = temp%10;
					temp /=10;
				}

				if(dir=='c'){
					if(count <= 0) break;
					for( i=0; i<14; i++)
						write(fd, &temp_val, 6);
					count--;
				}else if(dir=='r'){
					count=value;
					dir='c';
				}else if (dir='p'){
					for(i=0; i<14; i++)
						write(fd, &temp_val, 6);
				}
				if(kbhit()){
					ch = readch();
					switch(ch){
						case 'c':
						case 'p':
						case 'r':
						case 'q':
							dir=ch;
							break;
					}
				}
			}
			
		printf(" --------------------\n");
		printf("   7 segment Program \n");
		printf(" --------------------\n");
		printf("    [c] counter      \n");
		printf("    [q] exit         \n");
		printf(" --------------------\n\n");
		break;

	case 'q':
		break;
	}
	}
	
	close_keyboard();
	close(fd);
		return 0;
}
