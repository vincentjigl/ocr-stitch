static int g_spatialRad, g_colorRad , g_maxPryLevel; //空间，颜色，金字塔层数

class MyImgSegment
{
public:
	MyImgSegment(void){}
	~MyImgSegment(void){}

	///////////////////// 图像分割之区域生长法 Start //////////////////////////////
	// 二值化图中的一个点根据源灰度图颜色进行扩展。用于边缘生长扩展, 输入源灰度图和二值化图
	static cv::Mat binImgGrowFast(const cv::Mat &gray, cv::Mat matbin, const cv::Point2i seed, int throld)
	{
		if((seed.x < 0) || (seed.y < 0))
			return matbin;
		matbin.at<uchar>(seed.y, seed.x) = 255;

		//gray value of seed
		int seed_gray = gray.at<uchar>(seed.y, seed.x);
		//grow direction sequenc
		int grow_direction[8][2] = {{-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}};
		//seeds collection
		std::vector<cv::Point2i> seeds;
		seeds.push_back(seed);

		//start growing
		int k=0, maxExtNum=33;
		while(!seeds.empty()){
			//get a seed
			cv::Point2i current_seed = seeds.back();
			seeds.pop_back();

			for(int i = 0; i < 8; ++i){
				cv::Point2i neighbor_seed(current_seed.x + grow_direction[i][0], current_seed.y + grow_direction[i][1]);
				//check wether in image
				if(neighbor_seed.x < 0 || neighbor_seed.y < 0 || neighbor_seed.x > (gray.cols-1) || (neighbor_seed.y > gray.rows -1))
					continue;
				int value = gray.at<uchar>(neighbor_seed.y, neighbor_seed.x);
				if((matbin.at<uchar>(neighbor_seed.y, neighbor_seed.x) == 0) && (abs(value - seed_gray) <= throld))
				{
					matbin.at<uchar>(neighbor_seed.y, neighbor_seed.x) = 255;
					if(k++ > maxExtNum) return matbin;
					seeds.push_back(neighbor_seed);
				}
			}
		}

		return matbin;
	}

	// 二值化图边缘生长扩展, 输入源灰度图和二值化图, 输出扩展后的matext
	static void matbinExtend(const cv::Mat matgray, cv::Mat matbin, cv::Mat &matext, int th=20)
	{
		int grow_direction[8][2] = {{-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}};

		matbin.copyTo(matext);
		for(int i=0; i < matbin.rows; i++)
		{
			uchar *p = matbin.ptr<uchar>(i);
			for (int j = 0; j < matbin.cols; j++)
			{
				if (p[j] > 0)
				{
					for (int k = 0; k < 8; k++)
					{
						cv::Point2i neighbor(j+grow_direction[k][0], i+grow_direction[k][1]);
						if (neighbor.x < 0 || neighbor.y < 0 || neighbor.x >= matbin.cols || neighbor.y >= matbin.rows)
							continue;
						if (matbin.at<uchar>(neighbor) == 0)
						{
							matext.at<uchar>(i,j) = 255;
							binImgGrowFast(matgray, matext, cvPoint(j,i), th);
							break;
						}
					}
				}
			}
		}
	}

	// 4.1图像分割之区域生长法
	// https://blog.csdn.net/webzhuce/article/details/81412508
	// 基本思想是从一组生长点开始（生长点可以是单个像素(本方法)，也可以是某个小区域），将与该生长点性质相似的相邻像素或者区域与生长点合并，
	// 形成新的生长点，重复此过程直到不能生长为止。生长点和相似区域的相似性判断依据可以是灰度值、纹理、颜色等图像信息。
	// cv::Point2i seed(120, 120);
	static cv::Mat regionGrowFast(const cv::Mat &src, const cv::Point2i seed, int throld)
	{
	    //convert src to gray for getting gray value of every pixel
	    cv::Mat gray;
	    cv::cvtColor(src,gray, cv::COLOR_RGB2GRAY);
	    imshow("src_gray", gray);

	    // set every pixel to black
	    cv::Mat result = cv::Mat::zeros(src.size(), CV_8UC1);
	    if((seed.x < 0) || (seed.y < 0))
	        return result;
	    result.at<uchar>(seed.y, seed.x) = 255;

	    //gray value of seed
	    int seed_gray = gray.at<uchar>(seed.y, seed.x);
	    //grow direction sequenc
	    int grow_direction[8][2] = {{-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}};
	    //seeds collection
	    std::vector<cv::Point2i> seeds;
	    seeds.push_back(seed);

	    //start growing
	    while(! seeds.empty()){
	        //get a seed
	        cv::Point2i current_seed = seeds.back();
	        seeds.pop_back();

	        for(int i = 0; i < 8; ++i){
	            cv::Point2i neighbor_seed(current_seed.x + grow_direction[i][0], current_seed.y + grow_direction[i][1]);
	            //check wether in image
	            if(neighbor_seed.x < 0 || neighbor_seed.y < 0 || neighbor_seed.x > (gray.cols-1) || (neighbor_seed.y > gray.rows -1))
	                continue;
	            int value = gray.at<uchar>(neighbor_seed.y, neighbor_seed.x);
	            if((result.at<uchar>(neighbor_seed.y, neighbor_seed.x) == 0) && (abs(value - seed_gray) <= throld))
	            {
	                result.at<uchar>(neighbor_seed.y, neighbor_seed.x) = 255;
	                seeds.push_back(neighbor_seed);
	            }
	        }
	    }

	    cv::circle(src, seed, 8, Scalar(255, 0, 255), -1);
		imshow("matsrc", src);
		imshow("matres", result);
	    return result;
	}
	///////////////////// 图像分割之区域生长法 End //////////////////////////////

///////////////////// opencv 分水岭分割 Demo Start //////////////////////////////
	//生成随机颜色函数
	static Vec3b RandomColor(int value)
	{
	    value=value%255;  //生成0~255的随机数
	    RNG rng;
	    int aa=rng.uniform(0,value);
	    int bb=rng.uniform(0,value);
	    int cc=rng.uniform(0,value);
	    return Vec3b(aa,bb,cc);
	}

	// Opencv分水岭算法——watershed自动图像分割用法
	// https://blog.csdn.net/dcrmg/article/details/52498440#
	static int Demo_WaterShed(cv::Mat &image)
	{
	    // Mat image=imread(argv[1]);    //载入RGB彩色图像
	    imshow("Source Image",image);

	    //灰度化，滤波，Canny边缘检测
	    Mat imageGray;
	    cvtColor(image,imageGray,CV_RGB2GRAY);//灰度转换
	    // bitwise_not(imageGray, imageGray,Mat());//取反操作

	    GaussianBlur(imageGray,imageGray,Size(5,5),2);   //高斯滤波
	    imshow("Gray Image",imageGray);
	    Canny(imageGray,imageGray,80,150);
	    imshow("Canny Image",imageGray);

	    //查找轮廓
	    vector<vector<Point>> contours;
	    vector<Vec4i> hierarchy;
	    findContours(imageGray,contours,hierarchy,RETR_TREE,CHAIN_APPROX_SIMPLE,Point());
	    Mat imageContours=Mat::zeros(image.size(),CV_8UC1);  //轮廓
	    Mat marks(image.size(),CV_32S);   //Opencv分水岭第二个矩阵参数
	    marks=Scalar::all(0);
	    int index = 0;
	    int compCount = 0;
	    for( ; index >= 0; index = hierarchy[index][0], compCount++ )
	    {
	        //对marks进行标记，对不同区域的轮廓进行编号，相当于设置注水点，有多少轮廓，就有多少注水点
	        drawContours(marks, contours, index, Scalar::all(compCount+1), 1, 8, hierarchy);
	        drawContours(imageContours,contours,index,Scalar(255),1,8,hierarchy);
	    }

	    //我们来看一下传入的矩阵marks里是什么东西
	    Mat marksShows;
	    convertScaleAbs(marks,marksShows);
	    imshow("marksShow",marksShows);
	    imshow("轮廓",imageContours);
	    watershed(image,marks);

	    //我们再来看一下分水岭算法之后的矩阵marks里是什么东西
	    Mat afterWatershed;
	    convertScaleAbs(marks,afterWatershed);
	    imshow("After Watershed",afterWatershed);

	    //对每一个区域进行颜色填充
	    Mat PerspectiveImage=Mat::zeros(image.size(),CV_8UC3);
	    for(int i=0;i<marks.rows;i++)
	    {
	        for(int j=0;j<marks.cols;j++)
	        {
	            int index=marks.at<int>(i,j);
	            if(marks.at<int>(i,j)==-1)
	            {
	                PerspectiveImage.at<Vec3b>(i,j)=Vec3b(255,255,255);
	            }
	            else
	            {
	                PerspectiveImage.at<Vec3b>(i,j) =RandomColor(index);
	            }
	        }
	    }
	    imshow("After ColorFill",PerspectiveImage);

	    //分割并填充颜色的结果跟原始图像融合
	    Mat wshed;
	    addWeighted(image,0.4,PerspectiveImage,0.6,0,wshed);
	    imshow("AddWeighted Image",wshed);
	}
///////////////////// opencv 分水岭分割 Demo End //////////////////////////////

	// OpenCV——距离变换与分水岭算法的（图像分割）
	// https://www.cnblogs.com/long5683/p/9692845.html
	static int Demo_DistanceWaterShed(cv::Mat &src)
	{
	    // cv::Mat src = imread("分水岭.jpg");
	    if (src.empty())
	    {
	        printf("Can not load Image...");
	        return -1;
	    }
	    imshow("input Image",src);

	    //白色背景变成黑色
	    for (int row=0;row<src.rows;row++)
	    {
	        for (int col = 0; col < src.cols; col++) {
	            if (src.at<Vec3b>(row, col) == Vec3b(255, 255, 255)) {
	                src.at<Vec3b>(row, col)[0] = 0;
	                src.at<Vec3b>(row, col)[1] = 0;
	                src.at<Vec3b>(row, col)[2] = 0;
	            }
	        }
	    }
	    imshow("black backgroung", src);

	    //sharpen(提高对比度)
	    Mat kernel = (Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);

	    //make it more sharp
	    Mat imgLaplance;
	    Mat sharpenImg = src;
	    //拉普拉斯算子实现边缘提取
	    filter2D(src, imgLaplance, CV_32F, kernel, Point(-1, -1), 0, BORDER_DEFAULT);//拉普拉斯有浮点数计算，位数要提高到32
	    src.convertTo(sharpenImg, CV_32F);

	    //原图减边缘（白色）实现边缘增强
	    Mat resultImg = sharpenImg - imgLaplance;

	    resultImg.convertTo(resultImg,CV_8UC3);
	    imgLaplance.convertTo(imgLaplance, CV_8UC3);
	    imshow("sharpen Image", resultImg);

	    //转换成二值图
	    Mat binary;
	    cvtColor(resultImg, resultImg, CV_BGR2GRAY);
	    threshold(resultImg, binary, 40,255,THRESH_BINARY|THRESH_OTSU);
	    imshow("binary image",binary);

	    cvtColor(src, binary, CV_BGR2GRAY);
	    imshow("src161", src);

	    //距离变换
	    Mat distImg;
	//  OpenCV中distanceTransform方法用于计算图像中每一个非零点距离离自己最近的零点的距离，distanceTransform的
	//  第二个Mat矩阵参数dst保存了每一个点与最近的零点的距离信息，图像上越亮的点，代表了离零点的距离越远。
	//  可以根据距离变换的这个性质，经过简单的运算，用于细化字符的轮廓和查找物体质心（中心）。
	    distanceTransform(binary,distImg,DIST_L1,3,5);
	    normalize(distImg, distImg, 0, 1, NORM_MINMAX);
	    imshow("dist image",distImg);

	    //二值化
	    threshold(distImg, distImg, 0.4, 1, THRESH_BINARY);
	    imshow("dist binary image", distImg);

	    //腐蚀(使得连在一起的部分分开)
	    Mat k1 = Mat::ones(3, 3, CV_8UC1);
	    erode(distImg, distImg, k1);
	    imshow("分开", distImg);

	    //标记
	    Mat dist_8u;
	    distImg.convertTo(dist_8u,CV_8U);
	    vector<vector<Point>> contours;
	    findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

	    //创建标记
	    Mat marker = Mat::zeros(src.size(),CV_32SC1);

	    //画标记
	    for (size_t i = 0; i < contours.size(); i++)
	    {
	        drawContours(marker,contours,static_cast<int>(i),Scalar(static_cast<int>(i)+1),-1);
	    }

	    circle(marker, Point(5, 5), 3, Scalar(255, 255, 255), -1);
	    // imshow("marker",marker*1000);

	    //分水岭变换
	    watershed(src,marker);//根据距离变换的标记，在原图上分离
	    Mat water = Mat::zeros(marker.size(),CV_8UC1);
	    marker.convertTo(water,CV_8UC1);
	    bitwise_not(water, water,Mat());//取反操作
	    //imshow("源 image", src);
	    imshow("watershed Image", water);

	    // generate random color
	    vector<Vec3b> colors;
	    for (size_t i = 0; i < contours.size(); i++) {
	        int r = theRNG().uniform(0, 255);
	        int g = theRNG().uniform(0, 255);
	        int b = theRNG().uniform(0, 255);
	        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	    }

	    // fill with color and display final result
	    Mat dst = Mat::zeros(marker.size(), CV_8UC3);
	    for (int row = 0; row < marker.rows; row++) {
	        for (int col = 0; col < marker.cols; col++) {
	            int index = marker.at<int>(row, col);
	            if (index > 0 && index <= static_cast<int>(contours.size())) {
	                dst.at<Vec3b>(row, col) = colors[index - 1];
	            }
	            else {
	                dst.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
	            }
	        }
	    }
	    imshow("Final Result", dst);
	//    waitKey(0);
	    return 0;
	}

///////////////////// 金字塔均值漂移分割 Demo Start //////////////////////////////
	static void meanshift_seg(cv::Mat *src)
	{
		Mat dst;
	    // 调用meanshift图像金字塔进行分割
	    pyrMeanShiftFiltering(*src, dst, g_spatialRad, g_colorRad, g_maxPryLevel);
	    RNG rng = theRNG();
	    Mat mask(dst.rows + 2, dst.cols + 2, CV_8UC1, Scalar::all(0));
	    for (int i = 0; i<dst.rows; i++)    //opencv图像等矩阵也是基于0索引
	    {
	        for (int j = 0; j<dst.cols; j++)
	        {
	            if (mask.at<uchar>(i + 1, j + 1) == 0)
	            {
	                Scalar newcolor(rng(256), rng(256), rng(256));
	                floodFill(dst, mask, Point(j, i), newcolor, 0, Scalar::all(1), Scalar::all(1));
	            }
	        }
	    }
	    outInt2(dst.rows, dst.cols);
	    if(dst.rows > 0 && dst.cols > 0)
	        imshow("结果图像", dst);
	}

	static void meanshift_seg_s(int i, void *psrc)   //控制滑动块
	{
	    g_spatialRad = i;
	    meanshift_seg((cv::Mat *)psrc);
	}

	static void meanshift_seg_c(int i, void *psrc)
	{
	    g_colorRad = i;
	    meanshift_seg((cv::Mat *)psrc);
	}

	static void meanshift_seg_m(int i, void *psrc)
	{
	    g_maxPryLevel = i;
	    meanshift_seg((cv::Mat *)psrc);
	}

	static int Demo_pyrMeanShift(cv::Mat &src)
	{
	    namedWindow("载入原图像", WINDOW_AUTOSIZE);
	    namedWindow("结果图像", WINDOW_AUTOSIZE);

	    // src = imread("../pict1.jpg");
	    CV_Assert(!src.empty());
	    resize(src, src, cvSize(512,256));   // 最近邻缩放 保真
	//    outInt2(src.rows, src.cols);

	    g_spatialRad = 10;
		g_colorRad = 10;
		g_maxPryLevel = 1;
		createTrackbar("空间度", "结果图像", &g_spatialRad, 80, meanshift_seg_s, &src);
		createTrackbar("色度", "结果图像", &g_colorRad, 255, meanshift_seg_c, &src);
		createTrackbar("金字塔层级", "结果图像", &g_maxPryLevel, 5, meanshift_seg_m, &src);

		imshow("载入原图像", src);
		// imshow("结果图像", src);
		// waitKey();
		return 0;
	}
///////////////////// 金字塔均值漂移分割 Demo End //////////////////////////////

    // 对二值化图， 膨胀腐蚀+找包围轮廓，返回外接矩形
    static void binBlockBox(cv::Mat& matbin, vector<Rect> &boundRect, bool bDebug=0)
    {
        // (2) 1 消除黑色孤立点 2 消除黑柱的白色小洞    // 先消除黑色小物体，再消除白色小物体
        morphologyEx(matbin, matbin, MORPH_CLOSE,Mat(9,9,CV_8U),Point(-1,-1),1); // 闭运算：先膨胀再腐蚀，用于排除小型黑洞
        morphologyEx(matbin, matbin, MORPH_OPEN,Mat(5,5,CV_8U),Point(-1,-1),1);  // 开运算：先腐蚀再膨胀，用来消除小物体(白色的小物体)
        //erode(mmb.matImg,mmb.matImg,Mat(2,2,CV_8U),Point(-1,-1)); // 白色缩小，黑色扩大
        //dilate(mmb.matImg,mmb.matImg,Mat(5,5,CV_8U),Point(-1,-1),2);// 黑色缩小，白色扩大
        if(bDebug)
            imshow("erode_dilate", matbin);

         // (3) 找白色区域的包围轮廓
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        // CV_RETR_EXTERNAL 只检测出最外轮廓即c0。图2中第一个轮廓指向最外的序列，除此之外没有别的连接。
        // CV_CHAIN_APPROX_SIMPLE压缩水平，垂直或斜的部分，只保存最后一个点。
        findContours(matbin, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);// CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE
//        Mat resultImage = Mat ::zeros(matbin.size(),CV_8U);
//        drawContours(resultImage, contours, -1, Scalar(255, 0, 255));
//        if(bDebug)
//            imshow("resultImage", resultImage);

        // (4) 对包围轮廓找外接矩形
//        Mat drawing = matgray;
        for (int i = 0; i < contours.size(); i++)
        {
            cv::Rect bRect = boundingRect(Mat(contours[i]));
            if(bRect.width < 0.8*matbin.cols && bRect.height < 0.8*matbin.rows)
                boundRect.push_back(bRect);
            if(bDebug)
                rectangle(matbin, bRect, CV_RGB(188,8,188), 1, 8, 0);
        }
//        if(bDebug)
//            imshow("drawing", drawing);
    }

    // 二值化_图像分割_矩形包围框
    // 二值化, 形态学运算, 找包围轮廓, 最后找外接矩形
    static int binSegment(cv::Mat& matgray, vector<Rect> &boundRect, int binTh=18, int extendTh=12, bool bDebug=0)
    {
        // (1) 二值化
    	static double bth=binTh;
        cv::Mat matbin, matext;

        double otsuth = cv::threshold(matgray, matbin, 0, 255, CV_THRESH_OTSU);
        //outDouble2(bth, otsuth);
		bth += bth < binTh ? 1 : (bth == otsuth ? 0 : (bth < otsuth ? 1 : -1));
        threshold(matgray, matbin, bth, 255, CV_THRESH_BINARY);
       if(bDebug)
       	    imshow("matbin", matbin);

        // 二值化图像边缘生长扩展(需要灰度源图)
        matbinExtend(matgray, matbin, matext, extendTh);
        
        binBlockBox(matext, boundRect, bDebug);

        // if(bDebug)
        //     imshow("matbin_extend", matext);

        return 0;
    }
};
