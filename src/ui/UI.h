#ifndef HANDGESTURE_UI_H
#define HANDGESTURE_UI_H
#endif //HANDGESTURE_UI_H
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include "../detector/detector.h"
#include <stdio.h>
#include "../util/config.h"
#include "../util/util.h"
IplImage* gr_ui_getFrame(CvCapture* pCap){
    IplImage* pFrame =cvQueryFrame( pCap);
    return pFrame;
}

CvCapture* gr_ui_camLoad(){
    return cvCreateCameraCapture(-1);

}

IplImage* gr_ui_drawHist(IplImage* pframe,float val,int seq,int scale)
{
    int height=pframe->height;
    int width=pframe->width;
    CvPoint p1;
    if(width == seq)
        cvZero(pframe);
    p1.x = seq%width;
    p1.y = height;

    CvPoint p2;
    p2.x = seq%width;
    p2.y = height -  (int)(val*scale);

    cvLine(pframe,p1,p2,cvScalar(255,255,0,0),2,CV_AA,0);
    return pframe;


}



#define DEBUG
void gr_ui_loadSystem(char* systemName,char* classifierPath){
    CvCapture* pCapture  = gr_ui_camLoad();
    CvHaarClassifierCascade* hcc = gr_dc_loadDector(classifierPath);
    cvNamedWindow(systemName, 1);
    IplImage* pframe;
    IplImage* pframe_hsv=cvCreateImage(cvSize(IMAGE_SIZE_W,IMAGE_SIZE_H),8,3);
    IplImage* pframe_h=cvCreateImage(cvSize(IMAGE_SIZE_W,IMAGE_SIZE_H),8,1);
    IplImage* pframe_face=cvCreateImage(cvSize(IMAGE_SIZE_W,IMAGE_SIZE_H),8,3);
    IplImage* pframe_grey=cvCreateImage(cvSize(IMAGE_SIZE_W,IMAGE_SIZE_H),8,1);
    IplImage* pframePre;
    IplImage* pframeThes = cvCreateImage(cvSize(IMAGE_SIZE_W,IMAGE_SIZE_H),8,1);
    IplImage* pframe_prev =  cvCreateImage(cvSize(IMAGE_SIZE_W,IMAGE_SIZE_H),8,1);
    IplImage* pframe_thes_ = cvCreateImage(cvSize(IMAGE_SIZE_W,IMAGE_SIZE_H),8,1);
    IplImage* pframe_compose = cvCreateImage(cvSize(IMAGE_SIZE_W*2,200),8,3);
    CvRect* rect_face ;
    cvZero(pframe_compose);
    IplImage* hue= cvCreateImage(cvSize(IMAGE_SIZE_W,IMAGE_SIZE_H),8,1);

#ifdef DEBUG
    cvNamedWindow("pframeThes", 1);
#endif

    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *first_contour = NULL;
    int flag = 0 ;
    int seq = 0 ;
    while (1){

        pframePre  = gr_ui_getFrame(pCapture);
        pframe = gr_ui_resize(pframePre,IMAGE_SIZE_W,IMAGE_SIZE_H);
        rect_face = gr_dc_detectface(pframe,hcc);
        cvCvtColor(pframe,pframe_hsv,CV_RGB2YCrCb);
        cvCvtColor(pframe,pframe_grey,CV_RGB2GRAY);
        cvSplit(pframe_hsv, 0,0,pframe_h,0  );
        cvSmooth(pframe_h,pframe_h,CV_GAUSSIAN,5,0,0,0);
        gr_ui_SkinthresholdOtsu(pframe_h,pframeThes);
        cvDilate(pframeThes,pframeThes,0,1);
        cvErode(pframeThes,pframeThes,0,1);
        cvCopyImage(pframeThes, pframe_thes_);
#ifdef DEBUG
        cvShowImage("pframeThes",pframeThes);
#endif DEBUG

        cvFindContours(pframeThes, storage, &first_contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
        int cnt = 0;
        for(; first_contour != 0; first_contour = first_contour->h_next) {
            CvPoint2D32f p;
            float r;
            cvMinEnclosingCircle(first_contour,&p,&r);
            cnt++;
            double aera = cvContourArea(first_contour, CV_WHOLE_SEQ, 0);
            float rate = aera/(r*r);
            if (aera > 2000 && aera < 13000 && !IsRect(rect_face,p) && rate<1.4) {

                CvScalar color = CV_RGB(100,100,100);
                cvDrawContours(pframe, first_contour, color, color, 0, 2, CV_FILLED, cvPoint(0, 0));
                CvRect rect = cvBoundingRect( first_contour, 0);
                pframe_compose = gr_ui_drawHist(pframe_compose,rate-0.5,seq,100);
                //#################
                //##core function##
                //#################
                int fingersNum = gr_hc_isHnad(first_contour, pframe_thes_,pframe);
            }
        }
        cvShowImage("hist",pframe_compose);
        cvShowImage(systemName,pframe);
        //cvReleaseImage(&pframePre);
        char c=cvWaitKey(33);
        if(c==27)break;
        if(c==9){
            cvCopyImage(pframe_grey, pframe_prev);
        }
        seq++;

    }
    cvReleaseImage(&pframe);
    cvReleaseImage(&pframe_hsv);
    cvReleaseImage(&pframePre);
    cvReleaseImage(&pframeThes);
    cvReleaseImage(&pframe_thes_);
    cvReleaseImage(&pframe_prev);
    cvReleaseImage(&pframe_face);
    cvReleaseImage(&pframe_compose);
    cvReleaseImage(&pframe_compose);

}