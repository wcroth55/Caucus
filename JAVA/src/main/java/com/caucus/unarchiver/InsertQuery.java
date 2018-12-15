package com.caucus.unarchiver;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;

// Copyright (C) 2007 CaucusCare.com.  All rights reserved.
// This program is published under the terms of the GPL; see
// the file license.txt in the same directory.

public class InsertQuery {
	String action;   // e.g. "INSERT INTO tablename" or "REPLACE INTO tablename"
	StringBuffer names;
	StringBuffer values;
	String tableName;
	boolean empty;
	Connection connection;
	
	public InsertQuery (String action, Connection conn) {
		this.names = new StringBuffer();
		this.values = new StringBuffer();
		this.empty = true;
		this.action = action;
		this.connection = conn;
	}
	
	public void addStr (String fieldName, String fieldValue) {
		this.addPrep(fieldName);
		if (fieldValue == null)  this.values.append ("NULL");
		else this.values.append("'" + escapeSingleQuoteAndBackslash(fieldValue) + "'");
	}
	
	public void addStrIfNotNull (String fieldName, String fieldValue) {
		if (fieldValue != null)  addStr (fieldName, fieldValue);
	}
	
	public void addNum (String fieldName, int fieldValue) {
		this.addPrep(fieldName);
		this.values.append(String.valueOf(fieldValue));
	}
	
	public void addNumIfNotNull (String fieldName, String fieldValue) {
		if (fieldValue != null)  addNum (fieldName, fieldValue);
	}
	public void addNum (String fieldName, String fieldValue) {
		this.addPrep(fieldName);
		if (fieldValue == null)  this.values.append ("NULL");
		else this.values.append(fieldValue);
	}
	
	protected void addPrep (String fieldName) {
		if (! this.empty) {
			this.names.append  (", ");
			this.values.append (", ");
		}
		this.empty = false;
		this.names.append (fieldName);
	}
	
	protected String escapeSingleQuoteAndBackslash (String str) {
      if(str == null) return null;
      str = str.replaceAll("\\\\", "\\\\\\\\");
      str = str.replaceAll("'", "\\\\'");
      return str;
   }
	
	public String toString() {
		return this.action + " (" + this.names + ") VALUES (" + this.values + ")";
	}
	
	public String execute () {
		String newKey = "";
		String query = this.action + " (" + this.names + ") VALUES (" + this.values + ")";
		try {
			Statement statement = connection.createStatement();
			
			statement.executeUpdate(query, Statement.RETURN_GENERATED_KEYS);
			ResultSet results = statement.getGeneratedKeys();
			if (results != null) {
				if (results.next())   newKey = String.valueOf(results.getInt(1));
				results.close();
			}
			statement.close();
			System.out.println("newKey=" + newKey + ", query=" + query);
		}
		catch (Exception e) {
			System.out.println("ERROR: " + query);
		}
		return newKey;
	}
}
