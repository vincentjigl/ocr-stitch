#pragma once
#include "MyMatGray.h"

// ?????????????? 0??255

// ????????????????????????????????
// ?磺imshowNoBorder
// ????????????????????????????磺
//		MyMatBin(const char* imgFile, bool flags=1):MyMat(imgFile, flags){}
class MyMatBin:public MyMatGray
{
public:
	MyMatBin(void){};
	~MyMatBin(void){};

        // 使用父类构造函数
        MyMatBin(cv::Mat matSrc):MyMatGray(matSrc){
		//matSrc.copyTo(matImg);
		//AdaptiveThereshold(S,T);
	}

        // 使用父类构造函数
        MyMatBin(const char* imgFile):MyMatGray(imgFile){
        }

	void thresholdOTSU()	// ????????????
	{
		cv::threshold(matImg, matImg, 0, 255, CV_THRESH_OTSU);  
	}

	// threshold_type=CV_THRESH_BINARY:如果 src(x,y)>threshold ,dst(x,y) = max_value; 否则,dst（x,y）=0;
	void thresholdBin(int th)
	{
		cv::threshold(matImg, matImg, th, 255, CV_THRESH_BINARY);
	}

    // 自适应局部二值化
    /*  同opencv函数 cv::adaptiveThreshold
        int blockSize = 45;         // 要求奇数
        int constValue = 8;
        cv::Mat res1,res2;
        cv::adaptiveThreshold(matgray, res1, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, blockSize, constValue);
        cv::adaptiveThreshold(matgray, res2, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, constValue);
        imshow("res1",res1);
        imshow("res2",res2);
    */
	void thresholdAdaptive(int S=44, int T=8)
	{
		 int x1, y1, x2, y2;  
		 int count=0;
		 long long sum=0;  
		 //int S=src.rows>>3;  //??????????СS*S
		 //int S=44;
		 //printf("S= %d\n", S);
		 /*int T=8;         ??????????????????????????If the value of the current pixel is t percent less than this average  
							 then it is set to black, otherwise it is set to white.*/  
		 int W=matImg.cols;  
		 int H=matImg.rows;  
		 long long **Argv; 
		 Argv=new long long*[matImg.rows];  
		 for(int ii=0;ii<matImg.rows;ii++)  
			 Argv[ii]=new long long[matImg.cols];  

		 for(int i=0;i<W;i++)  
		 {  
			 sum=0;  
			 for(int j=0;j<H;j++)  
			 {             
				 sum+=matImg.at<uchar>(j,i);  
				 if(i==0)      
					 Argv[j][i]=sum;  
				 else  
					 Argv[j][i]=Argv[j][i-1]+sum;  
			 }  
		 }

		 for(int i=0;i<W;i++)  
		 {  
			 for(int j=0;j<H;j++)  
			 {  
				 x1=i-S/2;  
				 x2=i+S/2;  
				 y1=j-S/2;  
				 y2=j+S/2;  
				 if(x1<0)  
					 x1=0;  
				 if(x2>=W)  
					 x2=W-1;  
				 if(y1<0)  
					 y1=0;  
				 if(y2>=H)  
					 y2=H-1;  
				 count=(x2-x1)*(y2-y1);  
				 sum=Argv[y2][x2]-Argv[y1][x2]-Argv[y2][x1]+Argv[y1][x1];  

				 if((long long)(matImg.at<uchar>(j,i)*count)<(long long)sum*(100-T)/100)  
					 matImg.at<uchar>(j,i)=0;  
				 else  
					 matImg.at<uchar>(j,i)=255;  
			 }
		 }

		 for (int i = 0 ; i < matImg.rows; ++i)  
			 delete [] Argv[i];   
		 delete [] Argv;
	}

	// ???μ? ???????????????????????????????255????????????????
	// δ???????????????
	void findIsland()
	{
		int w = matImg.cols;
		int h = matImg.rows;
		int minLineWidth=1, maxLineWidth=9;
		cv::Mat img3ch;
		cv::Mat matDstBin;

		cvtColor(matImg, img3ch, CV_GRAY2BGR);
		//img3ch /= 8;

		int maxS0=0;
		int maxS255=0;
		int k=5;
		for (int i=maxLineWidth; i < h - maxLineWidth; i++)
		{
			for (int j=maxLineWidth; j < w - maxLineWidth; j++)
			{
				cv::Mat subWin(matImg(Rect(j,i,k,k)));
				cv::Mat subWinE(matImg(Rect(j-1,i-1,k+2,k+2)));
				cv::Scalar sc = cv::sum(subWin);
				cv::Scalar scE = cv::sum(subWinE);
				if( sc.val[0] < k*k*255 &&
					scE.val[0] - sc.val[0] == (k+1)*4*255)
					cv::rectangle(img3ch, cvPoint(j-1,i-1), cvPoint(j+k+2,i+k+2), CV_RGB(255,0,0));
			}
		}

		printf("maxS= %d\n", maxS0);
		imshow("img3ch", img3ch);
	}

	uchar myPointGetValue(MyPointI &pt, int x, int y)
	{
		MyPointI m0 = pt.shiftPoint(x, y);
		return m0.getPixelValue(matImg);
	}

#define CIRCLE_POINT_NUM 16
	// ???nLen??????????thVal, ???????thVal?????
	bool checkNeighbourhood(int x, int y, MyPoint<int> * pt, int centerVal)
	{
		int nError=0;	// ????????????
		uchar th=5;
 		uchar b[CIRCLE_POINT_NUM];

		b[0] = myPointGetValue(pt[0], x, y);

		if(b[0] < centerVal-th)// ????????
		{
			for (int i=1; i < CIRCLE_POINT_NUM-1; i+=2)
			{
				b[i] = myPointGetValue(pt[i], x, y);
				if(!(b[i] > b[i-1]+th))
				{
					//if(nError++ > 0)
						return false;
				}
				b[i+1] = myPointGetValue(pt[i+1], x, y);
			}
		}
		else if(b[0] > centerVal-th) // ????????
		{
			for (int i=1; i < CIRCLE_POINT_NUM-1; i+=2)
			{
				b[i] = myPointGetValue(pt[i], x, y);
				if(!(b[i] < b[i-1]-th))
				{
					//if(nError++ > 0)
						return false;
				}
				b[i+1] = myPointGetValue(pt[i+1], x, y);
			}
		}
		else
			return false;

		return true;
	}

	// ???(???)??????
	// int minR, int maxR ????????С?????? ?????Χ2~16
	void findAsterisk(vector<MyPointI> &vmp, int minR=2, int maxR=16)
	{
		int w = matImg.cols;
		int h = matImg.rows;

		// ????16??, ?8????45?????, ??8?????8??????? ?????????? ????
// 		POINT circle8ptInt[15*CIRCLE_POINT_NUM]={2,0,1,1,0,2,-1,1,-2,0,-1,-1,0,-2,1,-1,		2,1,1,2,-1,2,-2,1,-2,-1,-1,-2,1,-2,2,-1,// r=2 d=5,         
// 			3,0,2,2,0,3,-2,2,-3,0,-2,-2,0,-3,2,-2,                3,1,1,3,-1,3,-3,1,-3,-1,-1,-3,1,-3,3,-1,			// r=3 d=7,         
// 			4,0,3,3,0,4,-3,3,-4,0,-3,-3,0,-4,3,-3,                4,2,2,4,-2,4,-4,2,-4,-2,-2,-4,2,-4,4,-2,			// r=4 d=9,         
// 			5,0,4,4,0,5,-4,4,-5,0,-4,-4,0,-5,4,-4,                5,2,2,5,-2,5,-5,2,-5,-2,-2,-5,2,-5,5,-2,			// r=5 d=11,        
// 			6,0,4,4,0,6,-4,4,-6,0,-4,-4,0,-6,4,-4,                6,2,2,6,-2,6,-6,2,-6,-2,-2,-6,2,-6,6,-2,			// r=6 d=13,        
// 			7,0,5,5,0,7,-5,5,-7,0,-5,-5,0,-7,5,-5,                6,3,3,6,-3,6,-6,3,-6,-3,-3,-6,3,-6,6,-3,			// r=7 d=15,        
// 			8,0,6,6,0,8,-6,6,-8,0,-6,-6,0,-8,6,-6,                7,3,3,7,-3,7,-7,3,-7,-3,-3,-7,3,-7,7,-3,			// r=8 d=17,        
// 			9,0,6,6,0,9,-6,6,-9,0,-6,-6,0,-9,6,-6,                8,3,3,8,-3,8,-8,3,-8,-3,-3,-8,3,-8,8,-3,			// r=9 d=19,        
// 			10,0,7,7,0,10,-7,7,-10,0,-7,-7,0,-10,7,-7,            9,4,4,9,-4,9,-9,4,-9,-4,-4,-9,4,-9,9,-4,			// r=10 d=21,       
// 			11,0,8,8,0,11,-8,8,-11,0,-8,-8,0,-11,8,-8,            10,4,4,10,-4,10,-10,4,-10,-4,-4,-10,4,-10,10,-4,	// r=11 d=23,
// 			12,0,8,8,0,12,-8,8,-12,0,-8,-8,0,-12,8,-8,            11,5,5,11,-5,11,-11,5,-11,-5,-5,-11,5,-11,11,-5,	// r=12 d=25,
// 			13,0,9,9,0,13,-9,9,-13,0,-9,-9,0,-13,9,-9,            12,5,5,12,-5,12,-12,5,-12,-5,-5,-12,5,-12,12,-5,	// r=13 d=27,
// 			14,0,10,10,0,14,-10,10,-14,0,-10,-10,0,-14,10,-10,    13,5,5,13,-5,13,-13,5,-13,-5,-5,-13,5,-13,13,-5,	// r=14 d=29,
// 			15,0,11,11,0,15,-11,11,-15,0,-11,-11,0,-15,11,-11,    14,6,6,14,-6,14,-14,6,-14,-6,-6,-14,6,-14,14,-6,	// r=15 d=31,
// 			16,0,11,11,0,16,-11,11,-16,0,-11,-11,0,-16,11,-11,    15,6,6,15,-6,15,-15,6,-15,-6,-6,-15,6,-15,15,-6};	// r=16 d=33 
		// ????16?? ???? ???????0 ???22.5??
                int circle8ptInt[30*CIRCLE_POINT_NUM]={2,0,2,1,1,1,1,2,0,2,-1,2,-1,1,-2,1,-2,0,-2,-1,-1,-1,-1,-2,0,-2,1,-2,1,-1,2,-1,	// r=2   d=5,
			3,0,3,1,2,2,1,3,0,3,-1,3,-2,2,-3,1,-3,0,-3,-1,-2,-2,-1,-3,0,-3,1,-3,2,-2,3,-1,                        // r=3   d=7,
			4,0,4,2,3,3,2,4,0,4,-2,4,-3,3,-4,2,-4,0,-4,-2,-3,-3,-2,-4,0,-4,2,-4,3,-3,4,-2,                        // r=4   d=9,
			5,0,5,2,4,4,2,5,0,5,-2,5,-4,4,-5,2,-5,0,-5,-2,-4,-4,-2,-5,0,-5,2,-5,4,-4,5,-2,                        // r=5  d=11,
			6,0,6,2,4,4,2,6,0,6,-2,6,-4,4,-6,2,-6,0,-6,-2,-4,-4,-2,-6,0,-6,2,-6,4,-4,6,-2,                        // r=6  d=13,
			7,0,6,3,5,5,3,6,0,7,-3,6,-5,5,-6,3,-7,0,-6,-3,-5,-5,-3,-6,0,-7,3,-6,5,-5,6,-3,                        // r=7  d=15,
			8,0,7,3,6,6,3,7,0,8,-3,7,-6,6,-7,3,-8,0,-7,-3,-6,-6,-3,-7,0,-8,3,-7,6,-6,7,-3,                        // r=8  d=17,
			9,0,8,3,6,6,3,8,0,9,-3,8,-6,6,-8,3,-9,0,-8,-3,-6,-6,-3,-8,0,-9,3,-8,6,-6,8,-3,                        // r=9  d=19,
			10,0,9,4,7,7,4,9,0,10,-4,9,-7,7,-9,4,-10,0,-9,-4,-7,-7,-4,-9,0,-10,4,-9,7,-7,9,-4,                    // r=10 d=21,
			11,0,10,4,8,8,4,10,0,11,-4,10,-8,8,-10,4,-11,0,-10,-4,-8,-8,-4,-10,0,-11,4,-10,8,-8,10,-4,            // r=11 d=23,
			12,0,11,5,8,8,5,11,0,12,-5,11,-8,8,-11,5,-12,0,-11,-5,-8,-8,-5,-11,0,-12,5,-11,8,-8,11,-5,            // r=12 d=25,
			13,0,12,5,9,9,5,12,0,13,-5,12,-9,9,-12,5,-13,0,-12,-5,-9,-9,-5,-12,0,-13,5,-12,9,-9,12,-5,            // r=13 d=27,
			14,0,13,5,10,10,5,13,0,14,-5,13,-10,10,-13,5,-14,0,-13,-5,-10,-10,-5,-13,0,-14,5,-13,10,-10,13,-5,    // r=14 d=29,
			15,0,14,6,11,11,6,14,0,15,-6,14,-11,11,-14,6,-15,0,-14,-6,-11,-11,-6,-14,0,-15,6,-14,11,-11,14,-6,    // r=15 d=31,
			16,0,15,6,11,11,6,15,0,16,-6,15,-11,11,-15,6,-16,0,-15,-6,-11,-11,-6,-15,0,-16,6,-15,11,-11,15,-6};   // r=16 d=33,
		// ????16?? ???? ???????11.25?? ???22.5??
                int circle8ptInt2[30*CIRCLE_POINT_NUM]={2,0,2,1,1,2,0,2,0,2,-1,2,-2,1,-2,0,-2,0,-2,-1,-1,-2,0,-2,0,-2,1,-2,2,-1,2,0,	// r=2  d= 5,
			3,1,2,2,2,2,1,3,-1,3,-2,2,-2,2,-3,1,-3,-1,-2,-2,-2,-2,-1,-3,1,-3,2,-2,2,-2,3,-1,                   // r=3  d= 7,
			4,1,3,2,2,3,1,4,-1,4,-2,3,-3,2,-4,1,-4,-1,-3,-2,-2,-3,-1,-4,1,-4,2,-3,3,-2,4,-1,                   // r=4  d= 9,
			5,1,4,3,3,4,1,5,-1,5,-3,4,-4,3,-5,1,-5,-1,-4,-3,-3,-4,-1,-5,1,-5,3,-4,4,-3,5,-1,                   // r=5  d=11,
			6,1,5,3,3,5,1,6,-1,6,-3,5,-5,3,-6,1,-6,-1,-5,-3,-3,-5,-1,-6,1,-6,3,-5,5,-3,6,-1,                   // r=6  d=13,
			7,1,6,4,4,6,1,7,-1,7,-4,6,-6,4,-7,1,-7,-1,-6,-4,-4,-6,-1,-7,1,-7,4,-6,6,-4,7,-1,                   // r=7  d=15,
			8,2,7,4,4,7,2,8,-2,8,-4,7,-7,4,-8,2,-8,-2,-7,-4,-4,-7,-2,-8,2,-8,4,-7,7,-4,8,-2,                   // r=8  d=17,
			9,2,7,5,5,7,2,9,-2,9,-5,7,-7,5,-9,2,-9,-2,-7,-5,-5,-7,-2,-9,2,-9,5,-7,7,-5,9,-2,                   // r=9  d=19,
			10,2,8,6,6,8,2,10,-2,10,-6,8,-8,6,-10,2,-10,-2,-8,-6,-6,-8,-2,-10,2,-10,6,-8,8,-6,10,-2,           // r=10 d=21,
			11,2,9,6,6,9,2,11,-2,11,-6,9,-9,6,-11,2,-11,-2,-9,-6,-6,-9,-2,-11,2,-11,6,-9,9,-6,11,-2,           // r=11 d=23,
			12,2,10,7,7,10,2,12,-2,12,-7,10,-10,7,-12,2,-12,-2,-10,-7,-7,-10,-2,-12,2,-12,7,-10,10,-7,12,-2,   // r=12 d=25,
			13,3,11,7,7,11,3,13,-3,13,-7,11,-11,7,-13,3,-13,-3,-11,-7,-7,-11,-3,-13,3,-13,7,-11,11,-7,13,-3,   // r=13 d=27,
			14,3,12,8,8,12,3,14,-3,14,-8,12,-12,8,-14,3,-14,-3,-12,-8,-8,-12,-3,-14,3,-14,8,-12,12,-8,14,-3,   // r=14 d=29,
			15,3,12,8,8,12,3,15,-3,15,-8,12,-12,8,-15,3,-15,-3,-12,-8,-8,-12,-3,-15,3,-15,8,-12,12,-8,15,-3,   // r=15 d=31,
			16,3,13,9,9,13,3,16,-3,16,-9,13,-13,9,-16,3,-16,-3,-13,-9,-9,-13,-3,-16,3,-16,9,-13,13,-9,16,-3};  // r=16 d=33,
		MyPointI *mpi = (MyPointI*)circle8ptInt;
		MyPointI *mpi2 = (MyPointI*)circle8ptInt2;

		cv::Mat imgFlag = cv::Mat::zeros(matImg.size(),CV_8UC1);;
// 		Mat img3ch;
// 		cvtColor(matImg, img3ch, CV_GRAY2BGR);
		
		for (int c=minR-2; c < maxR-1; c++)
		{
			int ignoreW=mpi[c*CIRCLE_POINT_NUM].x;
			for (int i=ignoreW; i < h - ignoreW; i++)
			{
				for (int j=ignoreW; j < w - ignoreW; j++)
				{
					uchar b = matImg.at<uchar>(i,j);
					if(b < 127) continue;

					bool ze = checkNeighbourhood(j,i, (MyPointI *)mpi+c*CIRCLE_POINT_NUM, b);
	 				if(!ze)
						ze = checkNeighbourhood(j,i, (MyPointI *)mpi2+c*CIRCLE_POINT_NUM, b);
					if(ze)
						ze = _checkHalfSum(j, i, ignoreW);
					if(ze)
					{
						//cv::circle(img3ch, cvPoint(j,i),ignoreW, CV_RGB(255,0,0));
 						uchar b = imgFlag.at<uchar>(i,j); 
 						imgFlag.at<uchar>(i,j) = b+1;
					}
				}
			}
			//outInt2(c, ignoreW);
		}
		 
		_findPeaks(imgFlag, 8, vmp);
// 		for (int i=0; i < vmp.size(); i++)
// 		{
// 			cout << vmp[i] << endl;
// 			cv::circle(img3ch, vmp[i].cv(), 16, CV_RGB(0,0,255), 2);
// 		}
// 
  	//imshow("img3ch", img3ch);
// 		imshow("imgFlag", imgFlag*40);
	}

	// ???????????????????? ???????101 ????????010
	// ???????4?????, ??????????
	void find2AsteriskFeature(vector<MyPointI> &vmp)
	{
		if(vmp.size() < 4) return;
		pointArrange(vmp);

		MyPointI m1 = midPoint(vmp[0], vmp[1]);
		MyPointI m0 = midPoint(vmp[0], m1);
		MyPointI m2 = midPoint(m1, vmp[1]);
		uchar b0 = m0.getPixelValue(matImg);
		uchar b1 = m1.getPixelValue(matImg);
		uchar b2 = m2.getPixelValue(matImg);

		cout << "m0: " << m0 << " "; outInt1(b0);
		cout << "m0: " << m1 << " "; outInt1(b1);
		cout << "m0: " << m2 << " "; outInt1(b2);

		m1 = midPoint(vmp[2], vmp[3]);
		m0 = midPoint(vmp[2], m1);
		m2 = midPoint(m1, vmp[3]);
		b0 = m0.getPixelValue(matImg);
		b1 = m1.getPixelValue(matImg);
		b2 = m2.getPixelValue(matImg);

		cout << "m0: " << m0 << " "; outInt1(b0);
		cout << "m0: " << m1 << " "; outInt1(b1);
		cout << "m0: " << m2 << " "; outInt1(b2);
	}

//	void showImg(double fScale=1, char* showName=NULL)
//	{
//		if(showName == NULL)
//			MyMatGray::showImg(fScale, "MyMatBin");
//		else
//			MyMatGray::showImg(fScale, showName);
//	}

	void save(char* showName=NULL)
	{
		if(showName == NULL)
			cv::imwrite("MyMatBin.jpg", matImg);
		else
			cv::imwrite(showName, matImg);
	}

	// ????????????????, ????????, ???Χ????, ???????????
	int contoursProcess()
	{
		MyString ms;

		// (1) ?????二值化
		MyMatBin mmb(matImg);
		threshold(mmb.matImg, mmb.matImg, 0, 255, CV_THRESH_OTSU);
		bitwise_not(mmb.matImg,mmb.matImg);

		// (2) 1 ????????????? 2 ????????????С??	// ?????????С???壬?????????С????
		morphologyEx(mmb.matImg, mmb.matImg,MORPH_CLOSE,Mat(3,3,CV_8U),Point(-1,-1),1);	// ?????????????????????????С????
		morphologyEx(mmb.matImg, mmb.matImg,MORPH_OPEN,Mat(3,3,CV_8U),Point(-1,-1),1);	// ?????????????????????????С????(?????С????)
		//erode(mmb.matImg,mmb.matImg,Mat(2,2,CV_8U),Point(-1,-1));	// ?????С?????????
		//dilate(mmb.matImg,mmb.matImg,Mat(5,5,CV_8U),Point(-1,-1),2);// ?????С?????????
		//mmb.showImg(1, "mmb1dilate");

		// (3) ??????????Χ????
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		// CV_RETR_EXTERNAL ???????????????c0???2?е???????????????????У??????????б???????
		// CV_CHAIN_APPROX_SIMPLE????????????б???????????????????
		findContours(mmb.matImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);// CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE
		Mat resultImage = Mat ::zeros(mmb.matImg.size(),CV_8U);
		drawContours(resultImage, contours, -1, Scalar(255, 0, 255));
		imshow("resultImage", resultImage);

		// (4) ???Χ????????????
		ms.append("Bar Rect:");
		Mat drawing = Mat::zeros(resultImage.size(), CV_8UC3);
		vector<Rect> boundRect(contours.size());
		for (int i = 0; i < contours.size(); i++)  
		{
			boundRect[i] = boundingRect(Mat(contours[i]));
			ms.appendln(boundRect[i].x, boundRect[i].y, boundRect[i].width, boundRect[i].height);
			rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), CV_RGB(187,187,187), 1, 8, 0);
		}
		STRVAR_APPEND(ms, (int)contours.size());
		imshow("drawing", drawing);
		ms.print();
		
		return 0;
	}

	// 计算三种对称性的值
	void isSymmetry(float &symV, float &symH, float &symC)
	{
		int w=matImg.cols;
		int h=matImg.rows;
		int s=0;

		// 上下对称
		for (int i=0; i < h; i++)
		{
			uchar* p = matImg.ptr<uchar>(i);
			uchar* q = matImg.ptr<uchar>(h-1-i);
			for (int j=0; j < w/2; j++)
			{
				if(p[j] == q[j])
					s++;
			}
		}

		symV = 1.0*s/(w*h);

		// 左右对称 对称点相等的个数
		s=0;
		for (int i=0; i < h; i++)
		{
			uchar* p = matImg.ptr<uchar>(i);
			for (int j=0; j < w/2; j++)
			{
				if(p[j] == p[w-1-j])
					s++;
			}
		}

		symH = 1.0*s/(w*h);

		// 中心对称 对称点相等的个数
		s=0;
		for (int i=0; i < h; i++)
		{
			int i2 = h-1-i;	// 计算对称的y坐标 以图像中心为对称点
			uchar* p = matImg.ptr<uchar>(i);
			uchar* q = matImg.ptr<uchar>(i2);
			for (int j=0; j < w/2; j++)
			{
				int j2 = w-1-j;	// 计算对称的x坐标 以图像中心为对称点
				if(p[j] == q[j2])
					s++;
			}
		}

		symC = 1.0*s/(w*h);
	}

	// 判断与X标记模板的符合程度 要求是正方形
	float checkTemplateX(cv::Mat & srcGray)
	{
		int w=srcGray.cols;
		int h=srcGray.rows;
		int s=0;

		for (int i=0; i < h; i++)
		{
			uchar* p = srcGray.ptr<uchar>(i);
			for (int j=0; j < w; j++)
			{
				if(p[j] > 127)
				{
					if((i < j && i+j < w) || (i > j && i+j > w)) // 上三角 下三角
						s++;
				}
				else
				{
					if((i < j && i+j > w) || (i > j && i+j < w)) // 右三角 左三角
						s++;
				}
			}
		}

		return 1.0*s/(w*h);
	}

        float isInterX(cv::Mat srcGray, float &fSym, float &symC)
	{
		int w=srcGray.cols;

		// 检查四边中点
		uchar* pTop = srcGray.ptr<uchar>(0);
		uchar* pBottom = srcGray.ptr<uchar>(w-1);
		uchar* pMiddle = srcGray.ptr<uchar>(w/2);
		if(!(pTop[w/2] > 127 && pBottom[w/2] > 127 && pMiddle[0] < 127 && pTop[w-1] < 127))
			return 0;

		// 检查与模板的匹配度
		float fs = checkTemplateX(srcGray);

		// 检查上下和左右对称性
		float symV, symH;
		MyMatBin mmg(srcGray);
		mmg.isSymmetry(symV, symH, symC);

		fSym = min(symV, symH);// 取水平对称和竖直对称的较小值

		return fs;
	}

	// 找X形标记 模板匹配 粗找 下一步将矩形聚类合并MyRectI::mergeRect(vRect, vMerge);
	void findXMarker(vector<cv::Rect> &vRect)
	{
		for (int k=5; k < 15; k++)	// 半径
		{
			for (int i=k; i < matImg.rows-k; i++)
			{
				uchar* p = matImg.ptr<uchar>(i);
				for (int j=k; j < matImg.cols-k; j++)
				{
					float fsym=0, symC=0;
					float ftmp = isInterX(matImg(cvRect(j-k, i-k, k*2, k*2)), fsym, symC);
					if(p[j] < 127 && ftmp > 0.78 && fsym > 0.36 && symC > 0.36)
					{
						vRect.push_back(cvRect(j-k, i-k, k*2, k*2));
					}
				}
			}
		}
	}

    // 去黑边 只求上下边界
    // 直线从边界开始移动计算，大于 nBlack 个黑点则未边界
    static void getWhiteBoarderTD(cv::Mat matbin, int &y1, int &y2)
    {
        int nBlack = 3;
        y1 = 0;
        y2 = matbin.rows-1;

        Mat mr(matbin.rows, 1, CV_32S, Scalar(0));
        reduce(matbin, mr, 1, CV_REDUCE_SUM, CV_32S);
        // cout << "列向量" << mr << endl;

        // 上侧边界
        int *p = mr.ptr<int>(0);
        for(int j=0; j < mr.rows; j++)
        {
            if(p[j] >= nBlack*8)
            {
                y1 = j;
                break;
            }
        }

        // 下侧边界
        for(int j=mr.rows-1; j >= 0; j--)
        {
            if(p[j] >= nBlack*8)
            {
                y2 = j;
                break;
            }
        }

        // cv::line(matbin, cvPoint(0,y1), cvPoint(matbin.cols,y1), Scalar(155,155,155), 2);
        // cv::line(matbin, cvPoint(0,y2), cvPoint(matbin.cols,y2), Scalar(155,155,155), 2);
    }

    // 直线从边界开始移动计算，大于 nBlack 个黑点则未边界
    static void getWhiteBoarder(cv::Mat matbin, int &x1, int &y1, int &x2, int &y2)
    {
        int nBlack = 3;
        x1 = 0;
        y1 = 0;
        x2 = matbin.cols-1;
        y2 = matbin.rows-1;

        Mat mc(1, matbin.cols, CV_32S, Scalar(0));
        Mat mr(matbin.rows, 1, CV_32S, Scalar(0));
        reduce(matbin, mc, 0, CV_REDUCE_SUM, CV_32S);
        reduce(matbin, mr, 1, CV_REDUCE_SUM, CV_32S);
        //cout << "行向量" << mc << endl;
        //cout << "列向量" << mr << endl;
        
        // 左侧边界
        int *q = mc.ptr<int>(0);
        for(int j=0; j < mc.cols; j++)
        {
            if(q[j] >= nBlack*255)
            {
                x1 = j;
                break;
            }
        }

        // 右侧边界
        for(int j=mc.cols-1; j >= 0; j--)
        {
            if(q[j] >= nBlack*255)
            {
                x2 = j;
                break;
            }
        }

        // 上侧边界
        int *p = mr.ptr<int>(0);
        for(int j=0; j < mr.rows; j++)
        {
            if(p[j] >= nBlack*255)
            {
                y1 = j;
                break;
            }
        }

        // 下侧边界
        for(int j=mr.rows-1; j >= 0; j--)
        {
            if(p[j] >= nBlack*255)
            {
                y2 = j;
                break;
            }
        }

        // cv::line(matbin, cvPoint(x1,0), cvPoint(x1,matbin.rows), Scalar(155,155,155));
        // cv::line(matbin, cvPoint(x2,0), cvPoint(x2,matbin.rows), Scalar(155,155,155));
        // cv::line(matbin, cvPoint(0,y1), cvPoint(matbin.cols,y1), Scalar(155,155,155));
        // cv::line(matbin, cvPoint(0,y2), cvPoint(matbin.cols,y2), Scalar(155,155,155));
    }

private:		// _??????????		???????public????

	bool _checkHalfSum(int j, int i, int r)
	{
		// ?????????????
		Mat h0(matImg, Rect(j-r,i-r,r,r+r+1));
		Mat h1(matImg, Rect(j+1,i-r,r,r+r+1));
		Scalar sc0 = cv::sum(h0);
		Scalar sc1 = cv::sum(h1);
		double bl0 = sc0.val[0]/sc1.val[0];
		//outDouble3(sc0.val[0], sc1.val[0], bl0);
		if(bl0 < 0.9 || bl0 > 1.1)
                        return false;

		// ??????2??????
		Mat h2(matImg, Rect(j-r,i-r,r+r+1,r));
		Mat h3(matImg, Rect(j-r,i+1,r+r+1,r));
		Scalar sc2 = cv::sum(h2);
		Scalar sc3 = cv::sum(h3);
		double bl2 = sc2.val[0]/sc3.val[0];
		//outDouble3(sc2.val[0], sc3.val[0], bl2);
		if(bl2 < 0.9 || bl2 > 1.1)
                        return false;

		// ???б?????????1

		// ???б?????????2

                return true;
	}

	// ?????????????????
	// ?·??????????????0????, ??????(2*r+1)*(2*r+1)??Χ???????
	//		??????????????????????,??????λ?ò?????
	//		???????????
	void _findPeaks(Mat &imgBin, int r, vector<MyPointI> &vmp)
	{
		int w = imgBin.cols;
		int h = imgBin.rows;

		for (int i=r; i < h-r; i++)
		{
			uchar* p = imgBin.ptr<uchar>(i);
			for (int j=r; j < w-r; j++)
			{
				int px=0, py=0;
				if(p[j] > 0 /*&& p[j] < 200*/)	// 200 ???????????????
				{
					int mx = _findMax(imgBin, j, i, r, px, py);	// ????????????
					outInt3(px, py, mx);
					if(mx > 3)	// ??? ??????????
					{
						MyPointI mpi(px, py);
						vmp.push_back(mpi);
					}
				}
			}
		}
	}

	uchar _findMax(Mat imgBin, int ox, int oy, int r, int &px, int &py)
	{
		int newx, newy, mx=0;
		int sw = 2*r+1;

		// ??????? ?λ?÷???
		if(!(0 <= ox-r && ox-r+sw < imgBin.cols-1 &&
			0 <= oy-r && oy-r+sw < imgBin.rows-1))
		{
			px = ox;
			py = oy;
			return 0;
		}

		// ?????? ??ox,oy?????r???????????
		for (int i=oy-r; i < oy+r+1; i++)
		{
			uchar* p = imgBin.ptr<uchar>(i);
			for (int j=ox-r; j < ox+r+1; j++)
			{
				if(p[j] > mx)
				{
					mx = p[j];
					newx = j;
					newy = i;
				}
				p[j] = 0;	// ???????0
			}
		}

		// ??????????, ????λ??
		if(mx > 0)
		{
			px = newx;
			py = newy;
		}

		return mx;
	}

	// ?????????????????? ??????
	void _findLocalPeak(Mat imgBin, int ox, int oy, int r, int &px, int &py)	// iteration
	{
		int newX, newY;

		uchar mx = _findMax(imgBin, ox, oy, r, newX, newY);
		if(newX==ox && newY==oy)
		{
			px = newX;
			py = newY;
			return;
		}

		_findLocalPeak(imgBin, newX, newY, r, px, py);
	}
};
