//#include <iostream>
//#include <io.h>     // _finddata_t file; linux不支持20191206
//using namespace std;


class MyCmdParam
{
public:
        string infile;
        string outfile;
        int waittime;
        int nDebug;
        int tx, ty, tw, th;         // 用于测试的输入点, 区域
        int nType;
        bool bStop;

        int     nSkip;		// 视频播放时跳过n帧
        int     nScale;
        float   fScale;
        bool    bGray;      // 彩色或黑白视频
        int     showOpt;
        int     cmOpt;		// 移动或复制标记
        string  optstr;

        // 用于分别统计结果的数目
        int nRet0;		// 计数 处理结果 = 0
        int nRet1;		// 计数 处理结果 > 0
        int nRet_1;		// 计数 处理结果 < 0

        MyCmdParam(int argc, char* argv[]){
                waittime	= 1;
                nDebug		= 0;
                tx          = 8;
                ty          = 8;
                tw          = 8;
                th          = 8;
                nType		= 0;
                nSkip		= 0;
                nScale      = 1;
                fScale      = 1;
                bGray       = 0;
                bStop       = false;
                showOpt		= INT_MAX;
                cmOpt		= INT_MAX;

                parsecmd(argc, argv);

                nRet0=0, nRet1=0, nRet_1=0;
        };

        MyCmdParam(void){
                waittime	= 1;
                nDebug		= 0;
                tx          = 8;
                ty          = 8;
                tw          = 8;
                th          = 8;
                nType		= 0;
                nSkip		= 0;
                nScale      = 1;
                fScale      = 1;
                bGray       = 0;
                bStop       = false;
                showOpt		= INT_MAX;
                cmOpt		= INT_MAX;
                showOpt		= INT_MAX;
                cmOpt		= INT_MAX;
        }

        ~MyCmdParam(void){}

        void parsecmd(int argc, char **argv)
        {
                int copyOpt= INT_MAX, moveOpt= INT_MAX;

                if(argc < 2) return;

                // 使用说明
                if(argc < 3 || !strcmp(argv[1],"-h") || !strcmp(argv[1],"--h") || !strcmp(argv[1],"-help") || !strcmp(argv[1],"--help"))
                {
                        printf("usage: LineDectection.exe [-i 图片文件名或路径] [-m 是否移动图片] [-w 显示图片时间]\n");
                        exit(0);
                }

                // 命令参数解析
                for (int i=1; i < argc; i++)
                {
                    if(!strcmp(argv[i], "-i"))			// 输入图片路径或文件名	-i 文件或路径
                            infile = argv[++i];
                    else if(!strcmp(argv[i], "-w"))		// 显示图像等待时间		-wait 0 需配合-debug 1使用
                            waittime = atoi(argv[++i]);
                    else if(!strcmp(argv[i], "-d"))		// 调试 输出更多信息		-debug 0
                            nDebug = atoi(argv[++i]);
                    else if(!strcmp(argv[i], "-type"))	// 需要的类型/模型信息	-type 0
                            nType = atoi(argv[++i]);

                    // 根据图片处理结果操作, 参数: -1 小于0时, 0 等于0时, 1 大于0时
                    else if(!strcmp(argv[i], "-break"))	// 暂停显示,
                            showOpt = atoi(argv[++i]);
                    // else if(!strcmp(argv[i], "-scale"))	// 整数。 缩放大小: <0 缩小到1/n倍, >0 放大n倍
                    //         nScale = atoi(argv[++i]);
                    else if(!strcmp(argv[i], "-scale")) // 整数。 缩放大小: <0 缩小到1/n倍, >0 放大n倍
                        fScale = atof(argv[++i]);
                    else if(!strcmp(argv[i], "-gray"))  // 是否灰度图 1 是， 0 否。
                            bGray = atoi(argv[++i]);
                    else if(!strcmp(argv[i], "-skip"))  // 跳过n帧图片
                            nSkip = atoi(argv[++i]);
                    else if(!strcmp(argv[i], "-cp"))	// 复制图片, 复制到文件夹log0,log1,log-1
                            copyOpt = atoi(argv[++i]);
                    else if(!strcmp(argv[i], "-mv"))	// 移动图片, 移动到文件夹log0,log1,log-1
                            moveOpt = atoi(argv[++i]);
                    else if(!strcmp(argv[i], "-rc"))  // 输入测试点坐标
                    {
                            tx = atoi(argv[++i]);
                            ty = atoi(argv[++i]);
                            tw = atoi(argv[++i]);
                            th = atoi(argv[++i]);
                    }
                }

                // 复制或移动图片  1 返回结果为>0时, -1 返回结果为<0时, 0 返回结果为0时, 其他无操作
                if(copyOpt != INT_MAX)
                {
                        optstr = "copy \"";
                        cmOpt = copyOpt;
                }
                else if(moveOpt != INT_MAX)
                {
                        optstr = "move \"";
                        cmOpt = moveOpt;
                }

                if(cmOpt != INT_MAX)
                {
                        // system("md log0");
                        // system("md log1");
                        // system("md log-1");
                }
        }

        void optFile(int ret, string filename)
        {
                string cmdstr="";
                if(ret == 0)			// ret=0 移动到 log0 文件夹
                {
                        nRet0++;
                        if(showOpt == 0) waitKey(0);
                        if(cmOpt == 0)
                                cmdstr = optstr + filename + "\" log0";
                }
                else if(ret > 0)		// ret>0 移动到 log1 文件夹
                {
                        nRet1++;
                        if(showOpt == 1) waitKey(0);
                        if(cmOpt == 1)
                                cmdstr = optstr + filename + "\" log1";
                }
                else if(ret < 0)		// ret<0 移动到 log-1 文件夹
                {
                        nRet_1++;
                        if(showOpt == -1) waitKey(0);
                        if(cmOpt == -1)
                                cmdstr = optstr + filename + "\" log-1";
                }

                if(!cmdstr.empty())
                {
                        system(cmdstr.c_str());
                        printf("Image is copied! ret = %d.\n", ret);
                }

                if(waittime >= 0)
                        cv::waitKey(waittime);
                outInt3(nRet0, nRet1, nRet_1);
        }
};

class MyFile
{
    typedef int(*imgProcCall)(cv::Mat&, int);  // 函数指针
    typedef int(*imgProcCall2)(int, int, uchar*, int*, int);  // 函数指针

public:
	MyFile(void){};
	~MyFile(void){};

	// 判断是否图片文件文件名
	static bool isImageFilename(string filename)
	{
		int p = filename.rfind('.');
		if(p < 0) return false;

		string suff = filename.substr(p);
		if(!suff.compare(".jpg") ||
			!suff.compare(".jpeg") ||
			!suff.compare(".bmp") ||
			!suff.compare(".png"))	return true;
		return false;
	}

	// 判断是否视频文件文件名
	static bool isVideoFilename(string filename)
	{
		int p = filename.rfind('.');
		if(p < 0) return false;

		string suff = filename.substr(p);
		if(!suff.compare(".mp4") ||
			!suff.compare(".wmv") ||
			!suff.compare(".ts"))	return true;
		return false;
	}

	// 获取目录下所有文件
	static char getFilesInPath(std::vector<string>& vfilesname, const char* fileDir)
	{
//		int  n = 0;
/*		long lf;
		_finddata_t file;
		char picPath[1024];

		sprintf_s(picPath, 1024, "%s\\*.*", fileDir);
		if ((lf = _findfirst(picPath, &file)) == -1l)
		{
			//printf("文件没有找到!\n");
			return 0;
		}

		// 遍历目录图片测试
		//printf("\n文件列表:\n");
		while (_findnext(lf, &file) == 0)
		{
			if (file.attrib != _A_ARCH && file.attrib != 33) continue;
			//if (n++ < 0) continue;

			//printf("\n%d %s", n, file.name);
			sprintf_s(picPath, 1024, "%s\\%s", fileDir, file.name);
			vfilesname.push_back(picPath);
		}

		_findclose(lf);
*/
		return 0;
	}


static void runVideo2(imgProcCall ipc, string infile, float fScale=1, bool bGray=0, int nSkip=0, int maxFrame=INT_MAX, int waitTime=1, bool bShowFrame=false, bool bDebug=0)
        {
                int nError=0;
                int framePos=nSkip;
                cv::Mat frame;
        
                // printf("open video file failed 253\n!");
                // return;

                VideoCapture videoCap(infile);
//      if(framePos > 0)
//          //videoCap.set(CV_CAP_PROP_POS_FRAMES, framePos);   // 跳到指定帧
//          videoCap.set(CV_CAP_PROP_POS_MSEC, framePos*1000);  // 跳到指定时间ms

                if(!videoCap.isOpened())
                {
                        printf("open video file failed 263\n!");
                        return;
                }

                if(maxFrame == 0) maxFrame=INT_MAX;
                if(waitTime < 0) waitTime = 1;  // 默认连续播放

                for(int i=0; i < maxFrame; i++)
                {
                        videoCap >> frame;
                        if(!frame.data)
                        {
//              if(nError++ > 0) break;
//              printf("Error:%d ", nError);
//              continue;
                                printf("video end.\n");
                                break;
                        }

                        // if(i%100 == 0)   printf("iFrame=%d\n",i);
                        // if(i%2 > 0) continue;    // 隔两帧
                        if(i < framePos) continue;  // 跳到指定帧
                        printf("iFrame=%d ",i);

                        cv::resize(frame, frame, cvSize(0,0), fScale, fScale);
                        if(bGray)
                            cvtColor(frame, frame, CV_BGR2GRAY);
                        if(bShowFrame)
                                imshow("frame",frame);

                        ipc(frame, bDebug);

                        printf("\n");
                        int nkey=waitKey(waitTime);
                        if(nkey == ' ')                     // 空格键暂停
                                nkey = waitKey(0);
                        if(nkey == 's' || nkey == 'S')      // 保存图片 也暂停
                        {
                                char buf[64];
                                time_t t = time(NULL); //获取目前秒时间
                                tm* local = localtime(&t); //转为本地时间
                                strftime(buf, 64, "img%m%d%H%M%S.jpg", local);
                                imwrite(buf, frame);
                                printf("保存图片 %s\n", buf);
                                imshow(buf,frame);          // 保存后暂停显示1秒
                                waitKey(0);
                                destroyWindow(buf);
                        }
                        else if(nkey == 'b' || nkey == 'B')
                        {
                                i = i > 100 ? i-100 : 0;
                                videoCap.set(CV_CAP_PROP_POS_FRAMES, i);
                        }
                        else if(nkey == 'f' || nkey == 'F')
                        {
                                i += 100;
                                videoCap.set(CV_CAP_PROP_POS_FRAMES, i);
                        }
                        else if(nkey == 27 || nkey == 'q' || nkey == 'Q')       // 按Esc键关闭退出
                        {
                                // printf("%f %f, w=%f h=%f %f %f %f\n", videoCap.get(1), videoCap.get(2), videoCap.get(3), videoCap.get(4), videoCap.get(5), videoCap.get(6), videoCap.get(7));
                                break;
                        }
                }

                videoCap.release();
                destroyAllWindows();//destroyWindow("frame");
        }

        static void runVideo(MyCmdParam &mcp, imgProcCall2 ipc, int maxFrame=INT_MAX, bool bShow=false)
        {
                int nError=0;
                int waittime=mcp.waittime;
                int framePos=mcp.nSkip;
                int rcbuff[32];
                cv::Mat frame, matres;

                VideoCapture videoCap(mcp.infile);
                outIntStr(0, mcp.infile.c_str());

                if(!videoCap.isOpened())
                {
                        printf("open video file failed 350\n!");
                        return;
                }

                if(maxFrame == 0) maxFrame=INT_MAX;

                for(int i=0; i < maxFrame && !mcp.bStop; i++)
                {
                        videoCap >> frame;
                        if(!frame.data)
                        {
                            break;
            				if(nError++ > 8) break;
            				printf("video Error:%d\n", nError);
            				continue;
                        }

                        // if(i%100 == 0)	printf("iFrame=%d\n",i);
                        // if(i % 10 > 0) continue;	// 隔 n 帧
                        if(i < framePos) continue;	// 跳到指定帧
                        if(i > 200){
                            waitKey(0);
                            break;
                        }
                        if(i < framePos + 2) waitKey(0);
                        printf("iFrame=%d ",i);

                        cv::resize(frame, matres, cvSize(0,0), mcp.fScale, mcp.fScale);
                        if(mcp.bGray)
                            cvtColor(matres, matres, CV_BGR2GRAY);
                        if(bShow)
                            imshow("frame",matres);

                        // ipc(matres, mcp.nDebug);
                        ipc(matres.rows, matres.cols, matres.data, rcbuff, mcp.nDebug);

                        printf("\n");
                        int nkey=waitKey(waittime);
                        if(nkey == ' ' /*|| i==1400*/)						// 空格键暂停
                        	waittime=0;
                        else if(nkey == 'c' || nkey == 'C')
                        	waittime=mcp.waittime;
                        else if(nkey == 's' || nkey == 'S')		// 保存图片 也暂停
                        {
                                char buf[64];
                                time_t t = time(NULL); //获取目前秒时间
                                tm* local = localtime(&t); //转为本地时间
                                strftime(buf, 64, "img%m%d%H%M%S.jpg", local);
                                imwrite(buf, matres);
                                printf("save image: %s\n", buf);
                                imshow(buf,matres);			// 保存后暂停显示1秒
                                waitKey(0);
                                destroyWindow(buf);
                        }
                        else if(nkey == 'b' || nkey == 'B')
                        {
                                i = i > 100 ? i-100 : 0;
                                videoCap.set(CV_CAP_PROP_POS_FRAMES, i);
                        }
                        else if(nkey == 'f' || nkey == 'F')
                        {
                                i += 100;
                                videoCap.set(CV_CAP_PROP_POS_FRAMES, i);
                        }
                        else if(nkey == 27 || nkey == 'q' || nkey == 'Q')		// 按Esc键关闭退出
                        {
                                // printf("%f %f, w=%f h=%f %f %f %f\n", videoCap.get(1), videoCap.get(2), videoCap.get(3), videoCap.get(4), videoCap.get(5), videoCap.get(6), videoCap.get(7));
                                break;
                        }
                }

                outInt2(frame.cols, frame.rows);
                videoCap.release();
                destroyAllWindows();//destroyWindow("matres");
        }

		static int runImage(MyCmdParam &mcp, imgProcCall2 ipc)
		{
			// 视频文件处理
			if(isVideoFilename(mcp.infile))
			{
				if(mcp.waittime < 0)
					mcp.waittime = 1;
				// runVideo(mcp, ipc, 0, false);
				return 0;
			}

			// 得到图片列表
			std::vector<string> vfname;
			if(isImageFilename(mcp.infile))	// 单张图片处理 参数为一个图片文件
			{
				if(mcp.waittime < 0)
					mcp.waittime = 0;
				vfname.push_back(mcp.infile);
			}
			else	// 此函数仅用于windows平台			// 多张图片处理 参数为一个图片路径
				getFilesInPath(vfname, mcp.infile.c_str());
			float fCount=vfname.size();

			//LARGE_INTEGER nFreq,nBeginTime,nEndTime;// 需要#include <Windows.h>定义在MyHeadDef.h头部,不需MFC
			//QueryPerformanceFrequency(&nFreq);

			// 循环处理每张图片
			for (int i=0; i < fCount; i++)
			{
				printf("\n%d %s\n", i, vfname[i].c_str());
				Mat matSrc = imread(vfname[i].c_str());
				if(matSrc.empty())
				{
					printf("open image file failed! %s \n", vfname[i].c_str());
					continue;
				}

				// 图片处理
				//QueryPerformanceCounter(&nBeginTime);
				// int ret = ipc(matSrc, mcp.nDebug);
                // int ret = ipc(matSrc.rows, matSrc.cols, matSrc.data, mcp.nDebug);
				//QueryPerformanceCounter(&nEndTime);
				//int te = 1000.0*(nEndTime.QuadPart-nBeginTime.QuadPart)/nFreq.QuadPart;
				//printf(" %dms\n", te);

				// 统计 并根据处理结果操作图片, 移动或复制
				//mcp.optFile(ret, vfname[i]);

				int nkey=waitKey(0);
				if(nkey == 's' || nkey == 'S')		// 保存图片 也暂停
				{
				}
				else if(nkey == 27 || nkey == 'q' || nkey == 'Q')		// 按Esc键关闭退出
				{
					break;
				}
			}

			// 输出统计 正确率 误检率
// 			char chFlag = mcp.infile[mcp.infile.length()-1];
// 			if(chFlag == 'p' || chFlag == 'P')
// 				printf("正样本 Recall(TPR): %.2f   Missing Alarm(FNR): %.2f\n", mcp.nRet0/fCount, mcp.nRet1/fCount);
// 			else if(chFlag == 'n' || chFlag == 'N')
// 				printf("负样本 (TNR): %.2f   False Alarm(FPR): %.2f\n", mcp.nRet1/fCount, mcp.nRet0/fCount);
// 			else
// 				printf("nRet0: %.2f   nRet1: %.2f   nRet-1: %.2f\n", mcp.nRet0/fCount, mcp.nRet1/fCount, mcp.nRet_1/fCount);

			destroyAllWindows();
			return 0;
		}

};
