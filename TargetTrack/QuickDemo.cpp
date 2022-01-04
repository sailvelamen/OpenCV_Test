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

    //��������
    Mat kernel = getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));

    // ͼ��ָ�(GMM-��˹���ģ��)
    Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2();
    // ����ѧϰ(KNN-K�������)
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
                                    // ���� �ں˵ĳߴ� ê���λ��
    Mat kernel1 = getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));// �����ĽṹԪ�� 
    Mat kernel2 = getStructuringElement(MORPH_RECT,Size(5,5),Point(-1,-1));

    namedWindow("video",WINDOW_AUTOSIZE);
    namedWindow("track mask",WINDOW_AUTOSIZE);
    while(capture.read(frame))
    {

        inRange(frame,Scalar(0,127,0),Scalar(120,255,120),mask);    //�����ƻ�
            // Դͼ�� Ŀ��ͼ�� ���������� �ṹԪ�� �ṹԪ������λ�� ��Ե����
        morphologyEx(mask,mask,MORPH_OPEN,kernel1,Point(-1,-1),1);    //��̬ѧ����(������)
        dilate(mask,mask,kernel2,Point(-1,-1),4);   //���ͺ���

        this->processFrame(mask,roi); //����������λ�ñ궨
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
    vector<vector<Point>> contours;//˫������(����)
    //Vec4i��Vec<int,4>�ı�����������һ����������ÿһ��Ԫ�ذ�����4��int�ͱ�������������
    vector<Vec4i> hireachy;//������ÿ��Ԫ�ض���һ������4��int�͵�����

            //   ��ֵͼ�� ���� �������� ��������ģʽ �����Ľ��Ʒ��� Pointƫ����
    findContours(mask,contours,hireachy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point(0,0));//�������
                                    //ֻ�������Χ���� �����������Ĺյ���Ϣ
    if(contours.size()>0)
    {
        double maxArea = 0.0;
        for(size_t t=0;t<contours.size();t++)
        {
            double area = contourArea(contours[static_cast<int>(t)]);//�������
            if(area > maxArea)
            {
                maxArea = area;
                rect = boundingRect(contours[static_cast<int>(t)]);//��������������С������
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

    bool firstRead = true;  // �жϵ�һ֡
    float hrange[] = {0,180};
    const float* hranges = hrange;
    Rect selection;
    Mat frame,hsv,hue,mask,hist,backprojection;
    Mat drawImg = Mat::zeros(300,300,CV_8UC3); //ֱ��ͼ
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
                //��ά����ĳߴ�,������ͨ������
        hue = Mat(hsv.size(),hsv.depth());//��СΪ size������Ϊ type ��ͼ��
        int channels[] = {0,0};
        mixChannels(&hsv,1,&hue,1,channels,1);  //���ͨ��

        if(firstRead)
        {
            //ROIֱ��ͼ����//calculate histogram
            Mat roi(hue,selection); //������������
            Mat maskroi(mask,selection);
            //����ͼ��ָ��,����ͼ�����,ͳ�Ƶڼ�ͨ��,��ģ,ֱ��ͼ���ֵ,���ֱ��ͼ��ά��,������,ͳ����������
            calcHist(&roi,1,0,maskroi,hist,1,&bins,&hranges);//ֱ��ͼ����
                                    // ���Թ�һ��
            normalize(hist,hist,0,255,NORM_MINMAX);//��һ������(����,���,��Сֵ,���ֵ,��һ������)
            
            //show histogram
                            //cols:��
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