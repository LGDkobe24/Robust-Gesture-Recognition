//
// Created by 庾金科 on 6/30/16.
//

#ifndef HANDGESTURE_UTIL_H
#define HANDGESTURE_UTIL_H

#endif //HANDGESTURE_UTIL_H

#include "opencv/cv.h"

IplImage* gr_ui_resize(IplImage* src,int W,int H){
    CvSize dst_sz;
    dst_sz.height = H;
    dst_sz.width = W;
    IplImage * dst_img = cvCreateImage(dst_sz, src->depth, src->nChannels);
    cvResize(src,dst_img,CV_INTER_LINEAR);
    return dst_img;
}



void gr_ui_SkinthresholdOtsu(IplImage* src, IplImage* dst)
{
    int height=src->height;
    int width=src->width;

    //histogram
    float histogram[256]={0};
    for(int i=0;i<height;i++) {
        unsigned char* p=(unsigned char*)src->imageData+src->widthStep*i;
        for(int j=0;j<width;j++) {
            histogram[*p++]++;
        }
    }
    //normalize histogram
    int size=height*width;
    for(int i=0;i<256;i++) {
        histogram[i]=histogram[i]/size;
    }

    //average pixel value
    float avgValue=0;
    for(int i=0;i<256;i++) {
        avgValue+=i*histogram[i];
    }

    int threshold;
    float maxVariance=0;
    float w=0,u=0;
    for(int i=0;i<256;i++) {
        w+=histogram[i];
        u+=i*histogram[i];

        float t=avgValue*w-u;
        float variance=t*t/(w*(1-w));
        if(variance>maxVariance) {
            maxVariance=variance;
            threshold=i;
        }
    }

    cvThreshold(src,dst,threshold,255,CV_THRESH_BINARY);
}