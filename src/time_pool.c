/**
 * @file time_pool.h
 * @author Christofer Lind, Adam Risberg
 * @date 28/5 2014
 * @brief Generates a .csv-file with runtime statistics for mandelbrot calculation with threadpool.
 */

#include <stdio.h>
#include <stdlib.h>
#include "mandelbrot.h"
#include "colorpalette.h"
#include <math.h>
#include <pthread.h>
#include "threadpool.h"
#include <time.h>

#define NUMTASKS 16000 // 1600 // 16000
#define POOLSIZE 64 // 300 //868

long long timespecToMs(struct timespec spec)
{
    return(spec.tv_sec*1000LL + spec.tv_nsec/1000000);
}


void renderImageTime(double x, double y, double zoom, colorPalette * c, int numThreads, int splits)
{
    int imageWidth = 1024/2, imageHeight = 1024/2, iterations = 1000;
    double ratio = (double)imageHeight/(double)imageWidth;

    struct mandelData * d = mandel_createMandelData(iterations, x-1/zoom, y+1/zoom*ratio, x+1/zoom, y-1/zoom*ratio, imageWidth, imageHeight, c);

    mandel_render(d, numThreads, splits);
}


int main()
{
    int numThreads = 1;
    double splits = 2.0;
    struct timespec tspec;

    double x = 0.001643721971153, y = 0.8224676332988;
    double zoom = 0.5;
    long long withpool, before;

    colorPalette * c = color_createPalette(7);
    color_setColor(c, 0, 0, 0, 0);
    color_setColor(c, 0, 33, 109, 1);
    color_setColor(c, 255, 192, 0, 2);
    color_setColor(c, 255, 255, 255, 3);
    color_setColor(c, 255, 192, 0, 4);
    color_setColor(c, 96, 0, 16, 5);
    color_setColor(c, 0, 0, 0, 6);

    FILE *file;
    file = fopen("./bin/times_pool.csv","a+"); /* apend file (add text to a file)*/

    fprintf(file, "---mandelpool time tests with threadpool---\n");
    printf("---mandelpool time tests with threadpool---\n");
    fprintf(file, "imageSize: 512x512, iterations: 1000\n\n");
    printf("imageSize: 512x512, iterations: 1000\n\n");

    int nJobs = 1;

    //while (poolsiz < 32) {
    for(numThreads = 1; numThreads <= 128; numThreads *= 2) {
        fprintf(file, "\ntests with numThreads = %i\n\n", numThreads);
        printf("\ntests with numThreads = %i\n\n", numThreads);
        fprintf(file, "format: numJobs, time\n");
        printf("format: numJobs, time\n");

        int add = 2;

        if(numThreads > nJobs*nJobs) nJobs *= 2;

        int steps = (int)(128.0/10.0);

        for(splits = nJobs; splits <= 128; splits *= 1.5) {

            if(splits > 10) add = 5;
            if(splits > 50) add = 10;

            long long totalTime = 0;

            for(int i = 0; i < 8; i++) {

                //time
                clock_gettime(CLOCK_REALTIME, &tspec);
                before = timespecToMs(tspec);

                renderImageTime(x, y, zoom, c, numThreads, splits);

                //  withpool tim
                clock_gettime(CLOCK_REALTIME, &tspec);
                withpool = timespecToMs(tspec) - before;

                totalTime += withpool;
            }

            totalTime /= 8;

            // save withpool results to file

            fprintf(file,"%d, %d\n", (int)(splits*splits), totalTime);
            printf("%d, %d\n", (int)(splits*splits), totalTime);
        }
    }

    fclose(file);

    return 0;
}
