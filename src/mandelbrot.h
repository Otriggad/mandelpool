/**
 * @file mandelbrot.h
 * @author Christofer Lind, Sebastian Rautila, Adam Risberg
 * @date 7/6 2014
 * @brief Contains the interface for the mandelbrot-set visualizer.
 */

#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "threadpool.h"
#include "colorpalette.h"

/**
 * @struct mandelData
 * @brief the @ref mandelData struct contains the settings for a visualization of the mandelbrot set.
 */
typedef struct mandelData mandelData;

/**
 * @struct renderThread
 * @brief the @ref renderThread struct is used to keep track of the thread holding the threadpool and the image rendered.
 */
typedef struct renderThread {
    unsigned int * image;
    pthread_t thread;
} renderThread;

/**
 * @brief Creates a mandelData struct.
 * @param iterations Maximum number of iterations when deciding if a coordinate is in the mandelbrot set.
 * @param xFrom Left bound in the complex-plane that is to be rendered.
 * @param yFrom Upper bound in the complex-plane that is to be rendered.
 * @param xTo Right bound in the complex-plane that is to be rendered.
 * @param yTo Bottom bound in the complex-plane that is to be rendered.
 * @param imageWidth The width in pixels of the final image.
 * @param imageHeight The height in pixels of the final image.
 * @param c Colorpalette for coloring the visualization.
 * @return A new mandelData struct with the given values.
 */
struct mandelData * mandel_createMandelData(int iterations, double xFrom, double yFrom, double xTo, double yTo, int imageWidth, int imageHeight, colorPalette * c);

/**
 * @brief Renders a visualization of the mandelbrot-set.
 * @param m The settings of the visualization.
 * @param numthreads The number of threads to use for rendering the set.
 * @param split The number of splits to be done. Number of squares = split^2.
 * @return An image with the dimesions given in the settings. Basically a 3d-array with dimensions width*height*3, where 3 is the rgb componenets of each pixel.
 */
unsigned int * mandel_render(struct mandelData * m, int numthreads, int split);

/**
 * @brief Renders a visualization of the mandelbrot-set but instantly returns the image, even if it is not finished.
 * @param m The settings of the visualization.
 * @param numthreads The number of threads to use for rendering the set.
 * @param split The number of sprits to be done. Number of squares = split^2.
 * @return A pointer to a struct cointaining the rendered image and the thread running the threadpool. The image may not be finished and the thread must be joined to free resources.
 */
renderThread * mandel_renderUnfinished(struct mandelData * m, int numthreads, int split);

/**
 * @brief Destroys and deallocates a mandelData struct
 * @param m A mandelData struct.
 */
void mandel_destroyMandelData(mandelData * m);

#endif
