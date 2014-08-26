/**
 * @file time_pool.h
 * @author Christofer Lind, Adam Risberg
 * @date 28/5 2014
 * @brief Generates a .csv-file with runtime statistics for mandelbrot calculation without threadpool.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/mandelbrot.h"
#include "../include/colorpalette.h"
#include <math.h>
#include <pthread.h>
#include "../include/threadpool.h"
#include <time.h>

#define NUMTASKS 16000 // 1600 // 16000
#define POOLSIZE 64 // 300 //868

long long timespecToMs(struct timespec spec)
{
    return(spec.tv_sec*1000LL + spec.tv_nsec/1000000);
}


void renderImageTime(double x, double y, double zoom, colorPalette * c, int numThreads)
{
    int imageWidth = 512, imageHeight = 512, iterations = 1000;
    double ratio = (double)imageHeight/(double)imageWidth;

    struct mandelData * d = mandel_createMandelData(iterations, x-1/zoom, y+1/zoom*ratio, x+1/zoom, y-1/zoom*ratio, imageWidth, imageHeight, c);

    mandel_render(d, 0, numThreads);
}


int main()
{
    int numThreads = 1;
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
    file = fopen("./bin/times_nopool.csv","a+"); /* apend file (add text to a file)*/

    fprintf(file, "---mandelbrot time test, no pool---\n");
    fprintf(file, "imageSize = 512x512, iterations = 1000\n");
    fprintf(file, "format: numThreads, time\n\n");

    for (numThreads = 1; numThreads <= 128; numThreads += 2) {

        //time
        clock_gettime(CLOCK_REALTIME, &tspec);
        before = timespecToMs(tspec);

        renderImageTime(x, y, zoom, c, numThreads);

        //  withpool time
        clock_gettime(CLOCK_REALTIME, &tspec);
        withpool = timespecToMs(tspec) - before;

        // save withpool results to file

        fprintf(file,"%d, %d\n", numThreads*numThreads, withpool); /*writes*/
    }

    fclose(file);

    return 0;
}
