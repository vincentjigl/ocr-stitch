package com.zyb.stitching;

public class NativePanorama {
    public native static int processPanorama(long[] imageAddressArray, long outputAddress);
    public native static String getMessageFromJni();
}
