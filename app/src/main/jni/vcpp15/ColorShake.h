#include <omp.h>
#include "MyHeadDef.h"

class ColorShake
{
public:
    ColorShake(void){
// 		maxLoop = 28;	// 找亮度最大值的最大循环次数
// 		thBright = 6;	// 找亮度最大值时的最低阈值
// 		thExtend = 3;	// 亮度区域扩展容差
// 		thMinW = 1;		// 对找到的亮度区域过滤最小宽高
	}
    ~ColorShake(void){}

	static void addFrameDiff(cv::Mat &matcur, cv::Mat &matnxt, cv::Mat &matacc, int ACC_FRAME_DIFF_TH, int dx)
	{
		for (int i = 0; i < matacc.rows; i++)
		{
			Vec3b* q = matcur.ptr<Vec3b>(i);
			Vec3b* p = matnxt.ptr<Vec3b>(i);
			uchar* a = matacc.ptr<uchar>(i);
			for (int j = 0; j < matacc.cols; j++)
			{
				uchar b = p[j][0];
				uchar g = p[j][1];
				uchar r = p[j][2];
				//if (40 < r && r < 240 && g + 90 > r&& r > g + 8 && g > b + 8 &&
				//	b + 100 < r + r && r < b + b + 150 &&
				//	0.6 * g < b + 50 && 5 * g < b + 800)	// v0.4 2019.12.3 白天火 浅红黄色
				//if (r > g && g > b && r - 100 < g && g < 3 * r - 300 &&
				//	1.5 * g - 75 > b && b > 0.33 * g - 33)	// v0.3 2019.11.09
				if (r > 40 && r > g && g > b &&
					r-100 < g && g < b+b+b+100 &&
					b < r+r-100)							// v0.5 2019.12.4 合并白天版本范围大
				{
					uchar b2 = q[j][0];
					uchar g2 = q[j][1];
					uchar r2 = q[j][2];
					float db = b - b2;
					float dg = g - g2;
					float dr = r - r2;
					b = std::sqrt(db * db + dg * dg + dr * dr);
					if (b > ACC_FRAME_DIFF_TH)
						a[j] += dx;
				}
			}
		}
	}

	// 颜色帧差累计 累计帧数
	static void colorDistMap(char* vfm, cv::Mat matImg, cv::Mat& matacc, int ACC_FRAME_DIFF_SIZE, int ACC_FRAME_DIFF_TH)
	{
		int w = matImg.cols;
		int h = matImg.rows;
		int ws = matImg.step;
		int g = vfm[0];						// 当前帧放入的编号

		vector<cv::Mat> vcm;
		for (int i = 0; i < ACC_FRAME_DIFF_SIZE; i++)
        {
            cv::Mat matsrc(matImg.size(), CV_8UC3, vfm + i * h * ws);
            vcm.push_back(matsrc);
        }

		// 减第一帧帧差
		int fcur = g % ACC_FRAME_DIFF_SIZE;
		int fnxt = (fcur + 1) % ACC_FRAME_DIFF_SIZE;
		addFrameDiff(vcm[fcur], vcm[fnxt], matacc, ACC_FRAME_DIFF_TH, -1);

		matImg.copyTo(vcm[g]);
		vfm[0] = (g + 1) % ACC_FRAME_DIFF_SIZE;	// 下一帧要放入的编号

		// 加当前帧帧差
		fcur = g > 0 ? g - 1 : ACC_FRAME_DIFF_SIZE - 1;
		fnxt = g % ACC_FRAME_DIFF_SIZE;
		addFrameDiff(vcm[fcur], vcm[fnxt], matacc, ACC_FRAME_DIFF_TH, 1);
	}

	// 颜色帧差累计 累计帧数
	static void colorDistMapGray(char* vfm, cv::Mat matImg, cv::Mat& matacc, int ACC_FRAME_DIFF_SIZE, int ACC_FRAME_DIFF_TH)
	{
		int w = matImg.cols;
		int h = matImg.rows;
		int ws = matImg.step;
		int g = vfm[0];						// 当前帧放入的编号

		vector<cv::Mat> vcm;
		for (int i = 0; i < ACC_FRAME_DIFF_SIZE; i++)
                {
                    cv::Mat matsrc(matImg.size(), CV_8UC1, vfm + i * h * ws);
                    vcm.push_back(matsrc);
                }

		// 减第一帧帧差
		int fcur = g % ACC_FRAME_DIFF_SIZE;
		int fnxt = (fcur + 1) % ACC_FRAME_DIFF_SIZE;
		for (int i = 0; i < matImg.rows; i++)
		{
			uchar* q = vcm[fcur].ptr<uchar>(i);
			uchar* p = vcm[fnxt].ptr<uchar>(i);
			uchar* a = matacc.ptr<uchar>(i);
			for (int j = 0; j < matImg.cols; j++)
			{
				if (p[j] > 220 && abs(p[j] - q[j]) > ACC_FRAME_DIFF_TH)
					a[j]--;
			}
		}

		matImg.copyTo(vcm[g]);
		vfm[0] = (g + 1) % ACC_FRAME_DIFF_SIZE;	// 下一帧要放入的编号

		// 加当前帧帧差
		fcur = g > 0 ? g - 1 : ACC_FRAME_DIFF_SIZE - 1;
		fnxt = g % ACC_FRAME_DIFF_SIZE;
		for (int i = 0; i < matImg.rows; i++)
		{
			uchar* q = vcm[fcur].ptr<uchar>(i);
			uchar* p = vcm[fnxt].ptr<uchar>(i);
			uchar* a = matacc.ptr<uchar>(i);
			for (int j = 0; j < matImg.cols; j++)
			{
				if (p[j] > 220 && abs(p[j]-q[j]) > ACC_FRAME_DIFF_TH)
						a[j]++;
			}
		}
	}

	static int findBrightBlob(cv::Mat matacc, vector<cv::Rect>& vRect, vector<uchar> &vBVal,
						int maxLoop, int thBright, int thExtend, int thMinW, int nDebug)
	{
		MyMatGray	mmg(matacc);
		int i, mx, my, mb, nTotal = 0;
		cv::Mat imgFlag;

		for (i = 0; i < maxLoop; i++)
		{
			vector<cv::Point> outPoints;

			// 找亮度最大值点
			mb = mmg.findMax(mx, my, imgFlag);
			if (mb < thBright) break;	// 当检测到的最大亮度值 < thBright 时停止检测

			// 从最亮点区域生长
			int en = mmg.colorExtendMaxPoint(mx, my, outPoints, thExtend, imgFlag, 1888);
			if (en == 1888) break;	// 面积太大, 非目标区域

			// 对找到的方框区标记已处理 最少为一个点
			MyRectD mrd(outPoints[0].x, outPoints[1].y, outPoints[2].x - outPoints[0].x + 1, outPoints[3].y - outPoints[1].y + 1);
			imgFlag(mrd.cvt()).setTo(1);

			if (mrd.w >= thMinW && mrd.h >= thMinW)	// 找到符合宽高要求的区域
			{
				nTotal++;
				vRect.push_back(mrd.cvt());
				vBVal.push_back(mb);
			}
		}

		return nTotal;
	}
};
