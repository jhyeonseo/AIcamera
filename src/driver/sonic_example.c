#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

void driver_close(int dev);
int sonic_open();
int sonic(int dev);
int main(int argc, char** argv)
{
    int sonic_dev = sonic_open();
    
    while (1)
    {
        if (sonic(sonic_dev))
            printf("ON!\n");
        else
            printf("OFF!\n");
    }

    usleep(10000);
    
    return 0;
}
void driver_close(int dev)
{
    close(dev);
}
int sonic_open()
{
    int dev = open("/dev/my_sonic", O_RDWR);
    if (dev == -1)
    {
        printf("Opening was not possible!\n");
        return -1;
    }
    printf("Opening was successfull!\n");

    return dev;
}
int sonic(int dev)
{
    unsigned short buff;
    unsigned short tmp;
    struct timeval begin, end;

    tmp = 0;
    write(dev, &tmp, 1);
    usleep(2000);
    tmp = 1;
    write(dev, &tmp, 1);
    usleep(10000);
    tmp = 0;
    write(dev, &tmp, 1);

    // 1인 시간 측정 (PulseIn 함수의 기능)
    while (1)
    {
        buff = 0;
        read(dev, &buff, 1);
        if (buff == 1)
        {
            gettimeofday(&begin, NULL);
            while (1)
            {
                buff = 1;
                read(dev, &buff, 1);
                if (buff == 0)
                {
                    gettimeofday(&end, NULL);
                    break;
                }
            }
            break;
        }

    }
    double len = (double)(end.tv_usec - begin.tv_usec);
    if ((len > 17400) || (len < 870))
    {
        return 0;
    }
    else
    {
        double distance = ((double)len / 58.0);
        // printf("%f, %f\n", len, distance);
        return 1;
    }
}