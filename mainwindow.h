#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>
//#include <opencv2/highgui/highgui.hpp>

#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    cv::Mat mat_size;
    cv::Mat mat_opened;
    bool cameraStatus;
    cv::VideoCapture capture;
    QTimer *timer;

    QImage cvMat2QImage(const cv::Mat& mat);
    void scaledmat2label(cv::Mat mat, QLabel* label);

private slots:
    void on_actionOpen_triggered();

    void resizeEvent ( QResizeEvent * event );

    void on_actionClose_triggered();

    void on_actionExit_triggered();

    void on_actionGrey_triggered();

    void on_actionOpenCamera_triggered();

    void on_actionCloseCamera_triggered();

    void getframe();


    void on_actionOpen_Video_triggered();

    int on_actioncameraTest_triggered();

    void on_actioncamerainfo_triggered();

    void on_actionOpen_pylon_triggered();

    void on_actionClose_pylon_triggered();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
