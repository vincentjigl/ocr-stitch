#ifndef MYHEADDEF_H
#define MYHEADDEF_H
#pragma warning(disable:4018) //“<”: 有符号/无符号不匹配
#pragma warning(disable:4244) // 从“double”转换到“int”，可能丢失数据
#pragma warning(disable:4267) // 初始化”: 从“size_t”转换到“int”，可能丢失数据
#pragma warning(disable:4305) // “初始化”: 从“double”到“float”截断
#pragma warning(disable:4800) //“int”: 将值强制为布尔值“true”或“false”(性能警告)
#pragma warning(disable:4996) // 'strcat': This function or variable may be unsafe. 

#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <typeinfo>

// #include <cv.h>
#include <opencv/cv.h>
// #include <highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/opencv.hpp"

#ifdef __unix__
    #include <unistd.h>
    #include <pthread.h>
    #define FALSE   false
    #define TRUE    true
    #define  Sleep(t)   usleep(t*1000)
#elif defined WIN32
    #include <windows.h>
    // #include <opencv2/xfeatures2d.hpp>
    // #include "opencv2/xfeatures2d/nonfree.hpp"
    // using namespace cv::xfeatures2d;
#endif

using namespace cv;
using namespace std;	// std不能在自定义的头文件下面

// 获取变量名字
#define outln()				printf("\n")
#define outInt1(x1)			printf("%s = %d\n", #x1, x1)
#define outInt2(x1,x2)			printf("%s=%d\t%s=%d\n", #x1, x1, #x2, x2)
#define outInt3(x1,x2,x3)		printf("%s=%d\t%s=%d\t%s=%d\n", #x1, x1, #x2, x2, #x3, x3)
#define outInt4(x1,x2,x3,x4)            printf("%s=%d\t%s=%d\t%s=%d\t%s=%d\n", #x1, x1, #x2, x2, #x3, x3, #x4, x4)
#define outInt5(x1,x2,x3,x4,x5)         printf("%s=%d\t%s=%d\t%s=%d\t%s=%d\t%s=%d\n", #x1, x1, #x2, x2, #x3, x3, #x4, x4, #x5, x5)
#define outInt1Double1(x1,x2)		printf("%s=%d\t%s=%f\n", #x1, x1, #x2, x2)
#define outInt1Double2(x1,x2,x3)	printf("%s=%d\t%s=%.1f\t%s=%.1f\n", #x1, x1, #x2, x2, #x3, x3)
#define outInt2Double1(x1,x2,x3)	printf("%s=%d\t%s=%d\t%s=%f\n", #x1, x1, #x2, x2, #x3, x3)
#define outInt2Double2(x1,x2,x3,x4)	printf("%s=%d\t%s=%d\t%s=%.1f\t%s=%.1f\n", #x1, x1, #x2, x2, #x3, x3, #x4, x4)
#define outInt2Double3(x1,x2,x3,x4,x5)	printf("%s=%d\t%s=%d\t%s=%.1f\t%s=%.1f\t%s=%.1f\n", #x1, x1, #x2, x2, #x3, x3, #x4, x4, #x5, x5)
#define outDouble1(x1)			printf("%s = %f\n", #x1, x1)
#define outDouble2(x1,x2)		printf("%s=%f\t%s=%f\n", #x1, x1, #x2, x2)
#define outDouble3(x1,x2,x3)		printf("%s=%.2f\t%s=%.2f\t%s=%.2f\n", #x1, x1, #x2, x2, #x3, x3)
#define outDouble4(x1,x2,x3,x4)		printf("%s=%.2f\t%s=%.2f\t%s=%.2f\t%s=%.2f\n", #x1, x1, #x2, x2, #x3, x3, #x4, x4)
#define outIntStr(x1,str)		printf("%s=%d %s\n", #x1, x1, str)
#define outStrDouble(str,x1)		printf("%s %s=%f\n", str, #x1, x1)

#define qOutInt1(x1)			qDebug("%s = %d\n", #x1, x1)
#define qOutInt2(x1,x2)			qDebug("%s=%d\t%s=%d\n", #x1, x1, #x2, x2)
#define qOutInt3(x1,x2,x3)		qDebug("%s=%d\t%s=%d\t%s=%d\n", #x1, x1, #x2, x2, #x3, x3)
#define qOutDouble1(x1)			qDebug("%s = %f\n", #x1, x1)
#define qOutDouble2(x1,x2)		qDebug("%s=%f\t%s=%f\n", #x1, x1, #x2, x2)
#define qOutDouble3(x1,x2,x3)		qDebug("%s=%.2f\t%s=%.2f\t%s=%.2f\n", #x1, x1, #x2, x2, #x3, x3)

#define qOutIntStr(x1,str)		qDebug("%s=%d %s", #x1, x1, str)

#define outMatSize(mat)         printf("[mbh]%s.cols=%d %s.rows=%d %s_line:%d\n",#mat,mat.cols,#mat,mat.rows,__FUNCTION__,__LINE__)
#define outLinePos(x1)          printf("[mbh]%s=%d %s_line:%d\n",#x1,x1,__FUNCTION__,__LINE__)
#define outLinePosStr(x1,str)          printf("[mbh]%s=%d %s %s_line:%d\n",#x1,x1,str,__FUNCTION__,__LINE__)
//#define outLinePos(str)			printf("%s %s_line:%d -mbh\n",str,__FUNCTION__,__LINE__)
// 输出编译时间 可以保存编译时间
#define outCompileTime()		char* dd = __DATE__;char* tt = __TIME__;printf("%s %s\n",dd,tt)

#define outSuccess(bRet)		printf("%s = %d ",#bRet,bRet);bRet?printf("---- ^_^ ^_^ ---- 成功!\n"):printf("failed!\n")

// 数字转字符串
#define strInt1(str,x1)			sprintf_s(mystrbuff,256,"%s\t%s=%d", str, #x1, x1)
#define strInt3(x1,x2,x3)		sprintf_s(mystrbuff,256,"%s=%d\t%s=%d\t%s=%d", #x1, x1, #x2, x2, #x3, x3)

#define MYCHECK_IF(fun)			if(fun){printf("[ %s ] error! \t%s_line:%d -mbh\n",#fun,__FUNCTION__,__LINE__);return;}
#define MYCHECK_IFR(fun,ret)	if(fun){printf("[ %s ] error! \t%s_line:%d -mbh\n",#fun,__FUNCTION__,__LINE__);return ret;}	// ͬ��, �з���ֵ

// 最大值保存在a中 注意 单独一行
#define MyMAX(a,b,c)				if(a<b)a=b;if(a<c)a=c
#define MyMAX3(a,b,c)				max(max(a,b),c)
// 最小值保存在a中
#define MyMIN(a,b,c)				if(a>b)a=b;if(a>c)a=c

#define INRANGE(X,minX,maxX)					(minX<=X&&X<=maxX)					// 含端点(端点在范围内)
#define NOTINRANGE(X,minX,maxX)					(minX>X||X>maxX)					// 含端点(端点在范围内)
#define NOTINRANGE2(X,minX,maxX,Y,minY,maxY)	(minX>X||X>maxX||minY>Y||Y>maxY)	// 含端点(端点在范围内)
#define INRANGE_XY_MAT(X,Y,matImg)		(0<=X&&0<=Y&&X<matImg.cols&&Y<matImg.rows)
#define INRANGE_POINT_MAT(pt,matImg)	(0<=pt.x&&0<=pt.y&&pt.x<matImg.cols&&pt.y<matImg.rows)
#define INRANGE_XYWH_MAT(x,y,w,h,matImg) (0<=x&&0<=y&&0<w&&0<h&&x+w<matImg.cols&&y+h<matImg.rows)
#define INRANGE_RECT_MAT(rc,matImg)		(0<=rc.x&&0<=rc.y&&0<rc.width&&0<rc.height&&rc.x+rc.width<matImg.cols&&rc.y+rc.height<matImg.rows)

// 下面语句后面不能有 else
#define MyLimit(minVal,val,maxVal)	std::max(minVal,std::min(val,maxVal))		// 可以对表达式操作
#define TORANGE(X,minVal,maxVal)	if(X<minVal){X=minVal;}else if(X>maxVal)X=maxVal // 含端点 不能对表达式操作
#define TORANGE_POINT(pt,row,col)	if(pt.x<0){pt.x=0;}else if(pt.x>=col){pt.x=col-1;}	\
									if(pt.y<0){pt.y=0;}else if(pt.y>=row){pt.y=row-1;}	// 适合到范围,范围含0,不含row,col
#define TORANGE_XY_MAT(X,Y,matImg)	if(X<0){X=0;}else if(X>=matImg.cols){X=matImg.cols-1;}	\
									if(Y<0){Y=0;}else if(Y>=matImg.rows){Y=matImg.rows-1;}	// 适合到范围,范围含0,不含row,col
// TORANGE 对矩形调整最小只留一像素宽高度
// 对每个截取子图都要用TORANGE_RECT_MAT(cr,matgray);
// #define TORANGE_RECT(rc,row,col)	if(rc.x<0){rc.x=0;}if(rc.y<0){rc.y=0;}						\
// 									if(rc.x>col-1){rc.x=col-1;}if(rc.y>row-1){rc.y=row-1;}		\
// 									if(rc.width<1||rc.x+rc.width>col){rc.width=col-rc.x;}		\
// 									if(rc.height<1||rc.y+rc.height>row)rc.height=row-rc.y	// rc.width最小值为1,rc.x+rc.width最大值为col-1
#define TORANGE_RECT_MAT(rc,matImg) if(rc.x<0){rc.x=0;}if(rc.y<0){rc.y=0;}						\
/*只截取有效区域 出界时宽高不变*/		if(rc.width<0){rc.width=0;}if(rc.x+rc.width>matImg.cols){rc.x=matImg.cols-rc.width;}		\
									if(rc.height<0){rc.height=0;}if(rc.y+rc.height>matImg.rows)rc.y=matImg.rows-rc.height
#define TORANGE_RECT_MAT2(rc,matImg) if(rc.x<0){rc.width+=rc.x;rc.x=0;}if(rc.y<0){rc.height+=rc.y;rc.y=0;}						\
/*只截取有效区域 出界时宽高减小*/			if(rc.x>matImg.cols-1){rc.x=matImg.cols-1;}if(rc.y>matImg.rows-1){rc.y=matImg.rows-1;}		\
									if(rc.width<0){rc.width=0;}if(rc.x+rc.width>matImg.cols){rc.width=matImg.cols-rc.x;}		\
									if(rc.height<0){rc.height=0;}if(rc.y+rc.height>matImg.rows)rc.height=matImg.rows-rc.y

#define MYSWAP_INT(a,b)			int xx;xx=a;a=b;b=xx
#define MYSWAP_INT2(a,b,c,d)	int xx;xx=a;a=b;b=xx;xx=c;c=d;d=xx
#define MYSWAP_DOUBLE(a,b)		double yy;yy=a;a=b;b=yy
#define MYSWAP_DOUBLE2(a,b,c,d)	double yy;yy=a;a=b;b=yy;yy=c;c=d;d=yy
#define MYSWAP_MPD(a,b)			MyPointD pxx;pxx=a;a=b;b=pxx

#define MYINT_MAX 99999999
#define MAX_BUF_SIZE 32*1024

#define fmin(a,b) (((a) < (b)) ? (a) : (b))
#define fmax(a,b) (((a) > (b)) ? (a) : (b))


// 计算时间时差c++
// DWORD t1,t2;
// t1 = GetTickCount();
// 
// t2 = GetTickCount();
// int te = t2-t1;

// 精确计算时间时差c++
//#ifdef _MYLOG	// 调试日志记录开关
#define MY_DISP_TIME(str) QueryPerformanceCounter(&nEndTime); \
	printf("%s\t%.1fms\t%05d_%s_%s\n",str,1000.0*(nEndTime.QuadPart-nBeginTime.QuadPart)/nFreq.QuadPart,__LINE__,__FUNCTION__,__FILE__); \
	QueryPerformanceCounter(&nBeginTime)	// 需要预先定义变量
#define MY_STRING_TIME(strbuff,str) QueryPerformanceCounter(&nEndTime); \
	sprintf_s(strbuff,256,"%s\t%.1fms\t%05d_%s_%s",str,1000.0*(nEndTime.QuadPart-nBeginTime.QuadPart)/nFreq.QuadPart,__LINE__,__FUNCTION__,__FILE__); \
	QueryPerformanceCounter(&nBeginTime)	// 需要预先定义变量
// 使用举例:
//	char strbuff[256];	// 在程序文件头, 定义通用字符串变量
// 	LARGE_INTEGER nFreq,nBeginTime,nEndTime;// 预定义变量初始化
// 	QueryPerformanceFrequency(&nFreq);
// 	QueryPerformanceCounter(&nBeginTime);

//	QueryPerformanceCounter(&nEndTime);
//	int te = 1000.0*(nEndTime.QuadPart-nBeginTime.QuadPart)/nFreq.QuadPart;
	//MY_DISP_TIME;

// 调试记录 写日志 写文件
#define LOG_DATA(str,x,y)	fprintf(logFile,"\n[%s]_%d_%d",str,x,y)//需预先定义和打开：FILE * f; f=fopen(...); fclose(f);
#define LOG_LINE2(str,x,y)	fprintf(logFile,"\n%d:[%s]_%s_%d_%d",__LINE__,__FUNCTION__,str,x,y)//需预先定义和打开：FILE * f; f=fopen(...); fclose(f);
//	FILE * logFile=NULL;// 预定义变量初始化 全局变量
//#endif
// 使用举例:
// char strtime[32];
// time_t tt = time(0);//产生“YYYY-MM-DD hh:mm:ss”格式的字符串。
// strftime(strtime, sizeof(strtime), "%Y-%m-%d %H:%M:%S", localtime(&tt));
// strtime[31] = '\n';
// 
// logFile=fopen("log0802.txt", "a+");// 预定义变量初始化
// LOG_LINE2(strtime,printi++,0);
// int ret=PrintGeneralTag(istr, codeContent);
// fclose(logFile);

inline int roundI(double r){	return (r > 0.0) ? (int)floor(r + 0.5) : (int)ceil(r - 0.5);}
inline double round(double r){	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);}
//#include <windows.h>

#include "MyTools.h"
#include "MyTools_cv.h"
#include "MyMath.h"
#include "MyString.h"
#include "MyPoint.h"
#include "MyMat.h"
#include "MyLineKB.h"
#include "MyLine.h"
#include "MyRect.h"
#include "MyArray.h"
#include "MyMatGray.h"
#include "MyMatBin.h"
#include "MyMatColor.h"
#include "MyVideo.h"
//#include "MyImageCurve.h"
#include "MyPointArray.h"
#include "MyFile.h"
#include "MyProc.h"
#include "MyImgSegment.h"

// MFC相关 要放在对话框头文件中
//#include "MyMFCui.h"
// #ifdef _AFXDLL		// 定义这个符号表示动态连接MFC
// 	#include "MyConfig.h"
// #endif
//#include "MyConfig_std.h" 根据不同的应用修改

//static MyTools mtools;

//#include "SocketClient.h"
#endif // MYHEADDEF_H
