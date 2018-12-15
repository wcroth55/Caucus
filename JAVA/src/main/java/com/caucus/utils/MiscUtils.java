package com.caucus.utils;

public class MiscUtils {
	
	public static String null2Zero (String x) {
		return (x == null ? "0" : x);
	}
	
	public static String null2Empty (String x) {
		return (x == null ? "" : x);
	}

}
