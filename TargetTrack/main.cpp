#include <iostream>
#include <opencv2/opencv.hpp>
#include "QuickDemo.h"
#include "QuickDemo.cpp"

using namespace cv;
using namespace std;

int main(int agrc,char** argv)
{
    cout << "Hello world!" << endl;

    Rect roi;
    int smin = 15;
    int vmin = 40;
    int vmax = 255;
    int bins = 16;
    QuickDemo qd;
//    qd.BSM();
    // qd.ColorTrack(roi);
    qd.CAMShift(smin,vmin,vmax,bins);

    return 0;
}