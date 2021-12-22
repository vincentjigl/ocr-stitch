#ifndef IVIDEOSTITCH_H
#define IVIDEOSTITCH_H
#include "opencv2/cvconfig.h"
#include "../vcpp15/MyHeadDef.h"

/**
* @program: Z03 图像拼接接口
*
* @description: 这是一个接口类
*
* @author: Mabinghui
*
* @create: 2021-03-05 Friday 11:38:30
**/

typedef int(*imgProcCall)(int, int, char*, void*);

class IVideoStitch{

public:
    string imgpath;

 	/**
	* @Description: 启动摄像头取图
	* @Param: 		[ipc]	对图像处理的回调函数
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-03-05 Friday 11:47:06
	*/
	virtual int v4l2_capture_start(imgProcCall ipc) = 0;
	virtual int v4l2_capture_stop() = 0;

	/**
	* @Description: 清空上次的拼图
	* @Param: 		[]
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-03-05 Friday 11:47:06
	*/
	virtual int clear() = 0;

	/**
	* @Description: 拼接一帧图像
	* @Param: 		[matsrc, mode]	当前帧图像,mode=0,左手模式,mode=1,右手模式
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-03-05 Friday 11:47:06
	*/
	virtual int stitch(cv::Mat &matsrc, int mode) = 0;

 	/**
	* @Description: 根据指定长度返回拼接的图片
	* @Param: 		[matdst, width]	当width=0时,返回从头开始拼接的图片
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-03-05 Friday 11:47:06
	*/
    virtual int getStitchedImage(cv::Mat &matdst, int width) = 0;

 	/**
	* @Description: 返回拼接的图片
	* @Param: 		[matdst, type]	当type=0时,返回不旋转图片, 1 旋转图片, 2 拉直图片
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-05-15 Friday 11:47:06
	*/
    virtual int getStitchedImage2(cv::Mat &matdst, int type) = 0;

 	/**
	* @Description: 1. 扫题模式下 多行拼接
	* @Param: 		vector<cv::Mat> &vMatLine 中的图片为一行中的不同片段
					bool bNew bNew=true,本次图像拼接为新行,反之拼接替换最后一行
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-04-12 Monday 19:42:40
	*/
    virtual int lineMerge_set(vector<cv::Mat> &vMatLin, int lineID) = 0;

 	/**
	* @Description: 2. 扫题模式下 多行拼接图片的获取。调用一次之后图片将被清空。
	* @Param: 		cv::Mat &matdst 返回多行拼接的图片
	* @return: 		error number
	* @Author: 		Mabinghui
	* @Date: 		2021-04-12 Monday 19:42:40
	*/
    virtual int lineMerge_get(cv::Mat &matdst) = 0;

    // 3. 扫题模式下 行拼接 清空
    virtual int lineMerge_Clear() = 0;

};
#endif // IVIDEOSTITCH_H
