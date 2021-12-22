#pragma once

class MyLineKB
{
public:
	double k;
	double b;

	MyLineKB(void){};
	~MyLineKB(void){};

	MyLineKB(double K, double B){
		k = K;
		b = B;
	}

	MyLineKB(double x1, double y1, double x2, double y2){
		getLineKB(x1,y1,x2,y2,k,b);
	}

        MyLineKB(cv::Point _start, cv::Point _end){
            getLineKB(_start.x, _start.y, _end.x, _end.y, k, b);
        }

	void set(double K, double B){
		k = K;
		b = B;
	}

	void set(double x1, double y1, double x2, double y2){
		getLineKB(x1,y1,x2,y2,k,b);
	}

	// = += -= *= /= 对象组合操作
	const MyLineKB& operator=(const MyLineKB& ma){  // "="号的赋值操作 需要重载才可使用
		k = ma.k;
		b = ma.b;
		return *this;  
	}

	// 计算直线斜率
	static double getLineK(double x1, double y1, double x2, double y2)
	{
		if(x2 == x1) return MYINT_MAX;
		return ((double)y2 - y1) / (x2 - x1);
	}

	// 计算直线截距
	static double getLineB(double x1, double y1, double x2, double y2)
	{
		if(x1 == x2) return x1;
		if(y1 == y2) return y1;
		return ((double)x2 * y1 - x1 * y2) / (x2 - x1);
	}
	
	// 计算直线斜率和截距
	static void getLineKB(double x1, double y1, double x2, double y2, double &lineK, double &lineB)
	{
		lineK = getLineK(x1, y1, x2, y2);
		lineB = getLineB(x1, y1, x2, y2);
	}
	
	// 求过任意点(x,y)的垂线方程(dstK,dstB)
	static void getVerticalLine(double srcK, double srcB, double x, double y, double &dstK, double &dstB)
	{
		const double eps=0.0000001;

		if(srcK == MYINT_MAX)
		{
			dstK = 0;
			dstB = y;
			return;
		}

		if(-eps < srcK && srcK < eps)
		{
			dstK = MYINT_MAX;
			dstB = x;
			return;
		}

		dstK = -1/srcK;
		dstB = y - dstK*x;
	}
	
	// 求过点(x,y)的与mkb垂直的直线,修改本直线
	void getVerticalLine(MyLineKB &mkb, double x, double y)
	{
		getVerticalLine(mkb.k,mkb.b,x,y,k,b);
	}

	// 求过点(x,y)的与本直线垂直的直线,修改本直线
	void getVerticalLine(double x, double y)
	{
		getVerticalLine(k,b,x,y,k,b);
	}

	// 两线段的中点连线 线段1(x1,y1,x2,y2) 线段2(x3,y3,x4,y4)
	void getCenterLine(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
	{
		getLineKB((x1+x2)/2, (y1+y2)/2, (x3+x4)/2, (y3+y4)/2, k, b);
	}
	void getCenterLine(MyPointD mpd1, MyPointD mpd2, MyPointD mpd3, MyPointD mpd4)
	{
		getCenterLine(mpd1.x, mpd1.y, mpd2.x, mpd2.y, mpd3.x, mpd3.y, mpd4.x, mpd4.y);
	}

	// 计算直线的角度 弧度
	// K == MYINT_MAX 平行于Y轴, K == 0 平行于X轴
	static double getAngle(double K)
	{
		if(K == MYINT_MAX)
			return CV_PI/2;
		return atan(K);	// [-pi/2,+pi/2]
	}

        double getDegree()
        {
                return getAngle(k)*180/CV_PI;	// [-pi/2,+pi/2]
        }

	// 求角平分线
	void getAngularBisector(MyLineKB mkb1, MyLineKB mkb2)
	{
		MyPointD mpd = mkb1.crosspoint(mkb2);

		// 判断是否平行
		if(mpd.x==MYINT_MAX && mpd.y==MYINT_MAX)
		{
			k = mkb1.k;
			b = (mkb1.b + mkb2.b)/2;	// 若两直线为90度仍对
		}
		else	// 计算角的平均值
		{
			double pi2=CV_PI/2;
			double a1=getAngle(mkb1.k);
			double a2=getAngle(mkb2.k);
			k = tan((a1+a2)/2);
			if(!(a1>0&&a2>0 || a1<0&&a2<0 || abs(a1)+abs(a2)<pi2))// 计算夹角较小方向的平分线
				k = k==0 ? MYINT_MAX : -1/k;
			b = mpd.y - k*mpd.x;	// 通过交点计算b
		}
	}

	// 直线关于点对称 对称点为mpo
	// 等于旋转180度已测 MyLineKB::rotate(mkb[2].k,mkb[2].b,mco.x,mco.y,180);
	void symmtricLine(MyPointD mpo, MyLineKB &mkb)
	{
		if(k==MYINT_MAX)
			mkb.b = b + (mpo.x-b)*2;
		else
		{
			double y1 = k * mpo.x + b;
			mkb.b = b + (mpo.y-y1)*2;
		}

		mkb.k = k;
	}

	// 90度时未测试??
	// 直线左右翻转 随图像左右翻转 图像宽度为imgWidth
	// 直线镜像翻转 镜像中心线为x=imgWidth/2.0
	// 关于直线对称 直线的对称直线 对称中线为x=imgWidth/2.0
	static void flipH(double &K, double &B, double imgWidth)
	{
		// 如果原直线是90度的
		if(K==MYINT_MAX)
		{
			B = imgWidth-1-B;
		}
		else
		{
			double y = K*imgWidth/2.0 + B;
			K = -K;
			B = y - K*imgWidth/2.0;
		}
	}

	// 90度时未测试??
	// 顺时针翻转90度
	// 图像高度为imgHeight	//MyLineD::rotate(K[0], B[0], 0, 0, 90);经测试与旋转不同,还有加平移
	static void flip90(double &K, double &B, double imgHeight)
	{
		const double eps=0.0000001;

		// 如果原直线是90度的
		if(K==MYINT_MAX)
			K = 0;
		// 如果原直线是水平的
		else if(-eps < K && K < eps)
			K=MYINT_MAX;
		else
		{
			B = (imgHeight-1-B)/K;
			K = -1/K;
		}
	}

	// 90度时未测试??
	// 逆时针翻转90度		// 图像高度为imgHeight
	static void flip270(double &K, double &B, double imgWidth, double imgHeight)
	{
		const double eps=0.0000001;

		// 如果原直线是90度的
		if(K==MYINT_MAX)
		{
			K = 0;
			B = imgWidth - B;
		}
		// 如果原直线是水平的
		else if(-eps < K && K < eps)
		{
			K = MYINT_MAX;
			B = imgHeight - B;
		}
		else
		{
			B = imgWidth + B/K;
			K = -1/K;
		}
	}

	// 直线(K,B)绕点(ox,oy)顺时针(角度增加,左上原点)旋转degree度, 返回直线(dstK,dstB)
	// 要求degree在(-360,360)之间
	static void rotate(double &K, double &B, double ox, double oy, double degree)
	{
		double x=0, y=B, Angle = degree*CV_PI/180;
		double aa = getAngle(K);

		// 如果原直线是90度的
		if(K==MYINT_MAX)
		{
			x=B;
			y=0;
		}

		// 取直线上的一点旋转
		MyPointD::rotate(x, y, ox, oy, degree);

		// 旋转后的直线是否90度
		if(MyTools::check90Angle(aa+Angle))	// 如果转成了90度
		{
			K = MYINT_MAX;
			B = x;
		}
		else
		{
			K = tan(aa+Angle);
			B = y - K*x;
		}
	}

	void rotate(double ox, double oy, double degree)
	{
		double x=0, y=b, Angle = degree*CV_PI/180;
		double aa = getAngle(k);

		// 如果原直线是90度的
		if(k==MYINT_MAX)
		{
			x=b;
			y=0;
		}

		// 取直线上的一点旋转
		MyPointD::rotate(x, y, ox, oy, degree);

		// 旋转后的直线是否90度
		if(MyTools::check90Angle(aa+Angle))	// 如果转成了90度
		{
			k = MYINT_MAX;
			b = x;
		}
		else
		{
			k = tan(aa+Angle);
			b = y - k*x;
		}
	}

	// 功能1:直线坐标转换  功能2:直线移动
	// 返回新截距B,K不变  直线(K,B)移动,坐标增量(dx,dy)
	// 如果从子图中点的坐标转为源图坐标,要加上子图左上角在源图的坐标.
	// 反之如果从源图中点的坐标转为子图坐标,要减去子图左上角在源图的坐标.
	static double move(double K, double B, double dx, double dy)
	{
		// 如果直线是90度,只计算水平移动. 否则取直线上的一点移动,计算新截距B
		return K==MYINT_MAX ? B+dx : B+dy-K*dx;		// 这是合并之后的公式
	}

	// 计算直线向垂直方向平移距离xd后的KB参数 xd>0向上移动
	// K == MYINT_MAX 平行于Y轴, K == 0 平行于X轴
	static void moveX(double x1, double y1, double x2, double y2, double xd, double &K, double &B)
	{
		if(x1 == x2)
		{
			K = MYINT_MAX;	// 平行于Y轴	直线方程为 x=B
			B = x1 + xd;
			return;
		}

		if(y1 == y2)
		{
			K = 0;			// 平行于X轴	直线方程为 y=B
			B = x1 + xd;
			return;
		}

		K = getLineK(x1, y1, x2, y2);
		double lineB = getLineB(x1, y1, x2, y2);
		B = lineB + xd/cos(atan(K));
	}

	// 向垂直方向移动 KB方程,K不变,只返回newB
	static double moveX(double oldK, double oldB, double xd)
	{
		if(oldK == MYINT_MAX || oldK == 0)	// 平行于Y轴	直线方程为 x=B, // 平行于X轴	直线方程为 y=B
			return oldB + xd;
		return oldB + xd/cos(atan(oldK));
	}

	// 本直线向垂直方向移动距离d  d>0下移 d<0上移
	void moveX(double d)
	{
		b = moveX(k,b,d);
	}

	// 计算两直线交点
	static bool crosspoint(double k1, double b1, double k2, double b2, double &x, double &y)
	{
		if(k1 == k2)			// 两直线平行 返回点pf(MYINT_MAX, MYINT_MAX);
		{
			x=y=MYINT_MAX;
			return false;
		}
		
		if(k1 == MYINT_MAX)	// K1平行于y轴
		{
			x = b1;
			y = k2*x + b2;
		}
		else if(k2 == MYINT_MAX)	// K2平行于y轴
		{
			x = b2;
			y = k1*x + b1;
		}
		else
		{
			x = (b1-b2)/(k2-k1);
			y = k1*x + b1;
		}
		return true;
	}

	// 计算两直线交点
	static MyPointD crosspoint(double k1, double b1, double k2, double b2)
	{
		MyPointD pf;
		crosspoint(k1, b1, k2, b2, pf.x, pf.y);
		return pf;
	}

	// 计算两直线交点
	void crosspoint(MyLineKB mlk, double &x, double &y)
	{
		crosspoint(k, b, mlk.k, mlk.b, x, y);
	}

	// 计算两直线交点
	MyPointD crosspoint(MyLineKB mlk)
	{
		MyPointD pf;
		crosspoint(k, b, mlk.k, mlk.b, pf.x, pf.y);
		return pf;
	}

	// 获取多直线的交点 见MyPointArray类
	void crosspoint_MultiLines(MyLineKB *mlk, int n){}

	void print()
	{
		cout << k << " " << b << endl;
	}

	// 显示斜率为K的直线
	// 在200*200的图片中,过中心(100,100),所以截距为B=100-100*K
	// y=K*x+B,	(B=100-100*K)
	static void showLineK(double K)
	{
		int sw=100;
		int B=100-100*K;
		cv::Mat imgout = cv::Mat::zeros(2*sw,2*sw, CV_8UC3);
		imgout.setTo(255);
		if(K==MYINT_MAX)				// 与y轴平行
			cv::line(imgout, cvPoint(sw,0), cvPoint(sw,2*sw), CV_RGB(255,0,255));
		else if(K < -1 || K > 1)	// 在<-45度或>45度之间, 与上下边相交
			cv::line(imgout, cvPoint(-B/K,0), cvPoint((200-B)/K,200), CV_RGB(255,0,255));
		else //if(-1 <= K && K <= 1)	// 在-45度到45度之间, 与左右边相交
			cv::line(imgout, cvPoint(0,B), cvPoint(200,200*K+B), CV_RGB(255,0,255));
		imshow("showAngle",imgout);
	}

	// 显示直线 // y=K*x+B
	static void showLine(double K, double B)
	{
		int w=1000;
		cv::Mat imgout = cv::Mat::zeros(w, w, CV_8UC3);
		imgout.setTo(255);
		if(K==MYINT_MAX)				// 与y轴平行
			cv::line(imgout, cvPoint(B,0), cvPoint(B,w), CV_RGB(255,0,255));
		else if(K < -1 || K > 1)	// 在<-45度或>45度之间, 与上下边相交
		{
			cv::line(imgout, cvPoint(-B/K,0), cvPoint((w-B)/K,w), CV_RGB(255,0,255));
			outDouble2(-B/K, (w-B)/K);
		}
		else //if(-1 <= K && K <= 1)	// 在-45度到45度之间, 与左右边相交
			cv::line(imgout, cvPoint(0,B), cvPoint(w,w*K+B), CV_RGB(255,0,255));
		imshow("showLineKB",imgout);
	}

	// 显示直线数组 无显示名字时只绘图不显示不转彩色
	// 返回彩色绘图,以方便其他图形的绘制添加
	cv::Mat drawshow(cv::Mat &srcMat, const char* showName=NULL)
	{
		cv::Mat cm=srcMat;
		if(srcMat.channels() == 1)
			cvtColor(srcMat,cm,CV_GRAY2BGR);

		drawLine(cm, k, b);

		if(showName != NULL)
			cv::imshow(showName, cm);
		return cm;
	}

	// 绘图直线 // y=K*x+B
	static void drawLine(cv::Mat &imgout, double K, double B)
	{
		int w=imgout.cols;
		int h=imgout.rows;
		if(K==MYINT_MAX)				// 与y轴平行
			cv::line(imgout, cvPoint(round(B),0), cvPoint(round(B),h), CV_RGB(255,0,128));
		else if(K < -1 || K > 1)	// 在<-45度或>45度之间, 与上下边相交
			cv::line(imgout, cvPoint(round(-B/K),0), cvPoint(round((h-B)/K),h), CV_RGB(255,0,128));
		else //if(-1 <= K && K <= 1)	// 在-45度到45度之间, 与左右边相交
			cv::line(imgout, cvPoint(0,round(B)), cvPoint(w,round(w*K+B)), CV_RGB(255,0,128));
	}

	// 显示直线数组 无显示名字时不显示只绘图
	static cv::Mat drawLines(cv::Mat &srcMat, MyLineKB *mkb, int n, const char* showName=NULL)
	{
		cv::Mat cm=srcMat;
		if(showName != NULL && srcMat.channels() == 1)
			cvtColor(srcMat,cm,CV_GRAY2BGR);

		for (int i=0; i < n; i++)
			drawLine(cm, mkb[i].k, mkb[i].b);

		if(showName != NULL)
			cv::imshow(showName, cm);
		return cm;
	}

	// 计算点(x,y)到直线(K,B)的垂直距离
	// 点到直线的距离公式d=abs((A*x+B*x+C)/sqrt(A*A+B*B)), 公式描述：公式中的直线方程为Ax+By+C=0，点P的坐标为(x,y)。
	// 点(x0,y0)直线y=kx+b,	点到直线的距离=|kx0-y0+b|/√(k^2+1)
	static double distanceFromPoint(double K, double B, double x, double y)
	{
		if(K==MYINT_MAX)				// 与y轴平行
			return abs(B-x);
		return abs(K*x-y+B)/sqrt(K*K+1);
	}
	double distanceFromPoint(double x, double y)
	{
		if(k==MYINT_MAX)				// 与y轴平行
			return abs(b-x);
		return abs(k*x-y+b)/sqrt(k*k+1);
	}

	// 判断点(x,y)是否在直线(K,B)上方
	static bool isPointAboveLine(double K, double B, double x, double y)
	{
		if(K==MYINT_MAX)			// 与y轴平行, 点在左为下右为上(左下角坐标原点),为方便后面的旋转这样规定
			return x < B;
		return K*x+B < y;		// true 点在直线上方(左下角坐标原点)
	}

};
