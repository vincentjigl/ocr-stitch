package com.zyb.stitching;

public class NativeStitching {
    public native static int processStitching(long[] imageAddressArray, long outputAddress);
    public native static String getMessageFromJni();
}
