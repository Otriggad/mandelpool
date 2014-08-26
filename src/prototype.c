/**
 * @file prototype.c
 * @author Christofer Lind, Sebastian Rautila, Adam Risberg, Andreas Widmark
 * @date 7/6 2014
 * @brief Used to test the mandelpool by rendering only one image to a file.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../include/mandelbrot.h"
#include "../include/colorpalette.h"

/**
 * @brief writes the calculated Mandelbrot set to a .ppm image.
 * @param img Pointer to the calculated mandelbrot image
 * @param outputFile Target filename
 * @param width Image width
 * @param height Image height
 */
void printPPMImage(unsigned int * img, char * outputFile, int width, int height)
{
    FILE * f = fopen(outputFile, "w");
    if (f == NULL) {
        perror("output target folder doesnt exist!");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "P6\n%d %d\n255\n", width, height);
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            fwrite(&(img[y * width + x]), 3, 1, f);
        }
    }
    fclose(f);
}

/**
 * @brief Renders image of the mandelbrot set and writes it to a .ppm file
 * @param x X coordinate of image center
 * @param y Y coordinate of image center
 * @param zoom Zoom percentage
 * @param c colorPalette chosen
 * @param fileName Target filename
 */
void renderImage(double x, double y, double zoom, colorPalette * c, char * fileName)
{
    int iterations = 1024*2, imageWidth = 512*4, imageHeight = 512*4;
    double ratio = (double)imageHeight/(double)imageWidth;

    struct mandelData * d = mandel_createMandelData(iterations, x-1/zoom, y+1/zoom*ratio, x+1/zoom, y-1/zoom*ratio, imageWidth, imageHeight, c);

    printPPMImage(mandel_render(d, 1, 1), fileName, imageWidth, imageHeight);
    mandel_render(d, 8, 8);
    mandel_destroyMandelData(d);
}

int main()
{
    // interesting coordinates
    //double x = 0.001643721971153, y = 0.8224676332988;
    //double x = -0.77568377, y = 0.13646737;
    //double x = -1.768573656315270993281742915329544712934120053, y = -0.00096429685135828000017642720373819448274776122656563565285783153307047554366;
    double x = 0.0, y = 0.0;
    double zoom = 1;

    colorPalette * c = color_createPalette(7);
    color_setColor(c, 0, 0, 0, 0);
    color_setColor(c, 0, 15, 120, 1);
    color_setColor(c, 0, 125, 186, 2);
    color_setColor(c, 245, 234, 102, 3);
    color_setColor(c, 0, 125, 186, 4);
    color_setColor(c, 0, 15, 120, 5);
    color_setColor(c, 0, 0, 0, 6);

    static char fileName[] = "images/output.ppm";
    renderImage(x, y, zoom, c, fileName);

    color_destroyPalette(c);

    return 0;
}
