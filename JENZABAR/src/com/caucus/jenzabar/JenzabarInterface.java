package com.caucus.jenzabar;

import java.io.*;
import java.sql.*;
import java.util.*;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.input.SAXBuilder;

/**
 * Interface between Caucus and Jenzabar. Called immediately when a user tries to login to Caucus.
 * <ol>
 * <li>Verifies user's login information.
 * <li>Creates or updates user's name and email.
 * <li>Fetches list of courses user is enrolled in.
 * <li>Stores entries in Caucus' database, saying which courses
 * should be added, and which should be deleted.
 * </ol>
 * 
 * @author Charles Roth.  Copyright (C) 2008 CaucusCare.com.  All rights reserved.
 * This program is published under the terms of the GPL; see
 * the file license.txt in the same directory.
 * <p/>
 * 01/28/2008 First version.<br>
 * 07/18/2009 Added user name and email update/create.
 */
public class JenzabarInterface {
	private static final int SUCCESS = 0;
	private static final int BAD_NOARG = 1;
	private static final int BAD_NOREAD = 9;  // Can't connect to DB
	private static final int BAD_OLDPW = 4;   // Bad userid/password combo
	private static final int BAD_PROG = 12;
	
	
	private static final boolean DEBUG = false;

	/**
	 * Main program.  Run from Caucus during a user's attempted login, 
	 * through the "PW_Prog" interface (see Caucus configuration file swebd.conf).
	 * 
	 * JenzabarInterface does three things: <ol>
	 * <li>Verify that the userid/pw combination is correct.
	 * <li>Create or update the user's name and email address.
	 * <li>If correct, load the list of courses that this user is enrolled in.
	 * Tell Caucus (via a "user" variable) which matching "jenzabar" groups
	 * to add this user to, or remove this user from.
	 * </ol>
	 * 
	 * @param args 0=connectString 1=databaseId 2=databasePassword 3=jenzabarDomain
	 *     e.g. connectString = jdbc:mysql://hostName.com:3306/databaseName
	 * 
	 * Reads additional info from stdin, based on Caucus PW_Prog standard.
	 * (See prototype cpw2.c program for full details.)
	 * 
	 * Writes a success/error code to stdout.  (Two digits followed by newline, 00=success.)
	 */
	public static void main(String[] args) {
		if (args.length < 4)  exitWithValue(BAD_NOARG);
		
		Statement st = createStatementFromMySqlConnectionInfo(args[0], args[1], args[2]);
		if (st == null)  exitWithValue(BAD_NOREAD);
		String jenzabarDomain = args[3];
		
		//---Get input from pipe from Caucus.  Only the 'verify' function is supported.
		BufferedReader in = getReaderFromStdin();
		readLine(in);   //---Trustcode.  Ignored.
		readLine(in);   //---Password file.  Ignored.
		readLine(in);   //---Lock control.  Ignored.
		String func   = readLine(in);
		String userid = readLine(in);
		String password = readLine(in);
		if (! func.equals("verify")) exitWithValue(BAD_PROG);
		
		String responseHtml = getJenzabarResponseForUseridAndPassword (jenzabarDomain, userid, password);
		String responseText = getResponseSpan (responseHtml);
		boolean ok = isResponseValid (responseText);
		if (! ok) exitWithValue(BAD_OLDPW);
		
		CaucusPerson caucusUser = makeCaucusPersonFromJenzabarResponse(userid, responseText);
		createOrUpdateCaucusUser (st, caucusUser);
		
		String xmlText = getJenzabarXmlCourseList (jenzabarDomain, userid);
		HashSet<String> newCourses = getCoursesFromXml(xmlText);
		
		//---Get old course list from Caucus.
		String oldCoursesQuery = "SELECT DISTINCT name FROM grouprules "
			+ "WHERE userid=" + quoted(userid)
			+ "  AND owner='jenzabar' AND access=20";
		HashSet<String> oldCourses = getSetFromQuery(st, oldCoursesQuery, "name");
		
		//---Tell Caucus to delete the courses that do not appear in the new list.
		HashSet<String> deleteCourses = setSubtract (oldCourses, newCourses);
		if (DEBUG) System.err.println("oldCourses=" + oldCourses.toString());
		if (DEBUG) System.err.println("deleteCourses=" + deleteCourses.toString());
		String deleteCoursesQuery = "REPLACE INTO user_data (userid, name, value) VALUES "
			+ "(" + quoted(userid) + ", 'deleteJenzabarCourses', " +  quoted(join(deleteCourses," ")) + ")";
		silentSqlUpdate (st, deleteCoursesQuery);
		
		//---Tell Caucus to add the courses that are in the new list but not the old list.
		HashSet<String> addCourses = setSubtract (newCourses, oldCourses);
		String addCoursesQuery = "REPLACE INTO user_data (userid, name, value) VALUES "
			+ "(" + quoted(userid) + ", 'addJenzabarCourses', " +  quoted(join(addCourses," ")) + ")";
		silentSqlUpdate (st, addCoursesQuery);
		
		exitWithValue(SUCCESS);
	}
	
	private static String readLine (BufferedReader in) {
		String line = null;
		try {
			line = in.readLine();
		}
		catch (Exception e) { }
		return line;
	}
	
	private static String join (HashSet<String> x, String between) {
		StringBuffer result = new StringBuffer();
		boolean first = true;
		for (Object s : x) {
			if (! first) result.append(between);
			first = false;
			result.append((String) s);
		}
		return result.toString();
	}
	
	private static HashSet<String> setSubtract (HashSet<String> a, HashSet<String> b) {
		HashSet<String> result = (HashSet<String>) a.clone();
		result.removeAll(b);
		return result;
	}
	
//	private static HashSet<String> setUnion (HashSet<String> a, HashSet<String> b) {
//		HashSet<String> result = (HashSet<String>) a.clone();
//		result.addAll(b);
//		return result;
//	}
	
	protected static Statement createStatementFromMySqlConnectionInfo(String connectStr, String dbId, String dbPassword) {
		Connection connection = getMySqlConnection (connectStr, dbId, dbPassword);
		if (connection == null)  return null;
		Statement st = createStatement(connection);
		return st;
	}
	
	private static Connection getMySqlConnection(String connectStr, String dbId, String dbPw) {
		Connection connection = null;
		try {
			Class.forName("com.mysql.jdbc.Driver");
			connection = DriverManager.getConnection(connectStr, dbId, dbPw);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return connection;
	}
	
	private static Statement createStatement(Connection connection) {
		Statement st = null;
		try {
			st = connection.createStatement();
		}
		catch (Exception e) { }
		return st;
	}
	
	private static HashSet<String> getSetFromQuery (Statement st, String query, String columnName) {
		HashSet<String> set = new HashSet<String>();
		try {
			ResultSet r = st.executeQuery(query);
			while (r.next())  set.add(r.getString(columnName));
		}
		catch (Exception e) { }
		
		if (DEBUG) System.err.println ("getSetFromQuery: " + query + "\n  " + set.toString());
		return set;
	}
		
	private static HashSet<String> getCoursesFromXml (String xmlText) {
		HashSet<String> courses = new HashSet<String>();
		Element root = loadXMLFrom(xmlText);
		List children = root.getChildren();
		for (Iterator i=children.iterator();   i.hasNext();  ) {
			Element child = (Element) i.next();
			String courseName = child.getAttributeValue("yr_cde") + "-"
				+ child.getAttributeValue("trm_cde") + "-"
				+ child.getAttributeValue("crs_cde").replaceAll("  *", "-");
			courses.add(courseName);
		}
		
		return courses;
	}
	
	protected static String getJenzabarResponseForUseridAndPassword (String domain, String userid, String password) {
		HttpRequest r = new HttpRequest(domain + "/ics/JICSAuthCheck/JICSAuthCheck.aspx");
		r.addParameter("Login", userid);
		r.addParameter("Password", password);
		//String result = r.get();
		String result = r.post();
		return result;
	}
	
	protected static boolean isResponseValid (String response) {
		return (response.charAt(0) == '1');
	}
	
	protected static String getResponseSpan (String htmlText) {
		int spanOpenStart = htmlText.indexOf("<span");
		if (spanOpenStart < 0)  return "";
		int spanOpenEnd   = htmlText.indexOf(">", spanOpenStart);
		if (spanOpenEnd   < 0)  return "";
		int spanCloseStart = htmlText.indexOf("<", spanOpenEnd);
		if (spanCloseStart < 0)  return "";
		
		return htmlText.substring(spanOpenEnd+1, spanCloseStart);
	}
	
	private static String getJenzabarXmlCourseList(String domain, String userid) {
		HttpRequest r = new HttpRequest(domain + "/ics/StaticPages/Caucus/CaucusInterface.aspx");
		r.addParameter("id_num", userid);
		String xml = r.get();
		return xml;
	}
	
	private static void exitWithValue(int code) {
		String output = "" + code;
		if (output.length() < 2)  output = "0" + output;
		System.out.println(output);
		System.exit(0);
	}
	
	private static BufferedReader getReaderFromStdin() {
		InputStreamReader cvt = new InputStreamReader (System.in);
	   BufferedReader    in  = new BufferedReader (cvt);
	   return in;
	}
	
   public static Element loadXMLFrom(String text) {
   	byte[] bytes = text.getBytes();
      ByteArrayInputStream b = new ByteArrayInputStream(bytes);

      Element root = null;
      try {
         SAXBuilder builder = new SAXBuilder();
         Document doc = builder.build(b);
         root = doc.getRootElement();
      }
      catch (Exception e) {
      }
      return root;
   }
   
   private static String quoted(String text ) {
      if(text == null) text = "";
      text = text.replaceAll("\\\\", "\\\\\\\\");
      text = text.replaceAll("'", "\\\\'");
      return "'" + text + "'";
   }
   
   protected static CaucusPerson makeCaucusPersonFromJenzabarResponse (String userid, String response) {
	   String [] jenzabarParts = response.split("\\|");
	   if (jenzabarParts.length < 4)  return null;
	   return new CaucusPerson (userid, jenzabarParts[1], jenzabarParts[2], jenzabarParts[3]);
   }
   
   protected static boolean createOrUpdateCaucusUser (Statement st, CaucusPerson caucusUser) {
	   String insertUserQuery
	   		= "INSERT INTO user_info (userid, active, fname, lname, email) VALUES (" 
	   		+ quoted(caucusUser.getUserid()) + ", "
	   		+ " 10, "
   			+ quoted(caucusUser.getFirstName()) + ", "
   			+ quoted(caucusUser.getLastName()) + ","
			+ quoted(caucusUser.getEmailAddress()) + ")";
	   if (silentSqlUpdate(st, insertUserQuery)) return true;
	   
	   String updateUserQuery
	   		= "UPDATE user_info SET "
  			+ " fname = " + quoted(caucusUser.getFirstName()) + ","
			+ " lname = " + quoted(caucusUser.getLastName()) + ","
			+ " email = " + quoted(caucusUser.getEmailAddress())
			+ " WHERE userid = " + quoted(caucusUser.getUserid());
	   return silentSqlUpdate(st, updateUserQuery);
   }
   
	protected static boolean silentSqlUpdate(Statement st, String query) {
		boolean success = true;
		try {
			st.executeUpdate(query);
		}
		catch (Exception e) {
			success = false;
		}
		return success;
	}
}
