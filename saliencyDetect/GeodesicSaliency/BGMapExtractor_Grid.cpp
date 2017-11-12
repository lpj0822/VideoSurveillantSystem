#include "BGMapExtractor_Grid.h"
#include <string.h>
#include <queue>
#include <algorithm>
#include <functional>

#define NEIGHBORSIZE 4
#define INFINITE 0xFFFFFFFF

BGMapExtractor_Grid::BGMapExtractor_Grid(void)
{
    m_bInit = false;
    m_pbyGridRgbPixels = NULL;
    m_pfGridLabPixels = NULL;
    m_piGridRgbSum = NULL;

    m_iWidth = 0;
    m_iHeight = 0;
    m_iPatchWidth = 0;
    m_iPatchHeight = 0;
    m_iPatchNumX = 0;
    m_iPatchNumY = 0;
    m_iPatchNum = 0;
    m_iBoundaryPatchNum = 0;

    m_pfBGScores = NULL;
    m_piPreviousIds = NULL;
    m_piNeighborPatchIds = NULL;
    m_pfNeighborPatchDists = NULL;

    m_fNeighborDistThre = 0.f;
    m_pfBoundaryBgScores = NULL;
}

BGMapExtractor_Grid::~BGMapExtractor_Grid(void)
{
    if (m_bInit)
    {
        Release();
    }
}

void BGMapExtractor_Grid::Release()
{
    if(m_pbyGridRgbPixels!=NULL)
    {
        delete[] m_pbyGridRgbPixels;
        m_pbyGridRgbPixels=NULL;
    }
    if(m_pfGridLabPixels!=NULL)
    {
        delete[] m_pfGridLabPixels;
        m_pfGridLabPixels=NULL;
    }
    if(m_piGridRgbSum!=NULL)
    {
        delete[] m_piGridRgbSum;
        m_piGridRgbSum=NULL;
    }

    if(m_pfBGScores!=NULL)
    {
        delete[] m_pfBGScores;
        m_pfBGScores=NULL;
    }
    if(m_piPreviousIds!=NULL)
    {
        delete[] m_piPreviousIds;
        m_piPreviousIds=NULL;
    }
    if(m_piNeighborPatchIds!=NULL)
    {
        delete[] m_piNeighborPatchIds;
        m_piNeighborPatchIds=NULL;
    }
    if(m_pfNeighborPatchDists!=NULL)
    {
        delete[] m_pfNeighborPatchDists;
        m_pfNeighborPatchDists=NULL;
    }

    if(m_piBoundaryIds!=NULL)
    {
        delete[] m_piBoundaryIds;
        m_piBoundaryIds=NULL;
    }
    if(m_pfBoundaryBgScores)
    {
        delete[] m_pfBoundaryBgScores;
        m_pfBoundaryBgScores=NULL;
    }

    m_iWidth = 0;
    m_iHeight = 0;
    m_iPatchWidth = 0;
    m_iPatchHeight = 0;
    m_iPatchNumX = 0;
    m_iPatchNumY = 0;
    m_iPatchNum = 0;
    m_iBoundaryPatchNum = 0;
        
    m_fNeighborDistThre = 0.f;

    m_bInit = false;
}

bool BGMapExtractor_Grid::Initialize(int iMaxWidth, int iMaxHeight, int iPatchW, int iPatchH)
{
    if (m_bInit)
    {
        Release();
    }
    m_bInit = false;

    if (iMaxHeight < 1 || iMaxWidth < 1 || iPatchW < 1 || iPatchH < 1)
    {
        return false;
    }

    if (4 != NEIGHBORSIZE) //current code only support 4-neighbors
    {
        return false; 
    }
    m_iPatchWidth = iPatchW;
    m_iPatchHeight = iPatchH;

    int iMaxPatchNumX = (int)((float)iMaxWidth/(float)m_iPatchWidth+0.5f);
    int iMaxPatchNumY = (int)((float)iMaxHeight/(float)m_iPatchHeight+0.5f);

    int iMaxPatchNum = iMaxPatchNumX * iMaxPatchNumY;
    int iMaxBoundaryPatchNum = (iMaxPatchNumX + iMaxPatchNumY)*2-4;

    bool bRet = true;
    do{
        m_pbyGridRgbPixels = new unsigned char[iMaxPatchNum*3];
        if (NULL == m_pbyGridRgbPixels)
        {
            bRet = false;
            break;
        }
        m_pfGridLabPixels = new float[iMaxPatchNum*3];
        if (NULL == m_pfGridLabPixels)
        {
            bRet = false;
            break;
        }
        m_piGridRgbSum = new int[iMaxPatchNum*3];
        if (NULL == m_piGridRgbSum)
        {
            bRet = false;
            break;
        }
        m_pfBGScores = new float[iMaxPatchNum];
        if (NULL == m_pfBGScores)
        {
            bRet = false;
            break;
        }
        m_piPreviousIds = new int[iMaxPatchNum];
        if (NULL == m_piPreviousIds)
        {
            bRet = false;
            break;
        }
        m_piNeighborPatchIds = new int[iMaxPatchNum * NEIGHBORSIZE];
        if (NULL == m_piNeighborPatchIds)
        {
            bRet = false;
            break;
        }
        m_pfNeighborPatchDists = new float[iMaxPatchNum * NEIGHBORSIZE];
        if (NULL == m_pfNeighborPatchDists)
        {
            bRet = false;
            break;
        }
        m_piBoundaryIds = new int[iMaxBoundaryPatchNum];
        if (NULL == m_piBoundaryIds)
        {
            bRet = false;
            break;
        }
        m_pfBoundaryBgScores = new float[iMaxBoundaryPatchNum];
        if (NULL == m_pfBoundaryBgScores)
        {
            bRet = false;
            break;
        }
        m_bInit = true;
    }while(false);
    if (!bRet)
    {
        Release();
    }
    return bRet;
}
bool BGMapExtractor_Grid::BGMapExtract(const unsigned char *pbyRgbPixels, int iWidth, int iHeight, int iStride, float *pfBGMap)
{
    if (NULL == pbyRgbPixels || iWidth < 1 || iHeight < 1 || iStride < iWidth * 3 || NULL == pfBGMap)
    {
        return false;
    }

    if (!m_bInit)
    {
        return false;
    }

    bool bRet = true;
    do{
        BuildGridRgbImage(pbyRgbPixels, iWidth, iHeight, iStride);
        ConvertToGridLabImage();
        BuildNeighboringGraph();
        BoundaryAnalysis();
        InitBGScores();
        bRet = UpdateBGScores();
        if (!bRet)
        {
            break;
        }
        bRet = NormalizeBGScores();
        if (!bRet)
        {
            break;
        }
        OutputBGMap(iWidth, iHeight, pfBGMap);

    }while(false);
    return bRet;
}
void BGMapExtractor_Grid::BuildGridRgbImage(const unsigned char *pRgbPixels, int iWidth, int iHeight, int iStride)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;

    m_iPatchNumX = (int)((float)iWidth/(float)m_iPatchWidth+0.5f);
    m_iPatchNumY = (int)((float)iHeight/(float)m_iPatchHeight+0.5f);

    m_iPatchNum = m_iPatchNumX*m_iPatchNumY;

    memset(m_piGridRgbSum, 0, sizeof(int)*m_iPatchNum*3);

    unsigned char *pRowRgb = (unsigned char*)pRgbPixels;
    int *pRowGridSum = m_piGridRgbSum;
    for (int id_y = 0; id_y < m_iPatchNumY-1; id_y++)
    {
        for (int y0 = 0; y0 < m_iPatchHeight; y0++)
        {   
            int *pGridSumCrr = pRowGridSum;
            unsigned char *pRgbCurr = pRowRgb;
            for (int id_x = 0; id_x < m_iPatchNumX-1; id_x++)
            {
                for (int x0 = 0; x0 < m_iPatchWidth; x0++)
                {
                    pGridSumCrr[0] += pRgbCurr[0];
                    pGridSumCrr[1] += pRgbCurr[1];
                    pGridSumCrr[2] += pRgbCurr[2];
                    pRgbCurr+=3;
                }
                pGridSumCrr+=3;
            }
            for (int x = (m_iPatchNumX-1)*m_iPatchWidth; x < m_iWidth; x++)
            {
                pGridSumCrr[0] += pRgbCurr[0];
                pGridSumCrr[1] += pRgbCurr[1];
                pGridSumCrr[2] += pRgbCurr[2];
                pRgbCurr+=3;
            }
            pRowRgb += iStride;
        }
        pRowGridSum += m_iPatchNumX*3;
    }
    for (int y = (m_iPatchNumY-1)*m_iPatchHeight; y < m_iHeight; y++)
    {
        int *pGridSumCrr = pRowGridSum;
        unsigned char *pRgbCurr = pRowRgb;
        for (int id_x = 0; id_x < m_iPatchNumX-1; id_x++)
        {
            for (int x0 = 0; x0 < m_iPatchWidth; x0++)
            {
                pGridSumCrr[0] += pRgbCurr[0];
                pGridSumCrr[1] += pRgbCurr[1];
                pGridSumCrr[2] += pRgbCurr[2];
                pRgbCurr+=3;
            }
            pGridSumCrr+=3;
        }
        for (int x = (m_iPatchNumX-1)*m_iPatchWidth; x < m_iWidth; x++)
        {
            pGridSumCrr[0] += pRgbCurr[0];
            pGridSumCrr[1] += pRgbCurr[1];
            pGridSumCrr[2] += pRgbCurr[2];
            pRgbCurr+=3;
        }
        pRowRgb += iStride;
    }
    int iSize0 = m_iPatchWidth * m_iPatchHeight;
    int iSize1 = m_iPatchHeight * (iWidth - (m_iPatchNumX-1)*m_iPatchWidth);
    int iSize2 = m_iPatchWidth * (iHeight - (m_iPatchNumY-1)*m_iPatchHeight);
    int iSize3 = (iWidth - (m_iPatchNumX-1)*m_iPatchWidth)*(iHeight - (m_iPatchNumY-1)*m_iPatchHeight);

    pRowGridSum = m_piGridRgbSum;
    unsigned char *pRowGridRgb = m_pbyGridRgbPixels;
    for (int y=0; y < m_iPatchNumY-1; y++)
    {
        for (int x = 0; x < (m_iPatchNumX-1)*3; x++)
        {
            pRowGridRgb[x] = (unsigned char )(pRowGridSum[x] / iSize0);
        }
        for (int x = (m_iPatchNumX-1)*3; x < m_iPatchNumX*3; x++)
        {
            pRowGridRgb[x] = (unsigned char )(pRowGridSum[x] / iSize1);
        }
        pRowGridSum += m_iPatchNumX*3;
        pRowGridRgb += m_iPatchNumX*3;
    }
    for (int x = 0; x < (m_iPatchNumX-1)*3; x++)
    {
        pRowGridRgb[x] = (unsigned char )(pRowGridSum[x] / iSize2);
    }
    for (int x = (m_iPatchNumX-1)*3; x < m_iPatchNumX*3; x++)
    {
        pRowGridRgb[x] = (unsigned char )(pRowGridSum[x] / iSize3);
    }
    return;
}

void BGMapExtractor_Grid::ConvertToGridLabImage()
{
    memset(m_pfGridLabPixels, 0, sizeof(float)*m_iPatchNum*3);
    float *pRowGridLab = m_pfGridLabPixels;
    unsigned char *pRowGridRgb = m_pbyGridRgbPixels;
    float fL, fa, fb;
    for (int y = 0; y < m_iPatchNumY; y++)
    {
        for (int x = 0; x < m_iPatchNumX; x++)
        {
            RgbToLab(pRowGridRgb[x * 3 + 2], pRowGridRgb[x * 3 + 1], pRowGridRgb[x * 3], fL, fa, fb);
            pRowGridLab[x * 3] = fL;
            pRowGridLab[x * 3 + 1] = fa;
            pRowGridLab[x * 3 + 2] = fb;
        }
        pRowGridRgb += m_iPatchNumX*3;
        pRowGridLab += m_iPatchNumX*3;
    }
}

void BGMapExtractor_Grid::BuildNeighboringGraph()
{
    for (int i = 0; i < m_iPatchNum; i++)
    {
        int idx0 = i * NEIGHBORSIZE;
        for (int j = 0; j < NEIGHBORSIZE; j++)
        {
            m_piNeighborPatchIds[idx0 + j] = -1;
            m_pfNeighborPatchDists[idx0 + j] = (float)INFINITE;
        }
    }

    int id = 0;
    int iStride = m_iPatchNumX * 3;
    float *pfRowLab = m_pfGridLabPixels;
    for (int i = 0; i < m_iPatchNumY; i++)
    {
        float *ptrCur = pfRowLab;
        float *ptrNext = ptrCur+3;
        float *ptrNextRow = ptrCur+iStride;
        for (int j = 0; j < m_iPatchNumX; j++)
        {
            if (j + 1 < m_iPatchNumX)
            {
                int iNextId = id+1;
                float fDist = 0;

                fDist += (ptrCur[0] - ptrNext[0]) * (ptrCur[0] - ptrNext[0])
                    +(ptrCur[1] - ptrNext[1]) * (ptrCur[1] - ptrNext[1])
                    +(ptrCur[2] - ptrNext[2]) * (ptrCur[2] - ptrNext[2]);
                fDist = sqrtf(fDist);

                m_piNeighborPatchIds[id * NEIGHBORSIZE + 2] = iNextId;
                m_piNeighborPatchIds[iNextId * NEIGHBORSIZE + 1] = id;
                m_pfNeighborPatchDists[id * NEIGHBORSIZE + 2] = fDist;
                m_pfNeighborPatchDists[iNextId * NEIGHBORSIZE + 1] = fDist;
            }
            if (i + 1 < m_iPatchNumY)
            {
                int iNextId = id+m_iPatchNumX;
                float fDist = 0;

                fDist += (ptrCur[0] - ptrNextRow[0]) * (ptrCur[0] - ptrNextRow[0])
                          +(ptrCur[1] - ptrNextRow[1]) * (ptrCur[1] - ptrNextRow[1])
                          +(ptrCur[2] - ptrNextRow[2]) * (ptrCur[2] - ptrNextRow[2]);
                fDist = sqrtf(fDist);
                m_piNeighborPatchIds[id * NEIGHBORSIZE + 3] = iNextId;
                m_piNeighborPatchIds[iNextId * NEIGHBORSIZE] = id;
                m_pfNeighborPatchDists[id * NEIGHBORSIZE + 3] = fDist;
                m_pfNeighborPatchDists[iNextId * NEIGHBORSIZE] = fDist;
            }
            id++;
            ptrCur+=3;
            ptrNext+=3;
            ptrNextRow+=3;

        }//end for j
        pfRowLab += iStride;
    }//end for i

    //calculate neighbor distance threshold
    float fDisSum = 0.f;
    int count = 0;
    float *pfDists = m_pfNeighborPatchDists;
    for (int i = 0; i < m_iPatchNum; i++)
    {
        float fDisMin = (float)INFINITE;
        for (int j = 0; j < NEIGHBORSIZE; j++)
        {
            fDisMin = std::min(fDisMin, pfDists[j]);
        }
        if (fDisMin < (float)INFINITE)
        {
            fDisSum+=fDisMin;
            count++;
        }
        pfDists += NEIGHBORSIZE;
    }
    m_fNeighborDistThre = fDisSum/count;

    pfDists = m_pfNeighborPatchDists;
    for (int i = 0; i < m_iPatchNum; i++)
    {
        for (int j = 0; j < NEIGHBORSIZE; j++)
        {
            if (pfDists[j] < (float)INFINITE)
            {
                pfDists[j] = std::max(0.f, pfDists[j]-m_fNeighborDistThre);
            }
        }
        pfDists += NEIGHBORSIZE;
    }
    return;
}

void BGMapExtractor_Grid::BoundaryAnalysis()
{
    int *pIdx = m_piBoundaryIds;
    for (int i = 0; i < m_iPatchNumX; i++)
    {
        *pIdx = i;
        pIdx++;
    }
    for (int j = 1; j < m_iPatchNumY; j++)
    {
        *pIdx = j * m_iPatchNumX-1;
        pIdx++;
    }
    for (int i = m_iPatchNumX-2; i>=0; i--)
    {
        *pIdx = (m_iPatchNumY-1)*m_iPatchNumX + i;
        pIdx++;
    }
    for (int j = m_iPatchNumY-2; j >= 1; j--)
    {
        *pIdx = j * m_iPatchNumX;
        pIdx++;
    }

    int iNeighborSize = 10;
    float c = 3.f;
    float fNorm = 1.f/std::min(m_iPatchNumX, m_iPatchNumY);
    m_iBoundaryPatchNum = (m_iPatchNumX + m_iPatchNumY)*2-4;
    std::vector<float> vCombinedDis(m_iBoundaryPatchNum, 0.f);

    float fDisMin = (float)INFINITE;
    float fDisMax = 0.f;
    for (int i = 0; i < m_iBoundaryPatchNum; i++)
    {
        int id1 = m_piBoundaryIds[i];
        float *pfId1 = m_pfGridLabPixels + id1 * 3;
        for (int j = 0; j < m_iBoundaryPatchNum; j++)
        {
            if (j == i)
            {
                vCombinedDis[j] = (float)INFINITE;
                continue;
            }
            int id2 = m_piBoundaryIds[j];
            float *pfId2 = m_pfGridLabPixels + id2 * 3;
            float fColorDis = (pfId1[0] - pfId2[0]) * (pfId1[0] - pfId2[0])
                            +(pfId1[1] - pfId2[1]) * (pfId1[1] - pfId2[1])
                            +(pfId1[2] - pfId2[2]) * (pfId1[2] - pfId2[2]);
            fColorDis = sqrtf(fColorDis);
            int iPosDis = abs(i-j);
            iPosDis = std::min(iPosDis, m_iBoundaryPatchNum-iPosDis);
            vCombinedDis[j] = fColorDis/(1.f + c * iPosDis * fNorm);
        }
        std::partial_sort(vCombinedDis.begin(), vCombinedDis.begin()+iNeighborSize, vCombinedDis.end());
        float fScore = 0.f;
        for (int j = 0; j < iNeighborSize; j++)
        {
            fScore += vCombinedDis[j];
        }
        fScore/=iNeighborSize;
        fDisMax = std::max(fDisMax, fScore);
        fDisMin = std::min(fDisMin, fScore);
        m_pfBoundaryBgScores[i] = fScore;
    }

    if (fDisMax - fDisMin > 1)
    {
        float fScale = 1.f/(fDisMax-fDisMin);
        for (int i = 0; i < m_iBoundaryPatchNum; i++)
        {
            float fProb = (m_pfBoundaryBgScores[i] - fDisMin)*fScale;
            if (fProb >= 0.8f)
            {
                m_pfBoundaryBgScores[i] = (float)INFINITE;
                continue;
            }
            if (fProb < 0.5)
            {
                m_pfBoundaryBgScores[i] = 0.f;
            }
        }
    }
    else
    {
        memset(m_pfBoundaryBgScores, 0, sizeof(float)*m_iBoundaryPatchNum);
    }
}

void BGMapExtractor_Grid::InitBGScores()
{
    for (int i = 0; i < m_iPatchNum; i++)
    {
        m_pfBGScores[i] = (float)INFINITE;
    }
    for (int i = 0; i < m_iBoundaryPatchNum; i++)
    {
        int id = m_piBoundaryIds[i];
        m_pfBGScores[id] = m_pfBoundaryBgScores[i];
    }
}
bool BGMapExtractor_Grid::UpdateBGScores()
{
    bool *arrbInQueque = NULL;
    arrbInQueque = new bool[m_iPatchNum];
    if (NULL == arrbInQueque)
    {
        return false;
    }

    std:: queue<int> qIds;
    for (int i = 0; i < m_iPatchNum; i++)
    {
        m_piPreviousIds[i] = -1;
        arrbInQueque[i] = false;
        if (m_pfBGScores[i] < (float)INFINITE)
        {
            qIds.push(i);
            arrbInQueque[i] = true;
        }
    }

    while(!qIds.empty())
    {
        int id = qIds.front();
        qIds.pop();
        arrbInQueque[id] = false;
        for (int i = 0; i < NEIGHBORSIZE; i++)
        {
            int iNeighborId = m_piNeighborPatchIds[id * NEIGHBORSIZE + i];
            if (iNeighborId >= 0)
            {
                float fAltDis = m_pfBGScores[id] + m_pfNeighborPatchDists[id * NEIGHBORSIZE + i];
                if (m_pfBGScores[iNeighborId] > fAltDis)
                {
                    m_pfBGScores[iNeighborId] = fAltDis;
                    m_piPreviousIds[iNeighborId] = id;
                    if (!arrbInQueque[iNeighborId])
                    {
                        qIds.push(iNeighborId);
                        arrbInQueque[iNeighborId] = true;
                    }
                }
            }
        }
    }
    delete[] arrbInQueque;
    return true;
}
void BGMapExtractor_Grid::OutputBGMap(int iWidth, int iHeight, float* pfBGmap)
{
    memset(pfBGmap, 0, sizeof(float) * iWidth * iHeight);

    float *pfRowBGMap = pfBGmap;
    float *pfRowScore = m_pfBGScores;
    for (int id_y = 0; id_y < (m_iPatchNumY-1); id_y++)
    {
        for (int y0 = 0; y0 < m_iPatchHeight; y0++)
        {
            float *pfMapCurr = pfRowBGMap;
            float *pfScoreCurr = pfRowScore;
            for (int id_x = 0; id_x < (m_iPatchNumX-1); id_x++)
            {
                for (int x0 = 0; x0 < m_iPatchWidth; x0++)
                {
                    *pfMapCurr = *pfScoreCurr;
                    pfMapCurr++;
                }
                pfScoreCurr++;
            }
            for (int x = (m_iPatchNumX-1)*m_iPatchWidth; x < m_iWidth; x++)
            {
                *pfMapCurr = *pfScoreCurr;
                pfMapCurr++;
            }
            pfRowBGMap += m_iWidth;
        }
        pfRowScore+=m_iPatchNumX;
    }
    for (int y = (m_iPatchNumY-1)*m_iPatchHeight; y < m_iHeight; y++)
    {
        float *pfMapCurr = pfRowBGMap;
        float *pfScoreCurr = pfRowScore;
        for (int id_x = 0; id_x < (m_iPatchNumX-1); id_x++)
        {
            for (int x0 = 0; x0 < m_iPatchWidth; x0++)
            {
                *pfMapCurr = *pfScoreCurr;
                pfMapCurr++;
            }
            pfScoreCurr++;
        }
        for (int x = (m_iPatchNumX-1)*m_iPatchWidth; x < m_iWidth; x++)
        {
            *pfMapCurr = *pfScoreCurr;
            pfMapCurr++;
        }
        pfRowBGMap += m_iWidth;
    }

    return;
}

bool BGMapExtractor_Grid::NormalizeBGScores()
{
    float *pfBGScoresTemp = NULL;
    pfBGScoresTemp = new float[m_iPatchNum];
    if (NULL == pfBGScoresTemp)
    {
        return false;
    }
    memcpy(pfBGScoresTemp, m_pfBGScores, m_iPatchNum * sizeof(float));

    int iNum = (int)(m_iPatchNum * 0.02f);
    std::partial_sort(pfBGScoresTemp, pfBGScoresTemp+iNum, pfBGScoresTemp+m_iPatchNum, std::greater<float>());
    float fDistThre = pfBGScoresTemp[iNum-1];
    
    for (int i = 0; i < m_iPatchNum; i++)
    {
        m_pfBGScores[i] = std::min(fDistThre, m_pfBGScores[i])/fDistThre;
    }
    delete[] pfBGScoresTemp;
    return true;
}
