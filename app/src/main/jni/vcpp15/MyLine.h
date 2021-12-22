#pragma once

template<class T>
class MyLine
{
public:
	T x1;
	T y1;
	T x2;
	T y2;
	//MyPoint<T> start;
	//MyPoint<T> end;

	MyLine(void){};
	~MyLine(void){};

	MyLine(T x0, T y0, T mx1, T my1){  
		x1 = x0;
		y1 = y0;
		x2 = mx1;
		y2 = my1;
	}

	MyLine(MyPoint<T> _start, MyPoint<T> _end){  
		x1 = _start.x;
		y1 = _start.y;
		x2 = _end.x;
		y2 = _end.y;
	}

	MyLine(cv::Point _start, cv::Point _end){  
		x1 = _start.x;
		y1 = _start.y;
		x2 = _end.x;
		y2 = _end.y;
	}

	// 此定义可用于定义初始化的参数传递 如：MyLine<double> mld(lines[i]);
	// 此定义可用于函数参数传递，无需重载"="号操作，但参数不能使用引用(&)形式传递
	// 此定义可用于“=”号传递对象，定义时可以
	MyLine(Vec4i line){
		x1 = line[0];
		y1 = line[1];
		x2 = line[2];
		y2 = line[3];
	}

	// 此定义不能代替上面两种场合使用：初始化定义和函数参数传递
// 	const MyLine& operator=(const Vec4i& line){  // "="号的赋值操作 需要重载才可使用
// 		start.x = line[0];
// 		start.y = line[1];
// 		end.x = line[2];
// 		end.y = line[3];
// 		return *this;  
// 	}

	const MyLine& operator*=(double d){
		x1 *= d;
		y1 *= d;
		x2 *= d;
		y2 *= d;
		return *this;
	}
	const MyLine& operator/=(double d){
		x1 /= d;
		y1 /= d;
		x2 /= d;
		y2 /= d;
		return *this;
	}

	// 计算线段的中心
	void getCenter(double &ox, double &oy){
		ox = (x1 + x2)/2.0;
		oy = (y1 + y2)/2.0;
	}

	// 计算直线斜率
	static double getLineK(double mx1, double my1, double mx2, double my2)
	{
		if(mx2 == mx1) return MYINT_MAX;
		return ((double)my2 - my1) / (mx2 - mx1);
	}
	static double getLineK(MyPoint<T> start, MyPoint<T> end)
	{
		return getLineK(start.x, start.y, end.x, end.y);
	}

	// 计算直线截距
	static double getLineB(double mx1, double my1, double mx2, double my2)
	{
		if(mx2 == mx1) return MYINT_MAX;
		return ((double)mx2 * my1 - mx1 * my2) / (mx2 - mx1);
	}
	static double getLineB(MyPoint<T> start, MyPoint<T> end)
	{
		return getLineB(start.x, start.y, end.x, end.y);
	}

	// 计算直线斜率和截距
	static void getLineKB(double mx1, double my1, double mx2, double my2, double &lineK, double &lineB)
	{
		lineK = getLineK(mx1, my1, mx2, my2);
		lineB = getLineB(mx1, my1, mx2, my2);
	}
	static void getLineKB(MyPoint<T> start, MyPoint<T> end, double &lineK, double &lineB)
	{
		lineK = getLineK(start.x, start.y, end.x, end.y);
		lineB = getLineB(start.x, start.y, end.x, end.y);
	}
	void getLineKB(double &lineK, double &lineB)
	{
		getLineKB(x1, y1, x2, y2, lineK, lineB);
	}

	// 求过点(x,y)的垂线方程(dstK,dstB) 已存在在MyLineKB.h
// 	static void getVerticalLine(double srcK, double srcB, double x, double y, double &dstK, double &dstB)
// 	{
// 		const double eps=0.0000001;
// 
// 		if(srcK == MYINT_MAX)
// 		{
// 			dstK = 0;
// 			dstB = y;
// 			return;
// 		}
// 
// 		if(-eps < srcK && srcK < eps)
// 		{
// 			dstK = MYINT_MAX;
// 			dstB = x;
// 			return;
// 		}
// 
// 		dstK = -1/srcK;
// 		dstB = y - dstK*x;
// 	}

	// 计算直线的角度 弧度
	//		acos： 	Principal arc cosine of x, in the interval [0,pi] radians.
	// 		asin： 	Principal arc sine of x, in the interval [-pi/2,+pi/2] radians.
	// 		atan： 	Principal arc tangent of x, in the interval [-pi/2,+pi/2] radians.
	// 		One radian is equivalent to 180/PI degrees.
	double getAngle()	// atan得到与x轴正方向的夹角(与两点先后顺序无关),大于90度时用负值表示.
	{				// acos是0-180度的
		if(x2 == x1)
			return CV_PI/2;
		return atan((y2 - y1)/(x2 - x1));	// [-pi/2,+pi/2]
	}

	double getAngle2()	// 得到与x轴正方向的夹角(逆时针), 取值范围0~2*PI
	{
		double aa = atan2(y2-y1, x2-x1);	// [-pi,+pi]
		return aa < 0 ? 2*CV_PI+aa : aa;
	}

	double getAngle3()	// 得到与y轴正方向的夹角(顺时针), 取值范围0~2*PI
	{
		double aa = CV_PI/2 - atan2(y2-y1, x2-x1);	// 0~-90, 0~270
		return aa < 0 ? 2*CV_PI+aa : aa;
	}

	double getAngle4()	// 得到与y轴负方向的夹角(逆时针), 取值范围0~2*PI
	{
		double aa = CV_PI/2 + atan2(y2-y1, x2-x1);	// 0~-90, 0~270
		return aa < 0 ? 2*CV_PI+aa : aa;
	}

// 	double getAngleAcos()
// 	{				// acos是0-180度的
// 		if(end.x == start.x)
// 			return CV_PI/2;
// 		return acos((end.y - start.y)/(end.x - start.x));	// [-pi/2,+pi/2]
// 	}

	// 计算直线的角度 弧度
	// K == MYINT_MAX 平行于Y轴, K == 0 平行于X轴
	static double getAngle(double K)
	{
		if(K == MYINT_MAX)
			return CV_PI/2;
		return atan(K);	// [-pi/2,+pi/2]
	}

	// 两点的距离
	double length()
	{
		//return start.distance(end);
		return sqrt(((double)x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
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

	// 90度时未测试??
	// 直线左右翻转 随图像左右翻转 图像宽度为imgWidth
	// 直线镜像翻转 镜像中心线为x=imgWidth/2.0
	// 直线的对称直线 对称中线为x=imgWidth/2.0
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

	// 计算直线向垂直方向平移距离xd后的KB参数 xd>0向上移动
	// K == MYINT_MAX 平行于Y轴, K == 0 平行于X轴
	static void moveX(double mx1, double my1, double mx2, double my2, double xd, double &K, double &B)
	{
		if(mx1 == mx2)
		{
			K = MYINT_MAX;	// 平行于Y轴	直线方程为 x=B
			B = mx1 + xd;
			return;
		}

		if(my1 == my2)
		{
			K = 0;			// 平行于X轴	直线方程为 y=B
			B = mx1 + xd;
			return;
		}

		K = getLineK(mx1, my1, mx2, my2);
		double lineB = getLineB(mx1, my1, mx2, my2);
		B = lineB + xd/cos(atan(K));
	}

	// 向垂直方向移动 KB方程,K不变,只返回newB
	static double moveX(double oldK, double oldB, double xd)
	{
		if(oldK == MYINT_MAX || oldK == 0)	// 平行于Y轴	直线方程为 x=B, // 平行于X轴	直线方程为 y=B
			return oldB + xd;
		return oldB + xd/cos(atan(oldK));
	}

	// 首尾端点交换
	void swap()
	{
		T tmp = x1;
		x1 = x2;	x2 = tmp;
		tmp = y1;
		y1 = y2;	y2 = tmp;
	}

	// 计算两线段交点 bOnLine=true:交点在线段内, false在延长线上
	bool crosspoint(MyLine<T> &mlt, double &cx, double &cy, bool bOnLine=false)
	{
		double k1, b1, k2, b2;
		getLineKB(k1, b1);
		mlt.getLineKB(k2, b2);
		bool bCross = MyLineKB::crosspoint(k1, b1, k2, b2, cx, cy);
		return bCross ? (
						bOnLine ? (x1 <= cx && cx <= x2) || (x2 <= cx && cx <= x1) : true) :	// 相交时,判断是否要求交点在线内
						false;	// 两线段平行
	}

	// 求斜率
	static double slope(MyPointD my1,MyPointD my2, bool &flag)
	{
		if(my1.x-my2.x == 0)
		{
			flag=true;
			return 0;
		}
		return (my1.y-my2.y)/(my1.x-my2.x);
	}

	// 求直线(my1,my2)和直线(y3,y4)交点 代码还太冗余需要修改
	// 返回: 0 同线 1 平行无交点 2 有交点
	// 求两直线交点问题C++求解
	// http://blog.sina.com.cn/s/blog_9e6617e80102v5wr.html
	static int crosspoint(MyPointD my1,MyPointD my2,MyPointD y3,MyPointD y4, MyPointD &crosspt)
	{
		bool isVertical1=false;
		bool isVertical2=false;
		double k1=slope(my1, my2, isVertical1);// 计算斜率K
		double k2=slope(y3, y4, isVertical2);// 计算斜率K
		double b1=my1.y-k1*my1.x;// 计算B
		double b2=y3.y-k2*y3.x;// 计算B

		// 两个都与y轴平行
		if(isVertical1 && isVertical2)
			return my1.x==y3.x ? 0 : 1;	// 0 同线 1 平行无交点 2 有交点

		// 直线my1,my2与y轴平行
		if(isVertical1)
		{
			crosspt.x = my1.x;
			crosspt.y = my1.x*k2+b2;
			return 2;
		}

		// 直线y3,y4与y轴平行
		if(isVertical2)
		{
			crosspt.x = y3.x;
			crosspt.y = y3.x*k1+b1;
			return 2;
		}

		// 两直线斜平行
		if(k1==k2)
			return y3.y==k1*y3.x+b1 ? 0 : 1;

		// 都不与y轴平行 也不斜平行
		crosspt.x = (b2-b1)/(k1-k2);
		crosspt.y = k1*crosspt.x+b1;
		return 2;		// 0 同线 1 平行无交点 2 有交点
	}

	// 两线段的中点连线
	void getCenterLine(MyLine<double> mld)
	{
		x1 = (x1 + x2)/2;
		y1 = (y1 + y2)/2;
		x2 = (mld.x1 + mld.x2)/2;
		y2 = (mld.y1 + mld.y2)/2;
	}

	// 计算两条线段的中间线段 接近的两端的中点
	void midLine(MyLine<double> mld)
	{
		MyPointD start(x1,y1);
		MyPointD end(x2,y2);

		// 先将两直线头尾对正
		// 方法：
		// 1 分别计算首尾两端点的距离，取大=a
		// 2 首尾反向计算两端点的距离，取大=b
		// 3 a<b 为正常，反之一条直线头尾换向
		double s1 = start.distance(mld.x1, mld.y1);
		double e1 = end.distance(mld.x2, mld.y2);
		double s2 = start.distance(mld.x2, mld.y2);
		double e2 = end.distance(mld.x1, mld.y1);
		s1 = max(s1, e1);
		s2 = max(s2, e2);
		if(s2 < s1)	swap();

		x1 = (start.x + mld.x1)/2;
		y1 = (start.y + mld.y1)/2;
		x2 = (end.x + mld.x2)/2;
		y2 = (end.y + mld.y2)/2;
	}

	// 获取直线的包围矩形框
	Rect boundRect()
	{
		Rect rc;
		rc.x = min(x1, x2);
		rc.y = min(y1, y2);
		rc.width = abs(x2 - x1)+1;
		rc.height = abs(y2 - y1)+1;
		return rc;
	}

	// 获取直线的包围矩形框
	static Rect boundRect(vector<Vec4i> &vl)
	{
		int x0=vl[0][0],y0=vl[0][1];
		int mx1=vl[0][2],my1=vl[0][3];

		for (int i=1; i < vl.size(); i++)
		{
			Vec4i v = vl[i];
			if(v[0] < x0) x0 = v[0];
			else if(v[0] > mx1) mx1 = v[0];
			if(v[2] < x0) x0 = v[2];
			else if(v[2] > mx1) mx1 = v[2];

			if(v[1] < y0) y0 = v[1];
			else if(v[1] > my1) my1 = v[1];
			if(v[3] < y0) y0 = v[3];
			else if(v[3] > my1) my1 = v[3];
		}

		Rect rc(x0,y0,mx1-x0,my1-y0);
		return rc;
	}

	string toString()
	{
		stringstream s;
		s << x1 << "," << y1 << " " << x2 << "," << y2;
		return s.str();
	}

	void print()
	{
		cout << x1 << "," << y1 << " " << x2 << "," << y2 << endl;
	}

	static void print(vector<Vec4i> &vl)
	{
		for (int i=0; i < vl.size(); i++)
		{
			cout << vl[i][0] << ", " << vl[i][1] << ", " << vl[i][2] << ", " << vl[i][3] << endl;
		}
	}

	// 每次新的图像, 不叠加
        cv::Mat drawshow(cv::Mat srcMat, const char* showName=NULL)
	{
		cv::Mat cm=srcMat;
		if(srcMat.channels() == 1)
			cvtColor(srcMat,cm,CV_GRAY2BGR);
                cv::line(cm, cvPoint(x1,y1), cvPoint(x2,y2), Scalar(255,0,255));
                if(showName != NULL)
                    cv::imshow(showName, cm);
                return cm;
	}

	static void showLines(cv::Mat srcMat, vector<Vec4i> &vl)
	{
		cv::Mat cm=srcMat;
		if(srcMat.channels() == 1)
			cvtColor(srcMat,cm,CV_GRAY2BGR);
		
		for (int i=0; i < vl.size(); i++)
		{
			cv::line(cm, cvPoint(vl[i][0],vl[i][1]), cvPoint(vl[i][2],vl[i][3]), Scalar(255,0,255), 1, CV_AA);
		}
		cv::imshow("showLines", cm);
	}

	static void showLines(cv::Mat srcMat, vector<MyLine<double> > &vm)
	{
		cv::Mat cm=srcMat;
		if(srcMat.channels() == 1)
			cvtColor(srcMat,cm,CV_GRAY2BGR);

		for (int i=0; i < vm.size(); i++)
		{
			cv::line(cm, cvPoint(vm[i].x1,vm[i].y1), cvPoint(vm[i].x2,vm[i].y2), Scalar(255,0,255), 1, CV_AA);
// 			cv::imshow("MyLine", cm);
// 			cv::waitKey();
		}
		cv::imshow("MyLine", cm);
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

	// 显示直线 // y=K*x+B
	static void showLine(cv::Mat imgout, double K, double B)
	{
		int w=imgout.cols;
		int h=imgout.rows;

		if(K==MYINT_MAX)				// 与y轴平行
			cv::line(imgout, cvPoint(B,0), cvPoint(B,w), CV_RGB(255,0,255));
		else if(K < -1 || K > 1)	// 在<-45度或>45度之间, 与上下边相交
		{
			cv::line(imgout, cvPoint(-B/K,0), cvPoint((w-B)/K,w), CV_RGB(255,0,255));
			//outDouble2(-B/K, (w-B)/K);
		}
		else //if(-1 <= K && K <= 1)	// 在-45度到45度之间, 与左右边相交
			cv::line(imgout, cvPoint(0,B), cvPoint(w,w*K+B), CV_RGB(255,0,255));
		imshow("showLineKB",imgout);
	}

	// my画直线(精确) 控制折点 找到整数点 测试中未完成 
	static void drawLine2(cv::Mat &imgout, double mx1, double my1, double mx2, double my2)
	{
		double K, B;
                MyLine<double>::getLineKB(mx1, my1, mx2, my2, K, B);
		outDouble2(K, B);
		double mindx=1, mx, my;
		int y=my1;
		for (int i=0; i < 1000; i++)
		{
			double tx = (y-i-B)/K;
			double dx = tx - (int)tx;
			if(dx < mindx)
			{
				mindx = dx;
				mx = tx;
				my = y-i;
			}
			else
				break;
		}

		cv::line(imgout, cvPoint2D32f(mx,my), cvPoint2D32f(mx2,my2), CV_RGB(255,0,128));
	}

	// my逐点画直线(精确) 3通道
	static void myDrawLine(cv::Mat &imgout, double mx1, double my1, double mx2, double my2)
	{
		double K, B;
                MyLine<double>::getLineKB(mx1, my1, mx2, my2, K, B);
		outDouble2(K, B);

		// 横向较长
		if(abs(mx2-mx1) > abs(my2-my1))
		{
			for (int j=round(mx1); j < round(mx2); j++)
			{
				int i = round(j*K+B);
				imgout.at<Vec3b>(i,j)=Vec3b(255,0,255);		// imgout.channels()
			}
			return;
		}

		// 竖直情况
		if(mx2==mx1 || K==MYINT_MAX)
		{
			for(int i=round(my1); i < round(my2); i++)
				imgout.at<Vec3b>(i,round(mx1))=Vec3b(255,0,255);
			return;
		}
		
		// 纵向较长
		for(int i=round(my1); i < round(my2); i++)	// if(abs(my2-my1) >= abs(mx2-mx1))
		{
			int j = round((i-B)/K);
			imgout.at<Vec3b>(i,j)=Vec3b(255,0,255);
		}
	}

	// 根据端点距离合并直线, 合并到本类
	static void mergeSameLines(vector<MyLine<double> > &vmld, vector<Vec4i> lines, double th)
	{
		const int nLenMax = 64;	// 最多合并直线数
		int nLen = min(nLenMax, (int)lines.size());	// nLen为实际计算的直线个数
		std::vector<int> mLineGroup[nLenMax];	// 向量序号对应 直线序号
                MyLine<double> mld[nLenMax];

		for (int i=0; i < nLen; i++)
		{
			mld[i] = lines[i];
			mLineGroup[i].push_back(i);
		}

		// 对直线按距离阈值分组
		// 思路：从lines后面开始，第i个直线对前面的所有直线计算距离，放到最小直线序号的mLineGroup中.
		// 将剩下的直线直接放到对应序号的mLineGroup中
		// 对每个mLineGroup组合并
		for (int i=nLen-1; i >= 0; i--)
		{
			for (int j=0; j < i; j++)
			{
				if(mld[i].isSameLine(mld[j], th))	// 直线i如果是j的同线，则将i合并到j组中
				{
					mLineGroup[j].insert(mLineGroup[j].end(), mLineGroup[i].begin(), mLineGroup[i].end());
					mLineGroup[i].clear();
				}
			}
		}

		// 对组内直线进行合并	// 暂停待续 -mbh
		// 思路：
		//  1 若两直线长度差大于阈值时, 抛弃短线
		//	2 余下的直线, 求端点质心
		// 简化改为找一个距离最长的 求每组的最长直线
		int lineNum=0;
		for (int i=0; i < nLen; i++)
		{
			if(mLineGroup[i].size() > 0)
			{
				int mk=i;
				double maxLen=0;
				for (int j=0; j < mLineGroup[i].size(); j++)
				{
					int k = mLineGroup[i][j];
					double ml=mld[k].length();
					if(ml > maxLen)
					{
						mk=k;
						maxLen=ml;
					}
				}
				vmld.push_back(mld[mk]);	// 保存每组内的最长直线
			}
		}
	}

	// 检测两条直线是否是同一条线
	// 判断条件：
	//	1	若一条直线的两端点距另一条垂直距离小于th1
	//  2   且两直线最近处距离小于th2
	bool isSameLine(MyLine<double> mld, double th)
	{
		if((distanceFromPoint(mld.x1,mld.y1) < th && distanceFromPoint(mld.x2,mld.y2) < th ||
			mld.distanceFromPoint(x1,y1) < th && mld.distanceFromPoint(x2,y2) < th)
			&& minDistance(mld) < th)
			return true;
		return false;
	}

	// 计算点到直线的垂直距离 VerticalDistance(MyPointD mpd)
	double distanceFromPoint(double x, double y)
	{
		double A = y2 - y1;	// 包含与坐标轴平行的情况
		double B = x1 - x2;
		double C = x2 * y1 - x1 * y2;
		C = abs(A*x + B*y + C)/sqrt(A*A+B*B);
		return C;
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

	// 判断点(x,y)是否在直线(K,B)上方
	static bool isPointAboveLine(double K, double B, double x, double y)
	{
		if(K==MYINT_MAX)			// 与y轴平行, 点在左为下右为上(左下角坐标原点),为方便后面的旋转这样规定
			return x < B;
		return K*x+B < y;		// true 点在直线上方(左下角坐标原点)
	}

	// 计算两直线最近处的距离
	double minDistance(MyLine<double> mld)
	{
		// 未实现
		return 0;
	}
};

typedef MyLine<int>		MyLineI;
typedef MyLine<double>	MyLineD;

template<class T>
ostream& operator<<(ostream &output, const MyLine<T> &line){  
	output << line.x1 << "," << line.y1 << " " << line.x2 << "," << line.y2;
	return output;
}

// 构建一个直线对象
template<class T>
inline MyLine<T> myLine( MyPoint<T> mpt0, MyPoint<T> mpt1 )
{
	MyLine<T> mp;
	mp.x1 = mpt0.x;
	mp.y1 = mpt0.y;
	mp.x2 = mpt1.x;
	mp.y2 = mpt1.y;
	return mp;
}
