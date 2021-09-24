#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include<sys/stat.h>
#include <fcntl.h>

int main(int argc, char * argv[]){
    int dev, i;
    char temp, buf[] = {0};
    unsigned char t =0;

    if(argc<=1){
        printf("please input the parameter! ex) ./test 0xff \n");
        return -1;
    }
    dev = open("/dev/sm9s5422_led",O_WRONLY);

    if(dev != -1){
        if(argv[1][0] == '0' &&(argv[1][1] == 'x'|| argv[1][1] == 'X') )
        {
            temp = strtol(argv[1], NULL, 16);
        }else{
            temp = atoi(argv[1]);
        }
        printf("temp: %d  \n",(int)temp);
        for(int i=0; i<8; i++){
            buf[i] = (char)((temp>>i)%2);
            printf("%d",(int)buf[i]);
        }
        printf("\n");
        
    }else{
        printf("Device Open Error \n");
        return -1;
    }

    write(dev, buf, 8);
    close(dev);
    
    return 0;
    
}