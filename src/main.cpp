#include <fstream>
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace cv::dnn;

unsigned short paint_zero[8][8] = {
                    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 0
                    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 1
                    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 2
                    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 3
                    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 4
                    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 5
                    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 6
                    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 7
};
unsigned short paint_hit[8][8] = {
                    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 0
                    { 0, 1, 1, 0, 0, 1, 1, 0 }, // 1
                    { 1, 1, 1, 1, 1, 1, 1, 1 }, // 2
                    { 1, 1, 1, 1, 1, 1, 1, 1 }, // 3
                    { 1, 1, 1, 1, 1, 1, 1, 1 }, // 4
                    { 0, 1, 1, 1, 1, 1, 1, 0 }, // 5
                    { 0, 0, 1, 1, 1, 1, 0, 0 }, // 6
                    { 0, 0, 0, 1, 1, 0, 0, 0 }, // 7
};
unsigned short paint_miss[8][8] = {
                    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 0
                    { 0, 1, 1, 0, 0, 1, 1, 0 }, // 1
                    { 1, 0, 0, 1, 1, 0, 0, 1 }, // 2
                    { 1, 0, 0, 1, 1, 0, 0, 1 }, // 3
                    { 1, 0, 0, 0, 0, 0, 0, 1 }, // 4
                    { 0, 1, 0, 0, 0, 0, 1, 0 }, // 5
                    { 0, 0, 1, 0, 0, 1, 0, 0 }, // 6
                    { 0, 0, 0, 1, 1, 0, 0, 0 }, // 7
};
unsigned short paint_none[8][8] = {
                    { 1, 0, 0, 0, 0, 0, 0, 1 }, // 0
                    { 0, 1, 0, 0, 0, 0, 1, 0 }, // 1
                    { 0, 0, 1, 0, 0, 1, 0, 0 }, // 2
                    { 0, 0, 0, 1, 1, 0, 0, 0 }, // 3
                    { 0, 0, 0, 1, 1, 0, 0, 0 }, // 4
                    { 0, 0, 1, 0, 0, 1, 0, 0 }, // 5
                    { 0, 1, 0, 0, 0, 0, 1, 0 }, // 6
                    { 1, 0, 0, 0, 0, 0, 0, 1 }, // 7
};

void driver_start();
void driver_close(int dev);
int sonic_open();
int sonic(int dev);
int led_open();
void led(int dev, unsigned short paint[8][8]);
int camera();

int sonic_dev;
int led_dev;
CascadeClassifier cascade;
VideoCapture cap;
int main(int argc, char** argv)
{
    driver_start();
    int distance, position, yposition, xposition;

    while (1)
    {
        distance = sonic(sonic_dev);
        if (distance > 0)
        {
            position = camera();
            if (position > 0)
            {
                int flag = 0;
                xposition = position % 4;   // 1 = 중앙, 2 = 왼쪽, 3 = 오른쪽
                yposition = position / 4;   // 0 = 중앙, 1 = 낮다, 2 = 높다

                if (distance == 1)
                    printf("Close\n", position);
                else if (distance == 2)
                    flag++;
                else if (distance == 3)
                    printf("Far\n");

                if (xposition == 2)
                    printf("left\n", position);
                else if (xposition == 3)
                    printf("right\n", position);
                else if(xposition == 1)
                    flag++;

                if (yposition == 1)
                    printf("low\n", position);
                else if (yposition == 2)
                    printf("high\n", position);
                else if (yposition == 0)
                    flag++;

                if (flag == 3)
                {
                    printf("Ready\n");
                    led(led_dev, paint_hit);
                }
                else
                    led(led_dev, paint_miss);
            }
            else
                led(led_dev, paint_none);
        }
        else
            led(led_dev, paint_none);
    }

    driver_close(led_dev);
    driver_close(sonic_dev);
    return 0;
}
void driver_start()
{
    sonic_dev = sonic_open();
    led_dev = led_open();
    led(led_dev, paint_zero);
    cap.open("/dev/video0", CAP_V4L2);
    cascade.load("data/haarcascade_frontalface_default.xml");
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
        printf("Sonic opening was not possible!\n");
        return -1;
    }
    printf("Sonic opening was successfull!\n");

    return dev;
}
int sonic(int dev)
{
    unsigned short buff;
    unsigned short tmp;
    struct timeval begin, end;

    tmp = 0;
    write(dev, &tmp, 1);
    usleep(2);
    tmp = 1;
    write(dev, &tmp, 1);
    usleep(10);
    tmp = 0;
    write(dev, &tmp, 1);

    // 1인 시간 측정 (PulseIn 함수의 기능)
    unsigned short count = 0;
    while (1)
    {
        buff = 0;
        read(dev, &buff, 1);
        count++;
        if (count > 50000)
            break;
        if (buff == 1)
        {
            gettimeofday(&begin, NULL);
            while (1)
            {
                count++;
                if (count > 50000)
                    break;
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
    int dis = (int)((double)len / 58.0);
    if ((dis > 200) || (count > 50000) || (dis < 0))
        return 0;
    else if (dis > 100)
        return 3;
    else if (dis > 30)
        return 2;

    return 1;
}
int led_open()
{
    int dev = open("/dev/my_led", O_RDWR);
    if (dev == -1)
    {
        printf("Led opening was not possible!\n");
        return -1;
    }
    printf("Led opening was successfull!\n");

    return dev;
}
void led(int dev, unsigned short paint[8][8])
{
    unsigned short data = 0;
    unsigned short rowPin[8] = { 0,1,2,3,4,5,6,7 };
    unsigned short colPin[8] = { 8,9,10,11,12,13,14,15 };
    unsigned short count = 0;
    while (count < 500)
    {
        count++;
        for (int i = 0; i < 8; i++)      // row
        {
            for (int j = 0; j < 8; j++)  // col
            {
                data = colPin[j] + 16 * paint[i][j];
                write(dev, &data, 1);
            }
            data = rowPin[i];
            write(dev, &data, 1);
            usleep(100);
            data = rowPin[i] + 16;
            write(dev, &data, 1);
        }
    }
    return;
}
int camera()
{
    Mat img;
    cap.read(img);
    if (img.empty())
        return -1;
    resize(img, img, Size(416, 416));
    std::vector<Rect> faces;
    cascade.detectMultiScale(img, faces, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(32, 32));

    int max = 238;
    int min = 178;

    if (faces.size() == 0)
    {
        //imwrite("background.bmp", img);
        return 0;
    }
    else
    {
        int x, y;
        for (int i = 0; i < faces.size(); i++)
        {
            x = (int)(faces[i].x + (faces[i].width) / 2);
            y = (int)(faces[i].y + (faces[i].height) / 2);
            //circle(img, Point(x, y), 10, Scalar(255, 0, 255), 1, 8, 0);

            if (max <= x)       
            {
                if (max <= y)
                {
                 //   imwrite("pictures/leftlow.bmp", img);
                    return 2 + 4;
                }
                else if (min >= y)
                {
                //    imwrite("pictures/lefthigh.bmp", img);
                    return 2 + 8;
                }
                else
                {
                //    imwrite("pictures/left.bmp", img);
                    return 2;
                }
                    
            }
            else if (min >= x)  
            {
                if (max <= y)
                {
                //    imwrite("pictures/rightlow.bmp", img);
                    return 3 + 4;
                }
                else if (min >= y)
                {
                //    imwrite("pictures/righthigh.bmp", img);
                    return 3 + 8;
                }
                else
                {
                //    imwrite("pictures/right.bmp", img);
                    return 3;
                }
            }
            else  
            {
                if (max <= y)
                {
                 //   imwrite("pictures/low.bmp", img);
                    return 1 + 4;
                }
                else if (min >= y)
                {             
                 //   imwrite("pictures/high.bmp", img);
                    return 1 + 8;
                }
                else
                {
                 //   imwrite("pictures/correct.bmp", img);
                    return 1;
                }
            }
        }
    }
    
    return -1;
}
