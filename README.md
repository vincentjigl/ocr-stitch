# stitching-Android-OpenCV

This project is created using OpenCV-3.4.16 with NDK-17, since Stitcher functions were defined as a part of OpenCV in this version.

All Panoramas will be saved in /storage/emulated/0/Pictures

Download Panorama Stitching-v1.0.apk to run in your Android phone.

## Settings:
	Android.mk assign opencv root path
		OPENCVROOT:= C:\env\opencv\opencv-4.5.3-android-sdk\OpenCV-android-sdk
	Application.mk
		APP_ABI := arm64-v8a #armeabi-v7a #
		APP_STL := c++_static
	Setting.gradle app/build.gradle  import opencv sdk
		implementation project(':opencvsdk453')
		rootProject.name = "stitching"
		include ':opencvsdk453'
	NDK setting in app/build.gradle
		commandLine "C:\\env\\android-ndk-r17c\\ndk-build.cmd",
	Android SDK+NDK setting in local.properties
		sdk.dir=C\:\\env\\Sdk
		ndk.dir=C:\\env\\android-ndk-r17c

## input:

![](images/3x4_Grid.jpg)

## output:

![](images/openCV_1614151340710.png)

## Usage:

Click on "Capture".
Move the camera in such a way that some part of previous capture will be visible.
Click "Save" the Panorama.
