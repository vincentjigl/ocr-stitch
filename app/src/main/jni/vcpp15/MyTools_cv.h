﻿// 需要#include <Windows.h>

#ifdef _AFXDLL		// 定义这个符号表示动态连接MFC
	// 需在类外定义 冗余数据循环时可以不用取模
	// 4邻域序号 i*2+1 可对应8邻域序号
	static POINT neighbor4[7]={/*up*/0,-1,/*right*/1,0,/*down*/0,1,/*left*/-1,0,
							   /*up*/0,-1,/*right*/1,0,/*down*/0,1};
	static POINT neighbor8[15]={/*leftup*/-1,-1,0,-1,/*rightup*/1,-1,1,0,/*rightdown*/1,1,0,1,/*leftdown*/-1,1,-1,0,
								/*leftup*/-1,-1,0,-1,/*rightup*/1,-1,1,0,/*rightdown*/1,1,0,1,/*leftdown*/-1,1};	
#endif

class MyTools_cv
{
public:
	
	//static cv::Point dp[9];
	//static const POINT nb8[9];	// 8方向 8邻域 顺时针

	MyTools_cv(void){
// 		static POINT nbhd4[5]={/*up*/0,-1,/*right*/1,0,/*down*/0,1,/*left*/-1,0,0,-1};
// 		static POINT nbhd8[9]={/*leftup*/-1,-1,0,-1,/*rightup*/1,-1,1,0,/*rightdown*/1, 1,0,1,/*leftdown*/-1, 1,-1, 0,-1,-1};
// 		nb4 = nbhd4;
// 		nb8 = nbhd8;
// 		for (int i=0; i < 9; i++)
// 		{
// 			dp[i].x = nb[i].x;
// 			dp[i].y = nb[i].y;
// 		}
	};

	~MyTools_cv(void){};

	static double round(double r)
	{
		return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
	}

#ifdef _AFXDLL		// 定义这个符号表示动态连接MFC
	// 4方向邻域
	static cv::Point getneighbor4(cv::Point cp, int dir)
	{
		return cvPoint(cp.x+neighbor4[dir].x, cp.y+neighbor4[dir].y);
	}

	// 8方向邻域
	static cv::Point getneighbor8(cv::Point cp, int dir)
	{
		return cvPoint(cp.x+neighbor8[dir].x, cp.y+neighbor8[dir].y);
	}
#endif

	// 一维坐标上,已知两点(x1,x2)和对应的刻度值(v1,v2),求任一点x对应的值
	static double getInterValue(double x1, double x2, double v1, double v2, double x)
	{
		double percent=(v2-v1)/(x2-x1);
		return v1+percent*(x-x1);
	}

	// 当nlen<2时,则在图像的(x,y)处画一点. 经测试与imgout.at<cv::Vec3b>(i,j)=cv::Vec3b(0,0,0);功能相同
	static void crossLine(cv::Mat matSrc, double x, double y, int mb=0, int mg=255, int mr=255, int nlen=9, int thickness=1)
	{
		line(matSrc, cvPoint2D32f(x - nlen / 2, y), cvPoint2D32f(x + nlen / 2, y), CV_RGB(mr,mg,mb),thickness);
		line(matSrc, cvPoint2D32f(x, y - nlen / 2), cvPoint2D32f(x, y + nlen / 2), CV_RGB(mr,mg,mb),thickness);
	}

	static void crossLine(cv::Mat matSrc, cv::Point pt, int mb=0, int mg=255, int mr=255, int nlen=9, int thickness=1)
	{
		line(matSrc, cvPoint(pt.x - nlen / 2, pt.y), cvPoint(pt.x + nlen / 2, pt.y), CV_RGB(mr,mg,mb),thickness);
		line(matSrc, cvPoint(pt.x, pt.y - nlen / 2), cvPoint(pt.x, pt.y + nlen / 2), CV_RGB(mr,mg,mb),thickness);
	}

	// 判断角度是否pi/2(90度)的倍数
	static bool check90Angle(double Angle, double eps=0.0000001)
	{
		double cvPI2 = 3.1415926535897932384626433832795/2;
		double aa=abs(Angle);

		while(aa > CV_PI) aa-=CV_PI;

// 		if(CV_PI - aa < eps || aa < eps)	// true 是180度的倍数,返回false
// 			return false;

		return CV_PI - aa < eps || aa < eps ? false : abs(cvPI2 - aa) < eps;		// true 是90度的倍数
	}

	// 最小值与最大值的差值, 返回数列序号
	static int minmaxdiff(int a, int b, int c, int d)
	{
		int datamax=a, datamin=b;

		if(a < b)
		{
			int t=a; a=b; b=t;
		}
		if(c < d)
		{
			int t=c; c=d; d=t;
		}
		if(a < c)
			a=c;
		if(b < d)
			d=b;
		return a-d;
	}

	////////////////// 直线 /////////////////////

	// 两点的距离
	static double distance(cv::Point2f p, cv::Point2f q)
	{
		return sqrt((q.x-p.x)*(q.x-p.x) + (q.y-p.y)*(q.y-p.y));
	}
};
