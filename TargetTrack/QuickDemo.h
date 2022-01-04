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

        // Rect roi;
        void BSM();
        void ColorTrack(Rect &roi);
        void processFrame(Mat &mask,Rect &rect);
        void CAMShift(int smin,int vmin,int vmax,int bins);

    protected:

    private:
};

#endif // QUICKDEMO_H
