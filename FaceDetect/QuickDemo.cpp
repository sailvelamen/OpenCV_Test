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
    equalizeHist(gray,gray);    //ֱ��ͼ���⻯���������ͼ�������

    //����������͵Ķ�̬����
    vector<Rect> faces;
                                //(Ŀ��ͼƬ,������λ��,���ű���,������,Ĭ��0,��С�ߴ�,���ߴ�)
//    face_classifier.detectMultiScale(gray,faces,1.1,3,0,Size(24,24),Size(200,200));
    face_classifier.detectMultiScale(gray,faces);
    //size_t��һ���޷��ŵ�������������ȡֵû�и���
    for(size_t t=0;t<faces.size();t++)
    {
                //(ͼ��,����,������ɫ,������ϸ�̶�,����������,������С����λ��)
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
        equalizeHist(gray,gray);    //ֱ��ͼ���⻯���������ͼ�������
        face_classifier.detectMultiScale(gray,faces,1.2,3,0,Size(50,50));
        for(size_t t=0;t<faces.size();t++)
        {
            rectangle(frame,faces[t],Scalar(255,0,0),2,8,0);
        }
        flip(frame,frame,1);
        imshow("video",frame);
        char c = waitKey(25);
        if(c == 27)    // ��Esc�˳�
            break;
    }
    video.release();
    waitKey(0);
    destroyAllWindows();
}
