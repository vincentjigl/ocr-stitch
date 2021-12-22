LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#opencv
#OPENCVROOT:= D:\AndroidStudioProjects\opencv-2.4.13.2-android-sdk\OpenCV-android-sdk
OPENCVROOT:= C:\env\opencv\opencv-3.4.16-android-sdk\OpenCV-android-sdk
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES := com_prasoon_panoramastitching_NativePanorama.cpp lib/v4l2cap_img.cpp

LOCAL_LDLIBS += -llog -lstdc++
LOCAL_MODULE := MyLibs

LOCAL_C_INCLUDES += .

#LOCAL_STATIC_LIBRARIES += libopencv_stitching.a

#LOCAL_LDLIBS += -L$(OPENCVROOT)/sdk/native/staticlibs/armeabi-v7a
#LOCAL_LDLIBS += -lopencv_highgui -lopencv_imgcodecs #-lstdc++ -lopencv_stitching

include $(BUILD_SHARED_LIBRARY)