#include "gradientshadowdetector.h"
#include <iostream>

GradientShadowDetector::GradientShadowDetector()
{
    init();
    std::cout << "GradientShadowDetector()" << std::endl;
}

GradientShadowDetector::~GradientShadowDetector()
{
    std::cout << "~GradientShadowDetector()" << std::endl;
}

void GradientShadowDetector::initData()
{
    loadConfig();
}

/*============================================================================*/
/* SHADOW DETECTION                                                           */
/*============================================================================*/
/** The top-level function, which removes shadows from a given full-frame mask.
 * We assume each connected component corresponds to a different foreground
 * object.
 *
 * Parameters: Mat& img: image containing motion.
 *             Mat& mask: (foreground) motion mask.
 *             Mat& bg: the background.
 *             Mat& out: output image. It is the motion mask with shadow pixels
 *               removed.
 *
 * Return value: none.
 */
void GradientShadowDetector::removeShadows(cv::Mat &img, cv::Mat &mask, cv::Mat &bg, cv::Mat &out)
{
    CV_Assert(img.type () == CV_32FC1 || img.type () == CV_8UC1 || img.type () == CV_8UC3);
    CV_Assert(mask.type () == CV_8UC1);
    CV_Assert(bg.type () == CV_32FC1 || bg.type () == CV_8UC1 || bg.type () == CV_8UC3);
    CV_Assert(img.rows == mask.rows && img.rows == bg.rows && img.cols == mask.cols && img.cols == bg.cols);

    if(firstTime)
    {
        saveConfig();
        firstTime=false;
    }

    // The input image and background must be of type CV_32FC1. Convert them if needed.
    cv::Mat float_img = preprocessImage(img);
    cv::Mat float_bg = preprocessImage(bg);

    out=cv::Mat::zeros(mask.size(),mask.type());

    // Compute the gradients.
    cv::Mat img_dx = cv::Mat(float_img .size(),CV_32FC1);
    cv::Mat img_dy = cv::Mat(float_img .size(),CV_32FC1);
    cv::Mat img_mag = cv::Mat(float_img .size(),CV_32FC1);
    computeGradients(float_img, img_dx, img_dy, img_mag);

    cv::Mat bg_dx = cv::Mat(float_bg.size(),CV_32FC1);
    cv::Mat bg_dy = cv::Mat(float_bg.size(),CV_32FC1);
    cv::Mat bg_mag = cv::Mat(float_bg.size(),CV_32FC1);
    computeGradients(float_bg, bg_dx, bg_dy, bg_mag);

    cv::Mat ori_diff, window_ori_diff;
    computeTextureBuffers(img_dx, img_dy, img_mag, bg_dx, bg_dy, bg_mag, ori_diff, window_ori_diff);

    // Run through the images and test each pixel.
    for (int row = 0; row < img.rows; row++)
    {
        unsigned char* ptr_mask = (unsigned char*) (mask.data + row*mask.step);
        float* ptr_img = (float*) (float_img.data + row*float_img.step);
        float* ptr_bg = (float*) (float_bg.data + row*float_bg.step);
        float* ptr_img_mag = (float*) (img_mag.data + row*img_mag.step);
        float* ptr_bg_mag = (float*) (bg_mag.data + row*bg_mag.step);
        float* ptr_ori_diff = (float*) (window_ori_diff.data + row*window_ori_diff.step);
        unsigned char* ptr_out = (unsigned char*) (out.data + row*out.step);

        for (int col = 0; col < float_img.cols; col++)
        {
            if (*ptr_mask) // Foreground candidates.
            {
                if (testLuminance && !luminanceTellsItCanBeShadow (*ptr_img, *ptr_bg))
                    *ptr_out = *ptr_mask;
                else if (testMagnitudes && !magnitudeTellsItCanBeShadow (*ptr_img_mag, *ptr_bg_mag))
                    *ptr_out = *ptr_mask;
                else if (testTexture && !textureTellsItCanBeShadow (*ptr_ori_diff))
                    *ptr_out = *ptr_mask;
                else
                    *ptr_out = 0;
            }

            ptr_mask++;
            ptr_img++;
            ptr_bg++;
            ptr_img_mag++;
            ptr_bg_mag++;
            ptr_ori_diff++;
            ptr_out++;
        }
    }
}

/** A very specific function, called by removeShadows to obtain a CV_32FC1
 * version of the input images.
 *
 * Parameters: Mat& original: original image, of type CV_8UC3, CV_8UC1 or
 *               CV_32FC1.
 *             Mat& gs_buffer: space for the grayscale version of the image,
 *               (if needed).
 *             Mat& float_buffer: space for the floating point version of the
 *               image, (if needed).
 *
 * Return value: a Mat whose data is a region of interest inside the image to be used.
 */
cv::Mat GradientShadowDetector::preprocessImage(cv::Mat& original)
{
    if (original.type () == CV_32FC1)
        return cv::Mat(original); // The image is already ok.

    cv::Mat grayscale;
    if (original.type () == CV_8UC1)
        grayscale = cv::Mat(original);
    else
    {
        grayscale = cv::Mat(original.size(),CV_8UC1);
        cv::cvtColor (original, grayscale, cv::COLOR_BGR2GRAY); // Must first convert to grayscale.
    }

    cv::Mat floating_point = cv::Mat(original.size(),CV_32FC1);
    grayscale.convertTo (floating_point, CV_32FC1, 1.0f/255.0f);
    return floating_point;
}

/** Compute gradient dx, dy, and magnitudes.
 *
 * Parameters: Mat& img: input image, of type 32FC1.
 *             Mat& dx: output parameter for the x derivatives.
 *             Mat& dy: output parameter for the y derivatives.
 *             Mat& mag: output parameter for the magnitudes.
 *
 * Return value: none (we use the output parameter).
 */
void GradientShadowDetector::computeGradients (cv::Mat& img, cv::Mat& dx, cv::Mat& dy, cv::Mat& mag)
{
    // Derivatives.
    cv::Sobel (img, dx, -1, 1, 0, 1);
    cv::Sobel (img, dy, -1, 0, 1, 1);

    // Magnitudes.
    cv::magnitude(dx,dy,mag);
}

/** Another specific function called by removeShadows. This one prepares the
 * orientation difference buffers used by the region texture test.
 *
 * Parameters: Mat& img_dx: x derivatives from the image.
 *             Mat& img_dy: y derivatives from the image.
 *             Mat& img_mag: gradient magnitudes from the image.
 *             Mat& bg_dx: x derivatives from the background.
 *             Mat& bg_dy: y derivatives from the background.
 *             Mat& bg_mag: gradient magnitudes from the background.
 *             Mat& ori_diff: output parameter for the pixel orientation
 *               differences.
 *             Mat& window_ori_diff: output parameter for the region orientation
 *               differences.
 *
 * Return value: none (the output parameters are used).
*/
void GradientShadowDetector::computeTextureBuffers (cv::Mat& img_dx, cv::Mat& img_dy, cv::Mat& img_mag, cv::Mat& bg_dx, cv::Mat& bg_dy, cv::Mat& bg_mag, cv::Mat& ori_diff, cv::Mat& window_ori_diff)
{
    if (testTexture) // Only do something if needed.
    {
        ori_diff = cv::Mat(img_dx.size(),CV_32FC1);
        window_ori_diff = cv::Mat(img_dx.size(),CV_32FC1);
        computeOrientationDifferences(img_dx, img_dy, img_mag, bg_dx, bg_dy, bg_mag, ori_diff);

        if (textureTestFullWindow)
        {
            if (textureTestGaussianWindow)
                cv::GaussianBlur(ori_diff, window_ori_diff, cv::Size (textureWindowWidth, textureWindowWidth), 0);
            else
            {
                // FIXME: This is an inneficient hack, but at least it seems to be working...
                cv::Mat blur1;
                cv::Mat blur2;
                ori_diff.convertTo(blur1, CV_8UC1, 255);
                cv::boxFilter(blur1, blur2, CV_8U, cv::Size (textureWindowWidth, textureWindowWidth));
                blur2.convertTo(window_ori_diff, CV_32FC1, 1.0/255.0);

                // FIXME: the above lines should actually be replaced by the line below:
                // boxFilter (ori_diff, window_ori_diff, CV_32F, Size (textureWindowWidth, textureWindowWidth));
                // However, for some reason the blur and boxFilter functions are not working with images of type CV_32FC1 (they work fine for CV_8UC1), or when textureWindowWidth > 11.
                // I have no idea WHY this happens, but maybe it has something to do with large values being placed in floats.
            }
        }
        else
        {
            // This version works like a box filter, but instead of dividing the sum by the window size, divides by the number of values with orientation difference > 0.
            // The sum can be obtained by a non-normalized boxFilter. FIXME: see comment above.
            // boxFilter (ori_diff, window_ori_diff, CV_32F, Size (textureWindowWidth, textureWindowWidth), Point (-1,-1), false);

            // FIXME: This is an inneficient hack, but at least it seems to be working...
            cv::Mat blur1;
            cv::Mat blur2;
            ori_diff.convertTo (blur1, CV_8U, 255);
            cv::boxFilter (blur1, blur2, CV_8U, cv::Size (textureWindowWidth, textureWindowWidth));
            blur2.convertTo (window_ori_diff, CV_32F, 1.0/255.0*textureWindowWidth*textureWindowWidth);

            // The number of positions with orientation difference > 0 cannot be obtained by a linear filter, but a separable non-linear filter will do the trick.
            // We start counting the non-zero pixels in horizontal windows.
            cv::Mat non_zero_in_rows(ori_diff.size(), CV_32FC1);
            countNonZeroInRows(ori_diff, non_zero_in_rows, textureWindowWidth);

            // We can now use another box filter to add the values vertically.
            cv::Mat non_zero_in_windows (ori_diff.size(),CV_32FC1);
            cv::boxFilter(non_zero_in_rows, non_zero_in_windows, CV_32F, cv::Size (1, textureWindowWidth), cv::Point (-1,-1), false);

            // Almost there...
            cv::divide (window_ori_diff, non_zero_in_windows, window_ori_diff); // The function handles divisions by 0 by setting the result to 0.
        }
    }
}

/** Compute the orientation difference between the image and background
 * gradients, based on the cosine similarity. The cosine similarity is a
 * measure of the difference between two vectors. In the case, the vectors are
 * (img_dx,img_dy) and (bg_dx,bg_dy).
 *
 * Parameters: Mat& img_dx: image derivatives in x.
 *             Mat& img_dy: image derivatives in y.
 *             Mat& img_mag: gradient magnitudes in the image.
 *             Mat& bg_dx: background derivatives in x.
 *             Mat& bg_dy: background derivatives in y.
 *             Mat& bg_mag: gradient magnitudes in the background.
 *             Mat& out: computed differences.
 *
 * Return value: none.
 */
void GradientShadowDetector::computeOrientationDifferences (cv::Mat& img_dx, cv::Mat& img_dy, cv::Mat& img_mag, cv::Mat& bg_dx, cv::Mat& bg_dy, cv::Mat& bg_mag, cv::Mat& out)
{
    // Compute for each pixel...
    for (int row = 0; row < out.rows; row++)
    {
        float* ptr_img_dx = (float*) (img_dx.data + row*img_dx.step);
        float* ptr_img_dy = (float*) (img_dy.data + row*img_dy.step);
        float* ptr_img_mag = (float*) (img_mag.data + row*img_mag.step);
        float* ptr_bg_dx = (float*) (bg_dx.data + row*bg_dx.step);
        float* ptr_bg_dy = (float*) (bg_dy.data + row*bg_dy.step);
        float* ptr_bg_mag = (float*) (bg_mag.data + row*bg_mag.step);
        float* ptr_out = (float*) (out.data + row*out.step);

        for (int col = 0; col < out.cols; col++)
        {
            if (*ptr_img_mag < minMag || *ptr_bg_mag < minMag) // Very weak gradients have unstable orientations.
                *ptr_out = 0;
            else
            {
                *ptr_out = *ptr_img_dx * *ptr_bg_dx + *ptr_img_dy * *ptr_bg_dy; // Numerator.
                *ptr_out /= sqrtf ((*ptr_img_dx * *ptr_img_dx + *ptr_img_dy * *ptr_img_dy) * (*ptr_bg_dx * *ptr_bg_dx + *ptr_bg_dy * *ptr_bg_dy)); // Denominator.
                *ptr_out = (float)( acos (*ptr_out) / CV_PI); // Take the actual angle, to get the actual angle difference. Divide by PI to normalize.
            }

            ptr_img_dx++;
            ptr_img_dy++;
            ptr_img_mag++;
            ptr_bg_dx++;
            ptr_bg_dy++;
            ptr_bg_mag++;
            ptr_out++;
        }
    }
}

/** Count the non-zero pixels in a window with size 1 x width. Used as the
 * first half of a filter that counts non-zero pixels inside a window.
 *
 * Parameters: Mat& in: count here.
 *             Mat& out: put the results here.
 *             int width: window width.
 *
 * Return value: none (use the output parameter).
 */
void GradientShadowDetector::countNonZeroInRows(cv::Mat& in, cv::Mat& out, int width)
{
    int center_offset = width/2;
    for (int row = 0; row < in.rows; row++)
    {
        float* ptr_in = (float*) (in.data + row*in.step);
        float* ptr_out = (float*) (out.data + row*out.step);

        // Fill the left and right with 0s.
        for (int col = 0; col < center_offset; col++)
        {
            ptr_out [col] = 0;
            ptr_out [in.cols-1-col] = 0;
        }

        // Start counting non-zero pixels in the first columns.
        float count = 0;
        for (int col = 0; col < width; col++)
            if (ptr_in [col] > 0)
                count++;
        ptr_out [center_offset] = count;

        // Update for each column, while sliding the window.
        for (int col = center_offset+1; col < in.cols - center_offset; col++)
        {
            if (ptr_in [col-center_offset-1] > 0)
                count--;
            if (ptr_in [col+center_offset] > 0)
                count++;
            ptr_out [col] = count;
        }
    }
}

/** A function with a VERY descriptive name. It takes a pixel's intensity /
 * luminance value in the image and in the background, and based on that, tells
 * if this can be a shadow pixel in the image. Normally, a shadow pixel is
 * darker in the image. However, this rule can become unreliable for very low
 * pixel values. Moreover, we can reasonably assume a very bright pixel in the
 * image is not a shadow, even if it is darker than in the background.
 *
 * Parameters: float in_img: pixel intensity in the image.
 *             float in_bg: pixel intensity in the background.
 *
 * Return value: true if the test indicates the pixel can be a shadow pixel.
 */
bool GradientShadowDetector::luminanceTellsItCanBeShadow (float in_img, float in_bg)
{
    // The pixel is very dark in the image. We assume it can be a shadow pixel, even if it is brighter in the image than in the background.
    if (in_img < minMag)
        return true;

    // The pixel is very bright in the image. We assume it cannot be a shadow pixel.
    if (in_img > 1.0f - minMag)
        return false;

    // Shadows must be darker in the image...
    if (in_img < in_bg)
        return true;

    return false;
}

/** A function with a VERY descriptive name. It compares the gradient
 * magnitudes in the image and in the background. Shadows tend to have weaker
 * gradients.
 *
 * Parameters: float img_mag: gradient magnitude in the image.
 *             float bg_mag: gradient magnitude in the background.
 *
 * Return value: true if the test indicates the pixel can be a shadow pixel.
 */
bool GradientShadowDetector::magnitudeTellsItCanBeShadow (float img_mag, float bg_mag)
{
    // There is an edge in the image, but not in the background.
    // As shadows tend to make gradients weaker, this is probably not a shadow pixel.
    if (img_mag > minMag && img_mag > bg_mag)
        return false;

    // Large magnitude differences indicate the presence of edges only in the background or in the image.
    // That means we are probably looking at different structures.
    if (fabs (img_mag - bg_mag) > largeMagDiff)
        return false;

    return true;
}

/** A function with a VERY descriptive name. It checks the average orientation
 * difference computed from a region around one pixel. Shadows tend to have
 * low values here.
 *
 * Parameters: float img_mag: gradient magnitude in the image.
 *             float bg_mag: gradient magnitude in the background.
 *
 * Return value: true if the test indicates the pixel can be a shadow pixel.
 */
bool GradientShadowDetector::textureTellsItCanBeShadow (float ori_diff)
{
    if (textureTestFullWindow)
    {
        if (textureTestGaussianWindow)
            return (ori_diff < textureTestGaussianWindowThreshold);

        return (ori_diff < textureTestBoxWindowThreshold);
    }

    return (ori_diff < textureTestThreshold);
}

void GradientShadowDetector::init()
{
    firstTime=true;
    loadConfig();
}

void GradientShadowDetector::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/GradientShadowDetector.xml",cv::FileStorage::WRITE);

    cv::write(fs, "testLuminance", testLuminance);
    cv::write(fs, "testMagnitudes", testMagnitudes);
    cv::write(fs, "testTexture", testTexture);
    cv::write(fs,"textureTestFullWindow",textureTestFullWindow);
    cv::write(fs,"textureTestGaussianWindow",textureTestGaussianWindow);

    cv::write(fs, "minMag", minMag);
    cv::write(fs,"largeMagDiff",largeMagDiff);
    cv::write(fs,"textureWindowWidth",textureWindowWidth);
    cv::write(fs,"textureTestThreshold",textureTestThreshold);
    cv::write(fs,"textureTestBoxWindowThreshold",textureTestBoxWindowThreshold);
    cv::write(fs,"textureTestGaussianWindowThreshold",textureTestGaussianWindowThreshold);

    fs.release();
}

void GradientShadowDetector::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/GradientShadowDetector.xml",cv::FileStorage::READ);

    cv::read(fs["testLuminance"], testLuminance,true);
    cv::read(fs["testMagnitudes"],testMagnitudes, true);
    cv::read(fs["testTexture"], testTexture,true);
    cv::read(fs["textureTestFullWindow"],textureTestFullWindow,false);
    cv::read(fs["textureTestGaussianWindow"],textureTestGaussianWindow,true);

    cv::read(fs["minMag"], minMag,0.05f);
    cv::read(fs["largeMagDiff"],largeMagDiff,0.5f);
    cv::read(fs["textureWindowWidth"],textureWindowWidth,25);
    cv::read(fs["textureTestThreshold"],textureTestThreshold,0.5f);
    cv::read(fs["textureTestBoxWindowThreshold"],textureTestBoxWindowThreshold,0.05f);
    cv::read(fs["textureTestGaussianWindowThreshold"],textureTestGaussianWindowThreshold,0.05f);

    fs.release();
}

