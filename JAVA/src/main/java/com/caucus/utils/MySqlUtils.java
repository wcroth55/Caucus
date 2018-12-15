package com.caucus.utils;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;

public class MySqlUtils {

	public static Connection makeMySqlConnectionFromJdbcUrl(String url, String userName, String password) {
		Connection connection = null;
		try {
			Class.forName("com.mysql.jdbc.Driver");
			connection = DriverManager.getConnection(url, userName, password);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return connection;
	}
	
	public static void executeUpdate (String updateQuery, Connection connection) {
		try {
			Statement statement = connection.createStatement();
			
			statement.executeUpdate(updateQuery);
			statement.close();
			//System.out.println("newKey=" + newKey + ", query=" + query);
		}
		catch (Exception e) {
			//System.out.println("ERROR: " + query);
		}
	}
	
	public static String executeSimpleQueryWithOneResult (String query, Connection connection) {
		String answer = "";
		try {
			Statement statement = connection.createStatement();
			ResultSet results = statement.executeQuery(query);
			if (results != null  &&   results.next())  {
				answer = results.getString(1);
				results.close();
			}
			statement.close();
			System.out.println("answer=" + answer + ", query=" + query);
		}
		catch (Exception e) {
			System.out.println("ERROR: " + query);
		}
		
		return answer;
	}
}
