#define NO_FEATURES 99999

class YImageStitch
{
public:
    YImageStitch(void){};
    ~YImageStitch(void){};

    int imageStitch(cv::Mat matbin, cv::Mat matpre, int &dx, int &dy, int rightHand)
    {
    	int w = matbin.cols;
    	int h = matbin.rows;
    	int tx;

		Point 	minLoc, maxLoc;
        cv::Mat result;
        double 	minVal = -1, maxVal = -1;

        // 左半部匹配
        if(rightHand)
        {
	        cv::Rect tc(0, 16, 32, 96);		// 在新图中靠左取图 右移 96*128
        	Scalar ss = sum(matbin(tc));

			// 全白 全黑不拼
        	double th = ss[0]/(tc.width*tc.height*255);
        	if(0.005 > th || th > 0.995)
        		return NO_FEATURES;

	        matchTemplate(matpre(cvRect(0,0,w*0.5,h)), matbin(tc), result, CV_TM_CCOEFF_NORMED);
			minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);
	        dy = maxLoc.y - tc.y;

			tx = maxLoc.x - tc.x;    		// 当前帧在上一帧中的坐标
			if(tx > -1)	dx = tx;				// 仅容许 -1 左移动
        	// MyRectI::showRect(matbin, tc, "295");
        	// waitKey(8);
		}
	
        // 右半部匹配
        else{
	        cv::Rect tc(w-32, 32, 32, 64);	// 在新图中靠右取图 左移 96*128
           	Scalar ss = sum(matbin(tc));

			// 全白 全黑不拼
        	double th = ss[0]/(tc.width*tc.height*255);
        	if(0.005 > th || th > 0.995)
        		return NO_FEATURES;

	        matchTemplate(matpre(cvRect(w,0,w,h)), matbin(tc), result, CV_TM_SQDIFF_NORMED);
	        minMaxLoc(result, &minVal, NULL, &minLoc);
	        dy = minLoc.y - tc.y;

			tx = minLoc.x + w/2 - tc.x;    		// 当前帧在上一帧中的坐标
			if(tx < 1)	dx = tx;					// 仅容许 1 右移动
        }

        // 禁止纵向大的移动 (< 8px)
   		if(abs(dy) > 8)
			dy = 0;
        // if(dy > 0) dy = 1;
        // else if(dy < 0) dy = -1;
        
        // outInt3(tx, dx, dy);
        return minVal;
    }
};