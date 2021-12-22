#pragma once
#include "MyPoint.h"

// 申请内存 设置数组长度 也是内存长度
#define MyArray_MALLOC(_nLen) nLen=_nLen; \
	if(nLen<1)return; \
	data=(T*)malloc(nLen*sizeof(T)); \
	if(data)isMallocMem=true

// 同上,但返回NULL
#define MyArray_MALLOC2(_nLen) nLen=_nLen; \
	if(nLen<1)return NULL; \
	data=(T*)malloc(nLen*sizeof(T)); \
	if(data)isMallocMem=true

template<class T>  
class MyArray
{
public:
	int  nLen;			// 有效数据个数,不是 d 的所占内存大小
	T*   data;
	bool isMallocMem;	// 是否申请了内存,用释放内存的判断

	MyArray(void)
	{
		isMallocMem=false;
	};
	
	// 重新申请内存 释放旧的数据
	void remalloc(int _nLen)
	{
		if(isMallocMem) free(data);
		MyArray_MALLOC(_nLen);
		memset(data, 0, nLen*sizeof(T));
		nLen = 0;	// 有效数据个数,不是 d 的所占内存大小
	}

	// 初始化 长度为_nLen的数据 _nLen为预留数据长度
	MyArray(int _nLen)
	{
		MyArray_MALLOC(_nLen);
		memset(data, 0, nLen*sizeof(T));
		nLen = 0;	// 有效数据个数,不是 d 的所占内存大小
	};

	// 使用已存在的数据
	MyArray(T *pData, int _nLen)
	{
		data = pData;
		nLen = _nLen;
		isMallocMem=false;
	};

	MyArray(vector<T> v)
	{
		MyArray_MALLOC(v.size());
		for (int i=0; i < nLen; i++)
			data[i] = v[i];
	};

	MyArray(const MyArray &ma)
	{
		MyArray_MALLOC(ma.nLen);
		for (int i=0; i < nLen; i++)
			data[i] = ma.data[i];
	}

	// 初始化 复制 (如果下面两个函数合在一起,就必须相同类型的赋值,分开的花可以支持不同类型的赋值)
// 	MyArray(const MyArray<int> &ma)
// 	{
// 		MyArray_MALLOC(ma.nLen);
// 		for (int i=0; i < nLen; i++)
// 			data[i] = ma.data[i];
// 	}
// 	MyArray(const MyArray<double> &ma)
// 	{
// 		MyArray_MALLOC(ma.nLen);
// 		for (int i=0; i < nLen; i++)
// 			data[i] = ma.data[i];
// 	}

	// + - * / 双操作数
	MyArray operator+(const MyArray &ma) const
	{  
		MyArray m(nLen);
		for (int i=0; i < nLen; i++)
			data[i] += ma.data[i];
		return m;
	}

	// += -= *= /= 单数组合操作
	const MyArray& operator+=(double d){
		for (int i=0; i < nLen; i++)
			data[i] += d;
		return *this;
	}
	const MyArray& operator-=(double d){
		for (int i=0; i < nLen; i++)
			data[i] -= d;
		return *this;
	}
	const MyArray& operator*=(double d){
		for (int i=0; i < nLen; i++)
			data[i] *= d;
		return *this;
	}
	const MyArray& operator/=(double d){
		for (int i=0; i < nLen; i++)
			data[i] /= d;
		return *this;
	}

	// = += -= *= /= 对象组合操作
	const MyArray& operator=(const MyArray& ma){  // "="号的赋值操作 需要重载才可使用
		if(isMallocMem && data)
			free(data);
		MyArray_MALLOC2(ma.nLen);
		for (int i=0; i < nLen; i++)
			data[i] += ma.data[i];
		return *this;  
	}

	const MyArray& operator+=(const MyArray &ma){
		for (int i=0; i < nLen; i++)
			data[i] += ma.data[i];
		return *this;
	}

	// 初始化 截取 从nStart位置开始,截取w个数据
	MyArray(const MyArray &ma, int nStart, int w)
	{
		if(!(0 <= nStart && nStart+w-1 < ma.nLen)) return;
		MyArray_MALLOC(w);
		memcpy(data, ma.data+nStart, nLen*sizeof(T));
	}

	// 初始化 数据缩放
	MyArray(const MyArray &ma, int newLen)// 此处参数必须使用const &否则报错
	{
		double fScale = (ma.nLen-1.0)/(newLen-1);
		MyArray_MALLOC(newLen);
		for (int i=0; i < nLen; i++)
		{
			double x = i*fScale;
			int left = floor(x);
			int right = ceil(x);
			data[i] = (1-x+left)*ma.data[left] + (x-left)*ma.data[right];
		}
	}



////
// 调试
//////////////////////////


	~MyArray(void)
	{
		if(isMallocMem && data)
		{
			free(data);
                        isMallocMem=false;
			data = NULL;
		}
	};

	void push_back(T _data)
	{
		data[nLen] = _data;
		nLen++;
	}

	// 尾部添加一个元素
	void addtail(T _data)
	{
		data[nLen] = _data;
		nLen++;
	}

	// 复制自身n次 要求预留存储空间要足够
	void selfcopy(int cn=1)
	{
		for (int i=0; i < cn; i++)
			memcpy(data+nLen+i*nLen, data, nLen*sizeof(T));
		nLen *= 1+cn;
	}

	// 十六进制字母表示转十六进制数据 仅转1个字母,半字节
	int Hex2Char(char ch)
	{
		if('0' <= ch && ch <= '9') return ch-'0';
		if('a' <= ch && ch <= 'f') return ch-'a';
		if('A' <= ch && ch <= 'F') return ch-'A'+10;
		return 0;
	}

	// 十六进制字符串转十六进制数据 要先申请内存且>_nLen
	// 可以有空格不可有其他字符
	void HexString2Chars(const char* pStr, int _nLen)
	{
		int j=0;
		for (int i=0; i < _nLen; i++)
		{
			if(pStr[i] == ' ') continue;
			int aa = Hex2Char(pStr[i]);
			int bb = Hex2Char(pStr[i+1]);
			int dd = (aa<<4) + bb;
			data[j++] = (Hex2Char(pStr[i])<<4) + Hex2Char(pStr[i+1]);
			i++;
		}
		nLen = _nLen;
	}

	// 数据类型转换, 去掉高位,只留低位
	void short2char(unsigned short *pSrc, unsigned char *pDst, int _nLen)
	{
		for (int i=0; i < _nLen; i++)
			pDst[i] = pSrc[i];
	}

	// uchar数组转ushort 字节变长数值不变
// 	int ucharArray2ushortArray()
// 	{
// 		return 0;
// 	}

	// 统计元素值为0的个数
	int countZero()
	{
		int c=0;
		for (int i=0; i < nLen; i++)
			if(0 == data[i])
				c++;
		return c;
	}

	// 统计元素值在某范围内的个数, 不包含fmin与fmax
	int countRange(double fmin, double fmax)
	{
		int c=0;
		for (int i=0; i < nLen; i++)
			if(fmin < data[i] && data[i] < fmax)
				c++;
		return c;
	}

	// 求和
	T sum()
	{
		T av=0;
		for (int i=0; i < nLen; i++)
			av += data[i];
		return av;
	}

	// 计算均值
	double average()	// average
	{
		double av=0;
		for (int i=0; i < nLen; i++)
			av += data[i];
		return av/nLen;
	}
	static T average(vector<T> vt)	// average
	{
		int n=vt.size();
		T av=0;
		for (int i=0; i < n; i++)
			av += vt[i];
		return av/n;
	}

	// 计算方差
	double std()	// Standard Deviation
	{
		double s=0;
		double av = this->average();
		for (int i=0; i < nLen; i++)
			s += (data[i] - av)*(data[i] - av);
		return sqrt(s/nLen);
	}

	// 在上三角阵中(不含对角线) 计算最小值
	T minDataUp(int &row, int &col, int w, int h, int widthstep)	// 参数: row,col最小值位置, m 最小值, w,h三角阵的行列数
	{
		T m=MYINT_MAX;
		for (int i=0; i < h; i++)
		{
			for (int j=i+1; j < w; j++)
			{
				T tmp = data[i*widthstep+j];
				if(data[i*widthstep+j] < m)
				{
					m = tmp;
					row = i;
					col = j;
				}
			}
		}
		return m;
	}

	// 计算最小值
	T minData()
	{
		T m=data[0];
		for (int i=1; i < nLen; i++)
		{
			if(data[i] < m)
				m = data[i];
		}
		return m;
	}

	// 计算最小值
	T minData(int &mpos)	// 参数:mpos 返回最小值位置, m 最小值
	{
		T m=data[0];
		mpos = 0;
		for (int i=1; i < nLen; i++)
		{
			if(data[i] < m)
			{
				m = data[i];
				mpos = i;
			}
		}
		return m;
	}

	// 计算最大值
	static T maxData(vector<T> vdata, int &mpos)	// 参数:mpos 返回最大值位置, m 最大值
	{
		T maxT=vdata[0];
		mpos = 0;

		for (int j=1; j < vdata.size(); j++)
		{
			if(vdata[j] > maxT)
			{
				maxT = vdata[j];
				mpos = j;
			}
		}
		return maxT;
	}

	// 计算最大值
	T maxData()
	{
		T maxT=data[0];
		for (int j=1; j < nLen; j++)
		{
			if(data[j] > maxT)
				maxT = data[j];
		}
		return maxT;
	}

	// 计算最大值 返回的是第一个最大值的位置
	T maxData(int &mpos)	// 参数:mpos 返回最大值位置, m 最大值
	{
		T maxT=data[0];
		mpos = 0;

		for (int j=1; j < nLen; j++)
		{
			if(data[j] > maxT)
			{
				maxT = data[j];
				mpos = j;
			}
		}
		return maxT;
	}

	// 计算N个最大值 返回的是位置 冒泡法排序
	// maxN 找到最大值的个数, 当maxN==1时仅找一个最大值
	// int start_pos=0, int end_pos=0 在两个位置之间找峰值,含端点
	void maxDataN(vector<int> &vpos, int maxN=1, int start_pos=0, int end_pos=0)	// 参数:vpos 返回最大值位置
	{
		vector<int> tpos;
		for (int j=0; j < nLen; j++)
			tpos.push_back(j);
		
		if(end_pos==0) end_pos=nLen-1;

		for (int i=0; i < maxN; i++)
		{
			for (int j=start_pos; j < end_pos-i; j++)
			{
				if(data[tpos[j]] > data[tpos[j+1]])	// 由小到大排序
				{
					MYSWAP_INT(tpos[j], tpos[j+1]);
				}
			}
		}

		// 反序插入 由大到小
		for (int i=0; i < maxN; i++)
			vpos.push_back(tpos[end_pos-i]);
	}

	void valueInvert()
	{
		T datamax = maxData();
		for (int j=0; j < nLen; j++)
			data[j] = datamax - data[j];
	}

	// 曲线平滑操作 sw平滑窗口宽度 取均值
	// 必须sw取奇数
	void smooth(int sw=5)	// sw取奇数必须
	{
		int p=1;
		T   sm=data[0];
		T  *olddata=(T*)malloc(nLen*sizeof(T));

		//MyArray_MALLOC(nLen);	// 用这个会报错. 如果外部申请了有隐藏的备用空间的话
		memcpy(olddata,data,nLen*sizeof(T));

		data[0]=sm;
		for (int j=2; j < sw; j+=2)
		{
			sm += olddata[j-1] + olddata[j];
			data[p++] = sm/(j+1);
		}

		for (int j=sw; j < nLen; j++)
		{
			sm += olddata[j] - olddata[j-sw];
			data[p++] = sm/sw;
		}

		for (int j=sw; j > 1; j-=2)
		{
			sm -= olddata[nLen-j] + olddata[nLen-j+1];
			data[p++] = sm/(j-2);
		}

		free(olddata);
	}

	// 找出异常值, 过大或过小的值	 要求数据长度大于等于3
	// 异常值:最小值最大值之差大于阈值,则认为存在异常值
	// 返回异常值,pos返回异常值的位置
	T getAbnormalData(int th, int &pos)	// th为最小值最大值之差的阈值
	{
		int maxpos, minpos;
		T datamax = maxData(maxpos);
		T datamin = minData(minpos);
		T datasum = sum();
		
		// 去除最大最小值计算均值
		datasum = datasum - datamax - datamin;
		double dataavr = datasum/(nLen-2.0);
		
		// 判断最大最小值哪个距均值更远,返回更远的那个
		if(datamax-dataavr > dataavr-datamin)
		{
			pos = maxpos;
			return datamax;
		}

		pos = minpos;
		return datamin;
	}

	// 取数据间隔组成数组 后数减前数 可能有负值
	MyArray<T> gapArray()
	{
		MyArray<T> m(nLen-1);
		for (int i=0; i < nLen-1; i++)
			m.data[i] = data[i+1] - data[i];
		return m;
	}

	// 对所有数据取绝对值
	MyArray<T> abs()
	{
		MyArray<T> m(nLen);
		for (int i=0; i < nLen; i++)
			m.data[i] = data[i] < 0 ? -data[i] : data[i];
		return m;
	}

	// 对所有数据取整
	MyArray<T> myRound()
	{
		MyArray<T> m(nLen);
		for (int i=0; i < nLen; i++)
			m.data[i] = data[i] > 0.0 ? floor(data[i] + 0.5) : ceil(data[i] - 0.5);
		return m;
	}

	// 只取元素值在某范围内的数据组成数组返回, 不包含fmin与fmax
	MyArray<T> getDataAtRange(double fmin, double fmax)
	{
		int j=0;
		MyArray<T> m(nLen);
		for (int i=0; i < nLen; i++)
		{
			if(fmin < data[i] && data[i] < fmax)
				m.data[j++] = data[i];
		}
		m.nLen = j;
		return m;
	}

	// 取得灰度图像一条水平线数据  先初始化本对象长度
	// 从点(x,y)开始向右取n个点 n<0向左
	void getImageLineH(cv::Mat matGray, int x, int y, int n, bool bRight=true)
	{
		if(bRight)
		{
			for (int j=0; j < n; j++)
				data[j] = matGray.at<uchar>(y, x+j);
		}
		else
		{
			for (int j=0; j < n; j++)
				data[j] = matGray.at<uchar>(y, x-j);
		}
		nLen = n;
	}

	// 取得灰度图像一条竖直线数据  先初始化本对象长度
	// 从点(x,y)开始向下取n个点 n<0向上
	void getImageLineV(cv::Mat matGray, int x, int y, int n, bool bDown=true)
	{
		if(bDown)
		{
			for (int j=0; j < n; j++)
				data[j] = matGray.at<uchar>(y+j, x);
		}
		else
		{
			for (int j=0; j < n; j++)
				data[j] = matGray.at<uchar>(y-j, x);
		}
		nLen = n;
	}

	// 快速排序算法 由小到大排序
	static void Qsort(T aa[], int low, int high)	// high=nLen-1
	{
		if(low >= high)	return;

		int first = low;
		int last = high;
		T key = aa[first];			/*用字表的第一个记录作为枢轴*/

		while(first < last)
		{
			while(first < last && aa[last] >= key)
				--last;
			aa[first] = aa[last];		/*将比第一个小的移到低端*/

			while(first < last && aa[first] <= key)
				++first;
			aa[last] = aa[first];    	/*将比第一个大的移到高端*/
		}
		aa[first] = key;				/*枢轴记录到位*/
		Qsort(aa, low, first-1);
		Qsort(aa, first+1, high);
	}
	void Qsort()
	{
		Qsort(data, 0, nLen-1);
	}

	// 快速排序算法 由小到大排序 地址排序
	// int *pLabel预先初始化序列号数据(0 到 nLen-1)
	static void Qsort(T aa[], int *pAddr, int low, int high)	// high=nLen-1
	{
		if(low >= high)	return;

		int first = low;
		int last = high;
		T key = aa[pAddr[first]];			/*用字表的第一个记录作为枢轴*/

		while(first < last)
		{
			while(first < last && aa[pAddr[last]] >= key)
				--last;
			pAddr[first] = pAddr[last];		/*将比第一个小的移到低端*/

			while(first < last && aa[pAddr[first]] <= key)
				++first;
			pAddr[last] = pAddr[first];    	/*将比第一个大的移到高端*/
		}
		pAddr[first] = key;				/*枢轴记录到位*/
		Qsort(aa, pAddr, low, first-1);
		Qsort(aa, pAddr, first+1, high);
	}
	void Qsort(int *pAddress)
	{
		Qsort(data, pAddress, 0, nLen-1);
	}

	// 计算两两元素的距离(差的绝对值)
	// 第i元素与第j元素的|差|放在i*nLen+j的位置, 上三角阵
	void mutualDist(MyArray<double> &ma)
	{
		for (int i=0; i < nLen; i++)
		{
			for (int j=i+1; j < nLen; j++)
			{
				ma.data[i*nLen+j] = std::abs(data[i] - data[j]);
			}
		}
	}

	// 聚类 此为简单合并方法,使用阈值合并 适合团状合并,不适合线状合并
	// 层次聚类法, 缺点合并后的元素不能被修正,复杂度高ON2,不适应大数据集
	/*	1 初始化：
			(1) 类别向量vClass
			(2) 聚类中心向量vCenter
			(3) 距离矩阵M(上三角阵)
		2 在距离矩阵中找最小的距离两类a和b，后面合并到小序号(a<b)
			(1)	在类别向量中删除b类
			(2)	更新a类的聚类中心
			(3)	在距离矩阵M中，删除b列和b行
			(4)	在距离矩阵M中，更新a对其他类的距离
		3 重复2		*/
	// 返回：类及类内数据(二维向量,保存的是地址序号), 类中心(一维向量)
	int cluster(vector<vector<int> >	&vClass, vector<double> &vCenter, double th, bool bDebug=0)	// 定义合并的距离,当大于这个距离时不再合并
	{
// 		const int nLen=9;
// 		double data[nLen] = {57, 68, 59, 52, 72, 28, 96, 33, 24};
		MyArray<double> mai(data,nLen);

		// 1 初始化：
		// (1) 类别向量vClass	初始时每个元素为一个类别
		vClass.resize(nLen);     //指定元素数目，此处会有其他函数的调用，构造函数，拷贝等。（不够高效）
		for (int i=0; i < nLen; i++)
			vClass[i].push_back(i);

		// (2) 聚类中心向量vCenter	均值或质心
		vCenter.reserve(nLen);	 //预分配空间 size()仍为0
		vCenter.assign(&data[0],&data[nLen]);  //此方法应该比上一种速度要快些，因为直接拷贝赋值。
		//但是要注意数组的end地址为end[10]，而不是end[9]。

		// (3) 距离矩阵M(上三角阵)
		MyArray<double> mdist(nLen*nLen);
		mai.mutualDist(mdist);

		int m,n, classNum=vCenter.size(); 
		double maxVal=mdist.minDataUp(m, n, classNum, classNum, nLen);
		while (maxVal < th){
			double sumX=0;
			//	(1)	后面合并到小序号(m<n), 更新m类的聚类中心
			vClass[m].insert(vClass[m].end(), vClass[n].begin(), vClass[n].end());
			for(vector<int>::iterator it=vClass[m].begin(); it!=vClass[m].end(); it++)
				sumX += data[*it];
			vCenter[m] = sumX/vClass[m].size();//MyArrayD::avr(vClass[m]);

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
				mdist.data[i*nLen+m] = std::abs(vCenter[i] - vCenter[m]);
			for (int j=m+1; j < classNum; j++)	// 大于m的为列更新
				mdist.data[m*nLen+j] = std::abs(vCenter[m] - vCenter[j]);

			//2 在距离矩阵中找最小的距离两类m和n
			maxVal = mdist.minDataUp(m, n, classNum, classNum, nLen);
		}

		if(bDebug)
		{
			for (int i=0; i < vClass.size(); i++)
			{
				for(vector<int>::iterator it=vClass[i].begin(); it!=vClass[i].end(); it++)
					printf("%2d:%5.2f ", *it, data[*it]);//printf("%d ", *it);//
				printf(": %5.2f\n", vCenter[i]);
			}
		}

		return 0;
	}

	// 寻找第一个峰值位置 从位置p开始,大于th的为峰值区 找到一个返回
	// 数组数据不能正负混合
	// 返回峰值位置, 未找到返回nLen
	double findAPeak(double th, int p=0)
	{
		double	sumd=0, sumid=0;

		// 越过小于th区
		while(data[p] < th) p++;

		// 峰值区数据和 加权和
		while(data[p] >= th && p<nLen)
		{
			sumid+=p*data[p];
			sumd += data[p++];
		}

		// 返回峰值质心重心中心
		return sumd==0 ? nLen : sumid/sumd;
	}

	// 求峰值位置		// 小于均值的置为0,求质心
	// 警告:考虑最大值在两侧时,本操作需预留1倍数组空间
	// 算法:		// 	(最大值两侧大于均值的数据段,不含两端p和q)
	//	从mpos向左找p
	// 	if(p < 0)
	// 		复制全列数据到右,mpos+=nLen重找左端p
	// 		从mpos向右找q
	// 	else //if(未复制)
	// 	从mpos向右找q
	// 	if(q >= nLen)
	// 		复制全列数据到右,继续找右端q
	// 返回浮点数位置
	double findAvrPeak()
	{
		int		mpos;
		double	sumd=0, sumid=0;
		double	avr=average();
		double	mdata = maxData(mpos);// 第一个最大值的位置
		int		p=mpos-1,q=mpos+1;

		while(data[p] > avr && p >= 0) p--;
		if(p < 0)
		{
			selfcopy();	// 长度增加
			nLen/=2;	// 长度使用原来的
			p+=nLen;
			while(data[p] > avr && p >= 0) p--;
			if(p < 0) return nLen;	// 所有元素值相等
			q=mpos+1+nLen;
			while(data[q] > avr && q < nLen+nLen) q++;
		}
		else	// 不会两端同时超出的
		{
			while(data[q] > avr && q < nLen) q++;
			if(q >= nLen)
			{
				selfcopy();	// 长度增加
				nLen/=2;	// 长度使用原来的
			}
			while(data[q] > avr && q < nLen+nLen) q++;
		}

		// 大于均值的数据和 加权和
		// 最大值两侧大于均值的数据段,不含两端p和q
		for (int i=p+1; i < q; i++)
		{
			sumid+=i*data[i];
			sumd += data[i];
		}

		// 返回峰值质心重心中心
		if(sumd==0) return nLen;
		double fpos=sumid/sumd;
		return fpos > nLen ? fpos-nLen : fpos;
	}

	// 计算最大值mdata值附近取样,计算质心位置	// 用于寻找最峰(谷)值位置
	// 从mpos两侧各加取n个值用于计算峰值偏向
	// 返回浮点数位置, peakLen计算峰值区所需长度 peakLen<nLen
	// 警告:考虑最大值在两侧时,本操作需预留1倍数组空间
	// 若要取最小值的质心,应valueInvert然后按最大值计算
	double findMaxPeak(int peakLen)
	{
		int		nmx=0, mpos=0;	// nmx等于最大值的个数,mpos左边第一个最大值位置
		double	fpos=nLen;
		
		// 指定一个值的位置 可以改为其他值的位置,找到的是附近的质心
		double mdata = maxData(mpos);// 第一个最大值的位置

		// 计算所有等于最大值的元素个数n
		while(data[mpos+nmx]==mdata && mpos+nmx<nLen) nmx++;
		if(nmx >= peakLen)
			return mpos+(nmx-1.0)/2;	// error:太长的平坦区

		// 左右各取元素 满足峰值区计算所需数据个数
		int m=(peakLen-nmx+1)/2;	// 两侧各取m个,保持两边个数相等
		int p=mpos-m;			// 最左数据
		if(p < 0)	// 若超出左端,计算右端最大值个数与左端连接
		{
			// 如果右端也有相同最大值,则以右端向左最后一个最大值为始点,重新计算左端位置
			// 如果没有,则p+=nLen
			int q=nLen-1;
			while(data[q]==mdata && q>0) q--;
			if(nLen-1 > q)
			{
				nmx += nLen-1-q;
				if(nmx >= peakLen)
				{
					fpos = q+(nmx-1.0)/2;
					return fpos>nLen ? fpos-nLen : fpos;
				}
				m=(peakLen-nmx+1)/2;
				p=q-m;				// 重新计算左端位置
			}
			else
				p+=nLen;
		}
		peakLen=m+nmx+m;
		if(p+peakLen > nLen)
		{
			selfcopy();	// 长度增加
			nLen/=2;	// 长度恢复原来的
		}

		// 计算质心
		double	sumd=0, sumid=0;
		for(int i=p; i < p+peakLen; i++)
		{
			sumid+=i*data[i];
			sumd += data[i];
		}
		//outInt2(p, peakLen);

		if(sumd==0) return p+peakLen;
		fpos=sumid/sumd;	// 返回峰值质心重心中心
		return fpos>nLen ? fpos-nLen : fpos;
	}

	// 找峰值的预处理 利于体现局部峰值
	// 对每点计算到局部均值的差值(距离)(作为尖锐度)		2019.03.06
	// 根据预计峰值个数确定局部窗口大小(全长的几分之一):
	// 峰值个数=(1,1/2),(2,1/3),(3,1/3),(4,1/3),(5,1/4),(6,1/4)
	// bEndpeak 是否计算两端的边缘峰值
	void findPeak(vector<double>& vSharpDegree, int localdivide=3, bool bEndpeak=false)
	{
		double sumd=0;
		int dw = nLen/localdivide/2;	// 每个点左右各取的点数
		int sw = dw+1+dw;
		outInt2(dw, sw);

		// 左侧sw个元素和
		for (int i=0; i < sw; i++)
			sumd += data[i];

		// 左侧点与均值差
		for (int i=0; i <= dw; i++)
		{
			if(bEndpeak)
				vSharpDegree.push_back(data[i] - sumd/sw);
			else
				vSharpDegree.push_back(0);
		}

		// 中部sw个元素和
		for (int i=sw; i < nLen; i++)
		{
			sumd += data[i] - data[i-sw];
			vSharpDegree.push_back(data[i-dw] - sumd/sw);
		}

		// 右侧点与均值差
		for (int i=nLen-dw; i < nLen; i++)
		{
			if(bEndpeak)
				vSharpDegree.push_back(data[i] - sumd/sw);
			else
				vSharpDegree.push_back(0);
		}
	}

	// 寻找峰值点, 输出峰值位置
	// 投影曲线的波峰、波谷查找 https://www.cnblogs.com/aTianTianTianLan/articles/3827263.html
	void findPeak2(vector<int>& peakPositions)
	{
		vector<int> diff_v(nLen-1, 0);

		// 计算V的一阶差分和符号函数trend
		for (vector<int>::size_type i = 0; i !=diff_v.size(); i++)
		{
			if (data[i + 1] - data[i]>0)
				diff_v[i] = 1;
			else if (data[i + 1] - data[i] < 0)
				diff_v[i] = -1;
			else
				diff_v[i] = 0;
		}

		// 对Trend作了一个遍历
		for (int i = diff_v.size() - 1; i >= 0; i--)
		{
			if (diff_v[i] == 0 && i ==diff_v.size()-1)
			{
				diff_v[i] = 1;
			}
			else if (diff_v[i] == 0)
			{
				if (diff_v[i + 1] >= 0)
					diff_v[i] = 1;
				else
					diff_v[i] = -1;
			}
		}

		for (vector<int>::size_type i = 0; i != diff_v.size() - 1; i++)
		{
			if (diff_v[i + 1] - diff_v[i] == -2)
				peakPositions.push_back(i + 1);
		}
	}

	/* 返回的是浮点数组, 数组中记录的是峰谷值的位置: <0 谷值, >0 峰值, =0 无用值
	// 自己的判断峰谷的算法: 相同的值越过,在相同值中取中点	-mbh
	// 在源数据中首尾数据不作为峰谷值, 所有峰谷值位置(0 < j < nLen-1)
	// 若想得到峰谷值个数, 请统计数组中大于0和小于0的个数*/
	// bool bEndavr 端点设为均值,以便于计算两端的峰谷值,覆盖原有值
	void findPeakValley(MyArray<double>& fPeakValley, bool bEndavr=0)// 暂不做对小的峰谷值的忽略,太复杂	-mbh
	{
		int p=0, q=0, f=0;	// int f; 记录上升或下降的标记, 1 up, -1 down;
		int k=0;			// k 个峰谷值
		MyArray<double> fpv(nLen);

		// bool bEndavr 端点设为均值,以便于计算两端的峰谷值,覆盖原有值
		if(bEndavr)
		{
			data[0] = average();
			data[nLen-1] = data[0];
		}

		// 计算峰谷值位置 若峰谷值是平台则取其中间位置 会产生0.5小数
		for (int i=1; i < nLen; i++)
		{
			if(data[i] == data[p])		// 平坦区
				q = i;
			else if(data[i] > data[p])	// 曲线上升
			{
				if(f == -1)	// 若上次为下降则找到一个谷值
					fpv.data[k++] = -(p+q)/2.0;
				p = q = i;
				f = 1;
			}
			else if(data[i] < data[p])	// 曲线下降
			{
				if(f == 1)	// 若上次为上升则找到一个峰值
					fpv.data[k++] = (p+q)/2.0;
				p = q = i;
				f = -1;
			}
		}

		int cd = fpv.data[0];
		p = fpv.data[1];
		if (cd > 0)	// 峰值
			fPeakValley.data[cd] = data[cd] - data[-p];
		else
			fPeakValley.data[-cd] = data[-cd] - data[p];

		cd = fpv.data[k-1];
		q = fpv.data[k-2];
		if (cd > 0)	// 峰值
			fPeakValley.data[cd] = data[cd] - data[-q];
		else
			fPeakValley.data[-cd] = data[-cd] - data[q];

		// 计算峰值度大小( >0 )		峰值度大小: 峰值与两边谷值相差的较小者
		// 要求:前面计算的fpv.data 一定是峰谷值相间 即正负值相间的
		// 要求:所有的峰谷值位置是聚集在前部的,一旦遇到0即结束
		// 由于峰谷度利用两边的峰谷计算,所以两端的峰谷均无效
		for (int i=1; i < k-1; i++)
		{
			cd = fpv.data[i];
			p = fpv.data[i-1];
			q = fpv.data[i+1];
			double localGound;

			if (cd > 0)	// 峰值
			{
				localGound = max(data[-p], data[-q]);
				fPeakValley.data[cd] = data[cd] - localGound;
			}
			else		// 谷值
			{
				localGound = min(data[p], data[q]);
				fPeakValley.data[-cd] = data[-cd] - localGound;
			}
		}
		fPeakValley.nLen=nLen;
	}

	// 对峰谷点计算到局部均值的差值(距离)(作为尖锐度)		2019.03.06
	// 根据预计峰值个数确定局部窗口大小(全长的几分之一):
	// 峰值个数=(1,1/2),(2,1/3),(3,1/3),(4,1/3),(5,1/4),(6,1/4)
	double localHigh(int pos, int localdivide=3)	// localdivide值越大找到的峰谷越尖
	{
		double sumd=0;
		int dw = nLen/localdivide/2;	// 点左右各取的点数
		int sw = dw+1+dw;
		//outInt2(dw, sw);

		int p = pos-dw;
		int q = pos+dw;
		if(p < 0)
		{
			p=0;
			q=sw;
		}
		if(q > nLen)
		{
			q=nLen;
			p=nLen-sw;
		}

		for (int i=p; i < q; i++)
			sumd += data[i];
		return data[pos]-sumd/sw;
	}

	// 找峰值谷值 非峰谷置零 峰谷处计算峰谷度
	// 自己的判断峰谷的算法: 相同的值越过,在相同值中取中点	-mbh
	// 未使用,原因: 它将小的谷值也计算为了正值(与局部均值相减) -mbh
	void findPeakValley2(MyArray<double>& fpv)// 暂不做对小的峰谷值的忽略,太复杂	-mbh
	{
		int p=0, q=0, f=0;	// int f; 记录上升或下降的标记, 1 up, -1 down;
		int j=1;			// j=1; 便于后面计算峰值度 j=0 未使用(后面计算可作为哨兵)
		
		fpv.nLen=nLen;
		for (int i=0; i < fpv.nLen; i++)
			fpv.data[i]=0;

		// 计算峰谷值位置 若峰谷值是平台则取其中间位置 会产生0.5小数
		for (int i=1; i < nLen-1; i++)	// 两端不计算峰谷
		{
			if(data[i] == data[p])		// 平坦区
				q = i;
			else if(data[i] > data[p])	// 曲线上升
			{
				if(f == -1)	// 若上次为下降则找到一个谷值
				{
					int mpos = (p+q)/2;
					fpv.data[mpos] = localHigh(mpos, 6);
				}
				p = q = i;
				f = 1;
			}
			else if(data[i] < data[p])	// 曲线下降
			{
				if(f == 1)	// 若上次为上升则找到一个峰值
				{
					int mpos = (p+q)/2;
					fpv.data[mpos] = localHigh(mpos, 6);
				}
				p = q = i;
				f = -1;
			}
		}
	}

	// 计算峰谷间隔的均值 模块大小
	// 输入参数: MyArray<double> &ma  是由findPeakValley()计算得到的(本对象的)峰谷位置
	// 输入参数: double sharpDegreeTh 忽略 峰值度 较小的峰谷位置
	// 未使用哨兵 使用了本对象的data计算尖锐度
	void peakValleyGap(MyArray<double> &fpv, MyArray<double> &peakArray, MyArray<double> &valleyArray, double sharpDegreeTh)
	{
		double	sharpDegree;	// 峰值度
		int		numP=0, numV=0, p;

		// 找到第一个峰值 p
		for (p=0; p < fpv.nLen; p++)
		{
			if(fpv.data[p] > 0)
				break;
		}

		// 峰值点间隔 求和
		for (int i=p+1; i < fpv.nLen; i++)	// 峰谷位置标记从1开始
		{
			double peakPos = fpv.data[i];
			if (peakPos > 0)
			{
				// 计算尖锐度(=峰值减两边谷值的最小值)
				double peakData = data[roundI(peakPos)];
				int leftValley  = roundI(-fpv.data[i-1]);
				int rightValley = roundI(-fpv.data[i+1]);
				sharpDegree =	min(peakData - data[leftValley], peakData - data[rightValley]);

				if(sharpDegree > sharpDegreeTh)
				{
					peakArray.data[numP++] = peakPos - fpv.data[p];
					p = i;	// p记录上一个峰值位置
				}
			}
		}
		peakArray.nLen = numP;

		// 找到第一个谷值 k
		for (p=0; p < fpv.nLen; p++)
		{
			if(fpv.data[p] < 0)
				break;
		}

		// 谷值点间隔 求和
		for (int i=p+1; i < fpv.nLen; i++)	// 峰谷位置标记从1开始
		{
			double valleyPos = fpv.data[i];
			if (valleyPos < 0)
			{
				// 计算尖锐度 两边峰值减谷值的最小值
				double valleyData = data[roundI(-valleyPos)];
				int leftPeak  = roundI(fpv.data[i-1]);
				int rightPeak = roundI(fpv.data[i+1]);
				sharpDegree =	min(data[leftPeak]  - valleyData, data[rightPeak] - valleyData);

				if(sharpDegree > sharpDegreeTh)
				{
					valleyArray.data[numV++] = -valleyPos - (-fpv.data[p]);
					p = i;
				}
			}
		}
		valleyArray.nLen = numV;
	}
 
	// 按数据所在的位置输出, 数据间隔为1单位, 没有时补0
	// 注意: 数据应是已经排序好的
	void printAtPosition()
	{
		int i=0;
		for (int j=0; j <= data[nLen-1]; j++)
		{
			if(j == data[i])	cout << data[i++] << endl;
			else			cout << endl;
		}
	}

	// 屏幕输出字符串数组
	// hasEndl 是否含有回车	true每数据一个回车
	// outLen  输出长度
	void printChars(bool hasEndl=0, int outLen=0)
	{
		stringstream ss;  
		ss << typeid(*this).name();  	// 显示类名称
		cout /*<< endl*/ << ss.str() << ":" << endl;

		if(hasEndl)	cout << 0 << " ";
		cout << data[0];

		if(outLen <= 0) outLen=nLen;
		for (int j=1; j < outLen; j++)
		{
			if(hasEndl)	cout << endl << j << " ";
			else		cout << " ";
			cout << data[j];
		}
		cout << endl;
	}
	
	// 屏幕输出数组
	// hasEndl 是否含有回车	true每数据一个回车
	// outLen  输出长度
	void print(bool hasEndl=0, int outLen=0)
	{
		stringstream ss;  
		ss << typeid(*this).name();  	// 显示类名称
		cout << ss.str() << ":";// << endl;
		printf(" nLen= %d\n", nLen);
		if(nLen <=0) return;

		if(hasEndl)	cout << 0 << " ";
		printf("%.2f", data[0]);

		if(outLen <= 0) outLen=nLen;
		for (int j=1; j < outLen; j++)
		{
			if(hasEndl)	printf("\n%d ", j);
			else		printf(" ");
			printf("%.2f ", data[j]);
		}
		printf("\n");
	}

	static void print(vector<int> vd, bool hasEndl=0)
	{
		for (int i=0; i < vd.size(); i++)
		{
			if(hasEndl)
				printf("%d %d\n", i, vd[i]);
			else
				printf("%d ", vd[i]);
		}
		cout << endl;
	}

	static void print(vector<double> vd, bool hasEndl=0)
	{
		for (int i=0; i < vd.size(); i++)
		{
			if(hasEndl)
				printf("%d %.2f\n", i, vd[i]);
			else
				printf("%.2f ", vd[i]);	// cout << vd[i] << " ";	// 较大的数用科学计数法表示,这没关系.	//
		}
		cout << endl;
	}

	static void print(vector<string> vstr, bool hasEndl=0)
	{
		for (int i=0; i < vstr.size(); i++)
			if(hasEndl)
				printf("%d %s\n", i, vstr[i].c_str());
			else
				printf("%s ", vstr[i].c_str());
		cout << endl;
	}

	static void print(vector<vector<double> > vvd)
	{
		for (int i=0; i < vvd.size(); i++)
		{
			for (int j=0; j < vvd[i].size(); j++)
				cout << vvd[i][j] << " ";
			cout << endl;
		}
		cout << endl;
	}

		//////////////////// 曲线显示 方法 /////////////////////////////////////

	/* 功能：对数组缩放 输出float类型 输入数据为int类型
	int *pData, int nLen,	输入：数组数据及长度
	int newLen,				输入：新的数组长度
	float fDataScale		输入：数据内容的缩放比例(相乘)
	在函数内部申请了新数组的存储空间，需要外部释放
	*/ // 本函数不依赖与类内变量，可外部使用
	static float* myResize(int *pData, int nLen, int newLen, float fDataScale=1)
	{
		if (nLen < 1) return NULL;

		float fScaleX = (newLen - 1) / (nLen - 1.0);
		float* pA = (float*)malloc(newLen * sizeof(float));

		for (int j = 0; j < newLen; j++)
		{
			float fj = j / fScaleX;
			int jLeft = floor(fj);
			int jRight = ceil(fj);
			pA[j] = pData[jLeft] + (pData[jRight] - pData[jLeft])*(fj - jLeft);
			pA[j] *= fDataScale;
		}

		return pA;
	}

	// 功能：对数组缩放 输出double类型 输入数据为double类型
	static double* myResize(double *pData, int nLen, int newLen, double fDataScale=1)
	{
		if (nLen < 1) return NULL;

		double fScaleX = (newLen - 1) / (nLen - 1.0);
		double* pA = (double*)malloc(newLen * sizeof(double));

		for (int j = 0; j < newLen; j++)
		{
			double fj = j / fScaleX;
			int jLeft = floor(fj);
			int jRight = ceil(fj);
			pA[j] = pData[jLeft] + (pData[jRight] - pData[jLeft])*(fj - jLeft);
			pA[j] *= fDataScale;
		}

		return pA;
	}
	
	// 将数组长度进行插值缩放  内存重新申请
	void Resize(int _nLen)
	{
		T* p = myResize(data, nLen, _nLen);
		free(data);
		data = p;
		nLen = _nLen;
	}

	/* 在dataA中滑动查找dataB的匹配 (欧式距离比较)		int类型数据
		计算相位差phase difference
		要求dataA长于dataB
		返回 dataB在dataA中的匹配位置
	*/
	static int arraySlideMatch(int *dataA, int lenA, int *dataB, int lenB)// 加速匹配待续 测时间
	{
		int sumLen = lenA - lenB;
		int* psum = (int*)malloc(sumLen * sizeof(int));
		memset(psum, 0, sumLen * sizeof(int));

		int minstart, minend, minsum = MYINT_MAX;
		for (int i = 0; i < sumLen; i++)
		{
			int sum = 0;
			int *p = dataA + i;
			for(int j=0; j < lenB; j++)
			{
				sum += (p[j] - dataB[j])*(p[j] - dataB[j]);
			}
			sum = sqrt(sum/(float)lenB);

			if (sum <= minsum)
			{
				if (sum < minsum)
				{
					minsum = sum;
					minstart = i;
					minend = i;
				}
				else
					minend = i;
			}
			psum[i] = sum;
		}

		int d = 1;
		//printf("sumLen= %d\n", sumLen/2);
		//printf("minstart, minend, minsum: %d %d %d\n", minstart, minend, minsum);

		/*
		POINT pt[3] = {sumLen/2,353, (minstart+minend)/2,353, 2*WAVELEN/4,353};

		array2hist(psum, sumLen, NULL, 0,
			0, 10000,
			"psum", 800, 600, pt, 3);
		free(psum);*/

		return (minstart + minend) / 2;
	}

	/* 在dataA中滑动查找dataB的匹配 (欧式距离比较)		double类型数据
		计算相位差phase difference
		要求dataA长于dataB
		返回 dataB在dataA中的匹配位置
	*/
	static double arraySlideMatch(double *dataA, int lenA, double *dataB, int lenB)// 加速匹配待续 测时间
	{
		int sumLen = lenA - lenB;
		int minstart, minend;
		double minsum = MYINT_MAX;

		for (int i = 0; i < sumLen; i++)
		{
			double dsum = 0;
			double *p = dataA + i;
			for(int j=0; j < lenB; j++)
				dsum += (p[j] - dataB[j])*(p[j] - dataB[j]);
			dsum = sqrt(dsum/lenB);

			if (dsum <= minsum)	// 匹配曲线会有平坦区， 取中值
			{
				if (dsum < minsum)
				{
					minsum = dsum;
					minstart = i;
					minend = i;
				}
				else
					minend = i;
			}
			//outDouble2(i*1.0, dsum);
		}

		return (minstart + minend) / 2.0;
	}

	//************************************
	// Method:    getInterValue 获取两数之间的第i个插值
	// Parameter: double i			// 要计算的第i个数据	// 序号也用double
	// Parameter: double leftVal	// 输入左端值
	// Parameter: double left_i		// 输入左端值序号
	// Parameter: double rightVal	// 输入右端值
	// Parameter: double right_i	// 输入右端值序号
	//************************************
	double getInterValue(double i, double leftVal, double left_i, double rightVal, double right_i)
	{
		if(left_i==right_i)
		{
			printf("\n\tgetInterValue 左右端序号相等，除零错误！\n");
			return MYINT_MAX;
		}

//		double ival = rightVal*p - leftVal*(1-p);			// 左百分比乘右端数
		double ival = leftVal + (rightVal - leftVal)*(i-left_i)/(right_i-left_i);
		return ival;
	}

	//************************************
	// Method:    createSquarewave 生成方波
	// Parameter: int waveLen		波长,最好是2的倍数
	// Parameter: int flatWidth		平台宽度
	// Parameter: int minValue		波幅值最小值
	// Parameter: int maxValue		波幅值最大值
	// 波长和平台宽度都是数据个数
	// 波从最小值上升开始
	//************************************
	void createSquarewave(int waveLen, int flatWidth, double minValue=-10, double maxValue=10)
	{
		int i=0;
		double halfSlope = (waveLen-2*flatWidth)/4.0;

		MyArray_MALLOC(waveLen);

		// 峰左上升
		for (; i < halfSlope; i++)				// 斜坡含左端点, 不含右端点
			data[i] = getInterValue(i, (minValue+maxValue)/2, 0, maxValue, halfSlope);
		
		// 峰平坦区
		for (; i < halfSlope+flatWidth; i++)		
			data[i] = maxValue;

		// 峰右下降谷左下降
		for (; i < halfSlope*3+flatWidth; i++)		
			data[i] = getInterValue(i, maxValue, halfSlope+flatWidth, minValue, halfSlope*3+flatWidth);
		
		// 谷平坦区
		for (; i < halfSlope*3+flatWidth*2; i++)	
			data[i] = minValue;
		
		// 谷右上升
		for (; i < waveLen; i++)
			data[i] = getInterValue(i, minValue, halfSlope*3+flatWidth*2, (minValue+maxValue)/2, waveLen);
	}

	static void myCrossLine(Mat matSrc, int x, int y, int nlen=8)
	{
		line(matSrc, cvPoint(x - nlen / 2, y), cvPoint(x + nlen / 2, y), CV_RGB(255, 0, 255));
		line(matSrc, cvPoint(x, y - nlen / 2), cvPoint(x, y + nlen / 2), CV_RGB(255, 0, 255));
	}

	/* 功能：数组用直方图显示	(2路)	// 用自己的类对此方法进行简化修改, 待续 -mbh
		(若w==-1,则显示使用 w = 较长的数组长度)
		(若h==-1,则 h = maxData-minData)
		int *dataA, LONG64 lenA		第一路数组数据及长度,长度可为0(不显示)(废弃的参数,此处使用本对象数组)
		int *dataB, LONG64 lenB,	第二路数组数据及长度,长度可为0(不显示)
		int minData, int maxData,			数组数据内容的最大最小范围
		char* winname, int w, int h			显示直方图的窗口名称, 图像宽w高h(w,h=-1时自动取)
		POINT *pt, int ptlen				数据标记点坐标 数据进行同样的缩放,若无让其长度为0即可
		int r90								旋转90度, 1 顺时针, -1 逆时针+垂直翻转, 0 不旋转
		int dispMode
		最简单应用示例:
			mArray.array2hist(NULL, 0, 0, 10);// 仅显示一路数据
			mArray.array2hist(NULL, 0, 0, 10, "myHist3", 200, 100);// 仅显示一路数据
			mArray.array2hist(NULL, 0, -10, 265, "myHist3", 4*mArray.nLen, -1);// 仅显示一路数据
		有一个问题:
			缩放数据时不应该插值,插值显示会失真	待修改		-mbh
		*/
	void show_old(/*int *data, LONG64 nLen,*/ int *dataB, int lenB,
		int minData, int maxData,
		char* winname="MyArray", int w=0, int h=0,
		int r90=0, int dispMode=0, cv::Point *pt=NULL, int ptlen=0)
	{
		int		sLen = max((int)nLen, lenB);	// 取较长的数组计算 窗口显示比例(=w/nLen)
		if(w <= 0) w = sLen;
		if(h <= 0) h = maxData - minData;

		// 数据缩放 长度lenA缩放为w, 数据(高度)缩放(maxData-minData)*fScaleY
		int		wA = nLen*w / sLen;		// 其中有一个数组长度是等于w的
		int		wB = lenB*w / sLen;
		cv::Mat imghist(h+1, w, CV_8UC3, cv::Scalar::all(255));	// h+1 图像高度比最大最小的差大1(包含最大最小值)

		// 数据内容的缩放比例(相乘)
		float  fScaleY = (float)h / (maxData - minData);
		int	   limitL = minData * fScaleY;

		// 数组dataA数据点连线
		float *pA = myResize(data, nLen, wA, fScaleY);
		if(dispMode==0)	// 曲线图显示
		{
			for (int j = 0; j < wA - 1; j++)
			{
				int y1 = pA[j] - limitL;
				int y2 = pA[j + 1] - limitL;
				line(imghist, cvPoint(j, y1), cvPoint(j + 1, y2), CV_RGB(255, 0, 0), 1);
			}
		}
		else if(dispMode==1)	// 柱状图显示
		{
			for (int j = 0; j < wA; j++)
			{
				int y1 = pA[j] - limitL;
				if(y1 > 0)
					line(imghist, cvPoint(j, 0), cvPoint(j, y1), CV_RGB(18, 158, 235), 1);
			}
		}
		else if(dispMode==2)	// 点标记显示
		{
			for (int j = 0; j < wA; j++)
			{
				int y1 = pA[j] - limitL;
				if(y1 > 0)
					imghist.at<Vec3b>(y1,j)=Vec3b(255,0,255);
					//myCrossLine(imghist, j, y1, 6);
			}
		}
		free(pA);

		// 数组dataB数据点连线
		float *pB = myResize(dataB, lenB, wB, fScaleY);
		for (int j = 0; j < wB - 1; j++)
		{
			int y1 = pB[j] - limitL;
			int y2 = pB[j + 1] - limitL;
			//imghist.at<Vec3b>(y1, j) = Vec3b(0, 0, 255);
			line(imghist, cvPoint(j, y1), cvPoint(j + 1, y2), CV_RGB(0, 0, 255), 1);
		}
		free(pB);

		// 数据标记点 数据进行同样的缩放
 		float fScaleX = (float)w / sLen;
		for (int i = 0; i < ptlen; i++)
			myCrossLine(imghist, round(pt[i].x*fScaleX), round(pt[i].y*fScaleY - limitL), 16);

		// 均值线
	// 	float avrRate = 2;// 均值线所乘因子
	// 	LONG64 dataAavr=0, dataBavr=0;
	// 	for (int i = 0; i < lenA; i++) dataAavr += dataA[i];
	// 	for (int i = 0; i < lenB; i++) dataBavr += dataB[i];
	// 	dataAavr *= fScaleY*avrRate / lenA;
	// 	dataBavr *= fScaleY*avrRate / lenB;
	// 	line(imghist, cvPoint(0, dataAavr), cvPoint(wA - 1, dataAavr), CV_RGB(255, 0, 0), 1);
	// 	line(imghist, cvPoint(0, dataBavr), cvPoint(wB - 1, dataBavr), CV_RGB(0, 0, 255), 1);

		flip(imghist, imghist, 0);// 水平翻转,y轴向上增加

		// 标尺 纵向		// 若在flip前画标尺,字体就反了
		char numstr[32];
		int u,du=(maxData-minData)/10;
		for (u=10; u < du; u*=10)
		{
			if(du/u < 21)
			{
				du=du/u*u; break;
			}
		}
		for (int i=(minData/du+1)*du; i < maxData; i+=du)
		{
			int y1 = h - (i*fScaleY - limitL);
			sprintf(numstr,"%d", i);
			line(imghist, cvPoint(0, y1), cvPoint(w, y1), CV_RGB(230, 230, 230));
			putText(imghist,numstr,Point(8,y1),FONT_HERSHEY_SIMPLEX,0.4,Scalar(255,23,0),1,8);//在图片上写文字
		}

		// 标尺 横向 (需要修改 -mbh)
		int nf=1;//要以w/3的值来标注
		du=w/nf/10;
		if(du > 0)
		{
			for (u=10; u < du; u*=10)
			{
				if(du/u < 21)
				{
					du=du/u*u; break;
				}
			}
			for (int i=du; i < w/nf; i+=du)
			{
				int x1 = (i*nf+nf/2)*fScaleX;	// i*nf 是计算坐标位置的值
				sprintf(numstr,"%d", i);
				line(imghist, cvPoint(x1,0), cvPoint(x1,5), CV_RGB(150, 150, 150));
				putText(imghist,numstr,Point(x1,16),FONT_HERSHEY_SIMPLEX,0.4,Scalar(255,23,0),1,8);//在图片上写文字
			}
		}

		// 旋转与翻转
		if(r90==1)
			transpose(imghist, imghist), flip(imghist, imghist, 1);// 顺时针旋转90度
		else if (r90 == -1)
			transpose(imghist, imghist);	// 顺时针旋转90度+水平翻转(逆时针+垂直翻转)

		int nEdge=30;
		cv::Mat imghist2(h+nEdge*2, w+nEdge*2, CV_8UC3, cv::Scalar::all(230));
	//	rectangle(imghist2,cvPoint(nEdge-3,nEdge-3),cvPoint(w+nEdge+2,h+nEdge+2),CV_RGB(220,220,220));

		Mat imageROI = imghist2(Range(nEdge,imghist.rows+nEdge), Range(nEdge,imghist.cols+nEdge));
		imghist.copyTo(imageROI);
		imshow(winname, imghist2);
		//namedWindow(winname);
		//imshow(winname, imghist);
	}

	//************************************
	// Method:    dispCurve 用曲线显示数据
	// Parameter: int minData	数据最小值
	// Parameter: int maxData	数据最小值
	// Parameter: int wScale	宽度放大倍数(一个数据点所占跨度)
	//************************************
	cv::Mat showCurve(int mindata, int maxdata, int wScale=1, int hScale=1, char *showName=NULL, int rot90=0, int XUnit=50, int YUnit=50)
	{
		if(mindata == maxdata)
		{
			mindata = minData();
			mindata = max(0, mindata);
			maxdata = maxData() + 2;
		}

		int h = (maxdata - mindata)*hScale;
		cv::Mat curMat(h+1, nLen*wScale, CV_8UC3, cv::Scalar::all(255));// h+1 图像高度比最大最小的差大1(包含最大最小值)

		cout << "显示曲线图大小 w= " << nLen << "*" << wScale << " h= " << h+1 << endl;
		if(nLen < 3) return curMat;

		// y 轴单位刻度
		for (int i=mindata+1; i < maxdata; i++)
		{
			if(i % YUnit == 0)
				line(curMat, cvPoint(0,i-mindata), cvPoint(curMat.cols,i-mindata),Scalar::all(230));
		}

		// 底色 x轴每1单位间隔变色(为放大图像的区分单位使用)
		bool bFlag=false;
		for (int j=0; j < curMat.cols; j++)
		{
			if(j % wScale == 0)
				bFlag = !bFlag;
			if(bFlag)
				line(curMat, cvPoint(j,0), cvPoint(j,h-1),Scalar::all(235));
			// x 轴单位刻度线
			if(j > wScale && ((j/wScale)%XUnit == 0))
				line(curMat, cvPoint(j, 0), cvPoint(j, h-1), Scalar::all(200));
		}

		// y轴 零线刻度线 黑色实线
		if(mindata < 0)
			line(curMat, cvPoint(0,-mindata*hScale), cvPoint(curMat.cols,-mindata*hScale),Scalar::all(0));

		// 均值 粉色实线
		double avg = average();
		line(curMat, cvPoint(0,(avg-mindata)*hScale), cvPoint(curMat.cols,(avg-mindata)*hScale),CV_RGB(230,0,230));
		//outInt1Double1(mindata, avg);

		// 数据连线
		int y1, y2;
		for (int j = 0; j < nLen - 1; j++)
		{
			y1 = hScale*(data[j] - mindata);
			y2 = hScale*(data[j + 1] - mindata);
			// 每个数据点画在跨度中间
			line(curMat, cvPoint((j+0.5)*wScale, y1), cvPoint((j+1.5)*wScale, y2), CV_RGB(255, 0, 0), 1);
			//MyTools::crossLine(curMat, (j+0.5)*wScale, y1, 222);	// 十字线标记数据位置点
		}
		//MyTools::crossLine(curMat, (j+0.5)*wScale, y2, 222);		// 十字线标记数据位置点

		flip(curMat, curMat, 0);// 水平翻转,y轴向上增加
		MyMat mm(curMat);
		if(rot90==90)
		{
			mm.rotateN90();
		}

		if(showName==NULL)
			imshow("MyArray", mm.matImg);
		else
			imshow(showName, mm.matImg);
		return mm.matImg;
	}

	// 按数据所在的位置显示图像
	// 要求数组内的数据是位置信息,因此不可大于数据长度(data[i] < nLen)
	// int negSign=1 只标记数据大于0的位置, int negSign=-1 只标记数据小于0的位置
	void showAtPosition(int wScale=1, int XUnit=100, int negSign=1, char *showName=NULL)
	{
		if(nLen < 3) return;

		int h = 64;//maxData - minData;
		cv::Mat curMat(h, nLen*wScale, CV_8UC3, cv::Scalar::all(255));
		cout << "显示曲线图大小 w= " << nLen << "*" << wScale << " h= " << h << endl;

		// 此段可以提出来作为函数	待续  -mbh
		// 底色 x轴每1单位间隔变色(为放大图像的区分单位使用)
		bool bFlag=false;
		for (int j=0; j < curMat.cols; j++)
		{
			if(j % wScale == 0)
				bFlag = !bFlag;
			if(bFlag)
				line(curMat, cvPoint(j,0), cvPoint(j,h-1),Scalar::all(235));
			// x 轴单位刻度线
			if(j > wScale && ((j/wScale)%XUnit == 0))
				line(curMat, cvPoint(j, 0), cvPoint(j, h-1), Scalar::all(200));
		}

		// 数据位置点
		for (int j = 0; j < nLen; j++)
		{
			double x = data[j]*negSign;
			if(x > 0)
				myCrossLine(curMat, round((x+0.5)*wScale), h/2, 6);// 每个数据点画在跨度中间
		}

		if(showName==NULL)
			imshow("MyArray", curMat);
		else
			//mtools.imshowNoBorder(showName, curMat);
			imshow(showName, curMat);
	}

	// 建立和维护一个具有固定大小的队列, 先进先出
	// 输入参数示例:  static vector<int> vTData;
	//				 vTData.resize(5);
	static void push_stack(T data, vector<T> &vTData, int nLen)	// 不使用局部变量
	{
		//static vector<T> vInt(nLen);
		vTData.erase(vTData.begin());
		vTData.push_back(data);
	}

	void getImgHistH(cv::Mat &matGray)
	{
		for (int i=0; i < matGray.rows; i++)
		{
			uchar* p = matGray.ptr<uchar>(i);
			for (int j=0; j < matGray.cols; j++)
				data[j] += p[j];
		}
		nLen = matGray.cols;
	}

	void getImgHistV(cv::Mat &matGray)
	{
		for (int i=0; i < matGray.rows; i++)
		{
			uchar* p = matGray.ptr<uchar>(i);
			for (int j=0; j < matGray.cols; j++)
				data[i] += p[j];
		}
		nLen = matGray.rows;
	}

	// 生成 正方形的 边上所有点 到 中心的距离 数组
	// 重新申请内存 释放旧的数据
	void createRectDistCenter(double nSize)	// 所对应的正方形边长为nSize+1
	{
		double r=round(nSize/2.0);
		double r2=r*r;

		remalloc(5*(nSize+1));

		// 正方形的一个角 对中心的距离数组
		for (int i=0; i < r; i++)
			data[nLen++] = sqrt(r2+i*i)-r;
		for (int j=r; j > 0; j--)
			data[nLen++] = sqrt(j*j+r2)-r;

		// 复制4次得到 整个矩形的 多余一次用于滑动匹配方便
		selfcopy(4);
	}

	/////////////// 以下为矩阵操作 ////////////////////////

	// 删除行row, 删除列col	// 仅支持整数行
	// 此操作删除后,行字节数widthstep不变
	void remove(int row, int col, int w, int h, int widthstep)	// 小于0不删除
	{
		// 先删除列,否则会找不到行
		if(col >= 0)
		{
			for (int i=0; i < h; i++)
				memcpy(data+i*widthstep+col, data+i*widthstep+col+1, (w-col-1)*sizeof(T));
		}

		if(row >= 0)
		{
			for (int i=row+1; i < h; i++)
				memcpy(data+(i-1)*widthstep, data+i*widthstep, w*sizeof(T));
		}
	}
};  //end class MyArray

typedef MyArray<uchar>		MyArrayU;
typedef MyArray<ushort>		MyArrayUS;
typedef MyArray<int>		MyArrayI;
typedef MyArray<double>		MyArrayD;

template<class T>
ostream& operator<<(ostream &output, const MyArray<T> &ma){
	for (int i=0; i < ma.nLen; i++)
		output << " " << ma.data[i];  
	return output;
}

// 在灰度图上取两点之间的直线灰度值, 间隔为逻辑单位1
template<class T>	// 必须加此模板声明,且参数中也必须使用T类型,否则会发生重定义错误
MyArray<T> getLineData(Mat matGray, MyPoint<T> pt0, MyPoint<T> pt1)
{
	double	ptDist = pt0.distance(pt1);
	int		nLen = round(ptDist);
	MyArray<T> mb(nLen);

	for (int i=0; i < nLen; i++)
	{
		MyPoint<double> pf = pt0.shiftPointFast(pt0, pt1, i/ptDist);
		//ma.d[i] = binGray.at<uchar>(pf.cv());	// 最近邻
		mb.data[i] = pf.getPixelValue_Interpolation(matGray);		// 插值
	}

	return mb;
}

