#pragma once

// C++ģ�������������Ϊ��Ԫ�������� 
// http://blog.csdn.net/u010003835/article/details/47174127

//typedef struct tagPOINT
//{
//    int x;
//    int y;
//}POINT;

template<class T>
class MyPoint{
public:
	T x;
	T y;

	MyPoint(){}
	~MyPoint(){}

	MyPoint(int _x, int _y){  
		x = _x;
		y = _y;
	}

	MyPoint(double _x, double _y){  
		x = _x;
		y = _y;
	}

	MyPoint(int _x, double _y){  
		x = _x;
		y = _y;
	}

	MyPoint(double _x, int _y){  
		x = _x;
		y = _y;
	}

        MyPoint(cv::Point pt){
		x = pt.x;
		y = pt.y;
	}

	// �Ƿ���Ժϲ�Ϊһ��? MyPoint(const MyPoint<T> &pt){  ����
	// ��ʼ�� ���� (�������������������һ��,�ͱ�����ͬ���͵ĸ�ֵ,�ֿ��Ļ�����֧�ֲ�ͬ���͵ĸ�ֵ)
	MyPoint(const MyPoint<int> &pt){  
		x = pt.x;
		y = pt.y;
	}
	MyPoint(const MyPoint<double> &pt){  
		x = pt.x;
		y = pt.y;
	}

        const cv::Point cvt(){
                cv::Point pt={x,y};
		return pt;
	}

	// ���ڷ�������ǰ��&����,����������������ʱ��������
	const cv::Point cv(){
		return cvPoint(round(x), round(y));
	}

	MyPoint operator+(double d) const
	{
		MyPoint sum(x+d, y+d);
		return sum;
	}
	MyPoint operator-(double d) const
	{  
		MyPoint sum(x-d, y-d);
		return sum;
	}
	MyPoint operator*(double d) const
	{  
		MyPoint sum(x*d, y*d);
		return sum;
	}
	MyPoint operator/(double d) const
	{  
		MyPoint sum(x/d, y/d);
		return sum;
	}

	MyPoint operator+(const MyPoint &t) const
	{  
		MyPoint sum(x+t.x, y+t.y);
		return sum;
	}
	MyPoint operator-(const MyPoint &t) const
	{  
		MyPoint sum(x-t.x, y-t.y);
		return sum;
	}
// 	MyPoint operator*(const MyPoint &t) const
// 	{  
// 		MyPoint sum(x+t.x, y+t.y);
// 		return sum;
// 	}
// 	MyPoint operator/(const MyPoint &t) const
// 	{  
// 		MyPoint sum(x+t.x, y+t.y);
// 		return sum;
// 	}

	const MyPoint& operator+=(double d){
		x += d;
		y += d;
		return *this;  
	}
	const MyPoint& operator+=(const MyPoint &t){
		x += t.x;
		y += t.y;
		return *this;  
	}
	const MyPoint& operator-=(double d){
		x -= d;
		y -= d;
		return *this;  
	}
	const MyPoint& operator*=(double d){
		x *= d;
		y *= d;
		return *this;  
	}
	const MyPoint& operator/=(double d){
		x /= d;
		y /= d;
		return *this;
	}

	// ������ͬ�����ͱȽ�, ����<int>����<double>
	bool operator==(const MyPoint &right)const{  
		if (x == right.x && y == right.y)  
			return true;
		return false;  
	}

	// �жϵ��Ƿ��ڷ�Χ��,����ͼ��߽��ж� minX <= x && x < maxX &&	minY <= y && y < maxY
	bool inRange(int minX, int maxX, int minY, int maxY)
	{
		if( minX <= x && x < maxX &&
			minY <= y && y < maxY)
			return true;
		return false;
	}

	// ����������ɵ�,��Χ[0,nRange-1]
	void randm(int nRange)
	{
 		x = rand() % nRange;
 		y = rand() % nRange;
	}

	MyPoint<double> toDouble()
	{
		MyPoint<double> pf(x,y);
		return pf;
	}

	// ��������ľ���
	double distance(const MyPoint &r)
	{
		return sqrt((r.x-(double)x)*(r.x-x)+(r.y-y)*(r.y-y));
	}

	// 计算两点的距离
	double distance(double x1, double y1)
	{
		return sqrt(((double)x1-x)*(x1-x)+(y1-y)*(y1-y));
	}

	// 计算两点的距离
	static double distance(double x1, double y1, double x2, double y2)
	{
		return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	}

	// 判断点在直线的上方或下方	true: up, false: on line or down
	static bool isUpLine(double K, double B, int mx, int my)
	{
		return K == MYINT_MAX ? mx > B : my > K*mx+B;	// true: 右侧 或 上侧
	}
	bool isUpLine(double K, double B)
	{
		return K == MYINT_MAX ? x > B : y > K*x+B;	// true: 右侧 或 上侧
	}

	// 在直线AB上,求距A点为percent*len(AB)的点
	static MyPoint<T> getPointFromLine(MyPoint A, MyPoint B, T percent)
	{
		MyPoint<T> pf;
		pf.x = A.x + (B.x - A.x)*percent;
		pf.y = A.y + (B.y - A.y)*percent;
		return pf;
	}

	// ������x��y����ֱ�ƫ��dx��dy����������
	MyPoint<T> shiftPoint(T dx, T dy)
	{
		MyPoint<T> pf(x + dx, y + dy);
		return pf;
	}

	// ������ĳֱ��AB�����ƶ�����d�������:
	// ��ֱ��ΪA(x1,y1)��B(x2,y2),ĳ��C(x0,y0)�ƶ�����d�������(x,y)
	// ��������:
	//  	s=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));//�������
	//      x = x0 + (x2-x1)d/s
	// 		y = y0 + (y2-y1)d/s						// 	(֧��dΪ��ֵ)
	MyPoint<double> shiftPoint(MyPoint &A, MyPoint &B, double d)
	{
		double s = sqrt(((double)B.x-A.x)*(B.x-A.x)+(B.y-A.y)*(B.y-A.y));	// �������
		if(s==0) return *this;
		double dx = (B.x-A.x)*d/s;
		double dy = (B.y-A.y)*d/s;
		MyPoint<double> pf(x + dx, y + dy);
		return pf;
	}

	// ������ĳֱ��AB�����ƶ�����d�������:
	// �򻯰� Ϊѭ��ʹ�� ds=d/s (d �ƶ�����, s �������)
	MyPoint<double> shiftPointFast(MyPoint &A, MyPoint &B, double ds)
	{
		//double s = sqrt(((double)B.x-A.x)*(B.x-A.x)+(B.y-A.y)*(B.y-A.y));	// �������
		double dx = (B.x-A.x)*ds;
		double dy = (B.y-A.y)*ds;
		MyPoint<double> pf(x + dx, y + dy);
		return pf;
	}

	// ��ֱ��AB����Ϊr�ĵ�p, A��(������)Ϊ����, ��A��B��p��ֱ�����
	MyPoint<double> verticalLine(MyPoint &B, double r, bool bFlag=false)
	{
		double s = sqrt(((double)B.x-x)*(B.x-x)+(B.y-y)*(B.y-y));	// �������
		double dx = (B.y-y)*r/s;
		double dy = (B.x-x)*r/s;
		if(bFlag)	// ��ͼ�����꿴���ұ�
		{
			MyPoint<T> pf(x - dx, y + dy);
			return pf;
		}
		else		// ��ͼ�����꿴�����
		{
			MyPoint<T> pf(x + dx, y - dy);
			return pf;
		}
	}

	// �ӻҶ�ͼ���л�ȡ˫���Բ�ֵ�ĵ�ֵ ��ͨ��
	uchar getPixelValue(Mat &matGray) // �ǲ�ֵ
	{
		return matGray.at<uchar>(y,x);
	}

	// �ӻҶ�ͼ���л�ȡ˫���Բ�ֵ�ĵ�ֵ ��ͨ��
	double getPixelValue_Interpolation(Mat &matGray) // �޸�������ԭ��:getPixelValue
	{
		if(x < 0) x=0;
		if(y < 0) y=0;
		if(x > matGray.cols-1) x=matGray.cols-1;
		if(y > matGray.rows-1) y=matGray.rows-1;
//		if(!(0 <= x && x < matGray.cols && 0 <= y && y < matGray.rows)) return 0;

		int left = floor((double)x);	int right = ceil((double)x);
		int up = floor((double)y);		int down = ceil((double)y);

		if(matGray.channels() == 1)
		{
			uchar v00 = matGray.at<uchar>(up,left);		uchar v01 = matGray.at<uchar>(up,right);
			uchar v10 = matGray.at<uchar>(down,left);	uchar v11 = matGray.at<uchar>(down,right);
			
			double upmid   = (1.0-x+left)*v00 + (x-left)*v01;
			double downmid = (1.0-x+left)*v10 + (x-left)*v11;
			double central = (1.0-y+up)*upmid + (y-up)*downmid;
			
			return central;
		}

		if(matGray.channels() == 3)
		{
			// δʵ�� ����3ͨ��ʱ,����ֵҲҪ��	-mbh
			return 0;
		}

		return 0;
	}

	// 从图像中获取双线性插值的点值 n通道
	// 返回值 pCenter 数组长度n
	// double *dataTable : 类型可以修改
	// int dataFlag=0 : 如果表中有数据等于dataFlag,直接返回此数据 为0时不起作用
	// 使用举例:
	//			MyPointD mpd(x1,i);
	// 			double pCenter[3];
	// 			mpd.getPixelValue_Interpolation(srcdata,width,height,ws,3,pCenter);
	// 			dstdata[i*ws+j*3] = pCenter[0];
	// 			dstdata[i*ws+j*3+1] = pCenter[1];
	// 			dstdata[i*ws+j*3+2] = pCenter[2];
	double getPixelValue_Interpolation(uchar *dataTable, int width, int height, int widthstep, int nChannel, double *pCenter, int dataFlag=0)
	{
		// 		if(x < 0) {x=0; return 0;}
		// 		if(y < 0) {y=0; return 0;}
		// 		if(x > width-1)  {x=width-1; return 0;}
		// 		if(y > height-1) {y=height-1; return 0;}
		if(NOTINRANGE2(x, 0, width-1, y, 0, height-1)) return 0;

		int x0 = floor((double)x);	int x1 = ceil((double)x);
		int y0 = floor((double)y);	int y1 = ceil((double)y);

		for(int j=0; j < nChannel; j++)
		{
			double up0   = dataTable[y0*widthstep+x0*nChannel+j];
			double up1   = dataTable[y0*widthstep+x1*nChannel+j];
			double down0 = dataTable[y1*widthstep+x0*nChannel+j];
			double down1 = dataTable[y1*widthstep+x1*nChannel+j];
			if(dataFlag!=0 && (up0 == dataFlag || up1 == dataFlag || down0 == dataFlag || down1 == dataFlag))
			{
				pCenter[j] = dataFlag; continue;
			}
			double upmid   = (1.0-x+x0)*up0 + (x-x0)*up1;
			double downmid = (1.0-x+x0)*down0 + (x-x0)*down1;
			pCenter[j] = (1.0-y+y0)*upmid + (y-y0)*downmid;
		}

		return 0;	// 应该不会执行的此
	}

	// 从图像中获取双线性插值的点值 n通道
		// 返回值 pCenter 数组长度n
		double getPixelValue_Interpolation(double *dataTable, int width, int height, int widthstep, int nChannel, double *pCenter, int dataFlag=0)
		{
			if(x < 0) x=0;
			if(y < 0) y=0;
			if(x > width-1)  x=width-1;
			if(y > height-1) y=height-1;

			int x0 = floor((double)x);	int x1 = ceil((double)x);
			int y0 = floor((double)y);	int y1 = ceil((double)y);

			for(int j=0; j < nChannel; j++)
			{
				double up0   = dataTable[y0*widthstep+x0*nChannel+j];
				double up1   = dataTable[y0*widthstep+x1*nChannel+j];
				double down0 = dataTable[y1*widthstep+x0*nChannel+j];
				double down1 = dataTable[y1*widthstep+x1*nChannel+j];
				if(dataFlag!=0 && (up0 == dataFlag || up1 == dataFlag || down0 == dataFlag || down1 == dataFlag))
				{
						pCenter[j] = dataFlag; continue;
				}
				double upmid   = (1.0-x+x0)*up0 + (x-x0)*up1;
				double downmid = (1.0-x+x0)*down0 + (x-x0)*down1;
				pCenter[j] = (1.0-y+y0)*upmid + (y-y0)*downmid;
			}

		return 0;	// 应该不会执行的此
	}

	// 此函数仅支持旋转90\180\270度
	// imgw,imgh是x,y所在图像的宽高
	void rotateN90(int n90, int imgw, int imgh)
	{
		T t;
		switch(n90)
		{
		case 90:		// 顺时针旋转90度			rotx = h-1-y; roty = x; 逆运算:x=roty; y=h-1-rotx;
			t = x;
			x = imgh-1-y;
			y = t;
			break;
		case 180:		// 顺时针旋转180度
			x = imgw-1-x;
			y = imgh-1-y;
			break;
		case 270:		// 顺时针旋转270度	逆时针旋转90度
			t = x;
			x = y;
			y = imgw-1-t;
			break;
		default:
			break;
		}
	}

	// 以ox,oy为中心顺时针旋转Angle弧度 (_x,_y)作为输入和输出
	static void rotate(double& _x, double& _y, double ox, double oy, double degree)
	{
		double Angle = degree*CV_PI/180;
		double X1 = _x-ox;
		double Y1 = _y-oy;
		_x = ox + X1*cos(Angle)-Y1*sin(Angle);
		_y = oy + X1*sin(Angle)+Y1*cos(Angle);
	}
	// 以ox,oy为中心顺时针旋转Angle弧度 (屏幕左上角为原点看是顺时针,若按理论公式左下角坐标原点看为逆时针)
	void rotate(double ox, double oy, double degree)
	{
		rotate(x,y,ox,oy,degree);
	}

	// 显示点的十字线 无显示名字时只绘图不显示不转彩色
	// 返回彩色绘图,以方便其他图形的绘制添加
	cv::Mat drawshow(cv::Mat srcMat, const char* showName=NULL, int mb=128, int mg=0, int mr=255, int nlen=9, int thickness=1)
	{
		cv::Mat cm=srcMat;
		if(srcMat.channels() == 1)
			cvtColor(srcMat,cm,CV_GRAY2BGR);

		MyTools::crossLine(cm, x, y, mb, mg, mr, nlen, thickness);

		if(showName != NULL)
			cv::imshow(showName, cm);
		return cm;
	}

	// 在图像中显示点,用十字线表示
	static void show(cv::Mat &srcMat, double _x, double _y, int markLen=9, const char* showName=NULL)
	{
		cv::Mat cm=srcMat;
		if(srcMat.channels() == 1)
			cvtColor(srcMat,cm,CV_GRAY2BGR);

		MyTools::crossLine(cm, round(_x), round(_y), 255, 0, 255, markLen);

		if(showName == NULL)
			cv::imshow("MyPoint", cm);
		else
			cv::imshow(showName, cm);
	}

	// 在4点转正中, 本点为任意四边形中的点, 计算其对应的矩形中的坐标
	// 同 MyMat从矩形转任意四边形
	// int bgwidth: 矩形中四边扩展的宽度
	MyPoint<double> transformBy4Point2(double *ptx, double *pty, int dstw, int dsth, int bgwidth=0)
	{
		double  xb_xa = ptx[1] - ptx[0],	yb_ya = pty[1] - pty[0];	// a,b为上直线
		double  xd_xc = ptx[2] - ptx[3],	yd_yc = pty[2] - pty[3];	// c,d为下直线
		double  xc_xa = ptx[3] - ptx[0],	yc_ya = pty[3] - pty[0];	// a,c为左直线
		double  xd_xb = ptx[2] - ptx[1],	yd_yb = pty[2] - pty[1];	// b,d为右直线
		// 坐标变换  从任意四边形中的坐标 变换到 矩形中的坐标
		double		px1 = MyMath::mysolvefunction(xb_xa, ptx[0]-x, yd_yc, pty[3]-y,   xd_xc, ptx[3]-x, yb_ya, pty[0]-y);
		double		py1 = MyMath::mysolvefunction(xc_xa, ptx[0]-x, yd_yb, pty[1]-y,   xd_xb, ptx[1]-x, yc_ya, pty[0]-y);
		MyPoint<double> crosspt(bgwidth+px1*(dstw-2*bgwidth), bgwidth+py1*(dsth-2*bgwidth));
		return crosspt;
	}

	string toString()
	{
		stringstream s;
		s << x << "," << y;
		return s.str();
	}
};  //end class MyPoint

typedef MyPoint<int>	MyPointI;
typedef MyPoint<double> MyPointD;

template<class T>  
istream& operator>>(istream &input, MyPoint<T> &a){  
	input >> a.x >> a.y;  
	return input;  
}  

template<class T>
ostream& operator<<(ostream &output, const MyPoint<T> &a){  
	output << a.x << "," << a.y;
	return output;
}

// ���㴹���ڶ˵�A��ֱ��r�ĵ�p, ��A��B��p��ֱ�����
// template<class T>
// MyPoint<T> verticalLine(MyPoint<T> A, MyPoint<T> B, double r, bool bFlag=false)
// {
// 	double s = sqrt(((double)B.x-A.x)*(B.x-A.x)+(B.y-A.y)*(B.y-A.y));	// �������
// 	double x = (B.y-A.y)*r/s;
// 	double y = (B.x-A.x)*r/s;
// 	if(bFlag)	// ��ͼ�����꿴���ұ�
// 	{
// 		MyPoint<T> pf(A.x - x, A.y + y);
// 		return pf;
// 	}
// 	else		// ��ͼ�����꿴�����
// 	{
// 		MyPoint<T> pf(A.x + x, A.y - y);
// 		return pf;
// 	}
// }

// ����������ɵ�����,���ظ�,��Χ[0,nRange-1]
template<class T>	// bRepeatable, true ���ظ���
inline void setRandom(MyPoint<T> *mp, int nLen, int nRange, bool bRepeatable=false)
{
	for (int i=0; i < nLen; i++)
	{
		mp[i].x = rand() % nRange;
		mp[i].y = rand() % nRange;

		// ��������ɵĵ��Ƿ��ظ�
		for (int j=0; j < i && !bRepeatable; j++)
		{
			if(mp[i]==mp[j])
			{
				i--;break;
			}
		}
	}
}

// ����һ�������
template<class T>
inline MyPoint<T> myPoint( T x, T y )
{
	MyPoint<T> p;
	p.x = x;
	p.y = y;
	return p;
}

// �Ը���ĵ�ȡ��
template<class T>
inline MyPoint<T> myRound(MyPoint<T> &mp)
{
	MyPoint<T> p;
	p.x = round(mp.x);
	p.y = round(mp.y);
	return p;
}

// ��������
template<class T>
inline void swapPoint(MyPoint<T> &mp1, MyPoint<T> &mp2)
{
	MyPoint<T> p=mp1;
	mp1 = mp2;
	mp2 = p;
}

// ȡ�м��
template<class T>
inline MyPoint<T> midPoint(MyPoint<T> &mp1, MyPoint<T> &mp2)
{
	MyPoint<T> p;
	p.x = (mp1.x + mp2.x)/2;
	p.y = (mp1.y + mp2.y)/2;
	return p;
}

// �ĵ�˳��
// ԭ����һ�����߽�4���Ϊ���������֡�
// ���裺���Ȱ�x���򣬷����Ҹ�2�����ٰ�y���������
// ��������� ���� ���� ����
template<class T>
void pointArrange(vector<MyPoint<T> > &vmp)
{
	int n=vmp.size();	// n=4

	// ���Ȱ�x���� ð�����򾭵��㷨
	for (int i = 0; i < n-1; i++) {
		bool flag = true;//�趨һ����ǣ���Ϊtrue�����ʾ�˴�ѭ��û�н��н�����Ҳ���Ǵ��������Ѿ�����������Ȼ��ɡ�
		for (int j = 0; j < n-1-i; j++) {
			if (vmp[j].x > vmp[j+1].x) {
				swapPoint(vmp[j], vmp[j+1]);
				flag = false;
			}
		}
		if (flag) {
			break;
		}
	}

	// �ٰ�y���������
	if (vmp[0].y > vmp[1].y) 
		swapPoint(vmp[0], vmp[1]);
	if (vmp[2].y > vmp[3].y) 
		swapPoint(vmp[2], vmp[3]);
}

// �����ľۼ���, һ��㵽���ĵľ��뷽��
template<class T>
double pointDistVariance(MyPoint<T> *mp, int n)
{
	double vr=0;
	MyPoint<double> mu(0,0);	// ����

	// һ��������
	for (int i=0; i < n; i++)
	{
		mu.x += mp[i].x;
		mu.y += mp[i].y;
	}
	mu /= n;

	// һ��㵽���ĵľ�����
	for (int i=0; i < n; i++)
	{
		double fd = mu.distance(mp[i]);
		vr += fd*fd;
	}

	return sqrt(vr/n);
}


// �����е�
// template<class T>
// MyPoint<double> midPoint(MyPoint<T> *mp1, MyPoint<T> *mp2)
// {
// 	MyPoint<double> mid(mp1);
// 
// 	// һ��������
// 	for (int i=0; i < n; i++)
// 	{
// 		mu.x += mp[i].x;
// 		mu.y += mp[i].y;
// 	}
// 	mu /= n;
// 
// 	// һ��㵽���ĵľ�����
// 	for (int i=0; i < n; i++)
// 	{
// 		double fd = mu.distance(mp[i]);
// 		vr += fd*fd;
// 	}
// 
// 	return mid;
// }

/////////// С���� //////////////////
/*
1 ����POINT�򻯶�����ĸ�ֵ����
	POINT pt[4]={198,96, 464,127, 443,392, 166,365};

2 ǿ������ת��
	MyPoint<int> *mi=(MyPoint<int> *)pt;

*/
