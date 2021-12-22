
struct StImage
{
    int x, y;
    float matchScore;
    cv::Mat matImg;
};

class CoImage
{
public:
    // 已拼接图片信息
    int ps;         	// 已拼接张数
    int sminx, sminy, smaxx, smaxy, sdx, sdy;
    int rightMode;
    cv::Mat matstt;  	// 已拼接图片

    std::vector<StImage> gVs;

    // 多行拼接使用
    int gLastW, gLastH;	// (不含最后一行)宽高
    int gW, gH;			// (含最后一行)宽高 所有图片
    int pLastLine;		// 最后一行起始序号 在gVs的序号
    int lineIDpre;		// 最后一行图片行号 文字行号

    CoImage(void){
        ps = 0;         // 已拼接张数
        gW = 0;
        gH = 0;
        sminx = 0;
        sminy = 0;
        smaxx = 0;
        smaxy = 0;
        sdx = 0;
        sdy = 0;
        rightMode = 1;
        cv::Mat mattmp(2,2,CV_8UC1);
        matstt = mattmp.clone();
    };

    ~CoImage(void){};

    void clear()
    {
        ps = 0;         // 已拼接张数
        sminx = 0;
        sminy = 0;
        smaxx = 0;
        smaxy = 0;
        sdx = 0;
        sdy = 0;
        gVs.clear();

        cv::Mat mattmp(2,2,CV_8UC1);
        matstt = mattmp.clone();
    }

    int getMeanColor()
    {
    	if(gVs.size() > 0)
    	{
    		int i = gVs.size()/2;
    		Scalar m = mean(gVs[i].matImg);
    		return m[0];
    	}

    	return 128;
    }

    // 左侧融合拼接
    int mixCopyLeft(cv::Mat matdst, cv::Mat matsrc, int th)
    {
    	if(th > 18)
    		th = 18;

    	for (int i=0; i < matdst.rows; i++)
		{
			uchar* p = matsrc.ptr<uchar>(i);
			uchar* q = matdst.ptr<uchar>(i);
			#pragma omp parallel for num_threads(4)
			for (int j=0; j < matdst.cols; j++)
			{
				if(j < th)
					q[j] = (q[j]*(th-j) + p[j]*j)/th;
				else
					q[j] = p[j];
			}
		}

    	return 0;
    }

    // 右侧融合拼接
	int mixCopyRight(cv::Mat matdst, cv::Mat matsrc, int th)
    {
    	int w = matdst.cols;
		if(th > 18)
    		th = 18;
    	
		#pragma omp parallel for num_threads(4)
    	for (int i=0; i < matdst.rows; i++)
		{
			uchar* p = matsrc.ptr<uchar>(i);
			uchar* q = matdst.ptr<uchar>(i);
			for (int j=0; j < w-th; j++)
				q[j] = p[j];
			p += w - th;	// 重合部分坐标开始 src
			q += w - th;	// 重合部分坐标开始 dst
			for (int j=0; j < th; j++)
				q[j] = (q[j]*j + p[j]*(th-j))/th;
		}

    	return 0;
    }

    // 融合拼接图片 从i继续拼接到k
    int mergeImages(int i, int k, int &minx, int &miny, int &maxx, int &maxy, int &dx, int &dy, cv::Mat &matdst)
    {
        int width = gVs[0].matImg.cols;
        int height = gVs[0].matImg.rows;
        int ax=0, ay=0, dxold=0, dyold=0;
        cv::Mat matd = matdst.clone();

        if(!matd.empty())
        {
            ax = -minx;
            ay = -miny;
            dxold = dx;
            dyold = dy;
        }

        // 去掉最后的拼接误差大的
        if(k > 28)
        {
            while(k > i && gVs[k-1].matchScore < 0.8) k--;
            if(k == i) return k;
        }

        // 计算图片偏移xy最大最小值
        for(int j=i; j < k; j++)
        {
            dx += gVs[j].x;
            dy += gVs[j].y;
            if(dx < minx)
                minx = dx;
            if(dx > maxx)
                maxx = dx;
            if(dy < miny)
                miny = dy;
            if(dy > maxy)
                maxy = dy;
        }

        // 根据各图坐标合并
        cv::Scalar mean = cv::mean(gVs[0].matImg);
        int mb = mean.val[0];
        int nx = -minx;
        int ny = -miny;
        cv::Mat mattmp(maxy - miny + height, maxx - minx + width, CV_8UC1, Scalar(mb)); // 要求向量中各图尺寸相同
        if(!matd.empty())
        {
            matdst.copyTo(mattmp(cvRect(nx-ax, ny-ay, matdst.cols, matdst.rows)));
            nx += dxold;
            ny += dyold;
        }
        matdst = mattmp;

        // 原图所有像素+2, 0为底色, 1为未使用
        if(i < 1){
        	cv::Mat src;
	        nx += gVs[0].x;
	        ny += gVs[0].y;
            cv::resize(gVs[0].matImg(cvRect(0,1,width,height-3)), src, cvSize(width,height));	// 去掉上下黑脚
            src += 2;
            src.copyTo(matdst(cvRect(nx, ny, width, height)));
            i = 1;
        }

        for(int j=i; j < k; j++)
        {
        	cv::Mat src;
            nx += gVs[j].x;
            ny += gVs[j].y;
            cv::resize(gVs[j].matImg(cvRect(0,1,width,height-3)), src, cvSize(width,height));	// 去掉上下黑脚
            if(rightMode)
	            mixCopyLeft((matdst(cvRect(nx, ny, width, height))), src+2, width - gVs[j].x);	// 右手模式
	        else
	            mixCopyRight((matdst(cvRect(nx, ny, width, height))), src+2, width + gVs[j].x);	// 左手模式
        }

        matdst.at<uchar>(0,0) = mb;

        return k;
    }

    // 有新的图片产生，接续拼接所有图片   return:当前缓存图片数
    int mergeAllImages()
    {
        int ret = 0;
        int k = gVs.size();

        if(k > ps)
        {
            k = mergeImages(ps, k, sminx, sminy, smaxx, smaxy, sdx, sdy, matstt);
            ps = k;
            ret = 1;
        }

        return k;
    }

    // 拼接部分图片 从指定序号到末尾
    int mergeSubImages(int i, cv::Mat &matdst)
    {
        int k = gVs.size();
        int minx = 0, miny = 0, maxx = 0, maxy = 0, dx = 0, dy = 0;
        
        k = mergeImages(i, k, minx, miny, maxx, maxy, dx, dy, matdst);

        return k;
    }

    
    // 拼接部分图片 到末尾指定宽度
    int mergeSubImages2(cv::Mat &matdst, int width)
    {
        int dx=0, dy=0;
        int minx=0, miny=0, maxx=0, maxy=0;
        int j = gVs.size() - 1;
        int w = gVs[0].matImg.cols;

        // 计算图片偏移xy最大最小值
        for(; j >= 0; j--)
        {
            dx += gVs[j].x;
            dy += gVs[j].y;
            if(dx < minx) minx = dx;
            if(dx > maxx) maxx = dx;
            if(dy < miny) miny = dy;
            if(dy > maxy) maxy = dy;
            if(maxx - minx + w > width)
                break;
        }

        if(j < 0) j = 0;
        mergeSubImages(j, matdst);

        return 0;
    }

    //////////////////// 多行拼接 ///////////////////////
    
	// 多行拼接 计算ps到最后的坐标
	int calcLastLineOrd()
	{
    	int maxW=0;
    	int maxH=0;
    	int n=gVs.size();

		// 计算新行的行高
    	for(int i=pLastLine; i < n; i++)
    	{
    		if(gVs[i].matImg.rows > maxH)
    			maxH = gVs[i].matImg.rows;
    	}

    	// 计算在大图的坐标
    	gVs[pLastLine].x = 0;
		gVs[pLastLine].y = gH + (maxH - gVs[pLastLine].matImg.rows)/2;
    	for(int i=pLastLine+1; i < n; i++)
    	{
    		gVs[i].x = gVs[i-1].x + gVs[i-1].matImg.cols;
    		gVs[i].y = gH + (maxH - gVs[i].matImg.rows)/2;
    	}

    	maxW = gVs[n-1].x + gVs[n-1].matImg.cols;
    	if(maxW > gW)	gW = maxW;

    	gH += maxH;
    	
		return 0;
	}

    int multi_Line_set(vector<cv::Mat> &vMatLin, int lineID)
    {
    	static int lineIDpre = 0;
    	vector<cv::Mat> vMatLn;

    	// 判断输入图片是否为空
    	for(int i=0; i < vMatLin.size(); i++)
    	{
    		if(!vMatLin[i].empty())
	        	vMatLn.push_back(vMatLin[i]);
    	}
    	if(vMatLn.empty()) return -1;

    	// 覆盖旧行 清除最后一行信息
    	if(lineID == lineIDpre)
    	{
    		gVs.erase(gVs.begin() + pLastLine, gVs.end());
			gW = gLastW;
			gH = gLastH;
    		// 若已拼接 更新拼接张数信息
    		if(ps > gVs.size())
    		{
    			matstt = matstt(cvRect(0,0,gLastW,gLastH));
    			ps = gVs.size();
    		}
    	}
    	else
    	{
    		gLastW = gW;
    		gLastH = gH;
			pLastLine = gVs.size();
	    	lineIDpre = lineID;
    	}

		// 添加当前行 计算坐标
		for(int i=0; i < vMatLn.size(); i++)
	        gVs.push_back({0, 0, 0, vMatLn[i].clone()});

		calcLastLineOrd();

    	return 0;
    }

    // 拼接图片 从ps序号开始拼接 坐标已计算绝对坐标(单行拼接是相对坐标)
    int mergeImages(cv::Mat &matdst)
    {
    	if(ps < gVs.size() && gW > 0 && gH > 0)
    	{
	    	matdst = cv::Mat::zeros(gH, gW, CV_8UC1);
	    	if(!matstt.empty())
	    		matstt.copyTo(matdst(cvRect(0,0,matstt.cols,matstt.rows)));
	    	matstt = matdst;

	        for(int j=ps; j < gVs.size(); j++)
	        {
	        	cv::Rect rc(gVs[j].x, gVs[j].y, gVs[j].matImg.cols, gVs[j].matImg.rows);
	        	if(gVs[j].matImg.channels() == 3)
					cvtColor(gVs[j].matImg, matdst(rc), CV_BGR2GRAY);
				else
		        	gVs[j].matImg.copyTo(matdst(rc));
	        }

	        ps = gVs.size();
	    }
	    else
			matdst = matstt;

    	return 0;
    }

    int multi_Line_clear()
    {
    	ps 			= 0;
    	gW 			= 0;
    	gH 			= 0;
	    gLastW		= 0;
	    gLastH 		= 0;
	    pLastLine	= 0;
	    lineIDpre	= 0;

    	gVs.clear();

		matstt.release();

    	return 0;
    }
};
