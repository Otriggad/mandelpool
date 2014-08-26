/**
 * @file colorpalette.h
 * @author Adam Risberg
 * @date 6/6 2014
 * @brief The interface for a colorpalette.
 */

#ifndef COLORPALETTE_H
#define COLORPALETTE_H

/**
 * @struct colorPalette
 * @brief the @ref colorPalette struct contains a number of colors, which can be blended to create a gradient.
 */
typedef struct colorPalette colorPalette;

/**
 * @brief Creates a colorPalette struct.
 * @param numColors the number of colors to be in the palette.
 * @return A colorPalette struct.
 */
colorPalette * color_createPalette(int numColors);

/**
 * @brief Destroy and deallocates a colorPalette struct.
 * @param c A colorPalette struct.
 */
void color_destroyPalette(colorPalette* c);

/**
 * @brief Sets one of the colors in the given colorPalette struct.
 * @param c The colorPalette.
 * @param r The r-component of an rgb-color.
 * @param g The g-component of an rgb-color.
 * @param b The b-component of an rgb-color.
 * @param location The index of the color to be set.
 */
void color_setColor(colorPalette * c, unsigned char r, unsigned char g, unsigned char b, int location);

/**
 * @brief Samples the colorPalette-gradient.
 * @param c The colorPalette.
 * @param v A value between 0 <= v <= 1. Will sample and interpolate the color at index v*numColors in the palette.
 * @return The interpolated color as a 24-bit rgb int.
 */
unsigned int color_sample(colorPalette * c, double v);

/**
 * @brief Blends a number of colors together into one color.
 * @param colors An array of 24-bit rgb ints.
 * @param numColors The numbers of colors in the array.
 * @return The blended color.
 */
unsigned int color_blend(unsigned int * colors, int numColors);

#endif
