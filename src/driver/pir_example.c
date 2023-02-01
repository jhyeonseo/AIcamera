#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

int pir_open();
unsigned short pir(int dev);
int main()
{
	int pir_dev = pir_open();

	while (1)
	{
		printf("%u\n", pir(pir_dev));
		usleep(100000);
	}

	return 0;
}
int pir_open()
{
	int dev = open("/dev/my_pir", O_RDWR);
	if (dev == -1)
	{
		printf("Opening was not possible!\n");
		return -1;
	}
	printf("Opening was successfull!\n");

	return dev;
}
unsigned short pir(int dev)
{
	unsigned short sensor = 0;
	read(dev, &sensor, 1);

	return sensor;
}