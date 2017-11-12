#ifndef BGMAPEXTRACTOR_Grid_H
#define BGMAPEXTRACTOR_Grid_H

#include <cmath>

class BGMapExtractor_Grid
{
public:
    BGMapExtractor_Grid(void);
    ~BGMapExtractor_Grid(void);

    //initialization function
    //If we have multiple images to be processed, we only need to call this function once.
    //But note that the iMaxWidth and iMaxHeight must be set to the maximum size of all images
    //Usually if the image size is about 400*400, we can set patch size to 10*10
    bool Initialize(int iMaxWidth,  //max width of all images to be processed
                    int iMaxHeight, //max height of all images to be processed
                    int iPatchW,    //width of the grid
                    int iPatchH);   //height of the grid

    bool BGMapExtract(const unsigned char *pbyRgbPixels, //image pixels
                        int iWidth,                      //width of the image
                        int iHeight,                     //height of the image
                        int iStride,                     //stride of the image
                        float *pfBGMap);                 //saliency map (need to be allocated memory before calling this function)

private:
    bool m_bInit;
    void Release();

    unsigned char *m_pbyGridRgbPixels;
    float *m_pfGridLabPixels;

    int *m_piGridRgbSum;

    int m_iWidth;
    int m_iHeight;
    int m_iPatchWidth;
    int m_iPatchHeight;
    int m_iPatchNumX;
    int m_iPatchNumY;
    int m_iPatchNum;
    int m_iBoundaryPatchNum;

    void BuildGridRgbImage(const unsigned char *pRgbPixels, int iWidth, int iHeight, int iStride);
    void ConvertToGridLabImage();

    void BuildNeighboringGraph();

    void BoundaryAnalysis();

    void InitBGScores();
    bool UpdateBGScores();
    bool NormalizeBGScores();

    void OutputBGMap(int iWidth, int iHeight, float* pfBGmap);


    float *m_pfBGScores;
    int *m_piPreviousIds;
    int *m_piNeighborPatchIds;
    float *m_pfNeighborPatchDists;


    float m_fNeighborDistThre;
    
    int *m_piBoundaryIds;
    float *m_pfBoundaryBgScores;

    static __forceinline float Fxyz(const float t)
    {
        return ((t > 0.008856f)? pow(t, (1.0f/3.0f)) : (7.787f*t + 16.0f/116.0f));
    }
    static __forceinline void RgbToLab(unsigned char byR, unsigned char byG, unsigned char byB, float &fL, float &fa, float &fb)
    {
        // RGBtoXYZ
        // Normalize red, green, blue values.
        float rLinear = (float)byR/255.0f;
        float gLinear = (float)byG/255.0f;
        float bLinear = (float)byB/255.0f;

        // Convert to a sRGB form
        float r = (rLinear > 0.04045f)? powf((rLinear + 0.055f)/(1.055f), 2.2f) : (rLinear/12.92f) ;
        float g = (gLinear > 0.04045f)? powf((gLinear + 0.055f)/(1.055f), 2.2f) : (gLinear/12.92f) ;
        float b = (bLinear > 0.04045f)? powf((bLinear + 0.055f)/(1.055f), 2.2f) : (bLinear/12.92f) ;

        // Converts
        float x = r*0.4124f + g*0.3576f + b*0.1805f;
        float y = r*0.2126f + g*0.7152f + b*0.0722f;
        float z = r*0.0193f + g*0.1192f + b*0.9505f;

        x = (x>0.9505f)? 0.9505f : ((x<0.f)? 0.f : x);
        y = (y>1.0f)? 1.0f : ((y<0.f)? 0.f : y);
        z = (z>1.089f)? 1.089f : ((z<0.f)? 0.f : z);

        // XYZ to LAB
        float dD65_X = 0.9505f;
        float dD65_Y = 1.0f;
        float dD65_Z = 1.0890f;

        fL = 116.0f * Fxyz( y/dD65_Y ) - 16.f;
        fa = 500.0f * ( Fxyz( x/dD65_X ) - Fxyz( y/dD65_Y) );
        fb = 200.0f * ( Fxyz( y/dD65_Y ) - Fxyz( z/dD65_Z) );

    }
};

#endif // BGMAPEXTRACTOR_Grid_H
