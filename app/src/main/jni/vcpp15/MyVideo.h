#include <time.h>
typedef void(*lpFunc)(cv::Mat&);  // 函数指针

class MyVideo
{
public:
    VideoCapture videoCap;
    int		sleepTime;
    bool	bVideoStop;
    float   fScale;

    MyVideo(void){
        bVideoStop = false;
        sleepTime=170*1000;
        fScale = 1;
    };
    ~MyVideo(void){};

    bool init(int capId=0) {
//        videoCap.open(capId);
        videoCap.open("F:/abc/tmp/test2.avi");
        //usleep(sleepTime);
        if (!videoCap.isOpened()) {
            //printf("open camera %d failed!\n", capId);
            return false;
        }

        videoCap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);  	// 1280*1024 1600*1200
        videoCap.set(CV_CAP_PROP_FRAME_HEIGHT, 960);	// 640*480 1280*960

        Mat frame;
        for (int i = 0; i < 3; i++) {
            //usleep(sleepTime);	//delay(42);
            if (!videoCap.read(frame))
                printf("%d error  MyVideo()\n", i);
        }

        return true;
    }

    void close()
    {
        videoCap.release();
    }

    // 视频处理回调函数
    // 使用举例: MyVideo mv;
    //			mv.init();
    //			mv.play(imageproc, 2000);
    void play(lpFunc callfuct, bool bShowFrame=true, int waitTime=30)
    {
        int i=0,nError=0;
        cv::Mat frame;

        while(!bVideoStop)
        {
            videoCap >> frame;
            if(frame.empty()) break;
            if(!frame.data)
            {
                if(nError++ > 20) break;
                printf("%d ", nError);
#ifdef _AFXDLL
                Sleep(nError*3);
#endif
                continue;
            }

            printf("i=%d  \r",i++);
            callfuct(frame);
            if(bShowFrame)
                imshow("frame",frame);

            int nkey=waitKey(waitTime);
            if(nkey == ' ')						// 空格键暂?
                nkey = waitKey(0);
            if(nkey == 's' || nkey == 'S')		// 保存图片 也暂?
            {
                char buf[64];
                time_t t = time(NULL); //获取目前秒时?
                tm* local = localtime(&t); //转为本地时间
                strftime(buf, 64, "img%m%d%H%M%S.jpg", local);
                imwrite(buf, frame);
                printf("save image %s\n", buf);
                imshow(buf,frame);			// 保存后暂停显??
                waitKey(0);
                destroyWindow(buf);
            }
            else if(nkey == 27 || nkey == 'q')		// 按Esc键关闭退?
                bVideoStop = true;
        }

        videoCap.release();
        destroyWindow("frame");
    }

    static int simpleTest_main(int maxFrame=10, bool bSave=false)
    {
        VideoCapture videoCap(0);
        if(!videoCap.isOpened())
        {
            printf("打开摄像头失败！");
            return 0;
        }

        char fileName[256];
        int waitTime=50;
        cv::Mat frame;

        for(int i=0; i < maxFrame; i++)
        {
            printf("%d:", i);

            videoCap >> frame;
            if(!frame.data)
            {
                printf(" Read image error!\n");
                continue;
            }
            printf("\n");

            if(bSave==true)
            {
                sprintf(fileName, "img%03d.jpg", i);
                imwrite(fileName, frame);
            }
            else if(bSave==false)
            {
                imshow("simpleTest", frame);

                int nkey = waitKey(waitTime);
                if (nkey == ' ')			// 空格键暂?
                    waitKey(0);
                else if (nkey == 27 || nkey == 'q')		// 按Esc键关闭退?
                    break;
            }
        }

        videoCap.release();
        return 0;
    }

    int readVideo(string videofile, int maxFrame=10, bool bSave=false)
    {
        VideoCapture videoCap(videofile);
        if(!videoCap.isOpened())
        {
            printf("open video file failed 150.\n!");
            return 0;
        }

        char fileName[256];
        int waitTime=50;
        cv::Mat frame;

        for(int i=0; i < maxFrame && !bVideoStop; i++)
        {
            printf("%d:\r", i);

            videoCap >> frame;
            if(!frame.data)
            {
                printf(" Read image error!\n");
                continue;
            }

            cv::Mat matres;
            cv::resize(frame, matres, cvSize(0, 0), fScale, fScale);

            if(bSave==true)
            {
                sprintf(fileName, "img%03d.jpg", i);
                imwrite(fileName, matres);
            }
            else if(bSave==false)
            {
                imshow("simpleTest", matres);

                int nkey = waitKey(waitTime);
                if (nkey == ' ')			// 空格键暂?
                    waitKey(0);
                else if (nkey == 27 || nkey == 'q')		// 按Esc键关闭退?
                    break;
            }
        }

        videoCap.release();
        return 0;
    }

    static int saveVideo(int maxFrame=10, bool bSave=false)
    {
        //视频保存位置
        VideoCapture videoCap(0);
        videoCap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);  	// 1280*1024 1600*1200
        videoCap.set(CV_CAP_PROP_FRAME_HEIGHT, 960);	// 640*480 1280*960

        if(!videoCap.isOpened())
        {
            printf("打开摄像头失?");
            return 0;
        }

        int waitTime=1;
        cv::Mat frame;
        VideoWriter outputVideo;

        if(bSave)
        {
            //获取当前摄像头的视频信息
            cv::Size S = cv::Size((int)videoCap.get(CV_CAP_PROP_FRAME_WIDTH),
                                  (int)videoCap.get(CV_CAP_PROP_FRAME_HEIGHT));
            //打开视频路径，设置基本信?open函数中你参数跟上面给出的VideoWriter函数是一样的
            //outputVideo.open(outputVideoPath, -1, 30.0, S, true);// ?FFMPEG: tag  is not found (format 'avi / AVI，但可以保存
            string outputVideoPath = "test.avi";
            outputVideo.open(outputVideoPath, CV_FOURCC('M', 'J', 'P', 'G'), 10.0, S, true);
            //CV_FOURCC('P','I','M','1');
            //cv::VideoWriter_fourcc(*'XVID');
            if (!outputVideo.isOpened()) {
                cout << "fail to open outputVideo file!" << endl;
                return -1;
            }
        }

        for(int i=0; i < maxFrame; i++)
        {
            printf("%d:\r", i);

            videoCap >> frame;
            if(!frame.data)
            {
                printf(" Read image error!\n");
                continue;
            }

            //保存当前?
            if(bSave==true)
                outputVideo << frame;

            imshow("simpleTest", frame);

            int nkey = waitKey(waitTime);
            if (nkey == ' ')			// 空格键暂?
                waitKey(0);
            else if (nkey == 27 || nkey == 'q')		// 按Esc键关闭退?
                break;
        }

//        outputVideo.release();
        videoCap.release();
        return 0;
    }
};


