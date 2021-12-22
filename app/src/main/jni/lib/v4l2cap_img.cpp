// imgStitch.cpp : 定义 DLL 应用程序的导出函数。
#include "../vcpp15/MyHeadDef.h"
#include "IVideoStitch.h"

bool bPlay = false;

extern "C" {

	int v4l2cap_main(const char* videoName, int maxFrame, imgProcCall ipc, void *arg)
	{
		VideoCapture videoCap;
    	int nLen = strlen(videoName);

    	// 1. 读取摄像头 或视频
        if(MyTools::isnum(videoName)){
        	// videoCap.open(atoi(videoName));
        	printf("[mbh] open camera failed. %s\n", __FILE__);
            return -1;
        }

        else if(videoName[nLen-4] == '.')
			videoCap.open(videoName);
        
        // 2. 读取文件夹图片
        else
        {
        	int i=0;
	        char filename[88];
	        cv::Mat imageYUV;

	        bPlay = true;
	        for(; i < maxFrame && bPlay; i++)
	        {
	    		sprintf(filename, "%s/img%05d.jpg", videoName, i);
	            // printf("%s\n", filename);
		        cv::Mat frame = imread(filename, CV_LOAD_IMAGE_COLOR);
	            if(!frame.data)
	            {
	                // printf("[mbh] video end! line=34\n");
	                break;
	            }

	    		cv::cvtColor(frame, imageYUV, CV_BGR2YUV_YV12);
	            ipc(frame.rows, frame.cols, (char*)imageYUV.data, arg);
	        }

	        printf("[mbh]v4l2cap_imgs thread end. total frame num = %d\n", i);
        	return 0;
        }

        // 3. 读取视频
        if(videoCap.isOpened())
            printf("[mbh]打开视频成功！videoName = %s\n", videoName);
        else
        {
            printf("[mbh]打开视频失败！videoName = %s\n", videoName);
            return -2;
        }

        int i=0;
        cv::Mat frame, imageYUV;

        bPlay = true;
        for(; i < maxFrame && bPlay; i++)
        {
            videoCap >> frame;
            if(!frame.data)
            {
                printf("[mbh] video end! line=42\n");
                break;
            }

            // printf("i=%d:\n", i);
            // imshow("frame", frame);
            // waitKey(0);
            
    		cv::cvtColor(frame, imageYUV, CV_BGR2YUV_YV12);
            ipc(frame.rows, frame.cols, (char*)imageYUV.data, arg);
            Sleep(8);
        }

        printf("[mbh]v4l2cap thread end. total frame num = %d\n", i);
        videoCap.release();

		outLinePos(0);	// 不可去掉，报错

		return 0;
	}

	int v4l2cap_stop(){
		bPlay = false;
        return 0;
	}
}
