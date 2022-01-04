#include "QuickDemo.h"

QuickDemo::QuickDemo()
{
    //ctor
}

QuickDemo::~QuickDemo()
{
    //dtor
}

void QuickDemo::ImageDemo(Mat &src,CascadeClassifier &face_classifier)
{
    if(src.empty())
        cout << "Could not load image..." << endl;
    imshow("input image",src);
    Mat gray;
    cvtColor(src,gray,COLOR_BGR2GRAY);
    equalizeHist(gray,gray);    //直方图均衡化，用于提高图像的质量

    //存放任意类型的动态数组
    vector<Rect> faces;
                                //(目标图片,坐标存放位置,缩放倍数,检测次数,默认0,最小尺寸,最大尺寸)
//    face_classifier.detectMultiScale(gray,faces,1.1,3,0,Size(24,24),Size(200,200));
    face_classifier.detectMultiScale(gray,faces);
    //size_t是一种无符号的整型数，它的取值没有负数
    for(size_t t=0;t<faces.size();t++)
    {
                //(图像,坐标,线条颜色,线条粗细程度,线条的类型,坐标点的小数点位数)
        rectangle(src,faces[static_cast<int>(t)],Scalar(255,0,0),2,8,0);
    }
//    namedWindow("detect faces",WINDOW_AUTOSIZE);
    imshow("detect faces",src);
}

void QuickDemo::VideoDemo(CascadeClassifier &face_classifier)
{
//    VideoCapture video(0);
    VideoCapture video;
    video.open("FaceDetect\\slow_traffic_small.mp4");
    // video.open("D:/Programes/CodeBlocksProjects/FaceDetect/1.mp4");
//    video.open("http://192.168.137.92:8081");
//    video.open("rtsp://192.168.137.92:8554/live");
    namedWindow("video",WINDOW_AUTOSIZE);
    Mat frame,gray;
    vector<Rect> faces;
    while(video.read(frame))
    {
        cvtColor(frame,gray,COLOR_BGR2GRAY);
        equalizeHist(gray,gray);    //直方图均衡化，用于提高图像的质量
        face_classifier.detectMultiScale(gray,faces,1.2,3,0,Size(50,50));
        for(size_t t=0;t<faces.size();t++)
        {
            rectangle(frame,faces[t],Scalar(255,0,0),2,8,0);
        }
        flip(frame,frame,1);
        imshow("video",frame);
        char c = waitKey(25);
        if(c == 27)    // 按Esc退出
            break;
    }
    video.release();
    waitKey(0);
    destroyAllWindows();
}
