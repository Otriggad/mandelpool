/**
 * @file mandelbrot.c
 * @author Sebastian Rautila, Adam Risberg
 * @date 12/5 2014
 * @brief A concurrent mandelbrot-set visualizer using raw pthreads.
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/threadpool.h"
#include "../include/colorpalette.h"
#include <math.h>

//private structs and functions

/**
 * @struct rectangle
 * @brief A rectangle.
 */
typedef struct rectangle {
    double x, y, w ,h;
} rectangle;

/**
 * @struct mandelData
 * @brief Struct containing settings for a visualization.
 */
typedef struct mandelData {
    int iterations;
    rectangle location;
    int width, height;
    colorPalette * c;
    unsigned int * image;
} mandelData;

/**
 * @struct mandelJobArg
 * @brief Struct used to pass arguments to the threadpool.
 */
typedef struct mandelJobArg {
    rectangle calcLocation;
    mandelData * data;
} mandelJobArg;

typedef struct {
    int n;
    double x;
    double y;
} brotStruct;

struct threadArgs {
    mandelData * m;
    int numthreads;
    int split;
};

/**
 * @brief Divides a rectangle into smaller rectangles.
 * @param rect A rectangle that is to be divided.
 * @param split Divides rect into split*split pieces. All pieces will be the same size.
 * @return An array of smaller rectangles.
 */
rectangle ** divideRectangle(rectangle rect, int split)
{
    //create a 2d-array of size split*split
    rectangle ** subRects = (rectangle**) malloc(sizeof(rectangle*) * split);
    for(int a = 0; a < split; a++) {
        subRects[a] = (rectangle*) malloc(sizeof(rectangle) * split);
    }

    double subRectWidth = rect.w / (double)split;
    double subRectHeight = rect.h / (double)split;

    //split rect and put the smaller rectangles into subRects
    for(int x = 0; x < split; x++) {
        for(int y = 0; y < split; y++) {
            rectangle r;
            r.x = rect.x + subRectWidth * (double)x;
            r.y = rect.y + subRectHeight * (double)y;
            r.w = subRectWidth;
            r.h = subRectHeight;

            subRects[x][y] = r;
        }
    }

    return subRects;
}


/*Mandelbrot equation Z = Z� + C
  z is the real part of Z and zi is the imaginary
  C is a coordinate (x,y)
  (z + zi)� = z� + 2zzi - zi
  z = z� - zi� + x
  zi = 2zzi + y */

brotStruct inBrot(double x0, double y0, int nMax)
{
    int i = 0;
    double x = 0.0, y = 0.0, xTemp = -1.0, yTemp = -1.0;

    while(x*x+y*y < 100.0 && i < nMax) {
        xTemp = x*x - y*y + x0;
        yTemp = 2.0*x*y + y0;

        if(xTemp == x && yTemp == y) {
            i = nMax;
            break;
        }

        x = xTemp;
        y = yTemp;

        i++;
    }

    brotStruct b;
    b.n = i;
    b.x = x;
    b.y = y;

    return b;
}

int colorPixel(double fx, double fy, mandelData * m)
{
    brotStruct bs = inBrot(fx, fy, m->iterations);

    double f = log( log(sqrt(bs.x*bs.x+bs.y*bs.y)) / log(10))/log(2.0);
    if(f!=f) {
        f=0;
    }
    double v = (bs.n - f) / 1000.0;
    v *= 30.0;
    v = fmod(v, 1.0);

    unsigned int color = color_sample(m->c, v);

    /*unsigned int red = color[0];
    unsigned int green = color[1] << 8;
    unsigned int blue = color[2] << 16;
    unsigned int alpha = 255 << 24;*/

    if(bs.n >= m->iterations) {
        return 0 | (255 << 24);
    }

    return color;
}

/**
 * @brief Calculates every pixel in a rectangle.
 * @param calcLocation A rectangle in the complex-plane that is to be calculated.
 * @param m Settings for the visualization.
 */
void calculateRectangle(rectangle calcLocation, mandelData * m)   //added m-> to location.w & h
{
    //the rectangle calcLocation is located in the complex-plane
    //the size and location of this rectangle must be translated into screen-coordinates
    int xScreen = (int)((calcLocation.x - m->location.x)/m->location.w * (double) m->width);
    int yScreen = (int)((calcLocation.y - m->location.y)/m->location.h * (double) m->height);
    int rectScreenWidth = (int)(calcLocation.w/m->location.w*(double)m->width);
    int rectScreenHeight = (int)(calcLocation.h/m->location.h*(double)m->height);

    //when calcLocation has been mapped to screen-coordinates, each of the pixels in calcLocation is calculated
    for(int x = xScreen; x <= xScreen + rectScreenWidth; x++) {
        for(int y = yScreen; y <= yScreen + rectScreenHeight; y++) {
            if(x >= m-> width || y >= m-> height) continue;
            //translate from screen-coordinates to coodrinates in the complex-plane
            double fx = calcLocation.x + (double)(x - xScreen)/(double)rectScreenWidth*calcLocation.w;
            double fy = calcLocation.y + (double)(y - yScreen)/(double)rectScreenHeight*calcLocation.h;

            m->image[y * m->width + x] = colorPixel(fx, fy, m);
        }
    }
}

/**
 * @brief The function called by the threadpool. Decodes the void pointer.
 * @param arg The arguments supplied by the user when the job was created.
 */
void* mandelJob(void * arg)
{
    mandelJobArg * jobArg = (mandelJobArg*) arg;
    calculateRectangle(jobArg->calcLocation, jobArg->data);
    free(jobArg);
    return NULL;
}


//public functions

mandelData * mandel_createMandelData(int iterations, double xFrom, double yFrom, double xTo, double yTo, int imageWidth, int imageHeight, colorPalette * c)
{
    mandelData * m = (mandelData*) malloc(sizeof(mandelData));
    m->iterations = iterations;

    rectangle location;
    location.x = xFrom;
    location.y = yFrom;
    location.w = xTo - xFrom;
    location.h = yTo - yFrom;

    m->location = location;
    m->width = imageWidth;
    m->height = imageHeight;

    m->c = c;

    m->image = (unsigned int *) malloc(sizeof(int) * imageWidth * imageHeight);
    for(int a = 0; a < imageWidth * imageHeight; a++) m->image[a] = 0;

    return m;
}

unsigned int * mandel_render(mandelData * m, int numthreads, int split)
{
    rectangle ** subRects = divideRectangle(m->location, split);

    //struct threadpool * p = threadpool_create(numthreads);

    int threadcount = 0;
    pthread_t threads [split*split];

    //put jobs into the threadpool
    for (int x = 0; x < split; x++) {
        for(int y = 0; y < split; y++) {
            mandelJobArg * jobArg = (mandelJobArg*) malloc(sizeof(mandelJobArg));
            jobArg->calcLocation = subRects[x][y];
            jobArg->data = m;
            //threadpool_enqueue(p, mandelJob, jobArg);
            pthread_create(&(threads[threadcount]), NULL, mandelJob, jobArg);
            ++threadcount;
        }
    }

    //wait for the pool to finish
    //threadpool_wait(p);

    for(int a = 0; a < threadcount; a++) {
        pthread_join(threads[a], NULL);
    }

    for(int a = 0; a < split; a++) {
        free(subRects[a]);
    }
    free(subRects);

    return m->image;
}

void * threadFunc(void * v)
{
    struct threadArgs * t = (struct threadArgs *)v;
    printf("a\n");
    mandel_render(t->m, t->numthreads, t->split);
    printf("b\n");
    free(t);
    return NULL;
}

unsigned int * mandel_renderUnfinished(mandelData * m, int numthreads, int split)
{
    struct threadArgs * t = (struct threadArgs*) malloc(sizeof(struct threadArgs));
    t->m = m;
    t->numthreads = numthreads;
    t->split = split;
    pthread_t * thr = (pthread_t*) malloc(sizeof(pthread_t));
    pthread_create(thr, NULL, threadFunc, t);
    return m->image;
}

void mandel_destroyMandelData(mandelData * m)
{
    free(m->image);
    free(m);
}
