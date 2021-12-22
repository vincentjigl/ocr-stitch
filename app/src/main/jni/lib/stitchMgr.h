
typedef int(*imgProcCall)(int, int, char*, void*);

extern "C" int v4l2_capture_start(const char* videoName, int maxFrame, imgProcCall ipc, void *arg);
extern "C" int v4l2_capture_stop();

class YstitchMgr
{
public:
	int nFrame;
	int nSkip;
    int sumdx;  	// 总偏移
    int dx, dy;  	// 当前帧的移动量  <0:左移 >0:右移 (上帧在上上帧中的坐标)

    int 	rightHand;

    CoImage gci;
    CoImage gciStraight;
	std::mutex gci_mutex;
	std::mutex log_mutex;

    YstitchMgr(void)
    {
    	nFrame 	= 0;
    	nSkip	= 0;
    	sumdx	= 0;
    	dx		= 0;
    	dy		= 0;
		rightHand = 1;
    };

    ~YstitchMgr(void){};

    void clear()
    {
#ifdef SAVELOG
	    std::lock_guard<std::mutex> muto(log_mutex);
		FILE* logFile=fopen("log0415.txt", "a+");
    	fprintf(logFile, "Cleared. %3d %3d %3d %d %d line:%d\n", nFrame, dx, dy, sumdx, nSkip, __LINE__);
		fclose(logFile);
#endif
    	nFrame 	= 0;
    	nSkip	= 0;
    	sumdx	= 0;
    	dx		= 0;
    	dy		= 0;
    }

    // 单行拼接
    int videoStitch(cv::Mat &matsrc)
    {
        // 2. 二值化
        cv::Mat mattmp, matbin;
        cv::resize(matsrc, matbin, cvSize(matsrc.cols/2, matsrc.rows/2));
        cv::blur(matbin, mattmp, cvSize(3,3));
        MyMatGray::grad(mattmp, matbin);

        cv::Point mLoc;
        cv::Mat   result;
        double 	  maxVal=-1;
        static cv::Mat matpre = matbin.clone();

        int tw = max(96 - 16 - abs(dx)/(nSkip+1), 40)/2;
        int th = 112/2;
        YRect tc = rightHand ? cvRect(0, 4, tw, th) : cvRect(matbin.cols-tw, 4, tw, th);

        matchTemplate(matpre, matbin(tc), result, CV_TM_CCOEFF_NORMED);
		minMaxLoc(result, NULL, &maxVal, NULL, &mLoc);

		int tx = mLoc.x - tc.x;
		int ty = mLoc.y - tc.y;
		tx*=2;
		ty*=2;

		// 超过阈值后只能左移(右移)
        int nDirectionTh = 188;
        if((sumdx < -nDirectionTh && 0 < tx) || (tx < 0 && nDirectionTh < sumdx))
            dx = 0;
        else
        {
        	dx = tx;
        	sumdx += dx;
        }

   		// 禁止纵向大的移动 (> 8px)
   		if(abs(ty) > 16)
   			dy = 0;
   		else if(abs(ty) > abs(dx))
   			dy = ty > 0 ? abs(dx) : -abs(dx);
   		else
   			dy = ty;

#ifdef MDEBUG
    	printf("i=%d tx=%d dx=%d dy=%d %.3f tw=%d nSkip=%d\n", nFrame, tx, dx, dy, maxVal, tw, nSkip);
        cv::imshow("matcur", matbin);
        cv::imshow("matpre", matpre);
        MyRectI::showRect(matbin, tc, "tc");
#endif

#ifdef SAVELOG
	    std::lock_guard<std::mutex> muto(log_mutex);
        FILE* logFile=fopen("log0415.txt", "a+");
    	fprintf(logFile, "%3d %3d %3d %.3f %d %d %d\n", nFrame, dx, dy, maxVal, tw, sumdx, nSkip);
		fclose(logFile);
#endif

        // 防止极慢速误差累积
        if(nFrame++ > 2)
        {
			if(abs(dx) < 5 && abs(dy) < 3 && nSkip < 10){
				nSkip++;
				return maxVal;
			}
			else
				nSkip = 0;
		}

		matbin.copyTo(matpre);

        gci.gVs.push_back({dx, dy, (float)maxVal, matsrc.clone()});

		// 拉直 要求扫的时候不能倾斜角太大
       // if(dy > 1) dy=1;
       // else if(dy < -1) dy=-1;
        gciStraight.gVs.push_back({dx, dy/4, (float)maxVal, matsrc.clone()});

        return maxVal;
    }
};
 
