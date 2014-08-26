/**
 * @file mandelbrot.c
 * @author Christofer Lind, Sebastian Rautila, Adam Risberg
 * @date 7/6 2014
 * @brief A concurrent mandelbrot-set visualizer using a threadpool.
 */

#include "../include/mandelbrot.h"

//private structs and functions

unsigned int COLOR_BLACK = 0 | (255 << 24);
unsigned int COLOR_WHITE = 0 | (255 << 0) | (255 << 8) | (255 << 16) | (255 << 24);
unsigned int COLOR_GREEN = 0 | (255 << 8) | (255 << 24);

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
struct mandelData {
    int iterations;
    rectangle location;
    int width, height;
    colorPalette * c;
    unsigned int * image;
};

/**
 * @struct mandelJobArg
 * @brief Struct used to pass arguments to the threadpool.
 */
typedef struct mandelJobArg {
    rectangle calcLocation;
    mandelData * data;
} mandelJobArg;

/**
 * @struct brotStruct.
 * @brief internal struct.
 */
typedef struct {
    int n;
    double x;
    double y;
} brotStruct;

/**
 * @struct threadArgs.
 * @brief internal struct.
 */
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

/**
 * @brief Checks if a points is in the mandelbrot-set or not.
 * @param fx x-position in the complex-plane.
 * @param fy y-position in the complex-plane.
 * @param nMax the maximum number of iterations.
 * @return A struct containing the final iteration-count n, and the position of the point after n iterations.
 */
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

/**
 * @brief Does a fast and approximate distance-estimation from the give coordinate to the mandelbrot-set.
 * @param fx x-position in the complex-plane.
 * @param fy y-position in the complex-plane.
 * @return The estimated distance.
*/
double mandelDist(double x0, double y0)
{
    double i = 0;
    double x = 0.0, y = 0.0, xTemp = -1.0, yTemp = -1.0;
    double dx = 1.0, dy = 0.0, dxTemp = 0.0, dyTemp = 0.0;

    while(x*x + y*y < 10.0 && i < 300) {
        xTemp = x*x - y*y + x0;
        yTemp = 2.0*x*y + y0;

        dxTemp = 2.0*x*dx - 2.0*y*dy + 1.0;
        dyTemp = 2.0*x*dy + 2.0*y*dx;

        x = xTemp;
        y = yTemp;

        dx = dxTemp;
        dy = dyTemp;

        i++;
    }

    double dist = 2.0 * sqrt(x*x+y*y)*log(sqrt(x*x+y*y))/sqrt(dx*dx+dy*dy);

    if(i >= 300) dist = -1;

    return dist;
}

/**
 * @brief Calculates the color of a single point.
 * @param fx x-position in the complex-plane.
 * @param fy y-position in the complex-plane.
 * @param m The struct containing the settings of the visualization.
 * @return 8-bit rgb color encoded in a 24-bit int.
 */
unsigned int getColor(double fx, double fy, mandelData * m)
{
    brotStruct bs = inBrot(fx, fy, m->iterations);

    double f = log( log(sqrt(bs.x*bs.x+bs.y*bs.y)) / log(10))/log(2.0);
    if(f!=f) {
        f=0;
    }
    double v = (bs.n - f) / 1000.0;

    unsigned int color = color_sample(m->c, v);

    if(bs.n >= m->iterations) {
        return 0 | (255 << 24);
    }

    return color;
}

/**
 * @brief Calculates the color of a point in the set with anti-aliasing.
 * @param fx x-position in the complex-plane.
 * @param fy y-position in the complex-plane.
 * @param m The struct containing the settings of the visualization.
 * @param pixelSize The size of a pixel give in complex-plane coordinates.
 * @param pixelDivids Anti-aliasing level. 0 means just 1 sample per pixel, 1 means 9 samples per pixel, 2 means 25 samples per pixel.
 * @return 8-bit rgb color encoded in a 24-bit int.
 */
unsigned int colorPixel(double fx, double fy, mandelData * m, double pixelSize, int pixelDivids)
{
    int numSamples = (pixelDivids * 2 + 1);
    numSamples *= numSamples;

    unsigned int samples[numSamples];
    int sampleAt = 0;

    bool shouldBreak = false;

    unsigned int c;

    for(int x = -pixelDivids; x <= pixelDivids; x++) {
        for(int y = -pixelDivids; y <= pixelDivids; y++) {
            if(shouldBreak) c = 0;
            else c = getColor(fx + pixelSize/(double)(pixelDivids * 2 + 1) * (double)x, fy + pixelSize/(double)(pixelDivids * 2 + 1) * (double)y, m);

            if(c == (0U | (255 << 24))) shouldBreak = true; // 0U to fix warning for comparison between signed and unsigned

            samples[sampleAt] = c;
            sampleAt++;
        }
    }

    return color_blend(samples, numSamples);
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

    //a size of a screenpixel given i complex-plane coordinates
    double pixelSize = calcLocation.w/(double)rectScreenWidth;

    //zoom-estimation for scaling of the distance-estimation to the mandelbrot-set
    double zoomEst = 1.0/(m->location.w/2.0);

    //when calcLocation has been mapped to screen-coordinates, each of the pixels in calcLocation is calculated
    for(int x = xScreen; x <= xScreen + rectScreenWidth; x++) {
        for(int y = yScreen; y <= yScreen + rectScreenHeight; y++) {
	    //a hack used to solve a problem causes some pixels to be outside the image
            if(x >= m-> width || y >= m-> height) continue;

            //translate from screen-coordinates to coodrinates in the complex-plane
            double fx = calcLocation.x + (double)(x - xScreen)/(double)rectScreenWidth*calcLocation.w;
            double fy = calcLocation.y + (double)(y - yScreen)/(double)rectScreenHeight*calcLocation.h;

            double dist = mandelDist(fx, fy);

	    //if a pixel is to far away from the mandelbrot-set it does not need antialiasing
            if(dist > 0.05/zoomEst) m->image[y * m->width + x] = colorPixel(fx, fy, m, pixelSize, 0);
            else m->image[y * m->width + x] = colorPixel(fx, fy, m, pixelSize, 1);
        }
    }
}

/**
 * @brief The function called by the threadpool. Decodes the void pointer.
 * @param arg The arguments supplied by the user when the job was created.
 */
void mandelJob(void * arg)
{
    mandelJobArg * jobArg = (mandelJobArg*) arg;
    calculateRectangle(jobArg->calcLocation, jobArg->data);
    free(jobArg);
}

/**
 * @brief The function called by the thread created by mandel_renderUnifinished.
 * @param arg a threadArgs struct cast as a void pointer.
 */
void * threadFunc(void * arg)
{
    struct threadArgs * t = (struct threadArgs *)arg;
    mandel_render(t->m, t->numthreads, t->split);
    free(t);
    pthread_exit(NULL);
    return NULL;
}


// ---public functions---

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

    //init the image and set every pixel to 0
    m->image = (unsigned int *) malloc(sizeof(int) * imageWidth * imageHeight);
    for(int a = 0; a < imageWidth * imageHeight; a++) m->image[a] = 0;

    return m;
}

unsigned int * mandel_render(mandelData * m, int numthreads, int split)
{
    rectangle ** subRects = divideRectangle(m->location, split);

    struct threadpool * p = threadpool_create(numthreads);

    //put jobs into the threadpool
    for (int x = 0; x < split; x++) {
        for(int y = 0; y < split; y++) {
            mandelJobArg * jobArg = (mandelJobArg*) malloc(sizeof(mandelJobArg));
            jobArg->calcLocation = subRects[x][y];
            jobArg->data = m;
            threadpool_enqueue(p, mandelJob, jobArg);
        }
    }

    //free memory
    threadpool_destroy(p);

    for(int a = 0; a < split; a++) {
        free(subRects[a]);
    }
    free(subRects);

    return m->image;
}

renderThread * mandel_renderUnfinished(mandelData * m, int numthreads, int split)
{
    struct threadArgs * t = (struct threadArgs*) malloc(sizeof(struct threadArgs));

    t->m = m;
    t->numthreads = numthreads;
    t->split = split;

    //create a new thread that creates and frees the threadpool
    renderThread * newThread = malloc(sizeof(renderThread));
    pthread_create(&newThread->thread, NULL, threadFunc, t);
    newThread->image = m->image;

    return newThread;
}

void mandel_destroyMandelData(mandelData * m)
{
    free(m->image);
    free(m);
}
