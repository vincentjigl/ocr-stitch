#ifndef VIDEOSTITCH_H
#define VIDEOSTITCH_H

// #define MDEBUG
// #define SAVEPIC
// #define SAVELOG

#include <mutex>
//#include <sys/time.h>
#include "../lib/IVideoStitch.h"
#include "CoImage.h"
#include "stitchMgr.h"


extern "C" int v4l2cap_main(const char* videoName, int maxFrame, imgProcCall ipc, void *arg);
extern "C" int v4l2cap_stop();

/**
* @program: Z03 图像拼接类
*
* @description: 本类实现了接口IVideoStitch
*
* @author: Mabinghui
*
* @create: 2021-03-05 Friday 11:38:30
**/

class VideoStitch : public IVideoStitch {
    //int gXX[] = {1};
    //int gYY[] = {1};
    // # 截取区域
    int roiX;
    int roiY;
    int roiW;
    int roiH;

    // # 缩放
    int resizeW; 
    int resizeH;

	YstitchMgr ysm;
	std::mutex ysm_mutex;

	std::vector<cv::Mat> vMatsrc;
	
	// 多行拼接的结果图片列表
    CoImage mci;
	std::mutex mci_mutex;

public:
    VideoStitch(void){
		roiX = 200;
		roiY =  60;
		roiW = 240;
		roiH = 350;
		resizeW = 96;
		resizeH = 128;
    };
    ~VideoStitch(void){};

 	/**
	* @Description: 启动摄像头取图
	* @Param: 		[ipc]	对图像处理的回调函数
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-03-05 Friday 11:47:06
	*/
	int v4l2_capture_start(imgProcCall ipc){
		#ifdef  __x86_64__
			v4l2cap_main(imgpath.c_str(), 888, ipc, NULL);
        #elif defined WIN32
            v4l2cap_main(imgpath.c_str(), 888, ipc, NULL);
		#else
			v4l2cap_main("1", INT_MAX, ipc, NULL);			// 读取摄像头
		#endif

		return 0;
	}

	int v4l2_capture_stop(){
		v4l2cap_stop();
		return 0;
	}

	/**
	* @Description: 清空上次的拼图
	* @Param: 		[]
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-03-05 Friday 11:47:06
	*/
	int clear(){
		// 保存扫描的图片
#ifdef SAVEPIC
		if(true)	// 保存单张图片
		{
			MyMat::saveImages(vMatsrc, "/tmp/saved_imgs");
			vMatsrc.clear();
		}

		if(true)	// 保存拼接图片
		{
			cv::Mat matdst;
			getStitchedImage2(matdst, 1);
			MyMat::saveImage(matdst, "/tmp/saved_imgs");
			// MyMat::saveImage(ysm.gci.matstt, "/home/root/mbh");
		}
#endif

	    std::lock_guard<std::mutex> mutx(ysm.gci_mutex);
		ysm.gci.clear();
		ysm.gciStraight.clear();
	    std::lock_guard<std::mutex> muty(ysm_mutex);
	    ysm.clear();

		return 0;
	}

	void MyResize_warpAffine(cv::Mat src, cv::Mat &dst)
	{
	    /*
		int np = 10;
		int w = dst.cols;
		int h = dst.rows;
		int* px = gXX;
		int* py = gYY;

		// #pragma omp parallel for num_threads(4)
		for (int i=0; i < h; i++)
		{
			uchar* q = dst.ptr<uchar>(i);
			#pragma omp parallel for num_threads(4)
			for (int j=0; j < w; j++)
				q[j] = src.at<uchar>(py[j], px[j]);
			px += w;
			py += w;
		}
		*/
	}

	/**
	* @Description: 拼接一帧图像
	* @Param: 		[matsrc, mode]	当前帧图像,mode=0,左手模式,mode=1,右手模式
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-03-05 Friday 11:47:06
	*/
	int stitch(cv::Mat &matsrc, int mode){

		if(matsrc.empty()){
			outLinePosStr(matsrc.cols, "stitch image empty!");
			return -1;
		}

//		cv::Mat matroi(128, 96, CV_8UC1), matroi0;
//		MyResize_warpAffine(matsrc(cvRect(roiX, roiY, roiW, roiH)), matroi);

	    std::lock_guard<std::mutex> muty(ysm_mutex);
		ysm.rightHand = mode;	// mode=0,左手模式 mode=1,右手模式
    	int mVal = ysm.videoStitch(matsrc);

#ifdef MDEBUG
		cv::Mat matst;
		// getStitchedImage2(matst, 1);
  		// imshow("125", matst);
        ysm.gci.mergeAllImages();
        imshow("125", ysm.gci.matstt);
        static int waittime=0;
        int nk = waitKey(waittime);
        if(nk==' ') waittime=0;
        // 显示裁剪矩形框
        imshow("matroi", matsrc);
        // MyRectI::showRect(matsrc, cvRect(roiX, roiY, roiW, roiH));
#endif

		// save images
#ifdef SAVEPIC
		vMatsrc.push_back(matsrc);
#endif

		return mVal;
	}

	double imageAngle(cv::Mat matsrc)
	{
		int w = matsrc.cols;
		int h = matsrc.rows;
        int a1, a2, b1, b2;

        MyMatBin::getWhiteBoarderTD(matsrc(cvRect(0,0,8,h)), a1, a2);
        MyMatBin::getWhiteBoarderTD(matsrc(cvRect(w-8,0,8,h)), b1, b2);
        a1 = min(a1, h - b2);
        b1 = min(b1, h - a2);

		return a1 > b1 ? -atan((float)a1/(w-resizeW)) : atan((float)b1/(w-resizeW));
	}

 	/**
	* @Description: 根据指定长度返回拼接的图片
	* @Param: 		[matdst, width]	当width=0时,返回从头开始拼接的图片
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-03-05 Friday 11:47:06
	*/
    int getStitchedImage(cv::Mat &matdst, int width){
		cv::Mat matsti;

		if(ysm.gci.gVs.size() < 1)
			return -1;

		ysm.gci.rightMode = ysm.rightHand;

	    std::lock_guard<std::mutex> mutx(ysm.gci_mutex);
        if(width > 0)
	        ysm.gci.mergeSubImages2(matsti, width);
	    else
		{
            ysm.gci.mergeAllImages();
            matsti = ysm.gci.matstt.clone();
        }

       	int x1, y1, x2, y2;
	    MyMatBin::getWhiteBoarder(matsti, x1, y1, x2, y2);
	    if(x2-x1 > 0 && y2-y1 > 0)
        	matsti = matsti(cvRect(x1, y1, x2-x1, y2-y1));

        // 拼接图片为空
        if(matsti.empty())
         	return -2;
        // imshow("matsti455", matsti);

        // 移动量小于9像素时，表示只点了一下
        if(matsti.cols > resizeW + 9)
    		matsti = matsti(cvRect(0, 0, matsti.cols - resizeW*0.9, matsti.rows));

        if(matsti.cols > resizeW && matsti.rows > resizeH){
		    double a = imageAngle(matsti);
        	matsti = MyMat::ImgRotate(matsti, a);
		    MyMatBin::getWhiteBoarder(matsti, x1, y1, x2, y2);
		    if(x2-x1 > 0 && y2-y1 > 0)
	        	matsti = matsti(cvRect(x1, y1, x2-x1, y2-y1));
	        else
	        	return -3;
        }

	    if(!ysm.rightHand)
	    {
	    	flip(matsti, matsti, 0);
	    	flip(matsti, matsti, 1);
	    }
		
        cvtColor(matsti, matdst, CV_GRAY2BGR);
		return 0;
    }

 	/**
	* @Description: 返回拼接的图片
	* @Param: 		[matdst, type]	当type=0时,返回不旋转图片, 1 旋转图片, 2 拉直图片
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-05-15 Friday 11:47:06
	*/
    int getStitchedImage2(cv::Mat &matdst, int type){
		cv::Mat matsti;
        CoImage *gci;

        if(type > 1)
            gci = &ysm.gciStraight; 
        else
            gci = &ysm.gci; 

		if(gci->gVs.size() < 1)
			return -1;

		gci->rightMode = ysm.rightHand;

		// 1. 获取拼接图片
	    std::lock_guard<std::mutex> mutx(ysm.gci_mutex);
		{
            gci->mergeAllImages();
            matsti = gci->matstt.clone();
        }

       	int x1, y1, x2, y2;
	    MyMatBin::getWhiteBoarder(matsti, x1, y1, x2, y2);
	    if(x2-x1 > 0 && y2-y1 > 0)
        	matsti = matsti(cvRect(x1, y1, x2-x1, y2-y1));

        // 2. 判断拼接图片是否为空
        if(matsti.empty()) return -2;
        // imshow("matsti455", matsti);

        // 3. 移动量小于9像素时，表示只点了一下
        if(matsti.cols > resizeW + 19)
    		matsti = matsti(cvRect(0, 0, matsti.cols - resizeW*0.9, matsti.rows));

    	// 4. 当 type=1 或 2 时旋转
    	if(type > 0)
    	{
	        if(matsti.cols > resizeW && matsti.rows > resizeH)
	        {
                int mb = matsti.at<uchar>(0,0);
			    double a = imageAngle(matsti != mb);
	        	matsti = MyMat::ImgRotate(matsti, a);
			    MyMatBin::getWhiteBoarder(matsti != mb, x1, y1, x2, y2);
			    if(x2-x1 > 0 && y2-y1 > 0)
		        	matsti = matsti(cvRect(x1, y1, x2-x1, y2-y1));
		        else
		        	return -3;
	        }
	    }

	    // 6. 右手模式下翻转图片
	    if(!ysm.rightHand)
	    {
	    	flip(matsti, matsti, 0);
	    	flip(matsti, matsti, 1);
	    }
		
        cvtColor(matsti, matdst, CV_GRAY2BGR);

		// MyMat::saveImage(matdst, "/tmp/saved_imgs");

		return 0;
    }

////////////////////// 多行拼接 ////////////////////////////////

 	/**
	* @Description: 1. 扫题模式下 多行拼接
	* @Param: 		vector<cv::Mat> &vMatLine 中的图片为一行中的不同片段
					int lineID, 行号
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-04-12 Monday 19:42:40
	*/
    int lineMerge_set(vector<cv::Mat> &vMatLin, int lineID)
    {
    	if(vMatLin.size() < 1) return -1;

	    std::lock_guard<std::mutex> mutz(mci_mutex);
    	mci.multi_Line_set(vMatLin, lineID);

    	vMatLin.clear();	// 用完图片清空

    	return 0;
    }

 	/**
	* @Description: 2. 扫题模式下 多行拼接图片的获取。调用一次之后图片将被清空。
	* @Param: 		cv::Mat &matdst 返回多行拼接的图片
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-04-12 Monday 19:42:40
	*/
    int lineMerge_get(cv::Mat &matdst)
    {
		// 拼接
	    std::lock_guard<std::mutex> mutz(mci_mutex);
		mci.mergeImages(matdst);

    	return 0;
    }

    // 3. 扫题模式下 行拼接 清空
    int lineMerge_Clear()
    {
    	// 清空
	    std::lock_guard<std::mutex> mutz(mci_mutex);
    	mci.multi_Line_clear();

    	return 0;
    }
};
#endif // VIDEOSTITCH_H
