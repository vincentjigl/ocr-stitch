#pragma once

// 申请内存 设置数组长度 也是内存长度
#define MyPointArray_MALLOC(_nLen) nLen=_nLen; \
	isMallocMem=false; \
	if(nLen<1)return; \
	data=(cv::Point*)malloc(nLen*sizeof(cv::Point)); \
	if(data)isMallocMem=true

// 同上,但返回NULL
#define MyPointArray_MALLOC2(_nLen) nLen=_nLen; \
	isMallocMem=false; \
	if(nLen<1)return NULL; \
	data=(cv::Point*)malloc(nLen*sizeof(cv::Point)); \
	if(data)isMallocMem=true

// 本类全为double类型
class MyPointArray
{
public:							// 存入和取出数据时要加减nlen
	int			nLen;			// 有效数据个数,不是 d 的所占内存大小
	cv::Point*   data;
	bool		isMallocMem;	// 是否申请了内存,用释放内存的判断

	MyPointArray(void)
	{
		nLen=0;
		data=NULL;
		isMallocMem=false;
	};

	MyPointArray(int _nLen) // 初始化时为最大长度 但添加数据后则为有效数据个数,不再是最大长度 共用变量
	{
		MyPointArray_MALLOC(_nLen);
		memset(data, 0, nLen*sizeof(cv::Point));
		nLen=0;	// 有效数据个数,不是 d 的所占内存大小
	};

	// 从已有数据初始化,(x,y)按顺序排列
	MyPointArray(int *pData, int _nLen)
	{
		MyPointArray_MALLOC(_nLen);
		set(pData, _nLen);
	};

	MyPointArray(double* ptx, double* pty, int _nLen)
	{
		MyPointArray_MALLOC(_nLen);
		for (int i=0; i < _nLen; i++)
		{
			data[i].x = ptx[i];
			data[i].y = pty[i];
		}
	};

	MyPointArray(vector<cv::Point> vpt)
	{
		int _nLen = vpt.size();
		MyPointArray_MALLOC(_nLen);
		for (int i=0; i < _nLen; i++)
		{
			data[i].x = vpt[i].x;
			data[i].y = vpt[i].y;
		}
	};

	MyPointArray(vector<cv::Point2d> vpt)
	{
		int _nLen = vpt.size();
		MyPointArray_MALLOC(_nLen);
		for (int i=0; i < _nLen; i++)
		{
			data[i].x = vpt[i].x;
			data[i].y = vpt[i].y;
		}
	};

	// 复制数据到本对象
	void set(int *pData, int _nLen)
	{
		for (int i=0; i < _nLen; i++)
		{
			data[i].x = pData[i*2];
			data[i].y = pData[i*2+1];
		}
		nLen = _nLen;
	};

	~MyPointArray(void)
	{
		if(isMallocMem && data)
		{
			free(data);
			data = NULL;
		}
	};

	void getMax(double &maxX, double &maxY)
	{
		maxX = maxY = INT_MIN;
		for (int i=0; i < nLen; i++)
		{
			if(data[i].x > maxX) maxX = data[i].x;
			if(data[i].y > maxY) maxY = data[i].y;
		}
	}

	void push_back(double _x, double _y)
	{
		data[nLen].x = _x;
		data[nLen].y = _y;
		nLen++;
	}

	void push_back(cv::Point &numPos)
	{
		data[nLen].x = numPos.x;
		data[nLen].y = numPos.y;
		nLen++;
	}

	void push_back(MyPointArray &mpa)
	{
		for (int i=0; i < mpa.nLen; i++)
		{
			data[nLen].x = mpa.data[i].x;
			data[nLen].y = mpa.data[i].y;
			nLen++;
		}
	}

	void divide(double d)
	{
		for (int i=0; i < nLen; i++)
		{
			data[i].x /= d;
			data[i].y /= d;
		}
	}
	
	// 对坐标点快速排序 快排
	void Qsortx(cv::Point *pt, int low, int high)	// high=nLen-1
	{
		if(low >= high)	return;

		int first = low;
		int last = high;
		cv::Point key = pt[first];		// 用字表的第一个记录作为枢轴

		while(first < last)
		{
			while(first < last && pt[last].x >= key.x)
				--last;
			pt[first] = pt[last];		// 将比第一个小的移到低端
			while(first < last && pt[first].x <= key.x)
				++first;
			pt[last] = pt[first];		// 将比第一个大的移到高端
		}
		pt[first] = key;				// 枢轴记录到位

		Qsortx(pt, low, first-1);
		Qsortx(pt, first+1, high);
	}
	
	void Qsorty(cv::Point *pt, int low, int high)	// high=nLen-1
	{
		if(low >= high)	return;

		int first = low;
		int last = high;
		cv::Point key = pt[first];		// 用字表的第一个记录作为枢轴

		while(first < last)
		{
			while(first < last && pt[last].y >= key.y)
				--last;
			pt[first] = pt[last];		// 将比第一个小的移到低端
			while(first < last && pt[first].y <= key.y)
				++first;
			pt[last] = pt[first];		// 将比第一个大的移到高端
		}
		pt[first] = key;				// 枢轴记录到位

		Qsorty(pt, low, first-1);
		Qsorty(pt, first+1, high);
	}
	void Qsortx()
	{
		Qsortx(data, 0, nLen-1);
	}
	void Qsorty()
	{
		Qsorty(data, 0, nLen-1);
	}

	void convertto(vector<cv::Point> &vpt)
	{
		for (int i=0; i < nLen; i++)
			vpt.push_back(cvPoint(data[i].x, data[i].y));
	}

	// 求质心	// 一组点的质心 求x,y的均值
	cv::Point centroid()
	{
		cv::Point mu(0,0);	// 质心
		if(nLen < 1) return mu;
		for(int i=0; i < nLen; i++)
		{
			mu.x += data[i].x;
			mu.y += data[i].y;
		}
		mu.x /= nLen;
		mu.y /= nLen;
		return mu;
	}

	// 矩形对角线交点 四边形连线交点 四点交点
// 	bool diagonal_crosspoint(double &cx, double &cy)
// 	{
// 		MyLineD mld01(data[0].x, data[0].y, data[1].x, data[1].y);
// 		MyLineD mld23(data[2].x, data[2].y, data[3].x, data[3].y);
// 		bool bret = mld01.crosspoint(mld23, cx, cy, true);
// 		if(bret) return true;
// 
// 		MyLineD mld02(data[0].x, data[0].y, data[2].x, data[2].y);
// 		MyLineD mld13(data[1].x, data[1].y, data[3].x, data[3].y);
// 		bret = mld02.crosspoint(mld13, cx, cy, true);
// 		if(bret) return true;
// 
// 		MyLineD mld03(data[0].x, data[0].y, data[3].x, data[3].y);
// 		MyLineD mld12(data[1].x, data[1].y, data[2].x, data[2].y);
// 		bret = mld03.crosspoint(mld12, cx, cy, true);
// 		if(bret) return true;
// 	}

	// 所有点到某点的距离
// 	void disttopoint(double x, double y, MyArrayD &mdist)
// 	{
// 		for (int i=0; i < nLen; i++)
// 		{
// 			double d = data[i].distance(x, y);
// 			mdist.addtail(d);
// 		}
// 	}

	// 此函数仅支持旋转90\180\270度
// 	void rotateN90(int degree, int imgw, int imgh)
// 	{
// 		for (int i=0; i < nLen; i++)
// 			data[i].rotateN90(degree, imgw, imgh);
// 	}

	// 所有点以ox,oy为中心顺时针旋转degree度
// 	void rotate(double ox, double oy, double degree)
// 	{
// 		for (int i=0; i < nLen; i++)
// 			data[i].rotate(ox, oy, degree);
// 	}
// 	static void rotate(vector<cv::Point2d> &vPoint, double ox, double oy, double Angle)
// 	{
// 		for (int i=0; i < vPoint.size(); i++)
// 			MyPointD::rotate(vPoint[i].x, vPoint[i].y, ox, oy, Angle);
// 	}

	// 计算两两元素的距离(差的绝对值)
	// 第i元素与第j元素的|差|放在i*nLen+j的位置, 上三角阵
	void mutualDist(vector<double> &ma)
	{
		for (int i=0; i < nLen; i++)
		{
			for (int j=i+1; j < nLen; j++)
			{
				ma[i*nLen+j] = MyTools_cv::distance(data[i], data[j]);
			}
		}
	}

	// 计算所有矩形的中心 要先初始化本对象内存
	void getCenters(vector<cv::Rect> &vRect)
	{
		int _nLen = vRect.size();
		if(isMallocMem)	free(data);
		MyPointArray_MALLOC(_nLen);
		for (int i=0; i < _nLen; i++)
		{
			data[i].x = vRect[i].x+vRect[i].width/2.0;
			data[i].y = vRect[i].y+vRect[i].height/2.0;
		}
	}

	// 在直线上取n个点,不含始点(x1,y1)含终点(x2,y2)
	void getLinePoints(double x1, double y1, double x2, double y2, int n)
	{
		double dx=(x2-x1)/n;
		double dy=(y2-y1)/n;
		for (int i=1; i <= n; i++)
		{
			push_back(x1+dx*i, y1+dy*i);
		}
	}

	// 用一条线将四点分到两侧, 成功返回true
// 	bool split2Sides(MyLineKB mkb)
// 	{
// 		bool bPos[4]={false, false, false, false};	// 左上,右上,右下,左下
// 	
// 		// 仅取前四点
// 		bPos[0] = data[0].isUpLine(mkb.k, mkb.b);
// 		bPos[1] = data[1].isUpLine(mkb.k, mkb.b);
// 		bPos[2] = !data[2].isUpLine(mkb.k, mkb.b);
// 		bPos[3] = !data[3].isUpLine(mkb.k, mkb.b);
// 		
// 		// 如果对应位置为false,说明应交换到另一侧
// 		for (int i=0; i < 2; i++)
// 		{
// 			if(!bPos[i])
// 			{
// 				if(!bPos[2])
// 				{
// 					MYSWAP_MPD(data[i], data[2]);
// 					bPos[i] = bPos[2] = true;
// 				}
// 				else if(!bPos[3])
// 				{
// 					MYSWAP_MPD(data[i], data[3]);
// 					bPos[i] = bPos[3] = true;
// 				}
// 				else return false;
// 			}
// 		}
// 		
// 		return bPos[0] && bPos[1] && bPos[2] && bPos[3];
// 	}

	// 四点排列 四角排序 四角排列成一个四边形 修改源数据排列
	// 原理: 输入的两条直线已经可以把四个点划分开, 将4点按顺序排列(左上,右上,右下,左下)
// 	int arrange4Points(MyLineKB mkbH, MyLineKB mkbV)
// 	{
// 		bool bPos[4]={false, false, false, false};	// 左上,右上,右下,左下
// 
// 		// 接近水平的为横线, 接近竖直的为纵线
// 		if(abs(mkbH.k) > abs(mkbV.k))
// 		{
// 			MyLineKB mkb=mkbH; mkbH=mkbV; mkbV=mkb;
// 		}
// 
// 		// 将四点安排在对应的位置, 若有某位置已有点, 将其安排在其后, 返回第二个点的位置
// 		for (int i=0; i < 4; i++)	// 仅取前四点
// 		{
// 			if(bPos[i]) continue;
// 
// 			bool upH = !data[i].isUpLine(mkbH.k, mkbH.b);
// 			bool rightV = data[i].isUpLine(mkbV.k, mkbV.b);
// 			if(mkbV.k > 0 && mkbV.k != INT_MAX)
// 			{
// 				rightV = !rightV;
// 			}
// 			int  nDirection = upH ? (rightV ? 1 : 0) : (rightV ? 2 : 3);
// 			if(bPos[nDirection])
// 			{
// 				//nDirection = (nDirection+1) % 4;	// 发生冲突放在下一个位置
// 				//MYSWAP_MPD(data[nDirection], data[i]);
// 				return nDirection;
// 			}
// 			else
// 			{
// 				if(i != nDirection)
// 				{
// 					MYSWAP_MPD(data[nDirection], data[i]);
// 					if(!bPos[i]) i--;	// 本点重新检测, 本点数据是新的.
// 				}
// 				bPos[nDirection] = true;
// 			}
// 		}
// 
// 		return 4;
// 	}

	// 四点排列 四角排序 四角排列成一个四边形 修改源数据排列(左上,右上,右下,左下)
	// 用横线或竖线分成每边2点,两侧线段中点相连,
	// (未实现)如果两点共线,将长的边线中点改为1/4点.
	bool arrange4Points(bool bDebug=false)
	{
//		bool bSplit;
// 		MyLineKB mkb1, mkb2;
// 		MyPointD mpd=centroid();
// 		cv::Mat  cm;
// 
// 		for(int j=0; j < 2; j++)
// 		{
// 			// 质心法
// 			mkb1.set(0, mpd.y);
// 			mkb2.set(INT_MAX, mpd.x);
// 			bSplit = split2Sides(mkb1);
// 			if(!bSplit)
// 			{
// 				mkb1.set(INT_MAX, mpd.x);
// 				bSplit = split2Sides(mkb1);
// 			}
// 			mkb2.getCenterLine(data[0], data[1], data[2], data[3]);
// 
// 			if(bDebug)
// 			{
// 				cm = cv::Mat::zeros(600,600,CV_MAKETYPE(CV_8U,3));;
// 				drawshow(cm);
// 				mkb1.drawshow(cm);
// 				mkb2.drawshow(cm, "arrange4Points");
// 				outInt1(bSplit);
// 				//waitKey();
// 			}
// 			if(bSplit) break;
// 
// 			// 对角线交点法
// 			if(j == 0)
// 			{
// 				bool bCrossPoint=diagonal_crosspoint(mpd.x, mpd.y);
// 				if(!bCrossPoint) break;
// 			}
// 		}
// 		
// 		// 重新计算第一条直线
// 		if(!bSplit)
// 		{
// 			bSplit = split2Sides(mkb2);
// 			if(bSplit)
// 				mkb1.getCenterLine(data[0], data[1], data[2], data[3]);
// 			if(bDebug)
// 			{
// 				cm = cv::Mat::zeros(600,600,CV_MAKETYPE(CV_8U,3));;
// 				drawshow(cm);
// 				mkb1.drawshow(cm);
// 				mkb2.drawshow(cm, "arrange4Points");
// 				outInt1(bSplit);
// 				//waitKey();
// 			}
// 		}
// 		
// 		// 将4点按顺序排列(左上,右上,右下,左下)
// 		int ret = arrange4Points(mkb1, mkb2);
// 
// 		if(bDebug)
// 		{
// 			data[0].drawshow(cm, "arrange4Points", 0,0,255,9,9);
// 			data[1].drawshow(cm, "arrange4Points", 88,88,255,9,6);
// 			data[2].drawshow(cm, "arrange4Points", 178,178,255,9,3);
// 			data[3].drawshow(cm, "arrange4Points", 255,255,255,9);
// 			waitKey();
// 		}
// 
// 		return bSplit && ret==4;
		return false;
	}

	// 得到本点数组所对应的灰度值  从灰度图像中获取双线性插值的点值 单通道
	// pVal 要求长度>=nLen
	void getPixelValue_Interpolation(cv::Mat matGray, double *pVal) // 修改了名字原名:getPixelValue
	{
// 		for (int i=0; i < nLen; i++)
// 			pVal[i] = data[i].getPixelValue_Interpolation(matGray);
	}

	// 交换x,y坐标
	void swapXY()
	{
		for (int i=0; i < nLen; i++)
		{
			double t = data[i].x;
			data[i].x = data[i].y;
			data[i].y = t;
		}
	}

	// 求回归直线 拟合直线 最小二乘法 y=bx+a
	// 参考:最小二乘法求回归直线方程的推导过程 b=∑(xi*yi-n*mux*muy)/∑(xi*xi-n*mux*mux)  a=muy-b*mux
	// https://blog.csdn.net/marsjohn/article/details/54911788
	void fitLine(double &b, double &a, double mx, double my)
	{
		double suma=0, sumb1=0, sumb2=0;

		for (int i=0; i < nLen; i++)
		{
			sumb1 += data[i].x*data[i].y;
			sumb2 += data[i].x*data[i].x;
		}

		sumb1 -=  nLen*mx*my;
		sumb2 -=  nLen*mx*mx;
		if (sumb2 == 0)	// 拟合直线与y轴平行 方程为:x=a
		{
			b = INT_MAX;
			a = mx;
			return;
		}

		b = sumb1/sumb2;
		a = my - b*mx;
	}

	// 求回归直线的斜率 两次取斜率绝对值较小的那个为准
	// 因为fitLine存在问题:当拟合直线接近90度时不准
	// 返回直线的K和B
	double fitLine2(double &B)
	{
		double eps=1e-6;
		double K1, K2, B2;
		double sumqH=0, sumqV=0;
		cv::Point avrage = centroid();	// 计算均值

		// 计算斜率
		fitLine(K1, B, avrage.x, avrage.y);

		// 若为水平直线  则分别拟合水平和垂直线比较离差,水平离差小的为正确
		if(-eps < K1 && K1 < eps)
		{
			for (int i = 0; i < nLen; i++)	// 假设拟合水平直线  计算所有点到中心点的水平距离离差的平方和
				sumqH += (data[i].y - avrage.y)*(data[i].y - avrage.y);
			for (int i = 0; i < nLen; i++)	// 假设拟合垂直直线  计算所有点到中心点的垂直距离离差的平方和
				sumqV += (data[i].x - avrage.x)*(data[i].x - avrage.x);
			//outDouble2(sumqH, sumqV);
			if(sumqH < sumqV) return K1;
		}

		// 交换坐标, 计算斜率
		swapXY();
		fitLine(K2, B2, avrage.y, avrage.x);
		swapXY();

		// 若为竖直直线  则分别拟合水平和垂直线比较离差,垂直离差小的为正确
		if(-eps < K2 && K2 < eps)
		{
			for (int i = 0; i < nLen; i++)	// 假设拟合水平直线  计算所有点到中心点的水平距离离差的平方和 与 if(K1 < 0.001) 代码段相同
				sumqH += (data[i].y - avrage.y)*(data[i].y - avrage.y);
			for (int i = 0; i < nLen; i++)	// 假设拟合垂直直线  计算所有点到中心点的垂直距离离差的平方和
				sumqV += (data[i].x - avrage.x)*(data[i].x - avrage.x);
			//outDouble2(sumqH, sumqV);
			if(sumqV < sumqH)
			{
				B = B2;
				return INT_MAX;
			}
		}
		else if(abs(K2) < abs(K1))	// 取绝对值较小的斜率
		{
			B = -B2/K2; // 交换xy坐标计算KB,逆运算
			return 1/K2;
		}
		//else
			return K1;
	}

	// 获取多直线的交点 n条直线按顺序相交,最后一条与第一条相交
// 	static void getCrosspoints_MultiLines(MyLineKB *mlk, double* ptx, double* pty, int n)
// 	{
// 		mlk[n-1].crosspoint(mlk[0],ptx[0],pty[0]);
// 		for (int i=1; i < n; i++)
// 			mlk[i-1].crosspoint(mlk[i],ptx[i],pty[i]);
// 	}

	// 获取多直线的交点 n条直线按顺序相交,最后一条与第一条相交
// 	void getCrosspoints_MultiLines(MyLineKB *mlk, int n)
// 	{
// 		nLen=n;
// 		isMallocMem=false;
// 		if(nLen<1)return;
// 		data=(MyPoint<double>*)malloc(nLen*sizeof(MyPoint<double>));
// 		if(data)isMallocMem=true;
// 		//MyPointArray_MALLOC(n);
// 		data[0] = mlk[n-1].crosspoint(mlk[0]);
// 		for (int i=1; i < n; i++)
// 			data[i] = mlk[i-1].crosspoint(mlk[i]);
// 	}

	// 屏幕输出数组
	// hasEndl 是否含有回车	true每数据一个回车
	// outLen  输出长度
	void print(bool hasEndl=true, int outLen=0)
	{
		stringstream ss;  
		ss << typeid(*this).name();  	// 显示类名称
		cout << ss.str() << ":" << endl;
		if(nLen <=0) return;

		if(hasEndl)	cout << 0 << " ";
		printf("%d,%d ", data[0].x, data[0].y);

		if(outLen <= 0) outLen=nLen;
		for (int j=1; j < outLen; j++)
		{
			if(hasEndl)	printf("\n%d ", j);
			else		printf(" ");
			printf("%d,%d ", data[j].x, data[j].y);
		}
		printf("\nnLen=%d\n", nLen);
	}

	// 当nlen<2时,则在图像的(x,y)处画一点. 经测试与imgout.at<cv::Vec3b>(i,j)=cv::Vec3b(0,0,0);功能相同
	void crossLine(cv::Mat matImg, double x, double y, int mb=255, int mg=0, int mr=255, int nlen=9, int thickness=1)
	{
		line(matImg, cvPoint2D32f(x - nlen / 2, y), cvPoint2D32f(x + nlen / 2, y), CV_RGB(mr,mg,mb),thickness);
		line(matImg, cvPoint2D32f(x, y - nlen / 2), cvPoint2D32f(x, y + nlen / 2), CV_RGB(mr,mg,mb),thickness);
	}

	void crossLine(cv::Mat matImg, cv::Point pt, int mb=0, int mg=255, int mr=255, int nlen=9, int thickness=1)
	{
		line(matImg, cvPoint(pt.x - nlen / 2, pt.y), cvPoint(pt.x + nlen / 2, pt.y), CV_RGB(mr,mg,mb),thickness);
		line(matImg, cvPoint(pt.x, pt.y - nlen / 2), cvPoint(pt.x, pt.y + nlen / 2), CV_RGB(mr,mg,mb),thickness);
	}

	// 在图像中显示所有点,用十字线表示
	cv::Mat drawshow(cv::Mat &srcMat, int markLen=9, const char* showName=NULL)
	{
		cv::Mat cm=srcMat;
		if(srcMat.channels() == 1)
			cvtColor(srcMat,cm,CV_GRAY2BGR);

		for (int i=0; i < nLen; i++)
			crossLine(cm, round(data[i].x), round(data[i].y), 255, 0, 255, markLen);

		if(showName != NULL)
			cv::imshow("MyPointArray", cm);
		return cm;
	}

	// 画n个点连线	bCorner是否显示每个点 bClose是否闭合
	static void showPolygon(cv::Mat &srcMat, double *ptx, double *pty, int n, bool bCorner=true, bool bClose=true)
	{
// 		for (int i=0; i < n-1; i++)
// 			cv::line(srcMat, cvPoint(ptx[i], pty[i]), cvPoint(ptx[i+1], pty[i+1]), CV_RGB(2,222,2));
// 		if(bClose)
// 			cv::line(srcMat, cvPoint(ptx[0], pty[0]), cvPoint(ptx[n-1], pty[n-1]), CV_RGB(2,222,2));
// 		if(bCorner)
// 		{
// 			for (int i=0; i < n; i++)
// 				MyTools::crossLine(srcMat, ptx[i], pty[i], 255-60*i, 0, 75+60*i);
// 		}
	}
	void showPolygon(cv::Mat &srcMat, bool bCorner=true, bool bClose=true)
	{
// 		for (int i=0; i < nLen-1; i++)
// 			cv::line(srcMat, data[i].cv(), data[i+1].cv(), CV_RGB(2,222,2));
// 		if(bClose)
// 			cv::line(srcMat, data[0].cv(), data[nLen-1].cv(), CV_RGB(2,222,2));
// 		if(bCorner)
// 		{
// 			for (int i=0; i < nLen; i++)
// 				MyTools::crossLine(srcMat, data[i].cv(), 255-60*i, 0, 75+60*i);
// 		}
	}
};
