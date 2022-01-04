#include "QuickDemo.h"

QuickDemo::QuickDemo()
{
    //ctor
}

QuickDemo::~QuickDemo()
{
    //dtor
}

void QuickDemo::BSM()
{
    VideoCapture capture(0);
    if(!capture.isOpened())
    {
        cout << "Could not load video data..." << endl;
    }

    Mat frame,bsmaskMOG2,bsmaskKNN;
    namedWindow("video",WINDOW_AUTOSIZE);
    namedWindow("MOG2",WINDOW_AUTOSIZE);
//    namedWindow("KNN Model",WINDOW_AUTOSIZE);

    //消除噪声
    Mat kernel = getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));

    // 图像分割(GMM-高斯混合模型)
    Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2();
    // 机器学习(KNN-K个最近邻)
    Ptr<BackgroundSubtractor> pKNN = createBackgroundSubtractorKNN();
    while(capture.read(frame))
    {
        imshow("video",frame);
        //MOG BS
        pMOG2->apply(frame,bsmaskMOG2);
        morphologyEx(bsmaskMOG2,bsmaskMOG2,MORPH_OPEN,kernel,Point(-1,-1));
        imshow("MOG2",bsmaskMOG2);
//        //KNN BS
//        pKNN->apply(frame,bsmaskKNN);
//        imshow("KNN Model",bsmaskKNN);

        char c = waitKey(50);
        if(c == 27)
            break;
    }
    capture.release();
    waitKey(0);
}

void QuickDemo::ColorTrack(Rect &roi)
{
    VideoCapture capture(0);
    if(!capture.isOpened())
    {
        cout << "Could not load video data..." << endl;
    }

    Mat frame,mask;
                                    // 矩形 内核的尺寸 锚点的位置
    Mat kernel1 = getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));// 创建的结构元素 
    Mat kernel2 = getStructuringElement(MORPH_RECT,Size(5,5),Point(-1,-1));

    namedWindow("video",WINDOW_AUTOSIZE);
    namedWindow("track mask",WINDOW_AUTOSIZE);
    while(capture.read(frame))
    {

        inRange(frame,Scalar(0,127,0),Scalar(120,255,120),mask);    //二进制化
            // 源图像 目标图像 操作的类型 结构元素 结构元素中心位置 边缘类型
        morphologyEx(mask,mask,MORPH_OPEN,kernel1,Point(-1,-1),1);    //形态学操作(开操作)
        dilate(mask,mask,kernel2,Point(-1,-1),4);   //膨胀函数

        this->processFrame(mask,roi); //轮廓发现与位置标定
        rectangle(frame,roi,Scalar(0,0,255),3,8,0);
        
        imshow("video",frame);
        imshow("track mask",mask);
        
        char c = waitKey(50);
        if(c == 27)
            break;
    }
    capture.release();
    waitKey(0);
}

void QuickDemo::processFrame(Mat &mask,Rect &rect)
{
    vector<vector<Point>> contours;//双重向量(集合)
    //Vec4i是Vec<int,4>的别名，定义了一个“向量内每一个元素包含了4个int型变量”的向量。
    vector<Vec4i> hireachy;//向量内每个元素都是一个包含4个int型的数组

            //   二值图像 集合 二重向量 检索轮廓模式 轮廓的近似方法 Point偏移量
    findContours(mask,contours,hireachy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point(0,0));//检测轮廓
                                    //只检测最外围轮廓 仅保存轮廓的拐点信息
    if(contours.size()>0)
    {
        double maxArea = 0.0;
        for(size_t t=0;t<contours.size();t++)
        {
            double area = contourArea(contours[static_cast<int>(t)]);//计算面积
            if(area > maxArea)
            {
                maxArea = area;
                rect = boundingRect(contours[static_cast<int>(t)]);//包覆此轮廓的最小正矩形
            }
        }
    }
    else
    {
        rect.x = rect.y = rect.width = rect.height = 0;
    }
}

void QuickDemo::CAMShift(int smin,int vmin,int vmax,int bins)
{
    // VideoCapture capture(0);
    VideoCapture capture;
    capture.open("TargetTrack\\slow_traffic_small.mp4");
    if(!capture.isOpened())
    {
        cout << "Could not load video data..." << endl;
    }
    namedWindow("CAMShift Tracking",WINDOW_AUTOSIZE);   
    namedWindow("ROI Histogram",WINDOW_AUTOSIZE);  

    bool firstRead = true;  // 判断第一帧
    float hrange[] = {0,180};
    const float* hranges = hrange;
    Rect selection;
    Mat frame,hsv,hue,mask,hist,backprojection;
    Mat drawImg = Mat::zeros(300,300,CV_8UC3); //直方图
    double fps = capture.get(CAP_PROP_FPS);
    cout << "FPS:" << fps << endl;

    while(capture.read(frame))
    {
        if(firstRead)
        {
            Rect2d first = selectROI("CAMShift Tracking",frame);
            selection.x = first.x;
            selection.y = first.y;
            selection.width = first.width;
            selection.height = first.height;
            cout<<"ROI.x="<<selection.x<<",ROI.y="<<selection.y<<",width"<<selection.width<<",height"<<selection.height<<endl;
            // firstRead = false;
        }
        
        //convert to HSV
        cvtColor(frame,hsv,COLOR_BGR2HSV);
        inRange(hsv,Scalar(0,smin,vmin),Scalar(180,vmax,vmax),mask);
                //二维矩阵的尺寸,单独的通道类型
        hue = Mat(hsv.size(),hsv.depth());//大小为 size，类型为 type 的图像
        int channels[] = {0,0};
        mixChannels(&hsv,1,&hue,1,channels,1);  //拆分通道

        if(firstRead)
        {
            //ROI直方图计算//calculate histogram
            Mat roi(hue,selection); //创建矩形区域
            Mat maskroi(mask,selection);
            //输入图像指针,输入图像个数,统计第几通道,掩模,直方图输出值,输出直方图的维度,横坐标,统计像素区间
            calcHist(&roi,1,0,maskroi,hist,1,&bins,&hranges);//直方图计算
                                    // 线性归一化
            normalize(hist,hist,0,255,NORM_MINMAX);//归一化数据(输入,输出,最小值,最大值,归一化类型)
            
            //show histogram
                            //cols:行
            int binw =drawImg.cols / bins;
            Mat colorIndex = Mat(1,bins,CV_8UC3);
            for(int i = 0;i < bins; i++)
            {
                colorIndex.at<Vec3b>(0,i) = Vec3b(saturate_cast<uchar>(i*180 / bins),255,255);
            }
            cvtColor(colorIndex,colorIndex,COLOR_HSV2BGR);
            for(int i=0;i<bins;i++)
            {
                int val = saturate_cast<int>(hist.at<float>(i)*drawImg.rows / 255);
                rectangle(drawImg,Point(i*binw,drawImg.rows),Point((i+1)*binw,drawImg.rows-val),Scalar(colorIndex.at<Vec3b>(0,i)),-1,8,0);
            }
        }

        //back projection
        calcBackProject(&hue,1,0,hist,backprojection,&hranges);
        //CAMShift tracking
        backprojection &= mask;
        RotatedRect trackBox=CamShift(backprojection,selection,TermCriteria((TermCriteria::COUNT | TermCriteria::EPS),10,1));

        //draw location on frame
        ellipse(frame,trackBox,Scalar(0,0,255),3,8);

        if(firstRead)
        {
            firstRead = false;
        }
        
        imshow("CAMShift Tracking",frame);
        imshow("ROI Histogram",drawImg);
        char c = waitKey(30);
        if(c == 27)
            break;
    }
    capture.release();
    waitKey(0);
}