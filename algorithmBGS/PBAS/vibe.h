/** ViBe is a background subtraction algorithm, described in:
 *
 * Olivier Barnich and Marc Van Droogenbroeck,
 * ViBe: A Universal Background Subtraction Algorithm for Video Sequences,
 * IEEE Transactions on Image Processing, Vol. 20, No. 6, June 2011.
**/
#ifndef VIBE_H
#define VIBE_H

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

typedef struct
{
    unsigned char *samples;
    int numberOfSamples;
    int sizeOfSample;
}pixel;

typedef struct
{
    pixel *pixels;
    int width;
    int height;
    int stride;
    int numberOfSamples;
    int matchingThreshold;
    int matchingNumber;
    int updateFactor;
}vibeModel;

typedef vibeModel vibeModel_t;

static unsigned int *rnd;
static unsigned int rndSize;
static unsigned int rndPos;

vibeModel *libvibeModelNew();
vibeModel *libvibeModelNew(int numSamples, int radius, int minMatchNum, int update);
unsigned char getRandPixel(const unsigned char *image_data, const int width, const int height, const int stride, const unsigned int x, const unsigned int y);
int32_t libvibeModelInit(vibeModel *model, const unsigned char *image_data, const int width, const int height, const int stride);
int32_t libvibeModelUpdate(vibeModel *model, const unsigned char *image_data, unsigned char *segmentation_map);
int32_t libvibeModelFree(vibeModel *model);

void initRnd(unsigned int size);
unsigned int freeRnd();

#endif // VIBE_H
