
class MyMat
{
public:
    cv::Mat  	matImg;			// 所处理的图像

    MyMat(void){}

    MyMat(const MyMat &mm){  // 若要copy在类外手动copy
        matImg = mm.matImg;// mm.matImg.copyTo(matImg);
    }

    MyMat(cv::Mat matSrc){
        matImg = matSrc;// matSrc.copyTo(matImg);
    }

    // 图片合并 addImage 将img2添加到img1, 取两个图像合成的最大图像
    // int transparentTh=255 白色为透明, 0 黑色为透明, 其他无透明色(默认)
    // cv::Mat &imgFlag 返回受影响的点，二值图。被img2盖住的点为255，未被遮盖的点为0. 与img1大小相同
    MyMat(cv::Mat &img1, int sx, int sy, cv::Mat &img2, cv::Mat &imgFlag, int transparentTh=1)
    {
        // 计算添加后的图片大小
        int ox1 = 0, oy1 = 0;	// img1在新图坐标
        int ox2 = sx, oy2 = sy;	// img2在新图坐标
        int w = img1.cols;
        int h = img1.rows;

        if(sx < 0)
        {
            ox1 = -sx;
            ox2 = 0;
            w -= sx;
        }
        if(sy < 0)
        {
            oy1 = -sy;
            oy2 = 0;
            h -= sy;
        }
        if(sx + img2.cols > img1.cols) w += sx + img2.cols - img1.cols;
        if(sy + img2.rows > img1.rows) h += sy + img2.rows - img1.rows;

        matImg = cv::Mat(h,w,CV_8UC3,cv::Scalar(255,254,255));	// 合并后新图的背景
        imgFlag = cv::Mat(h,w,CV_8UC3,cv::Scalar(128,128,128));	// 返回被遮盖的点

        // 放入img1
        img1.copyTo(matImg(cvRect(ox1,oy1,img1.cols,img1.rows)));

        // 放入img2
        if(0 < transparentTh && transparentTh < 255)	// 无透明
        {
            img2.copyTo(matImg(cvRect(ox2,oy2,img2.cols,img2.rows)));
            imgFlag(cvRect(ox2,oy2,img2.cols,img2.rows)).setTo(cv::Scalar(0,0,0));
        }
        else	// 制作mask
        {
            cv::Mat matmask;
            cvtColor(img2,matmask,CV_BGR2GRAY);
            if(transparentTh == 255)		// 255为透明
                threshold(matmask, matmask, 254, 255, CV_THRESH_BINARY_INV);	// 当前点值大于阈值时，设置为0，否则设置为Maxval
            else //if(transparentTh == 0)	// 0为透明
                threshold(matmask, matmask, 0, 255, CV_THRESH_BINARY);			// 当前点值大于阈值时，取Maxval，否则设置为0
            img2.copyTo(matImg(cvRect(ox2,oy2,img2.cols,img2.rows)), matmask);	// 参数中黑色为透明色
            img2.copyTo(imgFlag(cvRect(ox2,oy2,img2.cols,img2.rows)), matmask);	// 参数中黑色为透明色
            //imshow("matmask", matmask);
        }

        imgFlag = imgFlag(cvRect(ox1,oy1,img1.cols,img1.rows));

        // 将指定颜色值128设为255(背景)，其他设为0(污损)
        thresholdColor(imgFlag, 128);
    }

    // 初始化图像全0
    MyMat(int w, int h, int nChannel=3){
        matImg = cv::Mat::zeros(h,w,CV_MAKETYPE(CV_8U,nChannel));
    }

    // 用已有的图像数据初始化Mat
    MyMat(char* buff, int w, int h, int nChannel=3){
        Mat iMat(h, w, CV_MAKETYPE(CV_8U,nChannel), buff, w*nChannel);
        matImg = iMat;
    }

    MyMat(const char* imgFile)	// 0, gray 1, color
    {
        // 		if(imgFile == NULL)	// 读取剪贴板图片
        // 			getImgFromClipboard();
        // 		else
        if(imgFile)
        {
            matImg = imread(imgFile, CV_LOAD_IMAGE_COLOR);
            if(!matImg.empty()) return;
        }
        printf("open image file failed! %s \n", imgFile);
        //exit(0);
    }

    ~MyMat(void){};

    void showMinMax()
    {
            double minVal, maxVal;
            minMaxLoc(matImg, &minVal, &maxVal);
            outDouble2(minVal, maxVal);
    }

    int getBrightValue(cv::Rect cr)
    {
        cv::Scalar mean1=cv::mean(matImg(cr));
        if(mean1.val[0] > mean1.val[1])
            return mean1.val[0] > mean1.val[2] ? mean1.val[0] : mean1.val[2];
        return mean1.val[1] > mean1.val[2] ? mean1.val[1] : mean1.val[2];
    }

    // 将指定颜色值设为255，其他设为0
    static void thresholdColor(cv::Mat &imgFlag, uchar bSelectColor)
    {
        threshold(imgFlag, imgFlag, bSelectColor, 255, CV_THRESH_TOZERO_INV);	// 当前点值大于阈值时，设置为0，否则不改变
        threshold(imgFlag, imgFlag, bSelectColor-1, 255, CV_THRESH_BINARY);		// 当前点值大于阈值时，取Maxval，否则设置为0
    }

    // 设置某点颜色, 支持灰度和彩色
    static void setPoint(cv::Mat matSrc, double x, double y, int B=255, int G=0, int R=255)
    {
        int xx=round(x);
        int yy=round(y);
        if (matSrc.channels() == 3)
        {
            matSrc.at<cv::Vec3b>(yy,xx) = cv::Vec3b(B, G, R);
        }
        else if (matSrc.channels() == 1)
        {
            matSrc.at<uchar>(yy,xx) = B;
        }
    }

    // 全部设置为同一个值 不需要了 用setTo
    // static void set(cv::Mat matsrc, int val)
    // {
    //     outInt2(matsrc.rows, matsrc.cols);
    //     for (int i = 0; i < matsrc.rows; i++)
    //     {
    //         Vec3b* p = matsrc.ptr<Vec3b>(i);
    //         for (int j = 0; j < matsrc.cols; j++)
    //         {
    //             p[j] = cv::Vec3b(val,val,val);
    //         }
    //     }
    // }

    // 缩放
    void Resize(double fScale, int interpolation=INTER_AREA)
    {
        // 		interpolation - 插值方法。共有5种：
        // 			１）INTER_NEAREST - 最近邻插值法
        // 			２）INTER_LINEAR - 双线性插值法（默认）
        // 			３）INTER_AREA - 基于局部像素的重采样（resampling using pixel area relation）。对于图像抽取（image decimation）来说，这可能是一个更好的方法。但如果是放大图像时，它和最近邻法的效果类似。
        // 			４）INTER_CUBIC - 基于4x4像素邻域的3次插值法
        // 			５）INTER_LANCZOS4 - 基于8x8像素邻域的Lanczos插值
        if(fScale != 0 && fScale != 1)
            cv::resize(matImg, matImg, cvSize(0,0), fScale, fScale, interpolation);
    }

    // 缩放2 宽高放大倍数都大于0且不同时为1
    void Resize(double fScaleX, double fScaleY, int interpolation=INTER_AREA)
    {
        if(fScaleX > 0 && fScaleY > 0 && !(fScaleX==1 && fScaleY==1))
            cv::resize(matImg, matImg, cvSize(0,0), fScaleX, fScaleY, interpolation);
    }

    // 切分图片 成多个相同大小的文件
    int  split(int w, int h, const char* fileName, bool bSave=false)
    {
        int n=0;
        for (int i=0; i < matImg.rows; i+=h) {
            for (int j=0; j < matImg.cols; j+=w) {
                cv::Rect cr(j,i,w,h);
                TORANGE_RECT_MAT2(cr, matImg);
                printf("%d %d %d %d, %s%d.bmp\n", i, j, cr.width, cr.height, fileName, n);
                if(cr.width != w || cr.height != h)
                    printf("切分图片大小不一致\n");
                if(bSave)
                {
                    char newName[256]={0};
                    sprintf(newName, "%s_%d.bmp", fileName, n++);
                    cv::imwrite(newName, matImg(cr));
                }
                cv::imshow("line172", matImg(cr));
                cv::waitKey(100);
            }
        }

        return n;
    }

    // 切分图片 成多个相同大小的文件
    int  split(int w, int h, char* names[], int nTotal=0, bool bSave=false)
    {
        int n=0;
        for (int i=0; i < matImg.rows; i+=h) {
            for (int j=0; j < matImg.cols; j+=w) {
                printf("%d %d %d %s\n", n, i, j, names[n]);
                //              cv::imshow(names[n++], matImg(cvRect(j,i,w,h)));
                //              cv::waitKey(1000);
                if(bSave)
                    cv::imwrite(names[n++],matImg(cvRect(j,i,w,h)));
                if(n > 0 && n >= nTotal) return n;
            }
        }

        return n;
    }

    // 图像叠加 img2放到img1中 原图不增加尺寸
    static int addImage1(cv::Mat &img1, cv::Mat &img2, int x, int y)
    {
        cv::Rect dstrc(x, y, img2.cols, img2.rows);
        if(INRANGE_RECT_MAT(dstrc, img1))
        {
            img2.copyTo(img1(dstrc));
            return 0;
        }
        else
            rectangle(img1,dstrc,CV_RGB(222,2,222),2);
        return -1;	// 放置图片超出范围
    }
    
    // 图像叠加 img2放到img1中 新图增加尺寸 不要求：img1与img2尺寸相同
    // int x, int y : 第二张图在第一张图中的坐标
    int addImage2(cv::Mat img1, cv::Mat img2, int x, int y)
    {
        // 总图尺寸
        int nW = x < 0 ? max(img1.cols-x, img2.cols) : max(img1.cols, img2.cols+x);
        int nH = y < 0 ? max(img1.rows-y, img2.rows) : max(img1.rows, img2.rows+y);

        // 两张图在新图中的坐标
        int x1 = x < 0 ? -x : 0;
        int y1 = y < 0 ? -y : 0;
        int x2 = x < 0 ? 0 : x;
        int y2 = y < 0 ? 0 : y;
        
        matImg = cv::Mat(nH,nW,img1.type(),cv::Scalar(0,0,0));  // 合并后新图的背景

        img1.copyTo(matImg(cvRect(x1,y1,img1.cols,img1.rows))); // 是否存在相同图像copy覆盖的问题？？
        img2.copyTo(matImg(cvRect(x2,y2,img2.cols,img2.rows)));

        return 0;
    }

    // 图像叠加 img2放到img1中 新图增加尺寸 要求：img1与img2尺寸相同
    // int x, int y : 第二张图在第一张图中的坐标
    // 并未考虑所有情况
    int addImage(cv::Mat img1, cv::Mat img2, int x, int y)
    {
        // 总图尺寸
        int nW = x < 0 ? img1.cols - x : max(img1.cols, img2.cols+x);
        int nH = y < 0 ? img1.rows - y : max(img1.rows, img2.rows+y);

        // 两张图在新图中的坐标
        int x1 = x < 0 ? -x : 0;
        int y1 = y < 0 ? -y : 0;
        int x2 = x < 0 ? 0 : x;
        int y2 = y < 0 ? 0 : y;
        
        matImg = cv::Mat(nH,nW,CV_8UC3,cv::Scalar(255,254,255));  // 合并后新图的背景

        img1.copyTo(matImg(cvRect(x1,y1,img1.cols,img1.rows)));
        img2.copyTo(matImg(cvRect(x2,y2,img2.cols,img2.rows)));

        return 0;
    }

    int addImage(cv::Mat &img, int x, int y)
    {
        return addImage(matImg, img, x, y);
    }

    // 图像叠加 中心对准 img2放到img1的中心
    static int addImageCenter(cv::Mat &img1, cv::Mat &img2)
    {
        if(img2.cols > img1.cols || img2.rows > img1.rows)
            return -1;
        cv::Rect dstrc((img1.cols-img2.cols)/2, (img1.rows-img2.rows)/2, img2.cols, img2.rows);
        img2.copyTo(img1(dstrc));
        return 0;
    }

    int addImageCenter(cv::Mat &img)
    {
        return addImageCenter(matImg, img);
    }

    // 添加图形 mask图形
    // 输入图片图形,可以是0-255 深色图形浅色背景
    // bool bMasknot false将黑色部分添加 true将白色部分添加
    //        int addMaskImage(int _x, int _y, cv::Mat matmask, bool bMasknot=false, bool bDebug=false)
    //        {
    //                cv::Mat matmasknot, matgraybak;
    //                cv::Rect roi(_x, _y, matmask.cols, matmask.rows);

    //                if(!INRANGE_RECT_MAT(roi, matImg))
    //                {
    //                        outInt4(_x, _y, matmask.cols, matmask.rows);
    //                        outInt2(matImg.cols, matImg.rows);
    //                        outLinePos("addMaskImage位置或大小超出源图!"); return -1;
    //                }

    //                cv::Mat part1 = matImg(roi);
    //                cvtColor(matmask,matgraybak,CV_BGR2GRAY);
    //                threshold(matgraybak, matmask, 127, 255, CV_THRESH_BINARY);
    //                bitwise_not(matmask, matmasknot);
    //                if(bMasknot)
    //                        matgraybak.copyTo(part1, matmask);
    //                else
    //                        matgraybak.copyTo(part1, matmasknot);

    //                if(bDebug)
    //                        imshow("addMaskImage", matmask);
    //                return 0;
    //        }

    // double fScale=1, mask图形缩放
    //        int addMaskImage(int _x, int _y, cv::Mat matmask, int _w, int _h, bool bMasknot=false, bool bDebug=false)
    //        {
    //                TORANGE(_w, 2, 800);
    //                TORANGE(_h, 2, 800);
    //                cv::resize(matmask, matmask, cvSize(_w,_h), 0, 0, INTER_NEAREST);
    //                addMaskImage(_x, _y, matmask, bMasknot, bDebug);
    //                return 0;
    //        }

    // double fScale=1, mask图形缩放
    int addMaskImage(int _x, int _y, cv::Mat matmask, double fScalex=1, double fScaley=1, bool bMasknot=false, bool bDebug=false)
    {
        if(fScalex <= 0) fScalex=1;
        if(fScaley <= 0) fScaley=1;
        cv::resize(matmask, matmask, cvSize(0,0), fScalex, fScaley, INTER_NEAREST);
        addMaskImage(_x, _y, matmask, bMasknot, bDebug);
        return 0;
    }

    // 基于OpenCV的直方图均衡化
    // cv::equalizeHist(mmg.matImg, mmg.matImg);	// 可以原地操作

    // 滤波 模糊 平滑
    void smooth_filter_blur(int sw=3, int nType=0, bool bDebug=false)
    {
        if(bDebug)
        {
            Mat out, outsum;
            blur(matImg, out, Size(sw, sw));
            imshow("0 均值滤波", out);
            outsum = out/5;
            boxFilter(matImg, out, -1, Size(sw, sw));
            imshow("1 方框滤波", out);
            outsum = outsum + out/5;
            GaussianBlur(matImg, out, Size(sw, sw), 0, 0);
            imshow("2 高斯滤波", out);
            outsum = outsum + out/5;
            medianBlur(matImg, out, sw);
            imshow("3 中值滤波", out);
            outsum = outsum + out/5;
            bilateralFilter(matImg, out, sw*sw, sw*sw*2, sw*sw/2);
            imshow("4 双边滤波", out);
            outsum = outsum + out/5;
            imshow("五种滤波的求和", outsum);
        }

        if(nType == 0)		// 缺点就是不能很好地保护细节，在图像去燥的同时也破坏了图像的而细节部分，从而使图像变得模糊，不能很好的去除噪点。
            blur(matImg, matImg, Size(sw, sw));							// 均值滤波  是方框滤波的一个特殊情况.
        else if(nType == 1)
            boxFilter(matImg, matImg, -1, Size(sw, sw));				// 方框滤波	-1指原图深度
        else if(nType == 2)	// 可以消除高斯噪声，广泛应用于图像处理的减噪过程。从效果看出，高斯滤波的而模糊感明显降低(比方框滤波)
            GaussianBlur(matImg, matImg, Size(sw, sw), 0, 0);			// 高斯滤波
        else if(nType == 3)	// 中值滤波对一些细节多（特别是细、尖顶的）的图像不太适合。中值滤波花费的时间是均值滤波的5倍以上
            medianBlur(matImg, matImg, sw);								// 中值滤波 第三个参数表示孔径的线性尺寸，它的值必须是大于1的奇数
        else if(nType == 4)	// 效果图看来，双边滤波是所有滤波中最清晰的。
            bilateralFilter(matImg, matImg, sw*sw, sw*sw*2, sw*sw/2);	// 双边滤波的最大特点就是做边缘保存
    }

    // 待测试
    /** 计算图像的质心 中心 重心
        * @param[in] src  输入的待处理图像
        * @param[out] center 重心坐标
        * @retval 0  操作成功
        * @retval -1 操作失败
        * @note xc=M10/M00, yc=M01/M00
        */
    int aoiGravityCenter(IplImage *src, CvPoint &center)
    {
        //if(!src)
        // return GRAVITYCENTER__SRC_IS_NULL;
//         double m00, m10, m01;
//         CvMoments moment;
//         cvMoments( src, &moment, 1);
//         m00 = cvGetSpatialMoment( &moment, 0, 0 );
//         if( m00 == 0)
//             return 1;
//         m10 = cvGetSpatialMoment( &moment, 1, 0 );
//         m01 = cvGetSpatialMoment( &moment, 0, 1 );
//         center.x = (int) (m10/m00);
//         center.y = (int) (m01/m00);
//         return 0;
    }

    // 去窗口边框
    void eraseFrame(int nTop=30, int nBottom=8, int nLeft=8, int nRight=8)
    {
        matImg = matImg(Rect(nLeft, nTop, matImg.cols-nLeft-nRight, matImg.rows-nTop-nBottom));
    }

    // 旋转 90度的倍数 (屏幕看)顺时针 不是以图像中心旋转,是用转置和翻转组成的
    // 必须使用&地址参数否则,影响下面的rotateN90(matImg, matImg, n90);
    // MyMat::rotateN90(matgray(cr),mc.matImg,270); 参数不同却不行??为什么
    static void rotateN90(cv::Mat &matSrc, cv::Mat &matDst, int n90=90)//此函数仅支持旋转90\180\270度
    {
        switch(n90)
        {
        case 90:		// 顺时针旋转90度			rotx = h-1-y; roty = x; 逆运算:x=roty; y=h-1-rotx;
            transpose(matSrc, matDst);
            flip(matSrc,matDst,1);
            break;
        case 180:		// 顺时针旋转180度
            flip(matSrc,matDst,1);	// 左右翻转
            flip(matSrc,matDst,0);	// 上下翻转
            //			transpose(matImg, matImg);
            break;
        case 270:		// 顺时针旋转270度	逆时针旋转90度
            transpose(matSrc, matDst);// 转置
            flip(matSrc,matDst,0);	// 上下翻转
            break;
        default:
            break;
        }
    }

    // 旋转 90度的倍数 (屏幕看)顺时针 不是以图像中心旋转,是用转置和翻转组成的
    void rotateN90(int n90=90)//此函数仅支持旋转90\180\270度
    {
        rotateN90(matImg, matImg, n90);
    }

    // 旋转(放大尺寸的)	多出部分为白底. 示例: MyMat mm1;
    // IplImage img = matImg;
    // mm1.matImg = mm0.rotateImage2(&img, 60);
    //
    static IplImage* rotateImage2(IplImage* img, double degree)
    {
        double angle = degree  * CV_PI / 180.;
        double a = sin(angle), b = cos(angle);
        int width = img->width, height = img->height;
        //旋转后的新图尺寸
        int width_rotate = int(height * fabs(a) + width * fabs(b));
        int height_rotate = int(width * fabs(a) + height * fabs(b));
        IplImage* img_rotate = cvCreateImage(cvSize(width_rotate, height_rotate), img->depth, img->nChannels);
        cvZero(img_rotate);

        //保证原图可以任意角度旋转的最小尺寸
        int tempLength = sqrt((double)width * width + (double)height *height) + 10;
        int tempX = (tempLength + 1) / 2 - width / 2;
        int tempY = (tempLength + 1) / 2 - height / 2;
        IplImage* temp = cvCreateImage(cvSize(tempLength, tempLength), img->depth, img->nChannels);
        cvSet(temp, cvScalarAll(255));	// cvZero(temp);	// 背景底色

        //将原图复制到临时图像tmp中心
//         cvSetImageROI(temp, cvRect(tempX, tempY, width, height));
//         cvCopy(img, temp, NULL);
//         cvResetImageROI(temp);
//         //旋转数组map
//         // [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
//         // [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]
//         float m[6];
//         int w = temp->width;
//         int h = temp->height;
//         m[0] = b;
//         m[1] = a;
//         m[3] = -m[1];
//         m[4] = m[0];
//         // 将旋转中心移至图像中间
//         m[2] = w * 0.5f;
//         m[5] = h * 0.5f;
//         CvMat M = cvMat(2, 3, CV_32F, m);
//         cvGetQuadrangleSubPix(temp, img_rotate, &M);
        cvReleaseImage(&temp);
        return img_rotate;
    }

    // 放大尺寸的	 三通道	以图像中心逆时针旋转(屏幕左上角为原点)
    void rotateImage2(Mat &imgout, double Angle)
    {
        // double Angle=CV_PI*degree/180;
        int h=matImg.rows;
        int w=matImg.cols;

        // %计算四个角点的新坐标，确定旋转后的显示区域
        double LeftBottom11=(h-1)*sin(Angle);
        double LeftBottom12=(h-1)*cos(Angle);

        double LeftTop11=0;
        double LeftTop12=0;

        double RightBottom11=(w-1)*cos(Angle)+(h-1)*sin(Angle);
        double RightBottom12=-(w-1)*sin(Angle)+(h-1)*cos(Angle);

        double RightTop11=(w-1)*cos(Angle);
        double RightTop12=-(w-1)*sin(Angle);

        // %计算显示区域的行列数
        double Xnew=MyTools::mymax(LeftTop11,LeftBottom11,RightTop11,RightBottom11)-MyTools::mymin(LeftTop11,LeftBottom11,RightTop11,RightBottom11);
        double Ynew=MyTools::mymax(LeftTop12,LeftBottom12,RightTop12,RightBottom12)-MyTools::mymin(LeftTop12,LeftBottom12,RightTop12,RightBottom12);
        int width =round(Xnew+1);
        int height=round(Ynew+1);

        imgout=cv::Mat::zeros(height,width, CV_8UC3);
        double OX=round(abs(MyTools::mymin(LeftTop11,LeftBottom11,RightTop11,RightBottom11)));
        double OY=round(abs(MyTools::mymin(LeftTop12,LeftBottom12,RightTop12,RightBottom12)));

        //	%%反向映射法
        for(int i=0; i < height; i++)
        {
            for(int j=0; j < width; j++)
            {
                double X1=j-OX;
                double Y1=i-OY;
                double Xoo=X1*cos(Angle)-Y1*sin(Angle);	// 与点的顺时针旋转公式相同
                double Yoo=X1*sin(Angle)+Y1*cos(Angle);

                // %双线性插值法 边界超出算作255的背景
                int left=floor(Xoo);
                int right=ceil(Xoo);
                int up=floor(Yoo);
                int down=ceil(Yoo);

                // 背景出界补充颜色白
                Vec3b upleft=Vec3b(0,0,0),upright=Vec3b(0,0,0),downleft=Vec3b(0,0,0),downright=Vec3b(0,0,0);
                // Vec3b upleft=Vec3b(255,255,255),upright=Vec3b(255,255,255),downleft=Vec3b(255,255,255),downright=Vec3b(255,255,255);

                if(0 <= up && up < h && 0 <= left && left < w)
                    upleft = matImg.at<Vec3b>(up,left);
                if(0 <= up && up < h && 0 <= right && right < w)
                    upright = matImg.at<Vec3b>(up,right);
                if(0 <= down && down < h && 0 <= left && left < w)
                    downleft = matImg.at<Vec3b>(down,left);
                if(0 <= down && down < h && 0 <= right && right < w)
                    downright = matImg.at<Vec3b>(down,right);

                Vec3b upmid=(1-Xoo+left)*  upleft+ (Xoo-left)* upright;
                Vec3b downmid=(1-Xoo+left)*downleft+(Xoo-left)*downright;
                Vec3b central=(1-Yoo+up)*upmid+(Yoo-up)*downmid;
                imgout.at<Vec3b>(i,j) = central;
            }
        }
    }

    // 放大尺寸的	 输入输出均为灰度图	以图像中心逆时针旋转(屏幕左上角为原点)
    void rotateImage2gray(Mat &imgGrayOut, double degree)
    {
        double Angle=CV_PI*degree/180;
        int h=matImg.rows;
        int w=matImg.cols;

        // %计算四个角点的新坐标，确定旋转后的显示区域
        double LeftBottom11=(h-1)*sin(Angle);
        double LeftBottom12=(h-1)*cos(Angle);
        double LeftTop11=0;
        double LeftTop12=0;
        double RightBottom11=(w-1)*cos(Angle)+(h-1)*sin(Angle);
        double RightBottom12=-(w-1)*sin(Angle)+(h-1)*cos(Angle);
        double RightTop11=(w-1)*cos(Angle);
        double RightTop12=-(w-1)*sin(Angle);

        // %计算显示区域的行列数
        double Xnew=MyTools::mymax(LeftTop11,LeftBottom11,RightTop11,RightBottom11)-MyTools::mymin(LeftTop11,LeftBottom11,RightTop11,RightBottom11);
        double Ynew=MyTools::mymax(LeftTop12,LeftBottom12,RightTop12,RightBottom12)-MyTools::mymin(LeftTop12,LeftBottom12,RightTop12,RightBottom12);
        int width =round(Xnew+1);
        int height=round(Ynew+1);

        // % 分配新显示区域矩阵
        imgGrayOut=cv::Mat::zeros(height,width, CV_8UC1);

        double OX=round(abs(MyTools::mymin(LeftTop11,LeftBottom11,RightTop11,RightBottom11)));
        double OY=round(abs(MyTools::mymin(LeftTop12,LeftBottom12,RightTop12,RightBottom12)));

        //	%%反向映射法
        for(int i=0; i < height; i++)
        {
            for(int j=0; j < width; j++)
            {
                double X1=j-OX;
                double Y1=i-OY;
                double Xoo=X1*cos(Angle)-Y1*sin(Angle);	// 与点的顺时针旋转公式相同
                double Yoo=X1*sin(Angle)+Y1*cos(Angle);

                // %双线性插值法 边界超出算作255的背景
                int left=floor(Xoo);
                int right=ceil(Xoo);
                int up=floor(Yoo);
                int down=ceil(Yoo);

                // 背景出界补充颜色白
                uchar upleft=255,upright=255,downleft=255,downright=255;

                if(0 <= up && up < h && 0 <= left && left < w)
                    upleft = matImg.at<uchar>(up,left);
                if(0 <= up && up < h && 0 <= right && right < w)
                    upright = matImg.at<uchar>(up,right);
                if(0 <= down && down < h && 0 <= left && left < w)
                    downleft = matImg.at<uchar>(down,left);
                if(0 <= down && down < h && 0 <= right && right < w)
                    downright = matImg.at<uchar>(down,right);

                uchar upmid=(1-Xoo+left)*  upleft+ (Xoo-left)* upright;
                uchar downmid=(1-Xoo+left)*downleft+(Xoo-left)*downright;
                uchar central=(1-Yoo+up)*upmid+(Yoo-up)*downmid;
                imgGrayOut.at<uchar>(i,j) = central;
            }
        }
    }

    // img2Gray0428图片旋转_最近邻无插值 放大尺寸 逆时针(左上角为原点)
    static void rotateImage2NN(cv::Mat &matSrc, cv::Mat &imgout, double degree)
    {
        int oldWidth = matSrc.cols;
        int oldHeight = matSrc.rows;
        float theta = -degree*CV_PI/180;

        // 源图四个角的坐标（以图像中心为坐标系原点）
        float fSrcX1,fSrcY1,fSrcX2,fSrcY2,fSrcX3,fSrcY3,fSrcX4,fSrcY4;
        fSrcX1 = (float)(- (oldWidth  - 1)/2.0);
        fSrcY1 = (float)(  (oldHeight - 1)/2.0);
        fSrcX2 = (float)(  (oldWidth  - 1)/2.0);
        fSrcY2 = (float)(  (oldHeight - 1)/2.0);
        fSrcX3 = (float)(- (oldWidth  - 1)/2.0);
        fSrcY3 = (float)(- (oldHeight - 1)/2.0);
        fSrcX4 = (float)(  (oldWidth  - 1)/2.0);
        fSrcY4 = (float)(- (oldHeight - 1)/2.0);

        // 旋转后四个角的坐标（以图像中心为坐标系原点）
        float fDstX1,fDstY1,fDstX2,fDstY2,fDstX3,fDstY3,fDstX4,fDstY4;
        fDstX1 =  cos(theta) * fSrcX1 + sin(theta) * fSrcY1;
        fDstY1 = -sin(theta) * fSrcX1 + cos(theta) * fSrcY1;
        fDstX2 =  cos(theta) * fSrcX2 + sin(theta) * fSrcY2;
        fDstY2 = -sin(theta) * fSrcX2 + cos(theta) * fSrcY2;
        fDstX3 =  cos(theta) * fSrcX3 + sin(theta) * fSrcY3;
        fDstY3 = -sin(theta) * fSrcX3 + cos(theta) * fSrcY3;
        fDstX4 =  cos(theta) * fSrcX4 + sin(theta) * fSrcY4;
        fDstY4 = -sin(theta) * fSrcX4 + cos(theta) * fSrcY4;

        // 新图宽高
        int newWidth  = 1 + round(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)));
        int newHeight = 1 + round(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)));
        imgout.create(newHeight, newWidth, matSrc.type());

        // 新图中心相对旧图中心的偏移量 图像的中心点为((w-1)/2.0,(h-1)/2.0)
        float dx = ((newWidth-1)*cos(theta) + (newHeight-1)*sin(theta) - (oldWidth-1))/2;
        float dy = ((newHeight-1)*cos(theta) - (newWidth-1)*sin(theta) - (oldHeight-1))/2;

        int x,y;
        for (int i=0; i<newHeight; i++)
        {
            for (int j=0; j<newWidth; j++)
            {
                // x = (j-w/2)*cos(theta) + (i-h/2)*sin(theta) + ox;  ox,oy旧图中心
                // x = j*cos(theta) + i*sin(theta) - (w/2*cos(theta) + h/2*sin(theta)) + ox;
                // 以新图中心为原点计算旋转 转换为旧图坐标,公式变换成为下式
                x = round(j*cos(theta) + i*sin(theta) - dx);
                y = round(i*cos(theta) - j*sin(theta) - dy);

                if ((x<0) || (x>=oldWidth) || (y<0) || (y>=oldHeight))
                {
                    if (matSrc.channels() == 3)
                    {
                        imgout.at<cv::Vec3b>(i,j) = cv::Vec3b(255,255,255);  // cv::Vec3b(0,0,0);// 底色(超出源图部分的颜色)
                    }
                    else if (matSrc.channels() == 1)
                    {
                        imgout.at<uchar>(i,j) = 0;
                    }
                }
                else
                {
                    if (matSrc.channels() == 3)
                    {
                        imgout.at<cv::Vec3b>(i,j) = matSrc.at<cv::Vec3b>(y,x);
                    }
                    else if (matSrc.channels() == 1)
                    {
                        imgout.at<uchar>(i,j) = matSrc.at<uchar>(y,x);
                    }
                }
            }
        }
    }

    //逆时针旋转图像degree角度（原尺寸） cv版
    static void rotateImage(IplImage* img, IplImage *img_rotate,double degree)
    {
        //旋转中心为图像中心
//         CvPoint2D32f center;
//         center.x=float (img->width/2.0+0.5);
//         center.y=float (img->height/2.0+0.5);
//         //计算二维旋转的仿射变换矩阵
//         float m[6];
//         CvMat M = cvMat( 2, 3, CV_32F, m );
//         cv2DRotationMatrix( center, degree,1, &M);
//         //变换图像，并用黑色填充其余值
//         cvWarpAffine(img,img_rotate, &M,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS,cvScalarAll(0) );
    }

    //逆时针旋转图像degree角度（原尺寸）  Mat版 是角度不是弧度
    static void rotateImage(cv::Mat &src, cv::Mat &dst, double degree) {
        //旋转中心为图像中心, 两边都切
        cv::Point2f center(src.cols / 2., src.rows / 2.);// 以原图中心旋转

        //指定旋转中心 min切下部max切上部,切去多的部分
        //int len = min(src.cols, src.rows);
        //cv::Point2f center(len / 2., len / 2.);

        //获取旋转矩阵（2x3矩阵）
        cv::Mat rot_mat = cv::getRotationMatrix2D(center, degree, 1.0);

        //根据旋转矩阵进行仿射变换
        cv::Size src_sz = src.size();
        //srand(time(0));
        //int bgcolor=rand()%256;
        cv::warpAffine(src, dst, rot_mat, src_sz, INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(222));
    }

    //逆时针旋转图像degree角度（原尺寸）  Mat版 // 以原图中心旋转
    void rotateImage(double degree) {
        cv::Mat matImg2;
        rotateImage(matImg, matImg2, degree);
        matImg2.copyTo(matImg);
    }

    static cv::Mat ImgRotate(const cv::Mat ucmatImg, double angle)
	{
	    Mat ucImgRotate;

	    // a = sin(dDegree  * CV_PI / 180);
	    // b = cos(dDegree  * CV_PI / 180);
	    double a = sin(angle);
	    double b = cos(angle);
	    int width = ucmatImg.cols;
	    int height = ucmatImg.rows;
        uchar mb = ucmatImg.at<uchar>(0,0);
	    int width_rotate = int(height * fabs(a) + width * fabs(b));
	    int height_rotate = int(width * fabs(a) + height * fabs(b));

	    Point center = Point(ucmatImg.cols / 2, ucmatImg.rows / 2);

	    Mat map_matrix = getRotationMatrix2D(center, angle*180/CV_PI, 1.0);
	    map_matrix.at<double>(0, 2) += (width_rotate - width) / 2;     // 修改坐标偏移
	    map_matrix.at<double>(1, 2) += (height_rotate - height) / 2;   // 修改坐标偏移

	    warpAffine(ucmatImg, ucImgRotate, map_matrix, { width_rotate, height_rotate },
	        CV_INTER_CUBIC | CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT, cvScalarAll(mb));
//         ucImgRotate.at<uchar>(0,0) = mb;

	    return ucImgRotate;
	}

	void conv(cv::Mat &dst32F, cv::Mat &kernel, bool bDebug=false)
	{
		cv::Mat src32F;
		matImg.convertTo(src32F, CV_32F);

		filter2D(src32F, dst32F, src32F.depth(), kernel, Point(-1,-1), 0, BORDER_CONSTANT);
		
		dst32F /= kernel.rows * kernel.cols;
		cv::normalize(dst32F, dst32F, kernel.cols);

		if(bDebug)
		{
			printf("conv result:\n");
			MyMat mdst, mker(kernel);
			dst32F.convertTo(mdst.matImg, CV_8UC1, 255);
			mker.print();
			mdst.showImg(4);
		}
	}

    void save(const char* showName=NULL)
    {
        if(showName == NULL)
            cv::imwrite("MyMat.jpg", matImg);
        else
            cv::imwrite(showName, matImg);
    }
    void save(int imgNum)
    {
        char fileName[256];
        sprintf(fileName, "%d.jpg", imgNum);
        //string fileName = itoa(imgNum)+".jpg";
        cv::imwrite(fileName, matImg);
    }

	static cv::Mat read32F(const char* showName=NULL)
	{
		FILE *fin = fopen(showName,"r");
		int w, h, ch;

		if(!fin) return cv::Mat(0,0,CV_32FC1);

		fscanf(fin,"%d %d %d", &w, &h, &ch);
		cv::Mat mat32f(w,h,CV_32FC1);
		
		for (int i = 0; i < mat32f.rows; i++)
		{
			float* p = mat32f.ptr<float>(i);
			for (int j = 0; j < mat32f.cols; j++)
				fscanf(fin, "%f", &p[j]);
		}

		fclose(fin);
		return mat32f;
	}

	static void save32F(cv::Mat mat32f, const char* showName=NULL)
	{
		FILE *fout = fopen(showName,"w");
		
		fprintf(fout,"%d %d %d\n", mat32f.rows, mat32f.cols, mat32f.channels());

		for (int i = 0; i < mat32f.rows; i++)
		{
			float* p = mat32f.ptr<float>(i);
			for (int j = 0; j < mat32f.cols; j++)
				fprintf(fout,"%8.3f", p[j]);
			fprintf(fout,"\n");
		}

		fclose(fout);
	}

    // 待续
    // 画直角直线 主线在中心,奇数时
    // lineType= 0 虚线, 1 实线, 2 反色线
    //        static void drawLine(CV_IN_OUT Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int lineType=0)
    //        {
    // 		for (int i=1; i < thickness/2.0; i++)
    // 		{
    // 			cv::line(img, cvPoint(pt1.x+i,pt1.y), pt2, color);
    // 			rec.x++; rec.y++;
    // 			rec.width-=2; rec.height-=2;
    // 		}
    //        }

    // 画直角矩形 线宽在矩形内
    // 当矩形宽或高度<=2*thickness时成为一条直线
    // lineType= 0 虚线, 1 实线, 2 反色线
    static void drawRectangle(CV_IN_OUT Mat& img, Rect rec, const Scalar& color, int thickness=1/*, int lineType=0*/)
    {
        for (int i=0; i < thickness; i++)
        {
            cv::rectangle(img, rec, color);
            rec.x++; rec.y++;
            rec.width-=2; rec.height-=2;
        }
    }

    static void showImg(cv::Mat matsrc, double fScale=1, const char* showName=NULL)
    {
        if(matsrc.empty()) return;

        Mat resizeMat;
        resize(matsrc, resizeMat, cvSize(0,0),fScale,fScale,CV_INTER_NN);
        if(showName == NULL)
            cv::imshow("MyMat", resizeMat);
        else
            cv::imshow(showName, resizeMat);
    }

    void showImg(double fScale=1, const char* showName=NULL)
    {
        if(matImg.empty()) return;

        Mat resizeMat;
        resize(matImg, resizeMat, cvSize(0,0),fScale,fScale,CV_INTER_NN);
        if(showName == NULL)
            cv::imshow("MyMat", resizeMat);
        else
            cv::imshow(showName, resizeMat);
    }

	static void print32F(cv::Mat matsrc)
	{
		for (int i = 0; i < matsrc.rows; i++)
		{
			float* p = matsrc.ptr<float>(i);
			for (int j = 0; j < matsrc.cols; j++)
				printf("%8.3f", p[j]);
			printf("\n");
		}
		printf("\n");
	}

	void print()
	{
		uchar depth = matImg.type() & CV_MAT_DEPTH_MASK;
		bool bFloat = (depth==CV_32F);// || depth==CV_64F);

		if (matImg.channels() == 3)
		{
			for (int i = 0; i < matImg.rows; i++) {
				Vec3b* p = matImg.ptr<Vec3b>(i);
				for (int j = 0; j < matImg.cols; j++)
					printf("%4d%4d%4d ", p[j][0], p[j][1], p[j][2]);
				printf("\n");
			}
		}
		else if (matImg.channels() == 1)
		{
			if(bFloat)
			{
				for (int i = 0; i < matImg.rows; i++) {
					float* p = matImg.ptr<float>(i);
					for (int j = 0; j < matImg.cols; j++)
						printf("%8.3f", p[j]);
					printf("\n");
				}
			}
			else
			{
				for (int i = 0; i < matImg.rows; i++) {
					uchar* p = matImg.ptr<uchar>(i);
					for (int j = 0; j < matImg.cols; j++)
						printf("%4d", p[j]);
					printf("\n");
				}
			}
			
		}
		printf("\n");
	}

	static void showImg32F(cv::Mat matsrc, double fScale=1, int alpha=100, const char* showName=NULL)
	{
		if(matsrc.empty()) return;
		cv::Mat mattmp;
		matsrc.convertTo(mattmp, CV_8UC1, alpha); // alpha: 相乘倍数

		Mat resizeMat;
		resize(mattmp, resizeMat, cvSize(0,0),fScale,fScale,CV_INTER_NN);
		if(showName == NULL)
			cv::imshow("MyMat", resizeMat);
		else
			cv::imshow(showName, resizeMat);
	}

    struct MyParams
    {
        int x,y,w,h,x1,y1;
        cv::Mat im, mattmp;
    };

    static void on_mouse(int event,int x,int y,int flags,void *ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号
    {
        int ix, iy;
        MyParams *p = (MyParams*)ustc;
        //::SetCursor(LoadCursor(NULL,IDC_SIZEALL));	// 需要头文件

        // 移动图片
        if (event == CV_EVENT_RBUTTONDOWN)		//左键按下，读取鼠标初始坐标
        {
            p->x1 = x;
            p->y1 = y;
        }
        else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_RBUTTON))//左键按下时，鼠标移动，则图像移动
        {
            ix = p->x - (x - p->x1);			// 计算相对于鼠标按下时的偏移量,移动图片
            iy = p->y - (y - p->y1);			//  如果使用每次的小增量移动,响应会感觉慢
            goto END;
        }
        else if (event == CV_EVENT_RBUTTONUP)	//左键抬起时
        {
            p->x = ix = p->x - (x - p->x1);
            p->y = iy = p->y - (y - p->y1);
            goto END;
        }

        // 画矩形
        else if (event == CV_EVENT_LBUTTONDOWN)		//左键按下，读取鼠标初始坐标
        {
            p->x1 = x;
            p->y1 = y;
        }
        else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则图像移动
        {
            cv::Rect cr(p->x,p->y,p->w,p->h);
            p->im(cr).copyTo(p->mattmp);
            rectangle(p->mattmp,cvPoint(p->x1,p->y1),cvPoint(x,y),Scalar(0,255,0,0),1,8,0);
            imshow("MyMat",p->mattmp);
        }
        else if (event == CV_EVENT_LBUTTONUP)	//左键抬起时
        {
            rectangle(p->mattmp,cvPoint(p->x1,p->y1),cvPoint(x,y),Scalar(0,0,255,0),1,8,0);//根据初始点和结束点，将矩形画到img上
            imshow("MyMat",p->mattmp);
        }
        return;

END:
        cv::Rect cr(ix,iy,p->w,p->h);
        TORANGE_RECT_MAT(cr,p->im);
        cv::imshow("MyMat", p->im(cr));
    }

    // 可用鼠标移动 显示大图
    static void showBigImg(cv::Mat &im, int w=800, int h=800)
    {
        TORANGE_XY_MAT(w,h,im);
        static MyParams mp={0,0,w,h,0,0,im,im};
        cv::imshow("MyMat", im(cvRect(0,0,w,h)));
        setMouseCallback("MyMat", &MyMat::on_mouse, &mp);
    }

    // 可用鼠标右键移动 显示大图
    void showBigImg(int w=800, int h=800)
    {
        showBigImg(matImg,w,h);
    }

    // 空格暂停+'s'保存
    //        static void waitkey(string imgname, int waittime)
    //        {
    //                if(waitKey(waittime) == ' ')	// 暂停 或保存
    //                {
    //                        if(waitKey() == 's')
    //                                MyString::copyFile(imgname);
    //                }
    //        }

    // 4 κ
    // 画4边形 用画多边形函数
    //        void polylines4(Mat bin3ch, MyPoint<int> *pt)
    //        {
    //                Point cvrve2[4]={pt[0].cv(), pt[1].cv(), pt[2].cv(), pt[3].cv()};
    //                const Point* ptc[1] = { cvrve2 }; //折线的顶点指针数组，也就是相应各个顶点的度度座标；
    //                int npt[1] = {4}; //折线的顶点个数数组
    //                cv::polylines( bin3ch, ptc, npt, 1, 1, Scalar(250,0,0)) ; // 画多个多边形
    //        }

    // 从中间切去一条 x竖y横
    void middleCutX(cv::Mat matImg, Mat &result, int xCut)
    {
        int gap=3;
        int sw=(matImg.cols-xCut)/2;	// 剩余半边的宽度, 忽略边缘1像素
        Mat image_one = matImg(cvRect(0,0,sw,matImg.rows));
        Mat image_two = matImg(cvRect(matImg.cols-sw,0,sw,matImg.rows));
        //创建连接后存入的图像，两幅图像按左右排列，所以列数+1
        result=cv::Mat::zeros(image_one.rows,image_one.cols+image_two.cols+gap,image_one.type());
        //从1开始索引，与我们以前使用的不同，因此，参数分别为0和image_one.cols
        image_one.colRange(0,image_one.cols).copyTo(result.colRange(0,image_one.cols));
        //第二幅图像拷贝,中间的一行作为两幅图像的分割线
        image_two.colRange(0,image_two.cols).copyTo(result.colRange(image_one.cols+gap,result.cols));
        //imshow("result",result);
    }

    void middleCutY(cv::Mat matImg, Mat &result, int yCut)
    {
        int gap=3;
        int sh=(matImg.rows-yCut)/2;	// 剩余半边的宽度, 忽略边缘1像素
        Mat image_one = matImg(cvRect(0,0,matImg.cols,sh));
        Mat image_two = matImg(cvRect(0,matImg.rows-sh,matImg.cols,sh));
        //创建连接后存入的图像，两幅图像按左右排列，所以列数+1
        result=cv::Mat::zeros(image_one.rows+image_two.rows+gap,image_one.cols,image_one.type());
        //从1开始索引，与我们以前使用的不同，因此，参数分别为0和image_one.cols
        image_one.rowRange(0,image_one.rows).copyTo(result.rowRange(0,image_one.rows));
        //第二幅图像拷贝,中间的一行作为两幅图像的分割线
        image_two.rowRange(0,image_two.rows).copyTo(result.rowRange(image_one.rows+gap,result.rows));
    }

    void middleCut(int xCut, int yCut, Mat &result)
    {
        cv::Mat result0;

        if(xCut > 0)
            middleCutX(matImg, result0, xCut);
        else
            result0 = matImg;

        if(yCut > 0)
            middleCutY(result0, result, yCut);
        else
            result = result0;
    }

    void middleCutShow(int xCut, int yCut, const char* showName=NULL)
    {
        if(matImg.empty()) return;
        cv::Mat result;
        middleCut(xCut, yCut, result);
        if(showName == NULL)
            cv::imshow("mCut", result);
        else
            cv::imshow(showName, result);
    }

	// 保存图片 请先建立保存图片的目录
    static void saveImage(cv::Mat matsrc, const char* save_path){
        static int i=0;
    	char filename[88], buff[32];

        time_t t = time(NULL); //获取目前秒时间
        tm* local = localtime(&t); //转为本地时间
        strftime(buff, 32, "%m%d%H%M%S", local);
        sprintf(filename, "%s/st%s%d.jpg", save_path, buff, i++);

    	if(matsrc.empty())
        	printf("[mbh] will to save image but it is empty. %s\n", filename);
    	else
    	{
	        imwrite(filename, matsrc);
	        printf("[mbh] image saved. %s\n", filename);
	    }
    }

    // 保存向量图片 清先建立保存图片的目录
    static void saveImages(std::vector<cv::Mat> &vMatsrc, const char* save_path){
        int i=0;
        cv::Mat matsrc;
    	char filename[88], buff[32];

    	printf("[mbh]Saveing images...\n");
    	
        for(; i < vMatsrc.size(); i++)
        {
        	// cvtColor(vMatsrc[i], matsrc, CV_YUV2BGR_NV21);
	        // cv::Mat  matsrc(480, 640, CV_8UC1, (uchar*)vMatsrc[i].data);
	        cv::Mat  matsrc(128, 96, CV_8UC1, (uchar*)vMatsrc[i].data);
	        time_t t = time(NULL); //获取目前秒时间
            tm* local = localtime(&t); //转为本地时间
            strftime(buff, 32, "%m%d%H%M%S", local);
            sprintf(filename, "%s/img%05d.jpg", save_path, i);
            // printf("%s\n", filename);
            imwrite(filename, matsrc);
        }

        printf("[mbh] %d images saved.\n", i);
    }

    // 四点校正 四点映射 四点转正 从任意四边形转矩形
    // 一个四边形中的点映射到矩形dstw,dsth中 3通道
    // 要求: srcx,srcy中的坐标不能超出srcw,srch
    // int *ptx, int *pty 左上角开始 顺时针排列
    //        static int transformBy4Point(cv::Mat &matSrc, cv::Mat &matDst, double *ptx, double *pty, int bgwidth)
    //        {
    //                double L0 = MyPointD::distance(ptx[0], pty[0], ptx[1], pty[1]);// 移动到里边
    //                double L1 = MyPointD::distance(ptx[1], pty[1], ptx[2], pty[2]);
    //                double L2 = MyPointD::distance(ptx[3], pty[3], ptx[2], pty[2]);
    //                double L3 = MyPointD::distance(ptx[0], pty[0], ptx[3], pty[3]);
    //                int nw = 2*bgwidth + (L0+L2)/2;	// 新图宽高取两边的平均值
    //                int nh = 2*bgwidth + (L1+L3)/2;

    //                if(nw > 800 || nh > 800) return -1;

    //                matDst=cv::Mat::zeros(nh, nw, CV_8UC3);
    //                int ret = MyMat::transformBy4Point(matSrc.data, matSrc.cols, matSrc.rows, matDst.data, matDst.cols, matDst.rows, ptx, pty, bgwidth);
    //                return ret;
    //        }

    // 四点校正 四点映射 四点转正 从任意四边形转矩形
    // 一个四边形中的点映射到矩形dstw,dsth中		3通道
    // 要求: srcx,srcy中的坐标不能超出srcw,srch
    // int *ptx, int *pty 左上角开始 顺时针排列
    //        static int transformBy4Point(unsigned char *srcdata, int srcw, int srch, unsigned char *dstdata, int dstw, int dsth,
    //                double *ptx, double *pty, int bgwidth, uchar b0=255, uchar g0=255, uchar r0=255)
    //        {
    //                MyPointD mpsrc0(ptx[0], pty[0]);
    //                MyPointD mpsrc1(ptx[1], pty[1]);
    //                MyPointD mpsrc2(ptx[2], pty[2]);
    //                MyPointD mpsrc3(ptx[3], pty[3]);
    //                int ws = dstw*3;

    //                if(dsth==2*bgwidth || dstw==2*bgwidth) return -1;
    //                memset(dstdata, 255, dsth*ws);

    //                for (int i=0; i < dsth; i++)
    //                {
    //                        double ypercent = double(i-bgwidth)/(dsth-2*bgwidth);
    //                        MyPointD mdlt = MyPointD::getPointFromLine(mpsrc0, mpsrc3, ypercent);
    //                        MyPointD mdrt = MyPointD::getPointFromLine(mpsrc1, mpsrc2, ypercent);
    //                        for (int j=0; j < dstw; j++)
    //                        {
    //                                MyPointD crosspt;
    //                                double xpercent = double(j-bgwidth)/(dstw-2*bgwidth);
    //                                MyPointD mdup = MyPointD::getPointFromLine(mpsrc0, mpsrc1, xpercent);
    //                                MyPointD mddn = MyPointD::getPointFromLine(mpsrc3, mpsrc2, xpercent);
    //                                int res = MyLineD::crosspoint(mdlt, mdrt, mdup, mddn, crosspt);
    //                                if(res == 2)	// 有交点
    //                                {
    //                                        double pCenter[3]={b0,g0,r0};
    //                                        crosspt.getPixelValue_Interpolation(srcdata,srcw,srch,srcw*3,3,pCenter);
    //                                        dstdata[i*ws+j*3] = pCenter[0];
    //                                        dstdata[i*ws+j*3+1] = pCenter[1];
    //                                        dstdata[i*ws+j*3+2] = pCenter[2];
    //                                }
    //                        }
    //                }

    //                return 0;
    //        }

    //        // 四点转正测试 mbh 从矩形转任意四边形
    //        // int *ptx, int *pty 目标图的4点坐标 左上角开始 顺时针排列
    //        // 设两交点: p1, p2
    //        // (p1x - x0)/(p1y - y0) = (p2x - x0)/(p2y - y0)
    //        // (p1x - x0)*(p2y - y0) = (p2x - x0)*(p1y - y0)
    //        // (xa + p*(xb-xa) - x0)*(yc + p*(yd-yc) - y0) = (xc + p*(xd-xc) - x0)*(ya + p*(yb-ya) - y0)
    //        // (p*(xb-xa) + (xa-x0))*(p*(yd-yc) + (yc-y0)) = (p*(xd-xc) + (xc-x0))*(p*(yb-ya) + (ya-y0))
    //        static void transformBy4Point2(unsigned char *srcdata, int srcw, int srch, unsigned char *dstdata, int dstw, int dsth,
    //                int *ptx, int *pty, int bgwidth=0)
    //        {
    //                int		ws = dstw*3;
    //                double  xb_xa = ptx[1] - ptx[0],	yb_ya = pty[1] - pty[0];	// a,b为上直线
    //                double  xd_xc = ptx[2] - ptx[3],	yd_yc = pty[2] - pty[3];	// c,d为下直线
    //                double  xc_xa = ptx[3] - ptx[0],	yc_ya = pty[3] - pty[0];	// a,c为左直线
    //                double  xd_xb = ptx[2] - ptx[1],	yd_yb = pty[2] - pty[1];	// b,d为右直线

    //                for (int i=0; i < dsth; i++)
    //                {
    //                        for (int j=0; j < dstw; j++)
    //                        {
    //                                double		pCenter[3]={255,255,255};

    //                                // 坐标变换  从任意四边形中的坐标 变换到 矩形中的坐标
    //                                double		px1 = MyMath::mysolvefunction(xb_xa, ptx[0]-j, yd_yc, pty[3]-i,   xd_xc, ptx[3]-j, yb_ya, pty[0]-i);
    //                                double		py1 = MyMath::mysolvefunction(xc_xa, ptx[0]-j, yd_yb, pty[1]-i,   xd_xb, ptx[1]-j, yc_ya, pty[0]-i);
    //                                MyPointD	crosspt(bgwidth+px1*(srcw-2*bgwidth), bgwidth+py1*(srch-2*bgwidth));

    //                                crosspt.getPixelValue_Interpolation(srcdata, srcw, srch, srcw*3, 3, pCenter);
    //                                dstdata[i*ws+j*3] = pCenter[0];
    //                                dstdata[i*ws+j*3+1] = pCenter[1];
    //                                dstdata[i*ws+j*3+2] = pCenter[2];
    //                        }
    //                }
    //        }

    //        // 这个方法不对, 这个方法转出来是四个3角形的拼图
    //        // 四点转正测试 mbh三角法 从矩形转任意四边形
    //        // int *ptx, int *pty 左上角开始 顺时针排列
    //        static void transformBy4Point3(unsigned char *srcdata, int srcw, int srch, unsigned char *dstdata, int dstw, int dsth,
    //                int *ptx, int *pty, int bgwidth=0)
    //        {
    //                MyPointD mpsrc0(ptx[0], pty[0]);
    //                MyPointD mpsrc1(ptx[1], pty[1]);
    //                MyPointD mpsrc2(ptx[2], pty[2]);
    //                MyPointD mpsrc3(ptx[3], pty[3]);
    //                int ws = dstw*3;
    //                memset(dstdata, 255, dsth*ws);

    //                // 源图
    //                double sk1 = MyLineD::getLineK(0, 0, srcw-1, srch-1);	// 主对角线斜率
    //                double sb1 = MyLineD::getLineB(0, 0, srcw-1, srch-1);
    //                double sk2 = MyLineD::getLineK(srcw-1, 0, 0, srch-1);	// 副对角线斜率
    //                double sb2 = MyLineD::getLineB(srcw-1, 0, 0, srch-1);
    //                double trish0 = MyPointD::distanceFromLineKB(sk2, sb2, 0, 0);
    //                double trish1 = MyPointD::distanceFromLineKB(sk1, sb1, srcw-1, 0);
    //                double trish2 = MyPointD::distanceFromLineKB(sk2, sb2, srcw-1, srch-1);
    //                double trish3 = MyPointD::distanceFromLineKB(sk1, sb1, 0, srch-1);

    //                // 目标图
    //                double k1 = MyLineD::getLineK(mpsrc0, mpsrc2);	// 主对角线斜率
    //                double b1 = MyLineD::getLineB(mpsrc0, mpsrc2);
    //                double k2 = MyLineD::getLineK(mpsrc1, mpsrc3);	// 副对角线斜率
    //                double b2 = MyLineD::getLineB(mpsrc1, mpsrc3);
    //                double trih0 = mpsrc0.distanceFromLine(mpsrc1, mpsrc3);	// 左上角到副对角线的距离
    //                double trih1 = mpsrc1.distanceFromLine(mpsrc0, mpsrc2);	// 右上角到主对角线的距离
    //                double trih2 = mpsrc2.distanceFromLine(mpsrc1, mpsrc3);	// 右下角到副对角线的距离
    //                double trih3 = mpsrc3.distanceFromLine(mpsrc0, mpsrc2);	// 左下角到主对角线的距离

    //                for (int i=0; i < dsth; i++)
    //                {
    //                        for (int j=0; j < dstw; j++)
    //                        {
    //                                double		pCenter[3]={255,255,255};
    //                                MyPointD	crosspt;
    //                                double h1 = MyPointD::distanceFromLineKB(k1, b1, j, i);	// 目标图中的点到主对角线的距离
    //                                double h2 = MyPointD::distanceFromLineKB(k2, b2, j, i);	// 目标图中的点到副对角线的距离
    //                                bool isupkb1 = MyPointD::isUpLine(k1, b1, j, i);		// 在主对角线上方
    //                                bool isupkb2 = MyPointD::isUpLine(k2, b2, j, i);		// 在副对角线上方
    //                                if(isupkb1 && isupkb2)		// 在下三角内
    //                                {
    //                                        double p1 = h1/trih3;	// 目标图中的点到主对角线高的百分比
    //                                        double p2 = h2/trih2;	// 目标图中的点到副对角线高的百分比
    //                                        h1 = p1*trish3;			// 源图中的点到主对角线的距离
    //                                        h2 = p2*trish2;			// 源图中的点到副对角线的距离
    //                                        double newB1 = MyLineD::moveX(sk1, sb1, h1);
    //                                        double newB2 = MyLineD::moveX(sk2, sb2, h2);
    //                                        crosspt = MyLineKB::crosspoint(sk1, newB1, sk2, newB2);
    //                                }
    //                                else if(!isupkb1 && !isupkb2)	// 在上三角内
    //                                {
    //                                        double p1 = h1/trih1;	// 目标图中的点到主对角线高的百分比
    //                                        double p2 = h2/trih0;	// 目标图中的点到副对角线高的百分比
    //                                        h1 = p1*trish1;			// 源图中的点到主对角线的距离
    //                                        h2 = p2*trish0;			// 源图中的点到副对角线的距离
    //                                        double newB1 = MyLineD::moveX(sk1, sb1, -h1);
    //                                        double newB2 = MyLineD::moveX(sk2, sb2, -h2);
    //                                        crosspt = MyLineKB::crosspoint(sk1, newB1, sk2, newB2);
    //                                }
    //                                else if(!isupkb1 && isupkb2)		// 在右三角内
    //                                {
    //                                        double p1 = h1/trih1;	// 目标图中的点到主对角线高的百分比
    //                                        double p2 = h2/trih2;	// 目标图中的点到副对角线高的百分比
    //                                        h1 = p1*trish1;			// 源图中的点到主对角线的距离
    //                                        h2 = p2*trish2;			// 源图中的点到副对角线的距离
    //                                        double newB1 = MyLineD::moveX(sk1, sb1, -h1);
    //                                        double newB2 = MyLineD::moveX(sk2, sb2, h2);
    //                                        crosspt = MyLineKB::crosspoint(sk1, newB1, sk2, newB2);
    //                                }
    //                                else if(isupkb1 && !isupkb2)		// 在左三角内
    //                                {
    //                                        double p1 = h1/trih3;	// 目标图中的点到主对角线高的百分比
    //                                        double p2 = h2/trih0;	// 目标图中的点到副对角线高的百分比
    //                                        h1 = p1*trish3;			// 源图中的点到主对角线的距离
    //                                        h2 = p2*trish0;			// 源图中的点到副对角线的距离
    //                                        double newB1 = MyLineD::moveX(sk1, sb1, h1);
    //                                        double newB2 = MyLineD::moveX(sk2, sb2, -h2);
    //                                        crosspt = MyLineKB::crosspoint(sk1, newB1, sk2, newB2);
    //                                }
    //                                crosspt.getPixelValue_Interpolation(srcdata,srcw,srch,srcw*3,3,pCenter);
    //                                dstdata[i*ws+j*3] = pCenter[0];
    //                                dstdata[i*ws+j*3+1] = pCenter[1];
    //                                dstdata[i*ws+j*3+2] = pCenter[2];
    //                        }
    //                }

    // 		MyPointD mpd(2, 5);
    // 		double dis0 = mpd.distanceFromLine(mpsrc2, mpsrc0);
    // 		double dis1 = mpd.distanceFromLine(mpsrc1, mpsrc3);
    // 		outDouble2(dis0, dis1);
    // 		double K, B;
    // 		MyLineD::moveX(40, 60, 10, 8, 12, K, B);
    // 		outDouble2(K, B);
    // 		double x1 = 8, x2 = 30;
    // 		outDouble2(x1, K*x1+B);
    // 		outDouble2(x2, K*x2+B);

    // 		double oK=-2, oB=80;
    // 		double nB = MyLineD::moveX(oK, oB, 12);
    // 		double x1 = 5, x2 = 30;
    // 		outDouble2(x1, oK*x1+oB);
    // 		outDouble2(x2, oK*x2+oB);
    // 		double x3 = 6, x4 = 18;
    // 		outDouble2(x3, oK*x3+nB);
    // 		outDouble2(x4, oK*x4+nB);
    //        }

#ifdef _AFXDLL
    /* MAT图像与HBITMAP互相转换
                        使用方法：
                                cv::Mat ipimg;
                                HBitmapToMat(hbmScreen, ipimg);
                                imshow("ipimg", ipimg);
                                waitKey(0); */
    static cv::Mat HBitmapToMat(HBITMAP& _hBmp)
    {
        BITMAP  bmp;
        Mat		srcImg,_mat;

        GetObject(_hBmp,sizeof(BITMAP),&bmp);
        int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel/8 ;
        int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

        srcImg.create(cvSize(bmp.bmWidth,bmp.bmHeight), CV_MAKETYPE(CV_8U,nChannels));
        GetBitmapBits(_hBmp,bmp.bmHeight*bmp.bmWidth*nChannels,srcImg.data);

        cvtColor(srcImg,_mat,CV_BGRA2BGR);	// 原来是4通道的
        return _mat;
    }

    // 这个用于粘贴板(剪切板)失败
    static BOOL MatToHBitmap(Mat& _mat,HBITMAP& _hBmp)
    {
        //MAT类的TYPE=（nChannels-1+ CV_8U）<<3
        int nChannels=(_mat.type()>>3)-CV_8U+1;
        int iSize=_mat.cols*_mat.rows*nChannels;
        _hBmp=CreateBitmap( _mat.cols,_mat.rows,1,nChannels*8,_mat.data);
        return TRUE;
    }

    // 这个用于粘贴板(剪切板)成功
    static HBITMAP MatToHBitmap32(Mat _mat)
    {
        cv::Mat bgramat;
        if(_mat.channels() == 1)
            cvtColor(_mat,bgramat,CV_GRAY2BGRA);
        else if(_mat.channels() == 3)
            cvtColor(_mat,bgramat,CV_BGR2BGRA);
        return CreateBitmap(bgramat.cols,bgramat.rows,1,32,bgramat.data);
    }

    // 将本对象图片写入粘贴板(剪切板) 3通道
    void writeClipboard()
    {
        HBITMAP hBitmap = MatToHBitmap32(matImg);
        if (OpenClipboard(NULL))
        {
            EmptyClipboard();
            SetClipboardData(CF_BITMAP, hBitmap);
            CloseClipboard();
        }
    }

    // 读取剪贴板图片,放入本对象 3通道
    void readClipboard()
    {
        ::OpenClipboard(NULL);
        HBITMAP   hBitmap   = (HBITMAP)::GetClipboardData(CF_BITMAP);
        CloseClipboard();

        if(hBitmap)
            matImg = HBitmapToMat(hBitmap);
        else
            printf("读取剪贴板失败!");
    }

    int mySnapWindow()
    {
        /*CDC dc;
                CDC *pDC = &dc;//屏幕DC

                HWND hwnd = ::GetForegroundWindow(); // 获得当前活动窗口
                HDC activeDC = ::GetWindowDC(hwnd);   //获得要截屏的窗口的hDC

                pDC->Attach(activeDC);//获取当前活动窗口的DC
                RECT rect;
                ::GetWindowRect(hwnd, &rect);//得到窗口的大小
                int Width = rect.right - rect.left;
                int Height = rect.bottom - rect.top;

                CDC memDC;//内存DC
                CBitmap memBitmap, *oldmemBitmap;//建立和屏幕兼容的bitmap
                memDC.CreateCompatibleDC(pDC);
                memBitmap.CreateCompatibleBitmap(pDC, Width, Height);
                oldmemBitmap = memDC.SelectObject(&memBitmap);//将memBitmap选入内存DC
                memDC.BitBlt(0, 0, Width, Height, pDC, 0, 0, SRCCOPY);//复制屏幕图像到内存DC
                //以下代码保存memDC中的位图到文件

                BITMAP bmp;

                memBitmap.GetBitmap(&bmp);//获得位图信息
                BITMAPINFOHEADER bih = { 0 };//位图信息头

                bih.biBitCount = bmp.bmBitsPixel;//每个像素字节大小
                bih.biCompression = BI_RGB;
                bih.biHeight = bmp.bmHeight;//高度
                bih.biPlanes = 1;
                bih.biSize = sizeof(BITMAPINFOHEADER);
                bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//图像数据大小
                bih.biWidth = bmp.bmWidth;//宽度

                byte * p = new byte[bmp.bmWidthBytes * bmp.bmHeight];//申请内存保存位图数据

                GetDIBits(memDC.m_hDC, (HBITMAP)memBitmap.m_hObject, 0, Height, p,
                        (LPBITMAPINFO)&bih, DIB_RGB_COLORS);//获取位图数据

                Mat iMat(Height, Width, CV_8UC4, p, bmp.bmWidthBytes);
                flip(iMat,iMat,0);

// 		matLast = matCur;
// 		matCur = iMat.clone();

                cv::imshow("cpoText145", iMat);
                free(p);*/
        return 0;
    }

#endif
};
