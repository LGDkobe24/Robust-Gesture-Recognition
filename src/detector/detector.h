//
// Created by 庾金科 on 6/28/16.
//

#ifndef HANDGESTURE_DETECTOR_H
#define HANDGESTURE_DETECTOR_H

#endif //HANDGESTURE_DETECTOR_H
#include <opencv/cv.h>
#include <math.h>

#include "stdio.h"
#define gPoint(A,i) *(CvConvexityDefect*)cvGetSeqElem(A,i).start

CvHaarClassifierCascade* gr_dc_loadDector(char* filename){
    CvHaarClassifierCascade* classifier = (CvHaarClassifierCascade*)cvLoad(filename,NULL,NULL,NULL);

    return classifier;

}

IplImage* gr_hc_getFingerNum(IplImage* src,CvPoint center,float radius){


    int height=src->height;
    int width=src->width;

    float histogram[256]={0};
    int flag= 0 ;
    int num = 0;
    uchar* data=(uchar *)src->imageData;
    int step = src->widthStep/sizeof(uchar);
    int channels = src->nChannels;
    int length = 0;

    for(int i= 0 ; i< 360;i++)
    {
        int r =(int)(center.y + radius*sin(i*3.1415/180));
        int c = (int)(center.x + radius*cos(i*3.1415/180));
    //    printf("%d \n",data[r*step+c*channels]);

        if(data[r*step+c*channels]==255 ){
            length++;
           if (flag==0){
            flag=1;
               }

        }
        else{
            if(flag==1)
            {
                if(length<18)
                num++;

            }

            flag = 0 ;
            length= 0 ;}

    }

  //  exit(0);


    return num;


}


CvPoint gr_hc_getContourCenter(CvSeq *defect){
    CvPoint p;
    float x,y;

    for(int i = 0 ; i < defect->total ; i++)
    {
        CvPoint *p = (CvPoint *)cvGetSeqElem(defect,i);
        x += p->x;
        y += p->y;

    }
    x/=defect->total;
    y/=defect->total;
    p.x = x;
    p.y = y*1.05;
    return p ;
}

double gr_hc_dis(CvPoint a,CvPoint b)
{
    double dis = sqrt( (a.x - b.x)*(a.x - b.x)  +(a.y - b.y)*(a.y - b.y));
    return dis;

}
struct Points_seq{
    CvPoint p;
    CvConvexityDefect df;
};

float  gr_hc_meanDis(CvSeq *contour)
{
    CvPoint center = gr_hc_getContourCenter(contour);
    float pn =  0 ;

    for (int i = 0; i < contour->total; i++) {
        CvPoint *p = (CvPoint *) cvGetSeqElem(contour, i);
      pn+= gr_hc_dis(*p, center);
    }
    return pn/contour->total - 5;


}


int gr_hc_getPeakNum(IplImage *dst,CvSeq *contour) {


    CvPoint center = gr_hc_getContourCenter(contour);

    //double *dis_s = (double *) malloc(sizeof(double) * (defect->total - 1));
    float current = 0, depth = 50000, mosthigher = 0;
    int q = 0, m = 0, k = 0, j = 0, notice = 0, spos = 0;
    CvPoint fingerTips_single;
    int Count = 0 ;

    for (int i = 1; i < contour->total; i++) {

        CvPoint *p = (CvPoint *)cvGetSeqElem(contour,i);

        float pn = gr_hc_dis(*p, center);
        if (pn >= current) {
            current = pn;
        }
        else {
            m++;
            if (m == 1) {
                notice = i;
                fingerTips_single = *p;
                depth = 500000;
            }
            if (depth >= pn) {
                depth = pn;
                k++;
            }
            else {
                if (k > 5) {
                    // spos=notice;
                    if (notice < 10) {
                        notice = 10;
                    }
                    for (int k = notice - 10; k < notice + 10; k++) {
                        CvPoint *couPoint = (CvPoint *)cvGetSeqElem(contour,k);
                        current = gr_hc_dis(*couPoint, center);;
                        if (current > mosthigher) {
                            mosthigher = current;
                            spos = k;
                        }
                    }
                    mosthigher = 0;
                    CvPoint *fingerTips_single = (CvPoint *) cvGetSeqElem(contour, spos);
                    if(abs(fingerTips_single->y-dst->height)>50 ) {
                        cvCircle(dst, *fingerTips_single, 3, cvScalar(0, 0, 255, 0), 2, CV_AA, 0);
                        Count++;
                    }
                }
                current = 0;

                m = 0;
                k = 0;
                k = 0;


            }

        }

    }



    return Count;

    //   free(dis_s);

}


int gr_ch_getNum_Hand(CvPoint center, IplImage* src)
{
    int height=src->height;
    int width=src->width;
    CvPoint new_offset;
    new_offset.x = center.x;
    new_offset.y = center.y + 20;


    //histogram
    float histogram[256]={0};
    for(int i=0;i<height;i++) {
        unsigned char* p=(unsigned char*)src->imageData+src->widthStep*i;
        for(int j=0;j<width;j++) {
            histogram[*p++]++;
        }
    }
}

double  gr_ch_angle(CvPoint start,CvPoint depth,CvPoint end)
{
    double  r =  gr_hc_dis(start,depth);
    double  l =  gr_hc_dis(end,depth);
    double m = gr_hc_dis(start,end);

    double angle = (r*r + l*l - m*m )/(2*r*l);
    return angle*180/3.1415 ;
}

#define DEBUG

int gr_hc_isHnad(CvSeq *contour ,IplImage *thes, IplImage *dst){

    CvSeq* hull = cvConvexHull2(contour,0,CV_CLOCKWISE,0);

    CvPoint pt0 = **(CvPoint**)cvGetSeqElem(hull,hull->total - 1);
    for(int i = 0;i<hull->total;++i){
        CvPoint pt1 = **(CvPoint**)cvGetSeqElem(hull,i);
        cvLine(dst,pt0,pt1,CV_RGB(0,0,255),0,CV_AA,0);
        pt0 = pt1;
    }
    CvSeq *defect = cvConvexityDefects(contour,hull,NULL);
    int fingersNum1 = gr_hc_getPeakNum(dst,contour);
    CvPoint center =  gr_hc_getContourCenter(contour);
    int meanDis = gr_hc_meanDis(contour)+10;
    int fingersNum2 = gr_hc_getFingerNum(thes,center,meanDis);
    int compose = (int)(fingersNum1*0 + fingersNum2*1);

    printf("M_Peak:%d M_circle:%d \n",fingersNum1,fingersNum2);
    char c[5];
    //sprintf(c,"%d",(fingersNum1 + fingersNum2 )/2);
    if(compose>5)compose=5;
#ifdef DEBUG
    sprintf(c,"%d",compose);
    CvFont font;
    cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX, 1.0f,1.0f,1.0f, 2, CV_AA);

    cvPutText(dst,c,center,&font,cvScalar(255,255,0,0));

    cvCircle(dst,center,meanDis, CV_RGB(255, 255, 255), 1, CV_AA, 0);
#endif

//        CvConvexityDefect df = *(CvConvexityDefect*)cvGetSeqElem(defect,i);
//        CvConvexityDefect df_prev = *(CvConvexityDefect*)cvGetSeqElem(defect,i-1);
//        CvConvexityDefect df_back = *(CvConvexityDefect*)cvGetSeqElem(defect,i+1);
//        double dis = gr_hc_dis(*df.start,*df_prev.start);
//        double angle =  gr_ch_angle(*df_prev.start,*df.depth_point,*df_back.end);
//
//        if(dis >10 ) {
//           // cvCircle(dst, *df.start, 2, CV_RGB(255, 255, 0), -1, CV_AA, 0);
//          //  printf("%f \n",angle);
//
//        }
//        if (i == 1)
//         //   cvCircle(dst, *df_prev.start, 2, CV_RGB(255, 255, 0), -1, CV_AA, 0);
//      //  cvCircle(dst,*df.end,2,CV_RGB(255,255,0),-1,CV_AA,0);
//        cvCircle(dst,*df.depth_point,2,CV_RGB(0,255,255),-1,CV_AA,0);
//    }
    return compose;


}


int IsRect(CvRect *rect,CvPoint2D32f xy)
{
    return (rect!=NULL && rect->x<xy.x && xy.x<rect->x+rect->width && rect->y<xy.y && xy.y<rect->y+rect->height);
}

CvRect* gr_dc_detectface(IplImage* img,CvHaarClassifierCascade* classifierCascade)
{
    double scale=1;
    static CvScalar colors[] = {
            {{100,255,100}},{{0,128,255}},{{0,255,255}},{{0,255,0}},
            {{255,128,0}},{{255,255,0}},{{255,0,0}},{{255,0,255}}
    };//Just some pretty colors to draw with

    //Image Preparation
    //
    IplImage* gray = cvCreateImage(cvSize(img->width,img->height),8,1);
    cvCvtColor(img,gray, CV_BGR2GRAY);


    cvEqualizeHist(gray,gray); //直方图均衡
     CvMemStorage* storage = cvCreateMemStorage(0);
    //Detect objects if any
    //
    cvClearMemStorage(storage);
    double t = (double)cvGetTickCount();
    CvSeq* objects = cvHaarDetectObjects(gray,
                                         classifierCascade,
                                         storage,
                                         1.1,
                                         3,
                                         0/*CV_HAAR_DO_CANNY_PRUNING*/,cvSize(30,30),
                                         cvSize(200,200));
    t = (double)cvGetTickCount() - t;
    CvRect* r = NULL;
    for(int i=0;i<(objects? objects->total:0);++i)
    {
        CvRect* r=(CvRect*)cvGetSeqElem(objects,i);
        cvRectangle(img, cvPoint(r->x*scale,r->y*scale), cvPoint((r->x+r->width)*scale,(r->y+r->height)*scale), colors[i%8],1,CV_AA,0);
        cvReleaseImage(&gray);
        return r;
    }
    return r;
}

