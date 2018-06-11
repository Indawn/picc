#include "mainwindow.h"
#include "ui_mainwindow.h"

/**  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ Pylon ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓   **/

#include <pylon/PylonIncludes.h>

#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <pylon/ImageFormatConverter.h>
//using namespace ImageFileFormat_Jpeg;
#include <pylon/usb/BaslerUsbInstantCamera.h>
typedef Pylon::CBaslerUsbInstantCamera Camera_t;

using namespace Basler_UsbCameraParams;
using namespace Pylon;  // Namespace for using pylon objects.

/**
#if defined ( USE_USB )
// Settings for using Basler USB cameras.
#include <pylon/usb/BaslerUsbInstantCamera.h>
typedef Pylon::CBaslerUsbInstantCamera Camera_t;
using namespace Basler_UsbCameraParams;
#else
#error Camera type is not specified. Define USE_USB for using Usb cameras.
#endif
**/

/**  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ QT ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓   **/

#include <QFileDialog>
#include <QLabel>
#include <QCamera>
#include <QCameraInfo>
#include <QMessageBox>

/**  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ openCV ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓   **/

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/face.hpp>
using namespace cv;
using namespace cv::face;

#include <iostream>
#include <conio.h>
using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cameraStatus = false;
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(getframe()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent ( QResizeEvent * event )
{
   scaledmat2label(mat_opened, ui->label_pic0);

}
void MainWindow::on_actionOpen_triggered()
{
   QFileDialog *fileDialog = new QFileDialog(this);//创建一个QFileDialog对象，构造函数中的参数可以有所添加。
    fileDialog->setWindowTitle(tr("Open"));//设置文件保存对话框的标题
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);//设置文件对话框为保存模式
    fileDialog->setFileMode(QFileDialog::AnyFile);//设置文件对话框弹出的时候显示任何文件，不论是文件夹还是文件
    fileDialog->setViewMode(QFileDialog::Detail);//文件以详细的形式显示，显示文件名，大小，创建日期等信息；
    fileDialog->setGeometry(10,30,300,200);//设置文件对话框的显示位置
    fileDialog->setDirectory(".");//设置文件对话框打开时初始打开的位置
    QStringList filters;
    filters << "Image files (*.png *.jpg *.bmp)"
            << "Any files (*)";
    fileDialog->setNameFilters(filters);//设置文件类型过滤器
    if(fileDialog->exec() == QDialog::Accepted)//注意使用的是QFileDialog::Accepted或者QDialog::Accepted,不是QFileDialog::Accept
    {
        QString  curr_picname = fileDialog->selectedFiles()[0];//得到用户选择的文件名
        mat_opened = imread(curr_picname.toLocal8Bit().constData());
        mat_size = mat_opened;
        if(mat_opened.data)
        {
            scaledmat2label(mat_opened, ui->label_pic0);
            ui->label_pic0->setCursor(Qt::CrossCursor);     //改变鼠标指针
        }
    }

}

void MainWindow::on_actionClose_triggered()
{
    mat_opened.release();
    ui->label_pic0->setText("Open picture");
    ui->label_pic0->setCursor(Qt::ArrowCursor);

}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionGrey_triggered()
{
    if(mat_opened.channels()==1){
        QMessageBox msg;
        msg.setText(u8"已是灰度图像");
        msg.exec();
    }
    else{
        cvtColor(mat_opened, mat_opened, COLOR_BGR2GRAY);
        scaledmat2label(mat_opened, ui->label_pic0);
    }
}


void MainWindow::on_actionOpenCamera_triggered()
{
   if (capture.isOpened())
       capture.release();     //decide if capture is already opened; if so,close it
   if(capture.open(0))
   {
       capture.set(CV_CAP_PROP_FRAME_WIDTH, 2592);
       capture.set(CV_CAP_PROP_FRAME_HEIGHT, 2048);
       timer->start(42);//按照每秒24帧计算，每过42ms执行一次getframe
   }
   else
   {
       QMessageBox msg;
       msg.setText(u8"摄像头未打开");
       msg.exec();
   }

}

/*
    // The exit code of the sample application.
     int exitCode = 0;

     // Before using any pylon methods, the pylon runtime must be initialized.
     PylonInitialize();

     try
     {
         // Define constants.
         static const uint32_t cNumTilesX = 3;
         static const uint32_t cNumTilesY = 2;
         static const uint32_t cWindowBorderSizeX = 25;
         static const uint32_t cWindowBorderSizeY = 125;
         static const uint32_t cScreenStartX = 40;
         static const uint32_t cScreenStartY = 40;
         static const uint32_t cMaxIndex = 31;
         static const size_t   cNumWindows = cNumTilesY * cNumTilesX ;
         static const uint32_t cMaxWidth = 640;
         static const uint32_t cMaxHeight = 480;

         // Create an array of image windows.
         CPylonImageWindow imageWindows[ cNumWindows ];

         // Create an Instant Camera object.
         CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());

         // Print the model name of the camera.
         cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

         // Start the grab. Only display the latest image.
         camera.StartGrabbing( c_countOfImagesToGrab, GrabStrategy_LatestImageOnly);

         // This smart pointer will receive the grab result data.
         CGrabResultPtr ptrGrabResult;

         // Grab images and show the tiles of each image in separate image windows.
         while ( camera.IsGrabbing())
         {
             // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
             camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

             // If the image was grabbed successfully.
             if ( ptrGrabResult->GrabSucceeded())
             {
                 // This image object is used for splitting the grabbed image into tiles.
                 CPylonImage image;

                 // Attach the grab result to a pylon image.
                 image.AttachGrabResultBuffer( ptrGrabResult);

                 // Compute tile sizes.
                 uint32_t imageTileWidth = min( image.GetWidth(), cMaxWidth) / cNumTilesX;
                 uint32_t imageTileHeight = min( image.GetHeight(), cMaxHeight) / cNumTilesY;
                 imageTileWidth -= imageTileWidth % GetPixelIncrementX( image.GetPixelType());
                 imageTileHeight -= imageTileWidth % GetPixelIncrementY( image.GetPixelType());

                 uint32_t windowTileWidth = imageTileWidth + cWindowBorderSizeX;
                 uint32_t windowTileHeight = imageTileHeight + cWindowBorderSizeY;

                 // Create and display the tiles of the grabbed image.
                 for ( uint32_t indexTileX = 0; indexTileX < cNumTilesX; ++indexTileX)
                 {
                     for ( uint32_t indexTileY = 0; indexTileY < cNumTilesY; ++indexTileY)
                     {
                         size_t arrayIndex = indexTileY * cNumTilesX + indexTileX;
                         bool windowCreated = false;

                         if ( !imageWindows[ arrayIndex ].IsValid())
                         {
                             // Create the image window and position the image window as a tile on the screen.
                             // The Image Window stores the last size and position.
                             // The last Image Window indices are used here to avoid changing
                             // the settings of the windows used for other samples.
                             size_t windowIndex = cMaxIndex - arrayIndex;
                             imageWindows[ arrayIndex ].Create( windowIndex,
                                 cScreenStartX + indexTileX * windowTileWidth,
                                 cScreenStartY + indexTileY * windowTileHeight,
                                 windowTileWidth,
                                 windowTileHeight
                                 );

                             windowCreated = true;
                         }

                         // Get the image area of interest (Image AOI) that includes the tile. This is a zero copy operation.
                         CPylonImage tile = image.GetAoi( indexTileX * imageTileWidth, indexTileY * imageTileHeight, imageTileWidth, imageTileHeight);

                         // Set the tile image.
                         imageWindows[ arrayIndex ].SetImage( tile);

                         // Show the image.
                         imageWindows[ arrayIndex ].Show();

                         if ( windowCreated)
                         {
                             // Wait a little to show how the windows appear on the screen.
                             ::Sleep( 200);
                         }
                     }
                 }
             }
             else
             {
                 throw RUNTIME_EXCEPTION( "Error image grab failed: %hs", ptrGrabResult->GetErrorDescription().c_str());
             }
         }

         // Destroy the windows.
         for ( size_t arrayIndex = 0; arrayIndex < cNumWindows; ++arrayIndex)
         {
             // Close() closes and destroys the window.
             imageWindows[ arrayIndex ].Close();

             // Wait a little to show how the windows are removed from the screen.
             ::Sleep( 200);
         }
     }
     catch (const GenericException &e)
     {
         // Error handling.
         cerr << "An exception occurred." << endl
         << e.GetDescription() << endl;
         exitCode = 1;

         cerr << endl << "Press Enter to exit." << endl;
         while( cin.get() != '\n');
     }

     // Releases all pylon resources.
     PylonTerminate();

}

*/

void MainWindow::on_actionCloseCamera_triggered()
{
     cameraStatus = false;
     capture.release();
     ui->label_pic0->clear();
}


void MainWindow::getframe()
{
    capture>>mat_opened; //从视频取帧
    // imshow("mat_opened", mat_opened);
    if(!mat_opened.data)
        timer->stop();//如果取不到数据，终止计时器
    else
    {
 /*       Mat src = mat_opened;
        Mat frame = src.clone();
        Mat facesRIO;
        //图像缩放，采用双线性插值。
        cv::resize(src,src,Size(512,128),0,0,cv::INTER_LINEAR);
        //图像灰度化。
        cv::cvtColor(src,src,COLOR_BGR2GRAY);
        //直方图均衡化，图像增强，暗的变亮，亮的变暗。
        cv::equalizeHist(src,src);
        //
        String face_cascade_name = "C:\\opencv341p\\etc\\haarcascades\\haarcascade_frontalface_alt.xml";
        //  String eyes_cascade_name = "C:\\opencv341p\\etc\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
        CascadeClassifier face_cascade,eyes_cascade;
        if(!face_cascade.load(face_cascade_name))
        {
            //加载脸部分类器失败！
            QMessageBox msg;
            msg.setText(u8"加载脸部分类器失败！");
            msg.exec();

        }

        //存储找到的脸部矩形。
        std::vector<Rect> faces;
        face_cascade.detectMultiScale(src,faces,1.1,2,0|CASCADE_SCALE_IMAGE,Size(30,30));
        for(size_t i=0;i<faces.size();++i)
        {
            //绘制矩形 BGR。
            rectangle(frame,faces[i],Scalar(0,0,255),1);

            mat_opened = frame;
 */           scaledmat2label(mat_opened, ui->label_pic0);
      //  }
    }
}

void MainWindow::on_actionOpen_Video_triggered()
{

     QFileDialog *fileDialog = new QFileDialog(this);//创建一个QFileDialog对象，构造函数中的参数可以有所添加。
     fileDialog->setWindowTitle(tr("Open"));//设置文件保存对话框的标题
     fileDialog->setAcceptMode(QFileDialog::AcceptOpen);//设置文件对话框为保存模式
     fileDialog->setFileMode(QFileDialog::AnyFile);//设置文件对话框弹出的时候显示任何文件，不论是文件夹还是文件
     fileDialog->setViewMode(QFileDialog::Detail);//文件以详细的形式显示，显示文件名，大小，创建日期等信息；
     fileDialog->setGeometry(10,30,300,200);//设置文件对话框的显示位置
     fileDialog->setDirectory(".");//设置文件对话框打开时初始打开的位置
     QStringList filters;
     filters << "Image files (*.mkv *.mp4 *.avi)"
             << "Any files (*)";
     fileDialog->setNameFilters(filters);//设置文件类型过滤器
     if(fileDialog->exec() == QDialog::Accepted)//注意使用的是QFileDialog::Accepted或者QDialog::Accepted,不是QFileDialog::Accept
     {
         QString  curr_videoname = fileDialog->selectedFiles()[0];//得到用户选择的文件名
         if (capture.isOpened())
         {
             capture.release();     //decide if capture is already opened; if so,close it

         }
         capture.open(curr_videoname.toLocal8Bit().constData());           //open the default camera
       // timer = new QTimer(this);
        timer->start(42);//按照每秒24帧计算，每过42ms执行一次getframe
     }

  //  capture.open("Z:\\Big.Bugs.2012.720p.BluRay.x264.DTS-WiKi\\Big.Bugs.2012.720p.BluRay.x264.DTS-WiKi.mkv");           //open the default camera
    //timer = new QTimer(this);
 //   timer->start(30);//按照每秒24帧计算，每过42ms执行一次getframe

}

int MainWindow::on_actioncameraTest_triggered()
{
    try
    {
        VideoCapture cap(0);//打开默认的摄像头
        throw 1;
    }
    catch(int i)
    {
        QMessageBox msg;
        msg.setText(u8"error");
        msg.exec();
    }
/*    if(!cap.isOpened())
    {
        return -1;
    }
    Mat frame;
    bool stop = false;
    while(!stop)
    {
        cap.read(frame); //  或cap>>frame;
        if(!frame.empty())
            imshow("Video",frame);
        else
        {
            QMessageBox msg;
            msg.setText(u8"帧为空");
            msg.exec();
        }
        if(waitKey(30)==27) //Esc键退出
        {
            stop = true;
        }
    }
    */
    return 0;

}

void MainWindow::on_actioncamerainfo_triggered()
{
     qDebug() << "#################################################################################################";
    QList<QCameraInfo> list;
    list = QCameraInfo::availableCameras();
    for (int i = 0; i < list.size(); i++)
    {
        qDebug() << list.at(i).deviceName();
        qDebug() << list.at(i).description();
        qDebug() << list.at(i).position();
        qDebug() << "###############";
    }
}

void MainWindow::on_actionOpen_pylon_triggered()
{
    PylonInitialize();
    // Only look for cameras supported by Camera_t.
    CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());
           cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
           camera.MaxNumBuffer =  5;
           camera.StartGrabbing();
           CGrabResultPtr ptrGrabResult;
    camera.RetrieveResult(5000 , ptrGrabResult, TimeoutHandling_ThrowException);
    if(ptrGrabResult->GrabSucceeded())
    {
          CPylonImage target;
        CImageFormatConverter converter;
         converter.OutputPixelFormat=PixelType_RGB8packed;
         converter.OutputBitAlignment=OutputBitAlignment_MsbAligned;
         converter.Convert(target,ptrGrabResult);
        mat_opened = cv::Mat(target.GetHeight(),target.GetWidth(),CV_8UC3,target.GetBuffer(),Mat::AUTO_STEP);
         ptrGrabResult.Release();

     //   mat_opened = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *)ptrGrabResult->GetBuffer());
        scaledmat2label(mat_opened, ui->label_pic0);
    }
    PylonTerminate();

/*      if (capture.isOpened())
        capture.release();     //decide if capture is already opened; if so,close it
    if(capture.open(0))
    {

        timer->start(42);//按照每秒24帧计算，每过42ms执行一次getframe
    }
    else
    {
        QMessageBox msg;
        msg.setText(u8"摄像头未打开");
        msg.exec();
    }  CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
    Pylon::CBaslerUsbInstantCamera Camera_t;
            camera.RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
   camera.Width.SetValue(1296);
    try
    {
        CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
     //   camera.Open();

           camera.MaxNumBuffer = 5;
        int i =1;
  //      while(camera.IsOpen())
   //     {
            cout<<"camera opened  ";
        //    camera.StopGrabbing();
            camera.StartGrabbing(1);
        CGrabResultPtr ptrGrabResult;
            while(camera.IsGrabbing())
            {
                camera.RetrieveResult(30 , ptrGrabResult, TimeoutHandling_ThrowException);
                if(ptrGrabResult->GrabSucceeded())
                {
                  //  Pylon::DisplayImage(1, ptrGrabResult);
                 //   cout<<"#"<<i<<" ";i++;
                    uchar* din = (uchar *)(ptrGrabResult->GetBuffer());
                    Mat Image(ptrGrabResult->GetHeight() ,ptrGrabResult->GetWidth() ,CV_8UC1,din);
                    mat_opened = Image;
                    scaledmat2label(mat_opened, ui->label_pic0);


                }
                else
                {
                    cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
                }
            }
            waitKey(50);
        }


 //   }
    catch (const GenericException &e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl
             << e.GetDescription() << endl;
        //exitCode = 1;
    }

    PylonTerminate();
    */
}

void MainWindow::on_actionClose_pylon_triggered()
{
    PylonTerminate();
}
