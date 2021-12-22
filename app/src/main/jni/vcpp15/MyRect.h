#pragma once

class YRect:public cv::Rect
{
public:
	YRect(){};
	~YRect(){};

	// 变量初始化等号右赋值
    YRect(CvRect rc):cv::Rect(rc){};
    YRect(cv::Rect rc):cv::Rect(rc){};

    YRect(int _x, int _y, int _w, int _h):cv::Rect(_x, _y, _w, _h){};

	const YRect& operator=(cv::Rect rc){
		x = rc.x;
		y = rc.y;
		width  = rc.width;
		height = rc.height;
		return *this;
	};
	
	YRect operator/(int d) const
	{  
		YRect sum(x/d, y/d, width/d, height/d);
		return sum;
	};

	YRect operator/=(int d)
	{
		x /= d;
		y /= d;
		width  /= d;
		height /= d;
		return *this;
	};
};

// 这个类主要使用它的静态函数,因为使用cv::Rect比较方便
template<class T>
class MyRect
{
public:
    T x;
    T y;
    T w;
    T h;

    MyRect(void){};
    ~MyRect(void){};

    MyRect(T _x, T _y, T _w, T _h){
        x = _x;
        y = _y;
        w = _w;
        h = _h;
    }

    // 	MyRect(MyPoint<T> _start, MyPoint<T> _end){
    // 		start = _start;
    // 		end = _end;
    // 	}
    //
    // 	MyLine(POINT _start, POINT _end){
    // 		start = _start;
    // 		end = _end;
    // 	}
    //
    // 	MyLine(cv::Point _start, cv::Point _end){
    // 		start = _start;
    // 		end = _end;
    // 	}

	// 此定义可用于定义初始化的参数传递 如：MyLine<double> mld(lines[i]);
	// 此定义可用于函数参数传递，无需重载"="号操作，但参数不能使用引用(&)形式传递
	// 此定义可用于“=”号传递对象，定义时可以
	MyRect(cv::Rect rc){
		x = rc.x;
		y = rc.y;
		w = rc.width;
		h = rc.height;
	}

    // �˶��岻�ܴ����������ֳ���ʹ�ã���ʼ������ͺ�����������
    // 	const MyLine& operator=(const Vec4i& line){  // "="�ŵĸ�ֵ���� ��Ҫ���زſ�ʹ��
    // 		start.x = line[0];
    // 		start.y = line[1];
    // 		end.x = line[2];
    // 		end.y = line[3];
    // 		return *this;
    // 	}
	// 获取点组的外接矩形 最小正矩形
	// boundingRect()得到包覆此轮廓的最小正矩形，minAreaRect()得到包覆轮廓的最小斜矩形，minEnclosingCircle()得到包覆此轮廓的最小圆形
	MyRect(T *ptx, T *pty, int n){  
		std::vector<cv::Point> vpt;
		for (int i=0; i < n; i++)
			vpt.push_back(cvPoint(ptx[i],pty[i]));
		cv::Rect rc = cv::boundingRect(vpt);
		x = rc.x;
		y = rc.y;
		w = rc.width;
		h = rc.height;
	}

    const MyRect& operator*=(double d){
        x *= d;
        y *= d;
        w *= d;
        h *= d;
        return *this;
    }
    const MyRect& operator/=(double d){
        x /= d;
        y /= d;
        w /= d;
        h /= d;
        return *this;
    }

    const Rect cvt(){
        Rect pt(x,y,w,h);
        return pt;
    }

	void set(T _x, T _y, T _w, T _h)
	{
		x = _x;
		y = _y;
		w = _w;
		h = _h;
	}

	void setCenter(T ox, T oy)
	{
		x = ox - w/2;
		y = oy - h/2;
	}

	// 检测本矩形框是否在宽高范围内
	bool checkInRange(int width, int height)
	{
		if( 0 < x && x+w-1 < width &&
			0 < y && y+h-1 < height)
			return true;
		return false;
	}
	bool checkInRange(cv::Size cs)
	{
		return checkInRange(cs.width,cs.height);
	}

	// 修改本矩形框到适合在宽高范围内 宽高改变
    void fitInRange(int width, int height)
    {
        if(x < 0) x=0;
        if(y < 0) y=0;
        if(x >= width) x = width-1;
        if(y >= height) y = height-1;
        if(w <= 0) w=1;
        if(h <= 0) h=1;
        if(x+w > width)  w = width-x;
        if(y+h > height) h = height-y;
    }
    
    // 移动本矩形框到适合在宽高范围内 宽高不变(但当大于图像宽高时宽高改变)
    void moveInRange(int width, int height)
    {
        if(w > width)  w=width;
        if(h > height) h=height;
        if(x < 0) x=0;
        if(y < 0) y=0;
        if(x >= width) x = width-1;
        if(y >= height) y = height-1;
        if(w <= 0) w=1;
        if(h <= 0) h=1;
        if(x+w > width)  x = width-w;
        if(y+h > height) y = height-h;
    }
    
    void fitInRange(cv::Size cs)
    {
        fitInRange(cs.width,cs.height);
    }

	// 本矩形向x,y方向移动offX,offY距离
	void shift(int offX, int offY)
	{
		x += offX;
		y += offY;
	}

	// 矩形相对于矩形rc的左上角坐标移动,rc的宽高不起作用
    void shift(Rect rc)
    {
        x += rc.x;
        y += rc.y;
    }
    void shift(MyRect<int> rc)
    {
        x += rc.x;
        y += rc.y;
    }

    void extend(int ex)
    {
        x -= ex;
        y -= ex;
        w += ex + ex;
        h += ex + ex;
    }

	// 四边裁切 >0外扩,<0缩小
    void extend(int left, int right, int top=0, int bottom=0)
    {
        x -= left;
        y -= top;
        w += left + right;
        h += top + bottom;
    }

	// 以某点为中心,取一个正方形  aLen=边长
	// bCenter=false,如果出界移动中心,大小不变,bCenter=true中心不变大小改变
	void getSquare(double ox, double oy, int aLen, int imgWidth=0, int imgHeight=0, bool bCenter=false)
	{
		double oa=aLen/2;	// 中心到边的距离

		if(imgWidth > 0 && imgHeight > 0)
		{
			if(bCenter)	// 中心不变 边长改变
			{
				if(oa > ox) oa=ox;
				if(oa > oy) oa=oy;
				if(oa > imgWidth-ox)  oa=imgWidth-ox;
				if(oa > imgHeight-oy) oa=imgHeight-oy;
			}
			else		// 中心改变 边长不变
			{
				if(aLen > imgWidth && aLen > imgHeight)		// 若图像宽高小于边长,取长边
				{
					aLen=max(imgWidth,imgHeight);
					oa=aLen/2;
				}

				if(aLen > imgWidth)
					ox=imgWidth/2;		// 若图像宽小于边长,取中心
				else
				{
					if(oa > ox) ox=oa;
					if(oa > imgWidth-ox)  ox=imgWidth-oa;
				}

				if(aLen > imgHeight)
					oy=imgHeight/2;		// 若图像高小于边长,取中心
				else
				{
					if(oa > oy) oy=oa;
					if(oa > imgHeight-oy) oy=imgHeight-oa;
				}
			}
		}

		x = ox-oa;
		y = oy-oa;
		w = oa+oa;
		h = w;
	}

	// 转为正方形,边长=(w+h)/2
	// 可选择图像边界限制
	// aLen=0取短边,aLen=-1取长边,aLen=-2取平均,其他aLen=边长
	static void convertSquare(cv::Rect &cr, int aLen=0, int imgWidth=0, int imgHeight=0)
	{
		if(cr.width!=cr.height)
		{
			double ox=cr.x+cr.width/2.0;
			double oy=cr.y+cr.height/2.0;
			double oa=1;

			// 确定半边长度 nType=0取短边,nType=1取长边,nType=2取平均
			if(aLen==0)			// 取短边
				oa=min(cr.width, cr.height)/2.0;
			else if(aLen==-1)	// 取长边
				oa=max(cr.width, cr.height)/2.0;
			else if(aLen==-2)	// 取平均
				oa=(cr.width+cr.height)/4.0;
			else if(aLen>1)
				oa = aLen/2;

			// 如果出界,则以原正方形中心缩小取正方形
			// 判断中心到四边的距离,使正方形不出四边
			if(imgWidth>0 && imgHeight>0)
			{
				if(oa > ox) oa=ox;
				if(oa > oy) oa=oy;
				if(oa > imgWidth-ox)  oa=imgWidth-ox;
				if(oa > imgHeight-oy) oa=imgHeight-oy;
			}

			cr.width=cr.height=oa+oa;
			cr.x=ox-oa;
			cr.y=oy-oa;
		}
	}

    // 	// ֱ�ߵĽǶ�
    // 	//		acos�� 	Principal arc cosine of x, in the interval [0,pi] radians.
    // 	// 		asin�� 	Principal arc sine of x, in the interval [-pi/2,+pi/2] radians.
    // 	// 		atan�� 	Principal arc tangent of x, in the interval [-pi/2,+pi/2] radians.
    // 	// 		One radian is equivalent to 180/PI degrees.
    // 	double angle()	// ��pi/2�������ܻ������ֵ,��˲�����acos
    // 	{
    // 		if(end.x == start.x)
    // 			return CV_PI/2;
    // 		return atan((end.y - start.y)/(end.x - start.x));
    // 	}
    //
    // 	// ����ľ���
    // 	double length()
    // 	{
    // 		return start.distance(end);
    // 		//return sqrt(((double)end.x-start.x)*(end.x-start.x) + (end.y-start.y)*(end.y-start.y));
    // 	}
    //
    // 	// ��β�˵㽻��
    // 	void swap()
    // 	{
    // 		MyPoint<T> mpt = start;
    // 		start = end;
    // 		end = mpt;
    // 	}
    //
    // 	void midLine(MyLine<double> mld)
    // 	{
    // 		// �Ƚ���ֱ��ͷβ����
    // 		// ������
    // 		// 1 �ֱ������β���˵�ľ��룬ȡ��=a
    // 		// 2 ��β����������˵�ľ��룬ȡ��=b
    // 		// 3 a<b Ϊ��������֮һ��ֱ��ͷβ����
    // 		double s1 = start.distance(mld.start);
    // 		double e1 = end.distance(mld.end);
    // 		double s2 = start.distance(mld.end);
    // 		double e2 = end.distance(mld.start);
    // 		s1 = max(s1, e1);
    // 		s2 = max(s2, e2);
    // 		if(s2 < s1)	swap();
    //
    // 		start += mld.start;
    // 		end += mld.end;
    // 		start /= 2;
    // 		end /= 2;
    // 	}
    //
    // 	// ��ȡֱ�ߵİ�Χ���ο�
    // 	Rect boundRect()
    // 	{
    // 		Rect rc;
    // 		rc.x = min(start.x, end.x);
    // 		rc.y = min(start.y, end.y);
    // 		rc.width = abs(end.x - start.x)+1;
    // 		rc.height = abs(end.y - start.y)+1;
    // 		return rc;
    // 	}
    //
    // 	_inline string toString()
    // 	{
    // 		stringstream s;
    // 		s << start.toString() << " " << end.toString();
    // 		return s.str();
    // 	}

    void println()
    {
        cout << x << " " << y << " " << w << " " << h << endl;
    }
    static void println(Rect &rc)
    {
        cout << rc.x << " " << rc.y << " " << rc.width << " " << rc.height << endl;
    }

    // 单通道显示, 线在框内, 反色点
    static void drawRectDot(cv::Mat srcMat, cv::Rect rc)
    {
        Vec3b *p, *q;
//#define drd_InverColor(pColor) Vec3b(0,255,0);
#define drd_InverColor(pColor) Vec3b(pColor[0]>127?0:255,pColor[1]>127?0:255,pColor[2]>127?0:255);
//#define drd_InverColor(pColor) Vec3b(120+pColor[0]/2,50+pColor[1]/2,120+pColor[2]/2);

        if(rc.width < 1) rc.width=1;
        if(rc.height < 1) rc.height=1;

        // 四角
        p = srcMat.ptr<Vec3b>(rc.y,rc.x);							*p = drd_InverColor(p[0]);
        p = srcMat.ptr<Vec3b>(rc.y,rc.x+rc.width-1);				*p = drd_InverColor(p[0]);
        p = srcMat.ptr<Vec3b>(rc.y+rc.height-1,rc.x);				*p = drd_InverColor(p[0]);
        p = srcMat.ptr<Vec3b>(rc.y+rc.height-1,rc.x+rc.width-1);	*p = drd_InverColor(p[0]);

        // 左右边
        int m = rc.height > 20 ? 10 : (rc.height > 10 ? 5 : rc.height/2);	// 虚线的段长度
        if(m < 1) m=1;
        for (int i=rc.y; i < rc.y+rc.height; i++)
        {
            p = srcMat.ptr<Vec3b>(i,rc.x);
            q = srcMat.ptr<Vec3b>(i,rc.x+rc.width-1);
            if(i % m == 0 || i % m == 1)
            {
                *p = drd_InverColor(p[0]);
                *q = drd_InverColor(q[0]);
            }
        }

        // 上下边
        m = rc.width > 20 ? 10 : (rc.width > 10 ? 5 : rc.width/2);	// 虚线的段长度
        if(m < 1) m=1;
        p = srcMat.ptr<Vec3b>(rc.y);
        q = srcMat.ptr<Vec3b>(rc.y+rc.height-1);
        for (int j=rc.x;  j < rc.x+rc.width; j++)
        {
            if(j % m == 0 || j % m == 1)	// 第一个点如果正好是角点，则角点变2次
            {
                p[j] = drd_InverColor(p[j]);
                q[j] = drd_InverColor(q[j]);
            }
        }
    }

    // 无显示名字时只绘图不显示不转彩色
    // 返回彩色绘图,以方便其他图形的绘制添加
    cv::Mat drawshow(cv::Mat &srcMat, const char* showName=NULL, int mb=128, int mg=0, int mr=255, int thickness=1)
    {
        MyRect<int> mri(x,y,w,h);
        mri.fitInRange(srcMat.size());

        cv::Mat cm=srcMat;
        if(showName != NULL && srcMat.channels() == 1)
            cvtColor(srcMat,cm,CV_GRAY2BGR);

        if(w > 0 && h > 0)
            rectangle(cm, mri.cvt(), Scalar(mb,mg,mr),thickness);

        if(showName != NULL)
            cv::imshow(showName, cm);
        return cm;
    }

    void show(cv::Mat &srcMat, bool bDisp=0, int cropWidth=0)
    {
        MyRect<int> mri(x,y,w,h);
        cv::Mat cm=srcMat;

        //if(srcMat.channels() == 1)
        //	cvtColor(srcMat,cm,CV_GRAY2BGR);
        rectangle(cm, mri.cvt(), Scalar(255,0,255));

        if(cropWidth > 0)
        {
            mri.extend(cropWidth);
            mri.fitInRange(cm.size());
            if(bDisp) cv::imshow("MyRect", cm(mri.cvt()));
        }
        else
        {
            if(bDisp) cv::imshow("MyRect", cm);
        }
    }

    static void showRect(cv::Mat srcMat, cv::Rect rc, const char* showName=NULL, double fScale=1)
    {
        cv::Mat cm=srcMat;
        if(srcMat.channels() == 1)
            cvtColor(srcMat,cm,CV_GRAY2BGR);

        rectangle(cm, rc, Scalar(255,0,255), fScale < 1 ? 2/fScale : 2);
        resize(cm, cm, cvSize(0,0),fScale,fScale,CV_INTER_NN);
        if(showName != NULL)
            cv::imshow(showName, cm);
        else
            cv::imshow("showRect", cm);
    }

    static void showRects(cv::Mat srcMat, vector<Rect> &vr)
    {
        cv::Mat cm=srcMat;
        if(srcMat.channels() == 1)
            cvtColor(srcMat,cm,CV_GRAY2BGR);

        for (int i=0; i < vr.size(); i++)
            rectangle(cm, vr[i], Scalar(255,0,255));
        cv::imshow("showRects", cm);
    }

    // 由两个矩形计算出他们的“并集矩形”
    static cv::Rect getUnionRect(cv::Rect &r1, cv::Rect &r2) {
        int startX	= r1.x < r2.x ? r1.x : r2.x;
        int startY	= r1.y < r2.y ? r1.y : r2.y;
        int endX	= r1.x+r1.width  > r2.x+r2.width  ? r1.x+r1.width  : r2.x+r2.width;
        int endY	= r1.y+r1.height > r2.y+r2.height ? r1.y+r1.height : r2.y+r2.height;
        return cvRect(startX, startY, endX-startX, endY-startY);
    }

    // 由两个矩形计算出他们的“交集矩形”
    static Rect getIntersectRect(Rect r1, Rect r2) {
        int startX	= r1.x > r2.x ? r1.x : r2.x;
        int startY	= r1.y > r2.y ? r1.y : r2.y;
        int endX	= r1.x+r1.width  < r2.x+r2.width  ? r1.x+r1.width  : r2.x+r2.width ;
        int endY	= r1.y+r1.height < r2.y+r2.height ? r1.y+r1.height : r2.y+r2.height;
        return cvRect(startX, startY, endX-startX, endY-startY);
    }

    // 由两个矩形计算“交集矩形”的面积
    static int getIntersectArea(Rect r1, Rect r2) {
        cv::Rect cr = getIntersectRect(r1, r2);

        // 判断矩形是否相交
        if(cr.width > 0 && cr.height > 0) // 不含边界相交的情况
            return cr.width*cr.height;
        else
            return 0;
    }

    // 将矩形聚类合并 简化版
    static void mergeRect(vector<cv::Rect> &vRect, vector<cv::Rect> &vMerge)
    {
        vMerge.push_back(vRect[0]);

        for (int i=1; i < vRect.size(); i++)
        {
            bool bMerge=false;
            int area1 = vRect[i].width*vRect[i].height;
            for (int j=0; j < vMerge.size(); j++)
            {
                // 得到较小矩形的面积
                int area2 = vMerge[j].width*vMerge[j].height;
                area1 = min(area1, area2);

                // 相交区域的面积
                float interArea = getIntersectArea(vRect[i], vMerge[j]);

                // 交叠区面积与较小矩形面积之比 > 0.6 时合并
                if(interArea/area1 > 0.6)
                {
                    vMerge[j] = getUnionRect(vRect[i], vMerge[j]);
                    bMerge = true;
                    break;
                }
            }

            if(!bMerge)
                vMerge.push_back(vRect[i]);
        }
    }
};

typedef MyRect<int>		MyRectI;
typedef MyRect<double>	MyRectD;

template<class T>
ostream& operator<<(ostream &output, const MyRect<T> &rc){  
    output << rc.x << "," << rc.y << "," << rc.w << "," << rc.h;
    return output;
}

// ����һ��ֱ�߶���
template<class T>
MyRect<T> myRect( MyRect<T> mpt0, MyRect<T> mpt1 )
{
    MyRect<T> mp;
    // 	mp.start = mpt0;
    // 	mp.end = mpt1;
    return mp;
}
