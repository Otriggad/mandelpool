/**
 * @file colorpalette.c
 * @author Adam Risberg
 * @date
 * @brief
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    unsigned char ** colors;
    int numColors;
    double colorWrapping;
} colorPalette;

colorPalette * color_createPalette(int numColors)
{
    colorPalette * c = (colorPalette*) malloc(sizeof(colorPalette));

    unsigned char ** colors = (unsigned char**) malloc(sizeof(unsigned char*) * numColors);
    for(int a = 0; a < numColors; a++) {
        colors[a] = (unsigned char*) malloc(sizeof(unsigned char) * 3);
    }

    c->colors = colors;
    c->numColors = numColors;
    c->colorWrapping = 15.0;

    return c;
}

void color_destroyPalette(colorPalette* c)
{
    for(int a = 0; a < c->numColors; a++) {
        free(c->colors[a]);
    }
    free(c->colors);
    free(c);
}

void color_setColor(colorPalette * c, unsigned char r, unsigned char g, unsigned char b, int location)
{
    c->colors[location][0] = r;
    c->colors[location][1] = g;
    c->colors[location][2] = b;
}

unsigned int color_sample(colorPalette * c, double v)
{
    v *= c->colorWrapping;
    v = fmod(v, 1.0);
    double location = ((double)(c->numColors - 1) * v);

    if(location >= c->numColors) location = c->numColors - 1;
    if(location < 0) location = 0;

    unsigned char color1[3];
    unsigned char color2[3];
    color1[0] = c->colors[(int)floor(location)][0];
    color1[1] = c->colors[(int)floor(location)][1];
    color1[2] = c->colors[(int)floor(location)][2];

    color2[0] = c->colors[(int)ceil(location)][0];
    color2[1] = c->colors[(int)ceil(location)][1];
    color2[2] = c->colors[(int)ceil(location)][2];

    double fi = location - floor(location);
    double f = 1.0 - fi;

    color1[0] = color1[0] * f + color2[0] * fi;
    color1[1] = color1[1] * f + color2[1] * fi;
    color1[2] = color1[2] * f + color2[2] * fi;

    unsigned int red = color1[0];
    unsigned int green = color1[1] << 8;
    unsigned int blue = color1[2] << 16;
    unsigned int alpha = 255 << 24;

    return red | green | blue | alpha;
}

unsigned int color_blend(unsigned int * colors, int numColors)
{
    unsigned int redTot = 0, greenTot = 0, blueTot = 0;
    for(int a = 0; a < numColors; a++) {
        redTot += (colors[a] >> 0) & 255;
        greenTot += (colors[a] >> 8) & 255;
        blueTot += (colors[a] >> 16) & 255;
    }

    redTot /= numColors;
    greenTot /= numColors;
    blueTot /= numColors;

    unsigned int color = redTot | (greenTot << 8) | (blueTot << 16) | (255 << 24);

    return color;
}
