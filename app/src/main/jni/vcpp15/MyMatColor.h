#pragma once

// ����಻��д, ������������, ����������������

class MyMatColor:public MyMat
{
public:
    MyMatColor(void){};
    ~MyMatColor(void){};

    MyMatColor(cv::Mat matSrc){
        matImg = matSrc;
    }

    MyMatColor(const char* imgFile)
    {
        matImg = imread(imgFile, IMREAD_COLOR);
    }

    // 	const cv::Mat cv(){
    // 		cv::Mat img(h, w, CV_8UC3, data, w*3);
    // 		return img;
    // 	}

    void rotate()
    {
    }

    // תhsvͼ��		h:0-360, s:0-1, v:0-1
    void cvtHsv(Mat &hsv)
    {
        cv::Mat bgr;
        matImg.convertTo(bgr, CV_32FC3, 1.0/255, 0);	// ��ɫͼ��ĻҶ�ֵ��һ��
        cvtColor(bgr, hsv, COLOR_BGR2HSV);
    }

    // 获取颜色mask 若sc1Min.val[0]==sc1Max.val[0]表后段有效
    void getColorMask(cv::Mat &mask, Scalar sc0Min, Scalar sc0Max, Scalar sc1Min, Scalar sc1Max, int dispMask = 0) {
        cv::Mat hsv, mask2;

        cvtHsv(hsv);

        inRange(hsv, sc0Min, sc0Max, mask);
        if(sc1Min.val[0] < sc1Max.val[0])	// 表示有两段范围
        {
            inRange(hsv, sc1Min, sc1Max, mask2);
            mask += mask2;
        }

        if (dispMask != 0) {
            cv::Mat barMat, masknot = mask;
            if (dispMask < 0)
                bitwise_not(masknot, masknot);
            matImg.copyTo(barMat, masknot);
            imshow("maskMat", masknot);
            imshow("ColorMat", barMat);
        }
    }

    // ��ȡ���л�ɫ����		dispMask=0 ����ʾ����ͼƬ, 1 ��ʾ��ѡ��ɫͼƬ, -1 ����ʾ��ѡ��ɫͼƬ
    void getColorMask(cv::Mat &mask, int colorType=0, int dispMask=0)
    {
        cv::Mat hsv, mask2;
        cvtHsv(hsv);

        switch(colorType)
        {
        case 0:		// yellow
            inRange(hsv, Scalar(45, 0.35, 0.4), Scalar(75, 1, 1), mask);	// Yellow 较暗
            //inRange(hsv, Scalar(45, 0.40, 0.6), Scalar(75, 1, 1), mask);	// Yellow
            //inRange(hsv, Scalar(45, 0.45, 0.6), Scalar(70, 1, 1), mask);	// Yellow
            break;
        case 1:		// red
            inRange(hsv, Scalar(0,   0.5, 0.3), Scalar( 30, 1, 1), mask);	// red1
            inRange(hsv, Scalar(320, 0.5, 0.3), Scalar(360, 1, 1), mask2);// red2
            mask += mask2;
            break;
        case 2:		// orange
            break;
        case 3:		// green
            inRange(hsv, Scalar(100, 0.4, 0.4), Scalar(190, 1, 1), mask);	// blue
            break;
        case 4:		//
            break;
        case 5:		//
            break;
        case 6:		// blue
            inRange(hsv, Scalar(180, 0.5, 0.5), Scalar(275, 1, 1), mask);	// blue
            break;
        case 7:		// purple
            break;
        default:
            break;
        }

        if(dispMask != 0)
        {
            cv::Mat barMat, masknot=mask;
            if(dispMask < 0)
                bitwise_not(masknot, masknot);
            matImg.copyTo(barMat, masknot);
            imshow("maskMat", masknot);
            imshow("ColorMat", barMat);
        }
    }

    // 每点与蓝色的距离值
    void getColorDist_blue(cv::Mat &mask)
    {
        mask = cv::Mat::zeros(matImg.size(),CV_8UC1);
        for (int i = 0; i < matImg.rows; i++) {
            Vec3b* p = matImg.ptr<Vec3b>(i);
            uchar* q = mask.ptr<uchar>(i);
            for (int j = 0; j < matImg.cols; j++) {
                uchar b = p[j][0];
                uchar g = p[j][1];
                uchar r = p[j][2];
                if(b > g && b > r)
                {
                    float m = (b+g+r)/3.0;
                    q[j] = sqrt((b-m)*(b-m)+(g-m)*(g-m)+(r-m)*(r-m));
                }
            }
        }
    }

    // 每点与绿色的距离值
    void getColorDist_green(cv::Mat &mask)
    {
        mask = cv::Mat::zeros(matImg.size(),CV_8UC1);
        for (int i = 0; i < matImg.rows; i++) {
            Vec3b* p = matImg.ptr<Vec3b>(i);
            uchar* q = mask.ptr<uchar>(i);
            for (int j = 0; j < matImg.cols; j++) {
                uchar b = p[j][0];
                uchar g = p[j][1];
                uchar r = p[j][2];
                if(g > b && g > r)
                {
                    float m = (b+g+r)/3.0;
                    q[j] = sqrt((b-m)*(b-m)+(g-m)*(g-m)+(r-m)*(r-m));
                }
            }
        }
    }

    // 每点与红色的距离值
    void getColorDist_red(cv::Mat &mask)
    {
        mask = cv::Mat::zeros(matImg.size(),CV_8UC1);
        for (int i = 0; i < matImg.rows; i++) {
            Vec3b* p = matImg.ptr<Vec3b>(i);
            uchar* q = mask.ptr<uchar>(i);
            for (int j = 0; j < matImg.cols; j++) {
                uchar b = p[j][0];
                uchar g = p[j][1];
                uchar r = p[j][2];
                if(r > b && r > g)
                {
                    float m = (b+g+r)/3.0;
                    q[j] = sqrt((b-m)*(b-m)+(g-m)*(g-m)+(r-m)*(r-m));
                }
            }
        }
    }

    // 灰色判断
    bool isGray(uchar b, uchar g, uchar r, uchar blackTh, uchar vOffset, uchar vMaxTh)
    {
        double avr=(b+g+r)/3.0;

        // 大于设置最大值 false
        if(b > vMaxTh || g > vMaxTh || r > vMaxTh) return false;

        // 左下角黑色区
        if(b < blackTh && g < blackTh && r < blackTh) return true;

        // 距立方体对角线在阈值范围内
        if(b-avr < vOffset && g-avr < vOffset && r-avr < vOffset) return true;

        return false;
    }

    // 分割灰色 小于grayTh的都为灰色
    void segmentGray(cv::Mat &mask, int grayTh) {
        mask = cv::Mat::zeros(matImg.size(), CV_8UC1);

        for (int i = 0; i < matImg.rows; i++) {
            Vec3b* p = matImg.ptr<Vec3b>(i);
            uchar* q = mask.ptr<uchar>(i);
            for (int j = 0; j < matImg.cols; j++) {
                //if(!isGray(p[j][0], p[j][1], p[j][2], 20, 18, grayTh))// 20 18 84
                //	q[j] = 255;
                if (p[j][0] < grayTh && p[j][1] < grayTh && p[j][2] < grayTh)// 分割灰度
                    q[j] = 0;
                else
                    q[j] = 255;
            }
        }
    }

    // �ָ�ڻ�ɫ
    void segmentBlackYellow(cv::Mat &mask, int BYSumth)
    {
        mask = cv::Mat::zeros(matImg.size(),CV_8UC1);

        for (int i=0; i < matImg.rows; i++)
        {
            Vec3b* p = matImg.ptr<Vec3b>(i);
            uchar* q = mask.ptr<uchar>(i);
            for (int j=0; j < matImg.cols; j++)
            {
                if(p[j][1] + p[j][2] > BYSumth)// �ָ�ڻ�ɫ ֱ����ֵ	// >Ϊ��ɫ,��ǰ�ɫ
                    q[j]=255;
            }
        }
    }

    // 提升亮度	三通道 渐变
    // 按图像高度提升 设置亮度不变的坐标零点,每行提升量fAdd百分比
    // void brightUp(int y0, double fAdd)
    // {
    //     int h = matImg.rows;
    //     for (int i = 0; i < h; i++)
    //     {
    //         Vec3b* p = matImg.ptr<Vec3b>(i);
    //         for (int j = 0; j < matImg.cols; j++)
    //         {
    //             int a = fAdd*(i-y0);	// 计算每行应增加量
    //             p[j][0] = MyLimit(0, p[j][0]+a, 255);
    //             p[j][1] = MyLimit(0, p[j][1]+a, 255);
    //             p[j][2] = MyLimit(0, p[j][2]+a, 255);
    //         }
    //     }
    // }

    // 椭圆渐变 两点中心近似椭圆
    // 输入两点中心，其他点根据最近点距离渐变, 距离增量百分比fAdd
    // 以radius为半径
    // static void gradualColor(cv::Mat &matsrc, int x1, int y1, int x2, int y2, int radius, double gradualRate)
    // {
    //     int h = matsrc.rows;
    //     MyLineD mld(x1, y1, x2, y2);
    //     double ptDistance = mld.length();
    //     double dtDiffTh = sqrt(radius*radius + ptDistance*ptDistance) - radius;

    //     for (int i = 0; i < h; i++)
    //     {
    //         Vec3b* p = matsrc.ptr<Vec3b>(i);
    //         for (int j = 0; j < matsrc.cols; j++)
    //         {
    //             double dt1 = MyPointD::distance(j, i, x1, y1);
    //             double dt2 = MyPointD::distance(j, i, x2, y2);
    //             double dtv = mld.distanceFromPoint(j, i);
    //             if(dt1 > dt2)
    //             {
    //                 MYSWAP_DOUBLE(dt1, dt2);
    //             }
    //             // ！(垂直距离<radius && (两边半径<radius || 到两点距离差<(sqrt(2)-1)*radius))
    //             // if(!(dtv < radius && (dt1 < radius || dt2-dt1 < 0.414213562373095*radius)))
    //             if(dtv > radius || (dt1 > radius && dt2-dt1 > dtDiffTh))
    //             {
    //                 int a = gradualRate*dt1;    // 计算每点增加量
    //                 p[j][0] = MyLimit(0, p[j][0]+a, 255);
    //                 p[j][1] = MyLimit(0, p[j][1]+a, 255);
    //                 p[j][2] = MyLimit(0, p[j][2]+a, 255);
    //             }
    //         }
    //     }
    // }

    static void matColorReplace(cv::Mat &matSrc, Vec3b srcColor, Vec3b dstColor)
    {
        for (int i = 0; i < matSrc.rows; i++)
        {
            Vec3b* p = matSrc.ptr<Vec3b>(i);
            for (int j = 0; j < matSrc.cols; j++)
            {
                if(p[j]==srcColor)
                    p[j] = dstColor;
            }
        }
    }

    // ȡ��ɫ������, HoughLinesP��ֱ��, �ϲ�ֱ��, ����ֱ�߽Ƕ�, ͼ��ת��, �����ֱ�߼н�, ������ֱ�ߵ�������
    Rect colorLineProcess()
    {
        Rect rt(0,0,0,0);
        MyMatColor mm2(matImg);

        double fscale=0.3;	// ͼ����С����
        mm2.Resize(fscale);

        // ȥ���ڱ߿�
        //mm.eraseFrame(50, 29);

        // ȡ��ɫ
        MyMatBin mask;	// ����
        mm2.getColorMask(mask.matImg, 3);

        // ��ֱ��
        vector<Vec4i> lines;
        HoughLinesP(mask.matImg, lines, 1, CV_PI/180, 50, 50, 10 );
        mm2.showImg();
        mask.showImg();
        //waitKey();
        if(lines.size() < 2) return rt;

        // �ϲ������ֱ��
        vector<MyLineD> vmld;
        MyLineD::mergeSameLines(vmld, lines, 2.1);
        // 		for (int i=0; i < vmld.size(); i++)
        // 			ms.appendln(vmld[i].toString());

        // ����ֱ�߽Ƕ�
        double sumA=0;
        for (int i=0; i < vmld.size(); i++)
        {
            sumA += vmld[i].getAngle();
            //outDouble1(vmld[i].angle());
        }
        outDouble1(sumA/vmld.size());

        // ͼ��ת��
        // 	MyMatColor mm3(mm.matImg);
        // 	IplImage ipl_img2 = mm2.matImg;
        // 	IplImage ipl_img3 = mm3.matImg;
        // 	mm3.rotateImage(&ipl_img3, &ipl_img3, -sumA/3);
        // 	mm3.showImg(1, "mm3");

        // �����ֱ�߼н�

        // ������ֱ�ߵ�������
        MyLineD mld(vmld[0]);
        mld.midLine(vmld[1]);
        mld /= fscale;
        //		ms.appendln(mld.toString());

        // ����ֱ��mldת��ͼ��

        // �õ��¶�����ֱ�ߺ�, ������Χ��������
        rt = mld.boundRect();
        rt.x -= 10;
        rt.width += 24;
        rt.height -= 8;

        return rt;
    }

    void showImg(double fScale=1, char* showName=NULL)
    {
        if(showName == NULL)
            MyMat::showImg(fScale, "MyMatColor");
        else
            MyMat::showImg(fScale, showName);
    }

    void save(char* showName=NULL)
    {
        if(showName == NULL)
            cv::imwrite("MyMatColor.jpg", matImg);
        else
            cv::imwrite(showName, matImg);
    }
};
