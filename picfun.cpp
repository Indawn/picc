#include "mainwindow.h"

#include <QLabel>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

QImage MainWindow::cvMat2QImage(const cv::Mat& mat)
{
 //   cvtColor(mat, mat, CV_BGR2RGB);
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}

void MainWindow::scaledmat2label(Mat mat, QLabel* label)
{
    if(mat.data)
    {
      //  Mat image;
        double pecentage=(double)label->width() / mat.cols;
        if((double)label->height() / mat.rows < pecentage)
            pecentage =(double)label->height() / mat.rows;
        cv::resize(mat, mat_size, cv::Size(0,0), pecentage, pecentage, INTER_AREA );
        label->clear();
        label->setPixmap(QPixmap::fromImage(cvMat2QImage(mat_size)));
        label->show();
    }


}




