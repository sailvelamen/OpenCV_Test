#ifndef QUICKDEMO_H
#define QUICKDEMO_H

#include<iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class QuickDemo
{
    public:
        QuickDemo();
        virtual ~QuickDemo();

        void ImageDemo(Mat &src,CascadeClassifier &face_classifier);
        void VideoDemo(CascadeClassifier &face_classifier);

    protected:

    private:
};

#endif // QUICKDEMO_H
