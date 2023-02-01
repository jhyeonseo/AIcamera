#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

unsigned short matrixHeart[8][8] = {
					{ 0, 0, 0, 0, 0, 0, 0, 0 }, // 0
					{ 0, 1, 1, 0, 0, 1, 1, 0 }, // 1
					{ 1, 1, 1, 1, 1, 1, 1, 1 }, // 2
					{ 1, 1, 1, 1, 1, 1, 1, 1 }, // 3
					{ 1, 1, 1, 1, 1, 1, 1, 1 }, // 4
					{ 0, 1, 1, 1, 1, 1, 1, 0 }, // 5
					{ 0, 0, 1, 1, 1, 1, 0, 0 }, // 6
					{ 0, 0, 0, 1, 1, 0, 0, 0 }, // 7
};
unsigned short fuck[8][8] = {
					{ 0, 0, 0, 0, 0, 0, 0, 0 }, // 0
					{ 0, 0, 1, 1, 1, 1, 0, 0 }, // 1
					{ 0, 0, 1, 1, 1, 1, 0, 0 }, // 2
					{ 0, 0, 1, 1, 1, 1, 0, 0 }, // 3
					{ 0, 0, 1, 1, 1, 1, 0, 0 }, // 4
					{ 1, 1, 1, 1, 1, 1, 1, 1 }, // 5
					{ 1, 1, 1, 1, 1, 1, 1, 1 }, // 6
					{ 1, 1, 1, 1, 1, 1, 1, 1 }, // 7
};
unsigned short rowPin[8] = { 0,1,2,3,4,5,6,7 };
unsigned short colPin[8] = { 8,9,10,11,12,13,14,15 };
unsigned short high = 16;
unsigned short low = 0;

int main(int argc, char** argv)
{
	unsigned short data = 0;
	int dev = open("/dev/my_led", O_RDWR);

	if (dev == -1)
	{
		printf("Opening wa not possible!\n");
		return -1;
	}
	printf("device opening was successfull!\n");

	for (int i = 0; i < 8; i++)
	{
		data = rowPin[i] + high;
		write(dev, &data, 1);
		data = colPin[i] + low;
		write(dev, &data, 1);
	}
	sleep(1);
	while (1)
	{
		for (int i = 0; i < 8; i++)      // row
		{
			for (int j = 0; j < 8; j++)  // col
			{
				data = colPin[j] + high * fuck[i][j];
				write(dev, &data, 1);
			}
			data = rowPin[i] + low;
			write(dev, &data, 1);
			usleep(50);
			data = rowPin[i] + high;
			write(dev, &data, 1);
		}
	}

	close(dev);
	return 0;
}