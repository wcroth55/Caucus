package com.caucus.utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;

public class IOUtils {
	
	public static Map<String, String> loadConfigurationParametersFromFile (String fileName) {
		Map<String,String> parameters = new HashMap<String,String>();
		String line;
		BufferedReader configuration = IOUtils.makeReaderFromFileName(fileName);
		if (configuration != null) {
			try {
				while ((line = configuration.readLine()) != null) {
					line = line.trim();
					if (line.indexOf("#") == 0 || line.length() == 0) continue;
					String[] fields = line.split(" ", 2);
					if (fields.length != 2) continue;
					parameters.put(fields[0], fields[1]);
				}
			}
			catch (Exception e) {
			}
		}
		return parameters;
	}
	
	public static BufferedReader makeReaderFromFileName(String fileName) {
		BufferedReader in = null;
		try {
			File file = new File(fileName);
			InputStream stream = new FileInputStream(file);
			InputStreamReader reader = new InputStreamReader(stream);
			in = new BufferedReader(reader);
		}
		catch (Exception e) {
		}
		return in;
	}
}
