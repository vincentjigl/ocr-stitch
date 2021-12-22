#pragma once

// 除构造函数外，其他函数都可直接访问父类的
// 如：imshowNoBorder
// 访问父类构造函数前需先定义对父类调用，如：
//		MyMatGray(const char* imgFile, bool flags=1):MyMat(imgFile, flags){}
class MyMatGray:public MyMat
{
public:
	MyMatGray(void){};
	~MyMatGray(void){matImg.release();};

	MyMatGray(cv::Mat matSrc){
		if(matImg.channels() == 1)
			matImg = matSrc;
			//matSrc.copyTo(matImg);
		//matImg = matSrc.clone();
		//matImg = cv::Mat::zeros(matImg.size(),CV_8UC1);
		if(matImg.channels() == 3)
			cvtColor(matSrc,matImg,CV_BGR2GRAY);
	}

	// 使用父类构造函数
	MyMatGray(char* buff, int w, int h):MyMat(buff,w,h,1){
	}

	MyMatGray(int* buff, int w, int h)
	{
		Mat iMat(h, w, CV_32SC1, buff, w*4);
		getfrom(iMat);
	}

	// 初始化图像全0
	MyMatGray(int w, int h){  
		matImg = cv::Mat::zeros(h,w,CV_8UC1);
	}

	// 初始化 子图拷贝 子图复制测试 子图可以部分在源图之外 子图坐标可以为负数
	// 只对灰度图 任意子图,甚至可以比源图大
	MyMatGray(cv::Mat matgray, cv::Rect cr)
	{
		int w = matgray.cols;
		int h = matgray.rows;
		int dstX=0,dstY=0;
		matImg = cv::Mat(cr.height,cr.width,CV_8UC1,cv::Scalar(255,255,255));	// Mat初始化设置全白

		// 子图在图像外
		if(cr.x >= matgray.cols || cr.y >= matgray.rows || cr.x+cr.width <= 0 || cr.y+cr.height <= 0) return;

		// 如果四边出界
		if(cr.x < 0)
		{
			dstX = -cr.x;
			cr.width+=cr.x;
			cr.x=0;
		}
		if(cr.y < 0)
		{
			dstY = -cr.y;
			cr.height+=cr.y;
			cr.y=0;
		}
		if(cr.x+cr.width > w)
			cr.width=w-cr.x;
		if(cr.y+cr.height > h)
			cr.height=h-cr.y;

		for (int i=0; i < cr.height; i++)
		{
			uchar* p = matgray.ptr<uchar>(cr.y+i)+cr.x;
			uchar* q = matImg.ptr<uchar>(dstY+i)+dstX;
			memcpy(q,p,cr.width);
		}
	}

 	MyMatGray(const char* imgFile){
		if(imgFile)
		{
			matImg = imread(imgFile, CV_LOAD_IMAGE_GRAYSCALE);
			if(!matImg.empty()) return;
		}
		printf("open image file failed! %s \n", imgFile);
		//exit(0);
	}

	// 与常量相加 对256取模
	void add(int dx)
	{
		for (int i=0; i < matImg.rows; i++)
		{
			uchar* p = matImg.ptr<uchar>(i);
			for (int j=0; j < matImg.cols; j++)
				p[j] += dx;
		}
	}

	// 与常量相加 对256取模
	static void add(cv::Mat srcMat, int dx)
	{
		for (int i=0; i < srcMat.rows; i++)
		{
			uchar* p = srcMat.ptr<uchar>(i);
			for (int j=0; j < srcMat.cols; j++)
				p[j] += dx;
		}
	}

	// 图像数据加权 乘数因子为对半径的函数
	// 要求 矩阵类型CV_32F
	void weightedR()
	{
		int ox=matImg.cols/2;
		int oy=matImg.rows/2;
		double maxR2=sqrt((double)matImg.cols*matImg.cols+matImg.rows*matImg.rows)/2;
		
		for (int i=0; i < matImg.rows; i++)
		{
			float* p = matImg.ptr<float>(i);
			for (int j=0; j < matImg.cols; j++)
			{
				int x=j-ox;
				int y=i-oy;
				double fr=sqrt((double)x*x+y*y);
				fr = (fr-maxR2)/maxR2;
				fr = 1 - fr*fr;
				p[j] *= fr*fr;
			}
		}
	}

	// CV_32F 转换为8位单通道图像进行显示
	static void convert8u(cv::Mat srcMat, cv::Mat &grayMat)
	{
		double maxVal;
		minMaxLoc(srcMat, NULL, &maxVal);
		srcMat.convertTo(grayMat, CV_8UC1, 255.0/maxVal, 0);
	}

	void getfrom(cv::Mat srcMat)
	{
		convert8u(srcMat, matImg);
	}

	// opencv 的Mat类也不能用函数返回的方式赋, 也报错	-mbh
	// = += -= *= /= 对象组合操作 =号赋值时没有调用不知为何？ -mbh
// 	const MyMatGray& operator=(const MyMatGray& ma){  // "="号的赋值操作 需要重载才可使用
// 		ma.matImg.copyTo(matImg);
// 		return *this;
// 	}
// 	
// 	// = += -= *= /= 对象组合操作 =号赋值时没有调用不知为何？ -mbh
// 	const MyMatGray& operator=(const cv::Mat& matSrc){  // "="号的赋值操作 需要重载才可使用
// 		matSrc.copyTo(matImg);
// 		return *this;
// 	}

	// 计算图像均值
	static double getMean(cv::Mat srcMat)
	{
		cv::Scalar m = cv::mean(srcMat);
		return m[0];
	}

	// 计算图像均值和标准差
	void getStdDev(double &meanval, double &stddevval)
	{
		Scalar mean, stddev;	//均值	//标准差
		cv::meanStdDev(matImg, mean, stddev ); //计算均值和标准差
		meanval = mean.val[0];
		stddevval = stddev.val[0]; 
	}

	// 计算小于等于th阈值的均值  =小于等于阈值的求和/小于等于阈值的个数
	// (含阈值) CV_32F CV_8UC1 均可
	static double meanbyth(cv::Mat srcMat, double th)
	{
		cv::Mat threshold5;
		cv::threshold(srcMat, threshold5, th, 255, THRESH_TOZERO_INV);
		//cv::threshold(threshold5, threshold5, 0.4, 255, THRESH_TOZERO);	// 小于等于0.4的置零
		//cv::Scalar meanVal = cv::mean( threshold5 ); 全图平均阈值,不适合
		cv::Scalar sumv = cv::sum( threshold5 );
		cv::Scalar nZero = cv::countNonZero( threshold5 );
		MyMatGray mangle;
		mangle.getfrom(threshold5);
		mangle.showImg(1, "threshold5");
		return sumv[0]/nZero[0];
	}

	// 对大于零的元素求加权平均值 矩阵类型CV_32F
	static double meanbyweight(cv::Mat srcMat, cv::Mat weightMat)
	{
		double sumf=0, sumw=0;
		for (int i=0; i < srcMat.rows; i++)
		{
			float* p = srcMat.ptr<float>(i);
			float* w = weightMat.ptr<float>(i);
			for (int j=0; j < srcMat.cols; j++)
			{
				if(p[j] > 0)
				{
					sumf += p[j]*w[j];
					sumw += w[j];
				}
			}
		}

		if(sumw == 0)
			return 0;
		return sumf/sumw;
	}

    static void grad(cv::Mat srcImg, cv::Mat &norm1)
    {
        // 求得x和y方向的一阶微分
        Mat sobelx, sobely, norm, dir;
        Sobel(srcImg, sobelx, CV_32F, 1, 0, 3);
        Sobel(srcImg, sobely, CV_32F, 0, 1, 3);

        // 求得梯度和方向
        cartToPolar(sobelx, sobely, norm, dir);
        sobelx.release();
        sobely.release();
        // cv::normalize(norm, norm1, 0, 1, cv::NORM_MINMAX);   // 归一化 梯度 范围0-1
        norm.convertTo(norm1, CV_8UC1, 1);  // 单通道，也可以使用3通道
    }

	// 求得梯度和方向 输出CV_32F格式 输出没有归一化
	// 需要外部定义: cv::Mat norm, dir;
	void gradanddir(cv::Mat &norm, cv::Mat &dir, bool bShow=false)
	{
		// 求得x和y方向的一阶微分
		Mat sobelx, sobely;
		Sobel(matImg, sobelx, CV_32F, 1, 0, 3);
		Sobel(matImg, sobely, CV_32F, 0, 1, 3);

		// 求得梯度和方向
		cartToPolar(sobelx, sobely, norm, dir);
		sobelx.release();
		sobely.release();

		if(bShow)
		{
			// If image is a Mat of type CV_32F, If the image is 32-bit floating-point,
			// imshow() multiplies the pixel values by 255 - that is, the value range [0,1]
			// is mapped to [0,255]. So your floating point image has to have a range 0 .. 1.
			// This will display a CV32F image, no matter what the range is.
			cv::normalize(norm, norm, 0, 1, cv::NORM_MINMAX);	// 归一化 梯度 范围0-1
			cv::normalize(dir, dir, 0, 1, cv::NORM_MINMAX);		// 归一化 梯度 范围0-1
			imshow("norm",norm);
			imshow("dir",dir);
		}
	}

	// 求梯度 大块面积计算梯度 待续
	void grad_bigblock(cv::Mat &imgNorm)
	{
		cv::Mat imgFlag = cv::Mat::zeros(matImg.size(),CV_8UC1);
		imgNorm = imgFlag;
	}

	// 在灰度图中找灰度最大值的位置
	uchar findMax(int &x, int &y)
	{
		int newx, newy, mx=0;
		for (int i=0; i < matImg.rows; i++)
		{
			uchar* p = matImg.ptr<uchar>(i);
			for (int j=0; j < matImg.cols; j++)
			{
				if(p[j] > mx)
				{
					mx = p[j];
					newx = j;
					newy = i;
				}
			}
		}

		// 如果找到最大值, 更新位置
		if(mx > 0)
		{
			x = newx;
			y = newy;
		}

		return mx;
	}


        // 在灰度图中找灰度最大值的位置
        // cv::Mat &imgFlag 大于0的点不查找, 仅在第一次使用时初始化,不修改像素值
        uchar findMax(int &x, int &y, cv::Mat &imgFlag)
        {
                int newx=0, newy=0, mx=0;
                if(imgFlag.empty())
                        imgFlag = cv::Mat::zeros(matImg.size(),CV_8UC1);

                for (int i=0; i < matImg.rows; i++)
                {
                        uchar* p = matImg.ptr<uchar>(i);
                        uchar* q = imgFlag.ptr<uchar>(i);
                        for (int j=0; j < matImg.cols; j++)
                        {
                                if(q[j] == 0 && p[j] > mx)	// 当q[j]!=0时找最大值
                                {
                                        mx = p[j];
                                        newx = j;
                                        newy = i;
                                }
                        }
                }

                x = newx;
                y = newy;

                return mx;	// 返回亮度最大值
        }

	// 黑色膨胀 取2*2最小值 不缩小 边缘1像素忽略
	void localMin(cv::Mat &matDst)
	{
		int w=matImg.cols;
		int h=matImg.rows;
		matDst = cv::Mat(matImg.size(),CV_8UC1);

		for (int i = 1; i < h-1; i++)
		{
			uchar* q = matDst.ptr<uchar>(i);
			uchar* p = matImg.ptr<uchar>(i);
			for (int j = 1; j < w-1; j++)
			{
				uchar b0 = p[j-w-1];		uchar b1 = p[j-w];	uchar b2 = p[j-w+1];
				uchar b3 = p[j-1];			uchar b4 = p[j];	uchar b5 = p[j+1];
				uchar b6 = p[j+w-1];		uchar b7 = p[j+w];	uchar b8 = p[j+w+1];
				b0 = min(b0,b1);	b1 = min(b2,b3);
				b2 = min(b4,b5);	b3 = min(b6,b7);
				b0 = min(b0,b1);	b1 = min(b2,b3);
				b0 = min(b0,b1);
				q[j] = min(b0,b8);
			}
		}
	}

	// 黑色膨胀 缩小一倍取2*2最小值
	void resizeLocalMin(cv::Mat &matDst)
	{
		int w=matImg.cols/2;
		int h=matImg.rows/2;
		matDst = cv::Mat(h, w, CV_8UC1);

		for (int i = 0; i < h; i++)
		{
			uchar* p = matDst.ptr<uchar>(i);
			uchar* lineUp = matImg.ptr<uchar>(i*2);
			uchar* lineDown = matImg.ptr<uchar>(i*2+1);
			for (int j = 0; j < w; j++)
			{
				uchar bUp = min(lineUp[j*2],lineUp[j*2+1]);
				uchar bDown = min(lineDown[j*2],lineDown[j*2+1]);
				p[j] = min(bUp,bDown);
			}
		}
	}

	// 显示本类的图片 如果是单通道转三通道显示
	void showImg(double fScale=1, const char* showName=NULL)
	{
		Mat resizeMat;
		if(matImg.empty()) return;

//		if(matImg.channels() == 1)
//			cvtColor(matImg, mat3ch, CV_GRAY2BGR);// 单通道转3通道
// 		else
// 			mat3ch = matImg;

		resize(matImg, resizeMat, cvSize(0,0),fScale,fScale,CV_INTER_NN);	// 最近邻缩放 保真
		if(showName == NULL)
			cv::imshow("MyMatGray", resizeMat);
		else
			cv::imshow(showName, resizeMat);
	}

	void save(char* showName=NULL)
	{
		if(showName == NULL)
			cv::imwrite("MyMatGray.jpg", matImg);
		else
			cv::imwrite(showName, matImg);
	}

	// 从一点开始颜色扩展 
	// bool bDispinMat 是否显示标记图,修改源图
	int colorExtend(int x, int y, int colorth=1, int maxcount=MYINT_MAX, bool bDispinMat=false)
	{
		int		n=0;
		uchar	b0 = matImg.at<uchar>(y, x);	// 只与最初的颜色比较
		vector<cv::Point> vPoints;
		cv::Mat imgFlag = cv::Mat::zeros(matImg.size(),CV_8UC1);

		vPoints.push_back(cvPoint(x, y));

		while(!vPoints.empty())
		{
			// 取头节点
			int i=vPoints[0].y;
			int j=vPoints[0].x;
			vPoints.erase(vPoints.begin());

			if(NOTINRANGE2(i,1,matImg.rows-2,j,1,matImg.cols-2)) continue;

			// 如果头节点已处理过continue;
			uchar	b = imgFlag.at<uchar>(i, j);
			if(b) continue;
			imgFlag.at<uchar>(i, j) = 1;

			// 此处可插入对所有节点的额外的处理过程
			// todo: proc_point();
			if(bDispinMat)
			{
				matImg.at<uchar>(i, j) = 128;	// 用于显示标记图
				//outInt4(n, i, j, vPoints.size());
			}
			
			// 最大扩展节点数限制
			if(++n >= maxcount) break;

			// 添加4方向的邻点
			if(j > 0)			// left
			{
				uchar	b = matImg.at<uchar>(i, j-1);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j-1, i));
			}
			if(j < matImg.cols)	// right
			{
				uchar	b = matImg.at<uchar>(i, j+1);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j+1, i));
			}
			if(i > 0)			// up
			{
				uchar	b = matImg.at<uchar>(i-1, j);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j, i-1));
			}
			if(i < matImg.rows)	// down
			{
				uchar	b = matImg.at<uchar>(i+1, j);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j, i+1));
			}
		}

		imgFlag.release();
		return n;
	}

        // 从一点开始颜色扩展 返回四边界值, 多点取顺时针最后一个
        // 可多次调用, 找多个点扩展
        // cv::Mat imgFlag 第一次调用时内部初始化
        int colorExtendMaxPoint(int x, int y, vector<cv::Point> &outPoints, int colorth, cv::Mat &imgFlag, int maxcount=INT_MAX)
        {
                int		n=0;
                uchar	b0 = matImg.at<uchar>(y, x);	// 只与最初的颜色比较
                vector<cv::Point> vPoints;

                if(imgFlag.empty())
                        imgFlag = cv::Mat::zeros(matImg.size(),CV_8UC1);

                // 四边极点,多点取顺时针最后一个  // 左上角开始,顺时针
// 		int x0=INT_MAX, y0=INT_MAX;	// 左边 先判断x0 取最上点
// 		int x1=0, y1=INT_MAX;		// 上边 先判断y1 取最右点
// 		int x2=0, y2=0;				// 右边 先判断x2 取最下点
// 		int x3=INT_MAX, y3=0;		// 下边 先判断y3 取最左点
                int x0=x, y0=y;	// 左边 先判断x0 取最上点
                int x1=x, y1=y;		// 上边 先判断y1 取最右点
                int x2=x, y2=y;				// 右边 先判断x2 取最下点
                int x3=x, y3=y;		// 下边 先判断y3 取最左点

                vPoints.push_back(cvPoint(x, y));

                while(!vPoints.empty())
                {
                        // 取头节点
                        int i=vPoints[0].y;
                        int j=vPoints[0].x;
                        vPoints.erase(vPoints.begin());

                        // 如果头节点已处理过continue;	// 0 未处理过的点
                        uchar	b = imgFlag.at<uchar>(i, j);
                        if(b) continue;
                        imgFlag.at<uchar>(i, j) = 1;

                        // 此处可插入对所有节点的额外的处理过程
                        // todo: proc_point();
                        if(j < x0)
                        {
                                x0=j;
                                y0=i;
                        }
                        else if(j == x0)
                        {
                                if(i < y0) y0=i;
                        }
                        else if(j > x2)
                        {
                                x2=j;
                                y2=i;
                        }
                        else if(j == x2)
                        {
                                if(i > y2) y2=i;
                        }

                        if(i < y1)
                        {
                                x1=j;
                                y1=i;
                        }
                        else if(i == y1)
                        {
                                if(j > x1) x1=j;
                        }
                        else if(i > y3)
                        {
                                x3=j;
                                y3=i;
                        }
                        else if(i == y3)
                        {
                                if(j < x3) x3=j;
                        }

                        // 最大扩展节点数限制
                        if(++n >= maxcount) break;

                        // 以下修改待测试 15:52 2019/10/31
                        // 添加4方向的邻点
                        if(j < matImg.cols-1)	// right
                        {
                                uchar	b = matImg.at<uchar>(i, j+1);
                                if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j+1, i));
                                if(j > 0)			// left
                                {
                                        uchar	b = matImg.at<uchar>(i, j-1);
                                        if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j-1, i));
                                }
                        }

                        if(i < matImg.rows-1)	// down
                        {
                                uchar	b = matImg.at<uchar>(i+1, j);
                                if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j, i+1));
                                if(i > 0)			// up
                                {
                                        uchar	b = matImg.at<uchar>(i-1, j);
                                        if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j, i-1));
                                }
                        }
                }

                //imgFlag.release();
                outPoints.push_back(cvPoint(x0,y0));	// 左上角开始,顺时针
                outPoints.push_back(cvPoint(x1,y1));
                outPoints.push_back(cvPoint(x2,y2));
                outPoints.push_back(cvPoint(x3,y3));
                return n;
        }

	// 从一点开始颜色扩展 求四边极点,多点取顺时针最后一个
	int colorExtendMaxPoint(int x, int y, vector<cv::Point> &outPoints, int colorth=1, int maxcount=MYINT_MAX)
	{
		int		n=0;
		uchar	b0 = matImg.at<uchar>(y, x);	// 只与最初的颜色比较
		vector<cv::Point> vPoints;
		cv::Mat imgFlag = cv::Mat::zeros(matImg.size(),CV_8UC1);

		// 四边极点,多点取顺时针最后一个  // 左上角开始,顺时针
		int x0=MYINT_MAX, y0=MYINT_MAX;	// 左边 先判断x0 取最上点
		int x1=0, y1=MYINT_MAX;		// 上边 先判断y1 取最右点
		int x2=0, y2=0;				// 右边 先判断x2 取最下点
		int x3=MYINT_MAX, y3=0;		// 下边 先判断y3 取最左点

		vPoints.push_back(cvPoint(x, y));

		while(!vPoints.empty())
		{
			// 取头节点
			int i=vPoints[0].y;
			int j=vPoints[0].x;
			vPoints.erase(vPoints.begin());

			// 如果头节点已处理过continue;
			uchar	b = imgFlag.at<uchar>(i, j);
			if(b) continue;
			imgFlag.at<uchar>(i, j) = 1;

			// 此处可插入对所有节点的额外的处理过程
			// todo: proc_point();
			if(j < x0)
			{
				x0=j;
				y0=i;
			}
			else if(j == x0)
			{
				if(i < y0) y0=i;
			}
			else if(j > x2)
			{
				x2=j;
				y2=i;
			}
			else if(j == x2)
			{
				if(i > y2) y2=i;
			}

			if(i < y1)
			{
				x1=j;
				y1=i;
			}
			else if(i == y1)
			{
				if(j > x1) x1=j;
			}
			else if(i > y3)
			{
				x3=j;
				y3=i;
			}
			else if(i == y3)
			{
				if(j < x3) x3=j;
			}
			
			// 最大扩展节点数限制
			if(++n >= maxcount) break;

			// 添加4方向的邻点
			if(j > 0)			// left
			{
				uchar	b = matImg.at<uchar>(i, j-1);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j-1, i));
			}
			if(j < matImg.cols)	// right
			{
				uchar	b = matImg.at<uchar>(i, j+1);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j+1, i));
			}
			if(i > 0)			// up
			{
				uchar	b = matImg.at<uchar>(i-1, j);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j, i-1));
			}
			if(i < matImg.rows)	// down
			{
				uchar	b = matImg.at<uchar>(i+1, j);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j, i+1));
			}
		}

		imgFlag.release();
		outPoints.push_back(cvPoint(x0,y0));	// 左上角开始,顺时针
		outPoints.push_back(cvPoint(x1,y1));
		outPoints.push_back(cvPoint(x2,y2));
		outPoints.push_back(cvPoint(x3,y3));
		return n;
	}

	// 从一点开始颜色扩展 返回所有点坐标
	// bool bDispinMat 是否显示标记图,修改源图
	void colorExtendPoints(int x, int y, vector<cv::Point> &outPoints, int colorth=1, int maxcount=MYINT_MAX, bool bDispinMat=false)
	{
		int		n=0;
		uchar	b0 = matImg.at<uchar>(y, x);	// 只与最初的颜色比较
		vector<cv::Point> vPoints;
		cv::Mat imgFlag = cv::Mat::zeros(matImg.size(),CV_8UC1);

		// 四边极点,多点取顺时针最后一个  // 左上角开始,顺时针
		int x0=MYINT_MAX, y0=MYINT_MAX;	// 左边 先判断x0 取最上点
		int x1=0, y1=MYINT_MAX;		// 上边 先判断y1 取最右点
		int x2=0, y2=0;				// 右边 先判断x2 取最下点
		int x3=MYINT_MAX, y3=0;		// 下边 先判断y3 取最左点

		vPoints.push_back(cvPoint(x, y));

		while(!vPoints.empty())
		{
			// 取头节点
			int i=vPoints[0].y;
			int j=vPoints[0].x;
			vPoints.erase(vPoints.begin());

			// 如果头节点已处理过continue;
			uchar	b = imgFlag.at<uchar>(i, j);
			if(b) continue;
			imgFlag.at<uchar>(i, j) = 1;

			// 此处可插入对所有节点的额外的处理过程
			// todo: proc_point();
			if(bDispinMat)
			{
				matImg.at<uchar>(i, j) = 128;	// 用于显示标记图
				outPoints.push_back(cvPoint(j,i));
			}

			// 最大扩展节点数限制
			if(++n >= maxcount) break;

			// 添加4方向的邻点
			if(j > 0)			// left
			{
				uchar	b = matImg.at<uchar>(i, j-1);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j-1, i));
			}
			if(j < matImg.cols)	// right
			{
				uchar	b = matImg.at<uchar>(i, j+1);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j+1, i));
			}
			if(i > 0)			// up
			{
				uchar	b = matImg.at<uchar>(i-1, j);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j, i-1));
			}
			if(i < matImg.rows)	// down
			{
				uchar	b = matImg.at<uchar>(i+1, j);
				if(abs(b - b0) < colorth) vPoints.push_back(cvPoint(j, i+1));
			}
		}

		imgFlag.release();
	}

	// 切去圆 圆mask 将大于半径r的部分切去 修改源图
	void circleErase(double r)
	{
		int w=matImg.cols;
		int h=matImg.rows;
		double ox=matImg.cols/2.0;
		double oy=matImg.rows/2.0;
		double r2 = r*r;

		for (int i = 0; i < h; i++)
		{
			uchar* p = matImg.ptr<uchar>(i);
			double ioy2=(i-oy)*(i-oy);
			for (int j = 0; j < w; j++)
			{
				if((j-ox)*(j-ox)+ioy2 > r2)
					p[j] = 255;
			}
		}
	}

	// 计算锯齿个数 二值化
	int sawtooth(bool bDebug=false)
	{
		int th=128;
		cv::Vec3b* q;
		cv::Mat mat3ch;
		if(bDebug)
			cvtColor(matImg, mat3ch, CV_GRAY2BGR);// 单通道转3通道

		int w=matImg.cols;
		int h=matImg.rows, n=0;

		for (int i=1; i < h-1; i++)
		{
			uchar* pup = matImg.ptr<uchar>(i-1);
			uchar* p = matImg.ptr<uchar>(i);
			uchar* pdown = matImg.ptr<uchar>(i+1);
			if(bDebug)
				q = mat3ch.ptr<cv::Vec3b>(i);
			for (int j=1; j < w-1; j++)
			{
				if(p[j]==0)		// 每个角连续3点为白对角为黑
				{
					if( pup[j]   >=th && pup[j-1]   >=th && p[j-1] >=th && pdown[j] <th && pdown[j+1] <th && p[j+1] <th ||	// 左上角右下角
						pup[j]   >=th && pup[j+1]   >=th && p[j+1] >=th && pdown[j] <th && pdown[j-1] <th && p[j-1] <th ||	// 右上角左下角
						pdown[j] >=th && pdown[j+1] >=th && p[j+1] >=th && pup[j]   <th && pup[j-1]   <th && p[j-1] <th ||	// 右下角左上角
						pdown[j] >=th && pdown[j-1] >=th && p[j-1] >=th && pup[j]   <th && pup[j+1]   <th && p[j+1] <th)	// 左下角右上角
					{
						n++;
						if(bDebug)
							q[j] = cv::Vec3b(255,0,255);
					}
				}
			}
		}

		if(bDebug)
			imshow("sawtooth", mat3ch);
		return n;
	}

        // 计算直方图中心 纵直方图最小值和横直方图最大值组成中心点
    void histCenter(cv::Point &cp)
    {
        MyArrayI maiV(matImg.rows);// 纵向直方图
        MyArrayI maiH(matImg.cols);// 横向直方图
        maiV.getImgHistV(matImg);
        maiH.getImgHistH(matImg);

        int mi = maiV.minData(cp.y);
        int ma = maiH.maxData(cp.x);
        //outInt2(mi, ma);

        //maiV /= 10;
        //maiH /= 10;
        //maiV.showCurve(0, 800);
        //maiH.showCurve(0, 800);
    }

	// 平移四角到中心，左上与右下交换位置，右上与左下交换位置 不可原地操作
	static void shiftConerCenter(cv::Mat &matsrc, cv::Mat &matdst)
	{
		int cx=matsrc.cols/2;int cy=matsrc.rows/2;
		matsrc(Rect(0,0,cx,cy)).copyTo(matdst(Rect(cx,cy,cx,cy)));
		matsrc(Rect(0,cy,cx,cy)).copyTo(matdst(Rect(cx,0,cx,cy)));
		matsrc(Rect(cx,0,cx,cy)).copyTo(matdst(Rect(0,cy,cx,cy)));
		matsrc(Rect(cx,cy,cx,cy)).copyTo(matdst(Rect(0,0,cx,cy)));
	}

	// opencv 傅立叶变换及其逆变换实例及其理解
	// https://blog.csdn.net/dang_boy/article/details/76150067
    // 使用最小最大值归一化 
	int Fourier_dft(cv::Mat &matdftG, cv::Mat &matdftP, double &minValG, double &maxValG, double &minValP, double &maxValP)
	{
		Mat padded, complexIm;
		int w=getOptimalDFTSize(matImg.cols);
		int h=getOptimalDFTSize(matImg.rows);//获取最佳尺寸，快速傅立叶变换要求尺寸为2的n次方
		copyMakeBorder(matImg,padded,0,h-matImg.rows,0,w-matImg.cols,BORDER_CONSTANT,Scalar::all(0));//填充图像保存到padded中
		Mat plane[]={Mat_<float>(padded), Mat::zeros(padded.size(),CV_32F)};//创建通道, plane[0]是原图的灰度浮点图

		// 图像归一化，额外加的mbh 20191223
		//normalize(plane[0], plane[0], 0, 1, CV_MINMAX);

		merge(plane,2,complexIm);//合并通道

		// 1. 进行傅立叶变换，结果保存在自身
		cv::dft(complexIm, complexIm);

		// 2. 分离通道
		std::vector<cv::Mat> channels_r;
		cv::split(complexIm, plane);
		channels_r.push_back(plane[0]);	// 实部
		channels_r.push_back(plane[1]);	// 虚部

		// 3. 求模
		Mat matmag, matpha;
		cartToPolar(plane[0], plane[1], matmag, matpha);	// 计算幅值 相位
		matmag += Scalar::all(1);//傅立叶变换后的图片不好分析，进行对数处理，结果比较好看
		cv::log(matmag, matmag);
		minMaxLoc(matmag, &minValG, &maxValG);
		minMaxLoc(matpha, &minValP, &maxValP);
		normalize(matmag, matmag, 0, 1, CV_MINMAX);	// 规范范围上下限, alpha和beta的取值顺序与归一化结果无关
		normalize(matpha, matpha, 0, 1, CV_MINMAX);

		// 4. 平移四角到中心，左上与右下交换位置，右上与左下交换位置
		matdftG = cv::Mat::zeros(matmag.size(), CV_32FC1);
		matdftP = cv::Mat::zeros(matmag.size(), CV_32FC1);
		shiftConerCenter(matmag, matdftG);
		shiftConerCenter(matpha, matdftP);

		return 0;
	}

	int Fourier_idft(cv::Mat matdftG, cv::Mat matdftP, double minValG, double maxValG, double minValP, double maxValP)
	{
		cv::Mat complexIm, plane0, plane1;
		cv::Mat matmag = cv::Mat::zeros(matdftG.size(), CV_32FC1);
		cv::Mat matpha = cv::Mat::zeros(matdftP.size(), CV_32FC1);

		// 4. 对幅值matdft部分修改, 修改后再合并, 查看影响效果
		std::vector<cv::Mat> channels_m;
		shiftConerCenter(matdftG, matmag);
		shiftConerCenter(matdftP, matpha);
		normalize(matmag, matmag, minValG, maxValG, CV_MINMAX);
		normalize(matpha, matpha, minValP, maxValP, CV_MINMAX);
		cv::exp(matmag, matmag);
		matmag -= Scalar::all(1);
		polarToCart(matmag, matpha, plane0, plane1);
		channels_m.push_back(plane0);	// 实部
		channels_m.push_back(plane1);	// 虚部
		merge(channels_m, complexIm);

		// 5. 傅立叶逆变换
		idft(complexIm, matImg, DFT_REAL_OUTPUT);
		normalize(matImg, matImg, 0, 1, CV_MINMAX);
		matImg.convertTo(matImg, CV_8UC1, 255);

		return 0;
	}

	// 无归一化 有归一化时幅值图会闪动
	int Fourier_dft(cv::Mat &matdftG, cv::Mat &matdftP)
	{
		Mat padded, complexIm;
		int w=getOptimalDFTSize(matImg.cols);
		int h=getOptimalDFTSize(matImg.rows);//获取最佳尺寸，快速傅立叶变换要求尺寸为2的n次方
		copyMakeBorder(matImg,padded,0,h-matImg.rows,0,w-matImg.cols,BORDER_CONSTANT,Scalar::all(0));//填充图像保存到padded中
		Mat plane[]={Mat_<float>(padded), Mat::zeros(padded.size(),CV_32F)};//创建通道, plane[0]是原图的灰度浮点图

		// 图像归一化，额外加的mbh 20191223
		//normalize(plane[0], plane[0], 0, 1, CV_MINMAX);

		merge(plane,2,complexIm);//合并通道

		// 1. 进行傅立叶变换，结果保存在自身
		cv::dft(complexIm, complexIm);

		// 2. 分离通道
		std::vector<cv::Mat> channels_r;
		cv::split(complexIm, plane);
		channels_r.push_back(plane[0]);	// 实部
		channels_r.push_back(plane[1]);	// 虚部

		// 3. 求模
		Mat matmag, matpha;
		cartToPolar(plane[0], plane[1], matmag, matpha);	// 计算幅值 相位
		matmag += Scalar::all(1);//傅立叶变换后的图片不好分析，进行对数处理，结果比较好看
		cv::log(matmag, matmag);
		if(0) // 不使用最小最大值归一化
		{
			double minValG, maxValG, minValP, maxValP;
			minMaxLoc(matmag, &minValG, &maxValG);
			minMaxLoc(matpha, &minValP, &maxValP);
			outDouble4(minValG, maxValG, minValP, maxValP);
		}	
		// normalize(matmag, matmag, 0, 1, CV_MINMAX);	// 规范范围上下限, alpha和beta的取值顺序与归一化结果无关
		// normalize(matpha, matpha, 0, 1, CV_MINMAX);

		// 4. 平移四角到中心，左上与右下交换位置，右上与左下交换位置
		matdftG = cv::Mat::zeros(matmag.size(), CV_32FC1);
		matdftP = cv::Mat::zeros(matmag.size(), CV_32FC1);
		shiftConerCenter(matmag, matdftG);
		shiftConerCenter(matpha, matdftP);

		return 0;
	}

	// 无归一化
	int Fourier_idft(cv::Mat matdftG, cv::Mat matdftP)
	{
		cv::Mat complexIm, plane0, plane1;
		cv::Mat matmag = cv::Mat::zeros(matdftG.size(), CV_32FC1);
		cv::Mat matpha = cv::Mat::zeros(matdftP.size(), CV_32FC1);

		// 4. 对幅值matdft部分修改, 修改后再合并, 查看影响效果
		std::vector<cv::Mat> channels_m;
		shiftConerCenter(matdftG, matmag);
		shiftConerCenter(matdftP, matpha);
		// normalize(matmag, matmag, minValG, maxValG, CV_MINMAX);
		// normalize(matpha, matpha, minValP, maxValP, CV_MINMAX);
		cv::exp(matmag, matmag);
		matmag -= Scalar::all(1);
		polarToCart(matmag, matpha, plane0, plane1);
		channels_m.push_back(plane0);	// 实部
		channels_m.push_back(plane1);	// 虚部
		merge(channels_m, complexIm);

		// 5. 傅立叶逆变换
		idft(complexIm, matImg, DFT_REAL_OUTPUT);
		normalize(matImg, matImg, 0, 1, CV_MINMAX);
		matImg.convertTo(matImg, CV_8UC1, 255);

		return 0;
	}

	// 计数大于阈值的点个数
	int countWhitePixel(int th)
	{
		int nWhite = 0;

		for (int i=0; i < matImg.rows; i++)
		{
			uchar* p = matImg.ptr<uchar>(i);
			for (int j=0; j < matImg.cols; j++)
			{
				if(p[j] > th)
					nWhite++;
			}
		}

		return nWhite;
	}

    // 在8邻域中，若最大值点在边且小于其外点，最大值点减d
    int maxPointPos(uchar* p, int j, int ws, int d)
    {
        // 计算最大值位置序号mi
        uchar mi=0, mv=p[j];
        int dx[9]={0, -1, 0, 1, -1, 1, -1, 0, 1};
        int dy[9]={0, -ws, -ws, -ws, 0, 0, ws, ws, ws};

        for (int i=1; i < 9; ++i)
        {
            uchar b = p[dy[i]+dx[i]];
            if(mv < b)
            {
                mv = b;
                mi = i;
            }
        }

        // 最大值在中心，不操作
        if(mi == 0)     return 0;

        // 外点偏移量2*de
        // int de = dy[mi]+dx[mi];
        // if(mv < p[de+de])
        // {
        //     // p[de] = 0;
        //     p[de] -= d;
        //     p[de+de]--;
        //     // outInt2(j, de);
        //     return 1;
        // }
        // return 0;

        // p[de]=0;
        return 1;
    }

    // 亮斑，光球，检测算法
    // ksize: 找光球的模板大小
    // 四周略去ksize/2的宽度
    int findBlob()
    {
        // int th=40;  // 亮度大于周围的阈值
        int k=2;
        cv::Mat mattmp=matImg.clone();

        for (int i=k; i < matImg.rows-k; i++)
        {
            uchar* p = matImg.ptr<uchar>(i);
            uchar* q = mattmp.ptr<uchar>(i);
            for (int j=k; j < matImg.cols-k; j++)
            {
                // 在8邻域中，若最大值点在边且小于其外点，最大值点减1
                int b = maxPointPos(p, j, matImg.step[0], 8);
                if(b==1)
                {
                    q[j] = 0;
                    outInt2(i, j);
                    cv::circle(mattmp, cvPoint(j,i), 5, cv::Scalar(0, 0, 255));
                    // imshow("matDst", matImg);
                    // cv::waitKey();
                    // exit(0);
                }
            }
        }

        imshow("mattmp", mattmp);
        // cv::waitKey();
        return 0;
    }

    // 亮斑，光球，检测算法
    // ksize: 找光球的模板大小
    // 四周略去ksize/2的宽度
    int findBlob2(int ksize=5)
    {
        int th=5;  // 亮度大于周围的阈值
        int k=1;
        cv::Mat matdst=matImg.clone();

        for (int i=k; i < matImg.rows-k; i++)
        {
            uchar* p0 = matImg.ptr<uchar>(i-k);
            uchar* p1 = matImg.ptr<uchar>(i);
            uchar* p2 = matImg.ptr<uchar>(i+k);
            uchar* q = matdst.ptr<uchar>(i);
            for (int j=k; j < matImg.cols-k; j++)
            {
                int b = p1[j] - th;
                if( b > p0[j-k] && b > p0[j] && b > p0[j+k] &&
                    b > p1[j-k] && b > p1[j+k] &&
                    b > p2[j-k] && b > p2[j] && b > p2[j+k])
                {// 中间为最大值时
                        // q[j] = 0;
                    // p1[j] = 0;
                    cv::circle(matdst, cvPoint(j,i), ksize, cv::Scalar(0, 0, 255));
                }
                else
                {// 中间为非最大值时
                    if(b > -th)
                    {
                        // q[j] *= 0.8;
                    }   
                    //     cv::circle(matdst, cvPoint(j,i), ksize, cv::Scalar(0, 0, 255));
                        // p1[j]--;
                }
            }
        }

        imshow("matDst2", matdst);
        return 0;
    }
};
