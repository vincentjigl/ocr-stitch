class MyProc
{
public:
	MyProc(void){}
	~MyProc(void){}

    static int detectBlob(cv::Mat matImg, vector<KeyPoint> &keypoints)
    {
        SimpleBlobDetector::Params params;
        // params.blobColor = 255; //表示只提取黑色斑点；如果该变量为255，表示只提取白色斑点
        //阈值控制
        params.thresholdStep = 5; //二值化的阈值步长，即公式1的t
        params.minThreshold = 5;
        params.maxThreshold = 255;
        //像素面积大小控制
        params.filterByArea = true;
        params.minArea = 60;    // 50
        //形状（凸）
        params.filterByCircularity = true;  //斑点圆度的限制变量，默认是不限制
        params.minCircularity = 0.6;        //斑点的最小圆度
        //形状（凹）
        params.filterByConvexity = true;    //斑点凸度的限制变量
        params.minConvexity = 0.8;          //斑点的最小凸度
        //形状（园）
        params.filterByInertia = true;     //斑点惯性率的限制变量
        params.minInertiaRatio = 0.3f;       //斑点的最小惯性率
        
        Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
        detector->detect(matImg,keypoints);

        return 0;
    }

	// 平均哈希算法  要求灰度图
	// 不相同的数据位数不超过5，就说明两张图像很相似；如果大于10，就说明这是两张不同
	static int aHash(Mat matSrc1, Mat matSrc2)
	{
		Mat matDst1, matDst2;

		//imshow("matSrc1", matSrc1);
		//imshow("matSrc2", matSrc2);

		cv::resize(matSrc1, matDst1, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
		cv::resize(matSrc2, matDst2, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);

		// 转灰度 要求灰度图
		//cv::cvtColor(matDst1, matDst1, CV_BGR2GRAY);
		//cv::cvtColor(matDst2, matDst2, CV_BGR2GRAY);

		int iAvg1 = 0, iAvg2 = 0;
		int arr1[64], arr2[64];

		for (int i = 0; i < 8; i++)
		{
			uchar* data1 = matDst1.ptr<uchar>(i);
			uchar* data2 = matDst2.ptr<uchar>(i);
			
			int tmp = i * 8;

			for (int j = 0; j < 8; j++)
			{
				int tmp1 = tmp + j;

				arr1[tmp1] = data1[j] / 4 * 4;
				arr2[tmp1] = data2[j] / 4 * 4;

				iAvg1 += arr1[tmp1];
				iAvg2 += arr2[tmp1];
			}
		}

		iAvg1 /= 64;
		iAvg2 /= 64;

		for (int i = 0; i < 64; i++)
		{
			arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
			arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
		}

		int iDiffNum = 0;

		for (int i = 0; i < 64; i++)
			if (arr1[i] != arr2[i])
				++iDiffNum;

		return iDiffNum;
	}

	// 感知哈希算法  要求灰度图
	// DCT计算后的矩阵是32 * 32
	static int pHash(Mat matSrc1, Mat matSrc2)
	{
		Mat matDst1, matDst2;
		cv::resize(matSrc1, matDst1, cv::Size(32, 32), 0, 0, cv::INTER_CUBIC);
		cv::resize(matSrc2, matDst2, cv::Size(32, 32), 0, 0, cv::INTER_CUBIC);

		// 转灰度 要求灰度图
		//cv::cvtColor(matDst1, matDst1, CV_BGR2GRAY);
		//cv::cvtColor(matDst2, matDst2, CV_BGR2GRAY);

		matDst1.convertTo(matDst1, CV_32F);
		matDst2.convertTo(matDst2, CV_32F);
		dct(matDst1, matDst1);
		dct(matDst2, matDst2);

		int iAvg1 = 0, iAvg2 = 0;
		int arr1[64], arr2[64];

		for (int i = 0; i < 8; i++)
		{
			uchar* data1 = matDst1.ptr<uchar>(i);
			uchar* data2 = matDst2.ptr<uchar>(i);

			int tmp = i * 8;

			for (int j = 0; j < 8; j++)
			{
				int tmp1 = tmp + j;

				arr1[tmp1] = data1[j];
				arr2[tmp1] = data2[j];

				iAvg1 += arr1[tmp1];
				iAvg2 += arr2[tmp1];
			}
		}

		iAvg1 /= 64;
		iAvg2 /= 64;

		for (int i = 0; i < 64; i++)
		{
			arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
			arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
		}

		int iDiffNum = 0;

		for (int i = 0; i < 64; i++)
			if (arr1[i] != arr2[i])
				++iDiffNum;

		return iDiffNum;
	}

	// dHash算法

	// 平均结构相似性 MSSIM 算法  要求图像>11*11 要求灰度图或彩色
	static double getMSSIM(const Mat i1, const Mat i2)
	{
		const double C1 = 6.5025, C2 = 58.5225;
		/***************************** INITS **********************************/
		int d = CV_32F;

		Mat I1, I2;
		i1.convertTo(I1, d);           // cannot calculate on one byte large values
		i2.convertTo(I2, d);

		imshow("i1", i1);
		imshow("i2", i2);

		Mat I2_2 = I2.mul(I2);        // I2^2
		Mat I1_2 = I1.mul(I1);        // I1^2
		Mat I1_I2 = I1.mul(I2);        // I1 * I2

		/*************************** END INITS **********************************/

		Mat mu1, mu2;   // PRELIMINARY COMPUTING
		GaussianBlur(I1, mu1, Size(11, 11), 1.5);
		GaussianBlur(I2, mu2, Size(11, 11), 1.5);

		Mat mu1_2 = mu1.mul(mu1);
		Mat mu2_2 = mu2.mul(mu2);
		Mat mu1_mu2 = mu1.mul(mu2);

		Mat sigma1_2, sigma2_2, sigma12;

		GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
		sigma1_2 -= mu1_2;

		GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
		sigma2_2 -= mu2_2;

		GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
		sigma12 -= mu1_mu2;

		///////////////////////////////// FORMULA ////////////////////////////////
		Mat t1, t2, t3;

		t1 = 2 * mu1_mu2 + C1;
		t2 = 2 * sigma12 + C2;
		t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

		t1 = mu1_2 + mu2_2 + C1;
		t2 = sigma1_2 + sigma2_2 + C2;
		t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

		Mat ssim_map;
		divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

		Scalar mssim = mean(ssim_map); // mssim = average of ssim map
		return (mssim[0] + mssim[1] + mssim[2])/3;
	}

	static int cluster(MyPointArray &mpa, vector<vector<int> > &vClass, vector<cv::Point2d> &vCenter, double th, bool bDebug=0)	// 定义合并的距离,当大于这个距离时不再合并
	{
		int nLen = mpa.nLen;

		// 1 初始化：
		// (1) 类别向量vClass	初始时每个元素为一个类别  存放的是源数据的地址
		vClass.resize(nLen);     //指定元素数目，此处会有其他函数的调用，构造函数，拷贝等。（不够高效）
		for (int i=0; i < nLen; i++)
			vClass[i].push_back(i);

		// (2) 聚类中心向量vCenter	均值或质心
		for (int i=0; i < nLen; i++)
			vCenter.push_back(cvPoint2D32f(mpa.data[i].x, mpa.data[i].y));
		if(nLen < 2) return 0;

		// (3) 距离矩阵M(上三角阵)
		vector<double> vdist(nLen*nLen, 0);
		mpa.mutualDist(vdist);
		MyArray<double> mdist(vdist, nLen*nLen);

		// (4) 初始时,每个元素为一类,循环每次将距离最小的两类合并
		int m,n, classNum=vCenter.size(); 
		double minVal=mdist.minDataUp(m, n, classNum, classNum, nLen);
		while (minVal < th){
			//	(1)	后面合并到小序号(m<n), 更新m类的聚类质心
			double sumX=0, sumY=0;
			vClass[m].insert(vClass[m].end(), vClass[n].begin(), vClass[n].end());
			for(vector<int>::iterator it=vClass[m].begin(); it!=vClass[m].end(); it++)
			{
				sumX += mpa.data[*it].x;
				sumY += mpa.data[*it].y;
			}
			vCenter[m].x = sumX/vClass[m].size();
			vCenter[m].y = sumY/vClass[m].size();

			//	(2)	在类别向量中删除n类, 在聚类中心向量中删除n元素
			vClass[n].clear();
			vClass.erase(vClass.begin()+n);	// 因为是上三角阵(列>行), 所以取列(准备删除列,因为列号大,合并到行)
			vCenter.erase(vCenter.begin()+n);
			if(vCenter.size() < 2) break;

			//	(3)	在距离矩阵M中，删除n列和n行
			mdist.remove(n, n, classNum, classNum,nLen);
			classNum--;

			//	(4)	在距离矩阵M中，更新m对其他类的距离
			for (int i=0; i < m; i++)		// 小于m的为行更新
				mdist.data[i*nLen+m] = MyTools_cv::distance(vCenter[i], vCenter[m]);
			for (int j=m+1; j < classNum; j++)	// 大于m的为列更新
				mdist.data[m*nLen+j] = MyTools_cv::distance(vCenter[m], vCenter[j]);

			//2 在距离矩阵中找最小的距离两类m和n
			minVal = mdist.minDataUp(m, n, classNum, classNum, nLen);
		}

		if(bDebug)
		{
			for (int i=0; i < vClass.size(); i++)
			{
				for(vector<int>::iterator it=vClass[i].begin(); it!=vClass[i].end(); it++)
					printf("%2d:%d,%d ", *it, mpa.data[*it].x, mpa.data[*it].y);//printf("%d ", *it);//
				printf(" Class Center: %5.2f,%5.2f\n", vCenter[i].x, vCenter[i].y);
			}
		}

		return 0;
	}

	// 已聚类的矩形合并vCluster, 每类一个矩形	 vClass为地址
	static void mergeClusterRect(vector<cv::Rect> &vRect, vector<uchar> &vBVal, vector<vector<int> > &vClass,
		vector<cv::Rect> &vCluster, vector<uchar> &vCVal)
	{
		int n = vClass.size();

		for (int i=0; i < n; i++)
		{
			int x1=INT_MAX, y1=INT_MAX, x2=0, y2=0, maxV=0;
			for (int j=0; j < vClass[i].size(); j++)
			{
				int k = vClass[i][j];
				if(vRect[k].x < x1) x1 = vRect[k].x;
				if(vRect[k].y < y1) y1 = vRect[k].y;
				if(vRect[k].x+vRect[k].width > x2) x2 = vRect[k].x+vRect[k].width;
				if(vRect[k].y+vRect[k].height > y2) y2 = vRect[k].y+vRect[k].height;
				if(vBVal[k] > maxV) maxV=vBVal[k];
			}
			vCluster.push_back(cvRect(x1,y1,x2-x1,y2-y1));
			vCVal.push_back(maxV);
		}
	}

	void sortClass(vector<vector<int> > &vClass)
	{
		for (int i=0; i < vClass.size(); i++)
		{
			bool bFlag=true;
			for (int j=0; j < vClass.size()-1-i; j++)
			{
				if(vClass[j].size() < vClass[j+1].size())
				{
					vClass[j].swap(vClass[j+1]);
					bFlag = false;
				}
			}
			if(bFlag) break;
		}
	}

	// 4. 同一位置n次报警判断 同一位置n次报警保留,其余去除
	static int checkAlarmSum(vector<cv::Rect> &vRect, vector<uchar> &vCVal, int nAlarmFrequency, double th,
								int* vrbuf, int ACC_FRAME_DIFF_SIZE, int &pa)
	{
		int i, g=pa;
		//int *vrbuf = vCluster2 + ACC_FRAME_DIFF_SIZE*4;	// 缓存buff 历史结果矩形框
		int *vrpa = vrbuf + g*ACC_FRAME_DIFF_SIZE*4;	// 当前结果矩形框要存放的地址
		pa = (pa+1) % nAlarmFrequency;					// 下一帧要放入的编号

		for (i=0; i < vRect.size(); i++)
		{
			vrpa[i*4]   = vRect[i].x;
			vrpa[i*4+1] = vRect[i].y;
			vrpa[i*4+2] = vRect[i].width;
			vrpa[i*4+3] = vRect[i].height;
		}
		vrpa[i*4+2] = 0;

		for (int k=vRect.size()-1; k >= 0; k--)
		{
			int nAlarm=0;
			int area1 = vRect[k].width*vRect[k].height;
			for (int i=1; i < nAlarmFrequency; i++)	// 第i帧
			{
				g = (g+1) % nAlarmFrequency;
				int *p = vrbuf + g*ACC_FRAME_DIFF_SIZE*4;
				for (int j=0; j < ACC_FRAME_DIFF_SIZE; j++) // 第i帧第j个框
				{
					cv::Rect cr(p[j*4], p[j*4+1], p[j*4+2], p[j*4+3]);
					if(cr == vRect[k] || cr.width == 0) break;

					int area2 = cr.width * cr.height;
					area1 = (area1+area2)/2;	// cr = min(cr, area2);

					// 相交区域的面积
					float interArea = MyRectD::getIntersectArea(vRect[k], cr);

					// 交叠区面积与较小矩形面积之比 > 0.5 时合并
					if(interArea/area1 > th)
					{
						nAlarm++;
						break;
					}
				}
			}

			if(nAlarm < nAlarmFrequency-1)	// 如果在历史帧中每帧有报了就保留,否则去除
				vRect.erase(vRect.begin()+k);
		}

		return 0;
	}

	// 桶颜色计数法
    // 使用多个颜色作为背景不成功，问题是噪声和人颜色范围大颜色数太多，总是会显示，同时火抖动颜色数却较少，被当作了背景。 2020-05-25 Monday 11:06:56
	static int bgbinproc(cv::Mat matsrc, cv::Mat &matdiff, float fUpdateRate=9, int maxCnt=8888, int thColor=8, bool bDebug=false)
	{
//	    float fUpdateRate=9;   // 背景更新速率 1~9
//	    int   maxCnt=8888;     // 颜色最大计数 999~9999减小可使背景更新加快
	    const int  NBIN=11;          // 背景颜色桶个数
        bool btmp=0;           // 当执行到某点和颜色不在背景色中时，显示测试图片

		// 单点测试
	    bool  bDebugPoint=0;
	    int   mcp_tx=244, mcp_ty=151;

	    cv::Mat matgray;
    	cv::cvtColor(matsrc, matgray, cv::COLOR_BGR2GRAY);
    	blur(matgray, matgray, Size(3,3));

	    static cv::Mat matfgp = cv::Mat::zeros(matgray.size(),CV_32FC1);
	    static cv::Mat vMatCnt[NBIN]={matfgp.clone(),matfgp.clone(),matfgp.clone(),matfgp.clone(),matfgp.clone(),matfgp.clone(),matfgp.clone(),matfgp.clone(),matfgp.clone(),matfgp.clone(),matfgp.clone()};
	    static cv::Mat vMatbgc[NBIN]={matgray.clone(),matgray.clone(),matgray.clone(),matgray.clone(),matgray.clone(),matgray.clone(),matgray.clone(),matgray.clone(),matgray.clone(),matgray.clone(),matgray.clone()};

        cv::Mat matdst=cv::Mat::zeros(matgray.size(), CV_8UC1);

	    // 下一步
	    // 考虑：根据视频的分块亮度变化，来改变更新速率
	    // 考虑：作滤波，中值，均值滤波
	    //      当一个点颜色数大于一个较大值后，都作为背景。思路是取多个背景色。排除小旗子。

	    for (int i=0; i < matgray.rows; i++)
	    {
	        float *t[NBIN];
	        uchar *q[NBIN];
	        uchar *p=matgray.ptr<uchar>(i);
            uchar *pdst=matdst.ptr<uchar>(i);
	        for (int k=0; k < NBIN; ++k)
	        {
	            q[k] = vMatbgc[k].ptr<uchar>(i);    // 10帧背景颜色值
	            t[k] = vMatCnt[k].ptr<float>(i);    // 10帧背景颜色计数 从小到大排序
	        }

	        for (int j=0; j < matgray.cols; j++)
	        {
	            if(bDebugPoint && j==mcp_tx && i==mcp_ty)
	                outInt1(p[j]);

	            // 更新背景颜色计数
	            int k;
	            for (k=NBIN-1; k >= 0; --k)
	            {
	                uchar b = abs(p[j] - q[k][j]);
	                if(b < thColor) // 找到当前颜色所在桶号为k
	                {
	                    // 桶计数更新
	                    if(t[k][j] < maxCnt)
	                    {
	                    	double fWeight = 0.01+std::pow(k/9.0, fUpdateRate);
	                        t[k][j] += fWeight; //0.01+std::pow(k/9.0, fUpdateRate);
	                        // if(t[k][j] < 5)
                         //            pdst[j]=255;
                            for(int e=k; e < NBIN-1; e++)    // sort count
	                        {
                                if(t[e][j] > t[e+1][j])
	                            {
	                                MYSWAP_INT(q[e][j], q[e+1][j]);
	                                MYSWAP_DOUBLE(t[e][j], t[e+1][j]);
	                            }
	                            else break;
	                        }
	                    }

	                    // 当桶计数大于最大计数值时, all color*0.8
	                    else
	                    {
	                        for(int e=0; e < NBIN; e++)
	                            t[e][j] *= 0.5;
	                    }

	                    break;
	                }
	            }

	            // 如果10帧中没有此色，从第一个加入。
	            if(k < 0)
	            {
	                q[0][j] = p[j];
	                t[0][j] = 1;
                    for(int e=0; e < NBIN-1; e++)    // sort count
                    {
                        if(t[e][j] > t[e+1][j])
                        {
                            MYSWAP_INT(q[e][j], q[e+1][j]);
                            MYSWAP_DOUBLE(t[e][j], t[e+1][j]);
                        }
                        else break;
                    }
                    pdst[j]=255;
	            }
                
	            if(bDebugPoint && j==mcp_tx && i==mcp_ty)
	            {
                    if(k < 0)
                        btmp=1;
	                for(int k=0; k < NBIN; ++k)
	                    printf("%7d", q[k][j]);
	                outln();
	                for(int k=0; k < NBIN; ++k)
	                    printf("%6.2f ", t[k][j]);
	            }

	        }
	    }

        // 背景减除
	    cv::absdiff(matgray, vMatbgc[NBIN-1], matdiff);

	    if(bDebug)
	    {
	    	if(bDebugPoint){
	    		cv::circle(matgray, cvPoint(mcp_tx,mcp_ty), 8, CV_RGB(0,0,0));
                cv::circle(matdst, cvPoint(mcp_tx,mcp_ty), 8, CV_RGB(111,111,111));
	    		// cv::circle(vMatbgc[9], cvPoint(mcp_tx,mcp_ty), 8, CV_RGB(255,255,255));
	    	}
            // cv::line(matgray, cvPoint(284,170), cvPoint(272,237), CV_RGB(0,0,0));
            imshow("matdst", matdst);
			imshow("matgray", matgray);
	    	imshow("vMatbgc[NBIN-1]", vMatbgc[NBIN-1]);
            if(btmp)
            {
                cv::waitKey();
                btmp=0;
            }
	    }

	    return 0;
	}

};
