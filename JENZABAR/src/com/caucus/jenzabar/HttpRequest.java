package com.caucus.jenzabar;

import java.io.*;
import java.net.URL;
import java.net.URLConnection;

/**
 * HttpRequest:  utility class for requesting a web page via HTTP GET or POST methods.
 * <p/>
 * @author Charles Roth 11/20/2007.  Copyright (C) 2007 CaucusCare.com.  All rights reserved.
 * This program is published under the terms of the GPL; see
 * the file license.txt in the same directory.
 */
public class HttpRequest {

	private String content;
	private String url;

	/**
	 * @param url URL of page to request.  Querystring parameters are usually
	 * added by addParameter().
	 */
	public HttpRequest(String url) {
		this.url = url;
		this.content = "";
	}
	
	/**
	 * @return useful String form of HttpRequest, as base URL and querystring.
	 */
	public String toString() {
		return url + " " + content;
	}

	/**
	 * Add a name/value parameter to the request.  Does any necessary encoding
	 * for you.  Call once for each parameter, as many times as desired.
	 * @param name Parameter name.
	 * @param value Parameter value.
	 */
	public void addParameter(String name, String value) {
		if (!content.equals("")) content += "&";
		content = content + name + "=" + UrlEncoder(value);
	}
	
	private static final char [] encodeChars = {'?', '&', '=', ' ' };

	/**
	 * Encode ("%hex-code" escape) characters that will be used in the query
	 * string of a URL.
	 *
	 * @param url the query string text.
	 * @return <code>String</code> appropriately escaped ("safe for URLs") text.
	 */
	public String UrlEncoder (String url) {
		boolean [] encodeThisChar = new boolean[256];
		char c;

		for (int i=0;   i<encodeChars.length;   ++i)
			encodeThisChar[encodeChars[i]] = true;

		StringBuffer buf = new StringBuffer();
		for (int i=0;   i<url.length();  ++i) {
			c = url.charAt(i);
			if (c < 256  &&  encodeThisChar[c]) {
				buf.append ("%");
				String hex = Integer.toHexString((int) c);
				if (hex.length() == 1)  buf.append("0");
				buf.append (hex);
			}
			else buf.append (c);
		}
		return buf.toString();
	}

	/**
	 * Request the page via the GET method.
	 * @return The entire HTML text of the page.
	 */
	public String get() {
		StringBuffer text = new StringBuffer("");
		try {
			URL target = new URL(content.equals("") ? url : url + "?" + content);
			URLConnection conn = target.openConnection();
			DataInputStream in = new DataInputStream(conn.getInputStream());
			BufferedReader d = new BufferedReader(new InputStreamReader(in));

			String line;
			while ((line = d.readLine()) != null) text.append(line + "\n");
			d.close();
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		return text.toString();
	}

	/**
	 * Request the page via the POST method.
	 * @return The entire HTML text of the page.
	 */
	public String post() {
		StringBuffer text = new StringBuffer();
		try {
			URL target = new URL(url);
			URLConnection urlConn = target.openConnection();
			urlConn.setDoInput(true);
			urlConn.setDoOutput(true);
			urlConn.setUseCaches(false);
			urlConn.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");

			DataOutputStream printout = new DataOutputStream(urlConn.getOutputStream());
			printout.writeBytes(content);
			printout.flush();
			printout.close();

			BufferedReader reader = new BufferedReader(new InputStreamReader(urlConn.getInputStream()));
			String line;
			while ((line = reader.readLine()) != null)
				text.append(line + "\n");
			reader.close();
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		return text.toString();
	}
}

