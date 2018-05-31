#include "mainwindow.h"
#include "opencv2/opencv.hpp"
//#include "opencv2/
#include <iostream>
#include <string>
using namespace cv;
//using namespace cv::face;
using namespace std;

void FindFaces(cv::Mat &dst)
{/*
    Mat src = imread("src.jpg");
    Mat frame = src.clone();
    Mat facesRIO;
    //图像缩放，采用双线性插值。
    //cv::resize(src,src,Size(128,128),0,0,cv::INTER_LINEAR);
    //图像灰度化。
    cv::cvtColor(src,src,COLOR_BGR2GRAY);
    //直方图均衡化，图像增强，暗的变亮，亮的变暗。
    cv::equalizeHist(src,src);
    //
    String face_cascade_name = "haarcascade_frontalface_alt.xml";
    String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
    CascadeClassifier face_cascade,eyes_cascade;
    if(!face_cascade.load(face_cascade_name))
    {
        //加载脸部分类器失败！
        return;
    }
    if(!eyes_cascade.load(eyes_cascade_name))
    {
        //加载眼睛分类器失败！
        return;
    }
    //存储找到的脸部矩形。
    std::vector<Rect> faces;
    face_cascade.detectMultiScale(src,faces,1.1,2,0|CASCADE_SCALE_IMAGE,Size(30,30));
    for(size_t i=0;i<faces.size();++i)
    {
        //绘制矩形 BGR。
        rectangle(frame,faces[i],Scalar(0,0,255),1);
        //获取矩形中心点。
        //Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
        //绘制圆形。
        //ellipse(frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        //获取脸部矩形区域。
        //Mat faceROI = src(faces[i]);
        //存储找到的眼睛矩形。
        //std::vector<Rect> eyes;
        //eyes_cascade.detectMultiScale(faceROI,eyes,1.1,2,0 |CASCADE_SCALE_IMAGE,Size(30,30));
        //for(size_t j=0;j<eyes.size();++j)
        //{
        //Point eye_center(faces[i].x + eyes[j].x + eyes[j].width/2,faces[i].y + eyes[j].y + eyes[j].height/2);
        //int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
        //circle(frame,eye_center,radius,Scalar( 255, 0, 0 ),4,8,0);
        //}
        //截取人脸。
        facesROI = frame(faces[i]);
        //图像缩放。
        cv::resize(facesROI,facesROI,Size(128,128),0,0,cv::INTER_LINEAR);
        //保存图像。
        dst = facesROI;
        cv::imwrite("dst.jpg",facesROI);
    }
    return;
    */
}
