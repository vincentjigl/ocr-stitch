#include "com_zyb_stitching_NativePanorama.h"
#include "opencv2/opencv.hpp"
#include "opencv2/stitching.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "lib/VideoStitch.h"

#include <android/log.h>
#define TAG "jgl_stitch"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

using namespace std;
using namespace cv;

JNIEXPORT jint JNICALL Java_com_zyb_stitching_NativePanorama_processPanorama
  (JNIEnv *env, jclass clazz, jlongArray imageAddressArray, jlong outputAddress){
  jint ret=1;
  // Get the length of the long array
  jsize a_len = env->GetArrayLength(imageAddressArray);
  // Convert the jlongArray to an array of jlong
  jlong *imgAddressArr = env->GetLongArrayElements(imageAddressArray, 0);
  // Create a vector to store all the image
  //vector<Mat> imgVec;

    int  HAND_MODE = 0;			// mode=0,左手模式 mode=1,右手模式
    bool bSaveImages = false;	// 保存原始图片
    //Mat matdst;
    Mat &matdst  = *(Mat*) outputAddress;
    vector<Mat> matSrc;

    IVideoStitch *ivs = new VideoStitch();
    LOGE("src image numbers %d \n", a_len);

    for(int k=0;k<a_len;k++){
        // Get the image
        Mat & curimage=*(Mat*)imgAddressArr[k];
        LOGE("image width %d, height %d, curimage %lld\n", curimage.rows, curimage.cols, imgAddressArr[k]);
        Mat newimage;

        // Convert to a 3 channel Mat to use with Stitcher module
        cvtColor(curimage, newimage, CV_BGR2YUV_YV12);
        cv::Mat tmp(newimage.rows, newimage.cols, CV_8UC1, (char*)newimage.data);

        // Reduce the resolution for fast computation
        //float scale = 1000.0f / curimage.rows;
        //resize(newimage, newimage, Size(scale * curimage.rows, scale * curimage.cols));
        //resize(newimage, newimage, Size(350,240));

        matSrc.push_back(tmp.clone());
      }

    // 2. 拼接
    int nFrame = matSrc.size();
    cv::Rect roi(200,60,240,350);
    //cv::Rect roi(0,0,imgVec[0].cols,imgVec[0].rows);
    for(int i=0; i < nFrame; i++){
        cv::Mat tmp = matSrc[i](roi);
        LOGE("image width %d, height %d \n", tmp.rows, tmp.cols);
        int mVal = ivs->stitch(tmp, HAND_MODE);
        // Sleep(6);
    }

    // 3. 获取拼接图片
    ivs->getStitchedImage2(matdst, 2);
    if(matdst.empty())
        LOGE("[mbh]Stitched image is Empty !\n");
    else{
        LOGE("[mbh]Stitched image: w h = %d %d\n", matdst.cols, matdst.rows);
        /*
        imshow("x86_mat", matdst);

        char filename[64];
        time_t t = time(NULL); //获取目前秒时间
        tm* local = localtime(&t); //转为本地时间
        strftime(filename, 64, "/tmp/saved_imgs/st%m%d%H%M%S.jpg", local);
        imwrite(filename, matdst);
        */
    }


    ivs->clear();
    delete ivs;

//    Ptr<Stitcher> stitcher = Stitcher::create((Stitcher::Mode)0);
//  Stitcher::Status status = stitcher->stitch(imgVec, result);
//  stitcher->setRegistrationResol(-1); /// 0.6
//  stitcher->setSeamEstimationResol(-1);   /// 0.1
//  stitcher->setCompositingResol(-1);   //1
//  stitcher->setPanoConfidenceThresh(-1);   //1
//  stitcher->setWaveCorrection(true);
//  stitcher->setWaveCorrectKind(detail::WAVE_CORRECT_HORIZ);
//
//   if (status != Stitcher::OK){
//                  ret=0;
//   }else{
//         cv::cvtColor(result, result, COLOR_RGBA2RGB, 4);
//         }

  // Release the jlong array
  env->ReleaseLongArrayElements(imageAddressArray, imgAddressArr ,0);
  return ret;

}


JNIEXPORT jstring JNICALL Java_com_zyb_stitching_NativePanorama_getMessageFromJni
  (JNIEnv *env, jclass obj){
  return env->NewStringUTF("This is a message from JNI");
  }