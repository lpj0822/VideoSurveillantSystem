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
    unsigned int numberOfSamples;
    unsigned int sizeOfSample;
}pixel;

typedef struct
{
    pixel *pixels;
    unsigned int width;
    unsigned int height;
    unsigned int stride;
    unsigned int numberOfSamples;
    unsigned int matchingThreshold;
    unsigned int matchingNumber;
    unsigned int updateFactor;
}vibeModel;

typedef vibeModel vibeModel_t;

static unsigned int *rnd;
static unsigned int rndSize;
static unsigned int rndPos;

vibeModel *libvibeModelNew();
vibeModel *libvibeModelNew(unsigned int numSamples,unsigned int radius,unsigned int minMatchNum,unsigned int update);
unsigned char getRandPixel(const unsigned char *image_data, const unsigned int width, const unsigned int height, const unsigned int stride, const unsigned int x, const unsigned int y);
int32_t libvibeModelInit(vibeModel *model, const unsigned char *image_data, const unsigned int width, const unsigned int height, const unsigned int stride);
int32_t libvibeModelUpdate(vibeModel *model, const unsigned char *image_data, unsigned char *segmentation_map);
int32_t libvibeModelFree(vibeModel *model);

void initRnd(unsigned int size);
unsigned int freeRnd();

#endif // VIBE_H
