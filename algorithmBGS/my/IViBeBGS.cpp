#include "ivibebgs.h"
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#ifdef __USE_TBB
// Used for parallel processing.
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
using namespace tbb;
#endif // __USE_TBB

IViBeBGS::IViBeBGS()
{
    init();
    std::cout<<"IViBeBGS()"<<std::endl;
}

IViBeBGS::~IViBeBGS()
{
    std::cout<<"~IViBeBGS()"<<std::endl;
}

void IViBeBGS::initModel(cv::Mat &bg)
{
    // Test parameters.
    CV_Assert(bg.rows >= 16 && bg.cols >= 16);
    CV_Assert(bg.type () == CV_8UC1 || bg.type () == CV_8UC3);
    CV_Assert(samples_per_pixel >= 1);

    // Keep the values.
    this->img_size = bg.size();
    this->img_type = bg.type();

    // Initialize.
    for (int i = 0; i < samples_per_pixel; i++)
        bg_samples.push_back( cv::Mat::zeros(img_size, img_type));

    initBGModel(bg);
    estimated_bg = bg.clone();
}

#ifndef __USE_TBB

/*============================================================================*/
/* FOREGROUND SEGMENTATION AND MODEL UPDATE                                   */
/*============================================================================*/
/** This is the central function of the detector. It detects foreground objects,
 * storing the results as a binary motion mask. Furthermore, the internal
 * background model is updated.
 *
 * Parameters: Mat& img: the original image. Must have the same size and type
 *               as the first sample given to the constructor.
 *             Mat& mask: output binary motion mask.
 *
 * Return value: none. We use the mask parameter, and update internal structures.
**/

void IViBeBGS::detectAndUpdate (const cv::Mat &img, cv::Mat& mask)
{
    CV_Assert(img.rows == img_size.height && img.cols == img_size.width);
    CV_Assert(img.type() == img_type);

    int count = 0;
    uchar** model_ptr = new unsigned char* [samples_per_pixel];
    mask=cv::Mat::zeros(img_size,CV_8UC1);

    for (int row = 0; row < img.rows-1; row++)
    {
        const uchar* img_ptr = img.ptr<uchar>(row);
        uchar* mask_ptr = mask.ptr<uchar>(row);
        uchar* estimated_bg_ptr=estimated_bg.ptr<uchar>(row);
        for (int i = 0; i < samples_per_pixel; i++)
            model_ptr[i] = bg_samples[i].ptr<uchar>(row);

        for (int col = 0; col < img.cols-1; col++)
        {
            // Check if this is a background pixel. Compare to the pixels in the model.
            count = 0;
            for (int i = 0; i < samples_per_pixel && count < min_neighbors_for_bg; i++)
            {
                if(pixelDistance(img_ptr, model_ptr[i], img.channels()) <= max_distance_for_bg)
                    count++;
            }

            if (count >= min_neighbors_for_bg) // Classified as background
            {
                *mask_ptr = 0;

                // Update... sometimes.
                if (rng.uniform(0,update_probability-1) == 0)
                {
                    uchar* to_replace = model_ptr[rng.uniform(0,samples_per_pixel-1)];
                    for (int i = 0; i < img.channels (); i++)
                    {
                        to_replace[i] = img_ptr[i];
                        estimated_bg_ptr[i] = img_ptr[i];
                    }
                }

                // Propagate to the neighborhood... sometimes.
                if (rng.uniform(0,update_probability-1) == 0)
                {
                    int row_offset = (rng.uniform(0,2)-1)*(int)img.step[0]; // -1, 0 or 1, *img.step to place in another row.
                    if (row == 0 && row_offset < 0)
                        row_offset = 0; // Avoid pixels outside the image.
                    if (row == img.rows-1 && row_offset > 0)
                        row_offset = 0; // Avoid pixels outside the image.

                    int col_offset = (rng.uniform(0,2)-1)*img.channels(); // -1, 0 or 1, *the number of channels.
                    if (col == 0 && col_offset < 0)
                        col_offset = 0; // Avoid pixels outside the image.
                    if (col == img.cols-1 && col_offset > 0)
                        col_offset = 0; // Avoid pixels outside the image.

                    uchar* to_replace = model_ptr[rng.uniform(0,samples_per_pixel-1)] + row_offset + col_offset;
                    for (int i = 0; i < img.channels(); i++)
                    {
                        to_replace[i] = img_ptr[i];
                    }
                }
            }
            else
            {
                *mask_ptr = 255;
            }

            img_ptr += img.channels();
            estimated_bg_ptr += estimated_bg.channels();
            for (int i = 0; i < samples_per_pixel; i++)
                model_ptr[i] += img.channels ();
            mask_ptr++;
        }
    }

    if(model_ptr)
    {
        delete [] model_ptr;
        model_ptr=NULL;
    }
}

/** Initialize the background model, based on a single frame. If this image is
 * already a good background estimation, results will improve much faster.
 *
 * Parameters: Mat& sample_bg: image used for initilizing the background model.
 *
 * Return value: none. Internal structures are updated.
**/

void IViBeBGS::initBGModel (cv::Mat& bg)
{
    uchar** model_ptr = new uchar* [samples_per_pixel];

    // Run through the sample background and all the model images.
    for (int row = 0; row < bg.rows; row++)
    {
        uchar* in_ptr= bg.ptr<uchar>(row);
        for (int i = 0; i < samples_per_pixel; i++)
            model_ptr[i] = bg_samples[i].ptr<uchar>(row);

        for (int col = 0; col < bg.cols; col++)
        {
            // Select samples_per_pixel pixels around the pixel in (row,col).
            // Suppose pixels at the margins are duplicated.
            for (int i = 0; i < samples_per_pixel; i++)
            {
                int row_offset = (rng.uniform(0,2)-1)*(int)bg.step[0]; // -1, 0 or 1, *bg.step to place in another row.
                if (row == 0 && row_offset < 0)
                    row_offset = 0; // Avoid pixels outside the image.
                if (row == bg.rows-1 && row_offset > 0)
                    row_offset = 0; // Avoid pixels outside the image.

                int col_offset = (rng.uniform(0,2)-1)*bg.channels(); // -1, 0 or 1, *the number of channels.
                if (col == 0 && col_offset < 0)
                    col_offset = 0; // Avoid pixels outside the image.
                if (col == bg.cols-1 && col_offset > 0)
                    col_offset = 0; // Avoid pixels outside the image.

                unsigned char* selected = in_ptr + row_offset + col_offset;
                for (int channel = 0; channel < bg.channels (); channel++)
                    model_ptr[i][channel] = selected[channel];
            }

            in_ptr += bg.channels();
            for (int i = 0; i < samples_per_pixel; i++)
                model_ptr[i] += bg.channels ();
        }
    }

    if(model_ptr)
    {
        delete [] model_ptr;
        model_ptr=NULL;
    }
}

#else //__USE_TBB

void IViBeBGS::detectAndUpdate (const cv::Mat &img, cv::Mat& mask)
{
    CV_Assert(img.rows == img_size.height && img.cols == img_size.width);
    CV_Assert(img.type() == img_type);

    mask=cv::Mat::zeros(img_size,CV_8UC1);

    // Internal class, used by the parallel_for.
    class TBBDetectAndUpdate
    {
        cv::Mat* img;
        cv::Mat* mask;
        unsigned int min_neighbors_for_bg;
        float max_distance_for_bg;
        unsigned int update_probability;
        cv::Mat* bg_samples;
        cv::Mat* estimated_bg;
        unsigned int samples_per_pixel;

    public:
        void operator () (const blocked_range <int>& range) const
        {
            unsigned char** model_ptr = new unsigned char* [samples_per_pixel];

            for (int row = range.begin (); row < range.end (); row++)
            {
                unsigned char* img_ptr = (unsigned char*) (img->data + row*img->step);
                unsigned char* mask_ptr = (unsigned char*) (mask->data + row*mask->step);
                for (unsigned int i = 0; i < samples_per_pixel; i++)
                    model_ptr [i] = (unsigned char*) (bg_samples [i].data + row*bg_samples [i].step);

                for (int col = 0; col < img->cols; col++)
                {
                    // Check if this is a background pixel. Compare to the pixels in the model.
                    unsigned int count = 0;
                    for (unsigned int i = 0; i < samples_per_pixel && count < min_neighbors_for_bg; i++)
                        if (pixelDistance (img_ptr, model_ptr [i], img->channels ()) <= max_distance_for_bg)
                            count++;

                    if (count >= min_neighbors_for_bg) // Classified as background?
                    {
                        *mask_ptr = 0;

                        // Update... sometimes.
                        if (rand () % update_probability == 0)
                        {
                            unsigned char* estimated_bg_ptr = (unsigned char*) estimated_bg->data + row*estimated_bg->step + col*estimated_bg->channels ();
                            unsigned char* to_replace = model_ptr [rand () % samples_per_pixel];
                            for (int i = 0; i < img->channels (); i++)
                            {
                                to_replace [i] = img_ptr [i];
                                estimated_bg_ptr [i] = img_ptr [i];
                            }
                        }

                        // Propagate to the neighborhood... sometimes.
                        // This part has a little problem when using parallel processing: near the limits of each block, the same pixel may be written by two different threads at the same time.
                        // Instead of employing some kind of access control policy, we simply hope that the problem is rare enough (or so hard to notice) that we can act as if it didn't exist.
                        if (rand () % update_probability == 0)
                        {
                            int row_offset = (rand ()%3-1)*img->step; // -1, 0 or 1, *img.step to place in another row.
                            if (row == 0 && row_offset < 0) row_offset = 0; // Avoid pixels outside the image.
                            if (row == img->rows-1 && row_offset > 0) row_offset = 0; // Avoid pixels outside the image.

                            int col_offset = (rand ()%3-1)*img->channels (); // -1, 0 or 1, *the number of channels.
                            if (col == 0 && col_offset < 0) col_offset = 0; // Avoid pixels outside the image.
                            if (col == img->cols-1 && col_offset > 0) col_offset = 0; // Avoid pixels outside the image.

                            unsigned char* estimated_bg_ptr = (unsigned char*) estimated_bg->data + row*estimated_bg->step + col*estimated_bg->channels ();
                            unsigned char* to_replace = model_ptr [rand () % samples_per_pixel] + row_offset + col_offset;
                            for (int i = 0; i < img->channels (); i++)
                            {
                                to_replace [i] = img_ptr [i];
                                estimated_bg_ptr [i] = img_ptr [i];
                            }
                        }
                    }
                    else
                        *mask_ptr = 255;

                    img_ptr += img->channels ();
                    for (unsigned int i = 0; i < samples_per_pixel; i++)
                        model_ptr [i] += img->channels ();
                    mask_ptr++;
                }
            }

            delete [] (model_ptr);
        }

        TBBDetectAndUpdate (cv::Mat* img, cv::Mat* mask, int min_neighbors_for_bg, float max_distance_for_bg, unsigned int update_probability, cv::Mat* bg_samples, Mat* estimated_bg, unsigned int samples_per_pixel)
        {
            this->img = img;
            this->mask = mask;
            this->min_neighbors_for_bg = min_neighbors_for_bg;
            this->max_distance_for_bg = max_distance_for_bg;
            this->update_probability = update_probability;
            this->bg_samples = bg_samples;
            this->estimated_bg = estimated_bg;
            this->samples_per_pixel = samples_per_pixel;
        }
    };

    parallel_for (blocked_range <int> (0, img.rows), TBBDetectAndUpdate (&img, &mask, min_neighbors_for_bg, max_distance_for_bg, update_probability, bg_samples, &estimated_bg, samples_per_pixel), auto_partitioner ());
}

void IViBeBGS::initBGModel (cv::Mat& sample_bg)
{
    // Internal class, used by the parallel_for.
    class TBBInitBGModel
    {
    private:
        cv::Mat* sample_bg;
        cv::Mat* bg_samples;
        unsigned int samples_per_pixel;

    public:
        void operator () (const blocked_range <int>& range) const
        {
            unsigned char** model_ptr = new unsigned char* [samples_per_pixel];

            // Run through the sample background and all the model images.
            for (int row = range.begin (); row < range.end (); row++)
            {
                unsigned char* in_ptr = (unsigned char*) (sample_bg->data + row*sample_bg->step);
                for (unsigned int i = 0; i < samples_per_pixel; i++)
                    model_ptr [i] = (unsigned char*) (bg_samples [i].data + row*bg_samples [i].step);

                for (int col = 0; col < sample_bg->cols; col++)
                {
                    // Select samples_per_pixel pixels around the pixel in (row,col).
                    // Suppose pixels at the margins are duplicated.
                    for (unsigned int i = 0; i < samples_per_pixel; i++)
                    {
                        int row_offset = (rand ()%3-1)*sample_bg->step; // -1, 0 or 1, *sample_bg.step to place in another row.
                        if (row == 0 && row_offset < 0) row_offset = 0; // Avoid pixels outside the image.
                        if (row == sample_bg->rows-1 && row_offset > 0) row_offset = 0; // Avoid pixels outside the image.

                        int col_offset = (rand ()%3-1)*sample_bg->channels (); // -1, 0 or 1, *the number of channels.
                        if (col == 0 && col_offset < 0) col_offset = 0; // Avoid pixels outside the image.
                        if (col == sample_bg->cols-1 && col_offset > 0) col_offset = 0; // Avoid pixels outside the image.

                        unsigned char* selected = in_ptr + row_offset + col_offset;
                        for (int channel = 0; channel < sample_bg->channels (); channel++)
                            model_ptr [i][channel] = selected [channel];
                    }

                    in_ptr += sample_bg->channels ();
                    for (unsigned int i = 0; i < samples_per_pixel; i++)
                        model_ptr [i] += sample_bg->channels ();
                }
            }

            delete [] (model_ptr);
        }

        TBBInitBGModel (cv::Mat* sample_bg, cv::Mat* bg_samples, unsigned int samples_per_pixel)
        {
            this->sample_bg = sample_bg;
            this->bg_samples = bg_samples;
            this->samples_per_pixel = samples_per_pixel;
        }
    };

    parallel_for(blocked_range <int> (0, sample_bg.rows), TBBInitBGModel (&sample_bg, bg_samples, samples_per_pixel), auto_partitioner ());
}

#endif // __USE_TBB

/** "Color distance" between two pixels, used to decide if they are similar.
 * For 1-channel images, is just the absolute difference between the pixels,
 * for 3-channel images, we use the euclidean distance in the RGB space (even
 * if this does not make that much sense!).
 *
 * Parameters: unsigned char* p1: 1 or 3-position array, representing a pixel.
 *             unsigned char* p2: same as above.
 *             int n_channels: 1 or 3.
 *
 * Return value: the computed distance between p1 and p3.
**/
int IViBeBGS::pixelDistance (const uchar* p1, const uchar* p2, int n_channels)
{
    if (n_channels == 1)
        return abs(*p1 - *p2);

    int diff0 = p1[0] - p2[0];
    int diff1 = p1[1] - p2[1];
    int diff2 = p1[2] - p2[2];

    return (int)sqrt(diff0*diff0 + diff1*diff1 + diff2*diff2);
}

void IViBeBGS::init()
{
    long seed = (long)time(0);
    rng=cv::RNG(seed);
    //srand(seed);//随机数初始化函数
    samples_per_pixel= 20;
    min_neighbors_for_bg = 2;
    max_distance_for_bg = 20;
    update_probability = 16;
    bg_samples.clear();
}
