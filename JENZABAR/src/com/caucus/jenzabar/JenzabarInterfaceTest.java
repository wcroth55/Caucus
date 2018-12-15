package com.caucus.jenzabar;

import static org.junit.Assert.*;
import org.junit.Test;
import java.sql.*;


public class JenzabarInterfaceTest {
	private static final String JENZABAR_DOMAIN = "https://pathway.ciis.edu";
	private static final String CONNECT_STR = "jdbc:mysql://localhost:3306/caucus_june";
	private static final String DBID = "june";
	private static final String DBPASSWD = "lookfar";
	
	@Test
	public void testFailedLoginToJenzabar() {
		String userid = "xyz";
		String password = "123";
		String response = JenzabarInterface.getJenzabarResponseForUseridAndPassword (JENZABAR_DOMAIN, userid, password);
		//System.out.println (response);
		assertFalse (JenzabarInterface.isResponseValid(response));
	}
	
	@Test
	public void testSuccessfulLoginToJenzabar() {
		String userid = "111932";
		String password = "Ae6c4J5Qns";
		String response = JenzabarInterface.getJenzabarResponseForUseridAndPassword (JENZABAR_DOMAIN, userid, password);
		System.out.println (response);
		String span = JenzabarInterface.getResponseSpan(response);
		assertTrue (JenzabarInterface.isResponseValid(span));
	}
	
	@Test
	public void testIsResponseValid() {
		String span = "0 FAILURE JICSAuthCheck";
		assertFalse (JenzabarInterface.isResponseValid(span));
	}
	
	@Test
	public void testGetResponseSpan() {
		String span = "0 FAILURE JICSAuthCheck";
		String response = "<span id=\"lblResult\">" + span + "</span>";
		assertEquals (span, JenzabarInterface.getResponseSpan(response));
	}
	
	@Test
	public void testMakeCaucusPersonFromJenzabarResponse() {
		String response = "1 SUCCESS JICSAuthCheck|Charles|Roth|roth@thedance.net";
		CaucusPerson roth = JenzabarInterface.makeCaucusPersonFromJenzabarResponse("roth", response);
		assertNotNull(roth);
		assertEquals ("Charles", roth.getFirstName());
		assertEquals ("Roth", roth.getLastName());
		assertEquals ("roth@thedance.net", roth.getEmailAddress());
	}
	

	@Test
	public void  testCreateStatementFromMySqlConnectionInfo() {
		Statement st = JenzabarInterface.createStatementFromMySqlConnectionInfo(CONNECT_STR, DBID, DBPASSWD);
		String query = "SELECT 1 as q";
		
		String result = selectOneColumnFromQuery(st, query, "q");
		assertNotNull(result);
		assertEquals (result, "1");
	}
	
	@Test
	public void testCreateCaucusUser() {
		String firstName = "Chortles";
		CaucusPerson wrath = new CaucusPerson ("wrath", firstName, "Wrath", "wrath@thedance.net");
		Statement st = JenzabarInterface.createStatementFromMySqlConnectionInfo(CONNECT_STR, DBID, DBPASSWD);
		String query = "DELETE FROM user_info WHERE userid='wrath'";
		
		assertTrue (JenzabarInterface.silentSqlUpdate (st, query));
		assertTrue (JenzabarInterface.createOrUpdateCaucusUser (st, wrath));
		
		query = "SELECT fname FROM user_info WHERE userid='wrath'";
		String result = selectOneColumnFromQuery (st, query, "fname");
		assertNotNull (result);
		assertEquals  (firstName, result);
	}
	
	@Test
	public void testUpdate() {
		String firstName = "Chassle";
		CaucusPerson wrath = new CaucusPerson ("wrath", firstName, "Wrath", "wrath@thedance.net");
		Statement st = JenzabarInterface.createStatementFromMySqlConnectionInfo(CONNECT_STR, DBID, DBPASSWD);
		
		assertTrue (JenzabarInterface.createOrUpdateCaucusUser (st, wrath));
		
		String query = "SELECT fname FROM user_info WHERE userid='wrath'";
		String result = selectOneColumnFromQuery (st, query, "fname");
		assertNotNull (result);
		assertEquals  (firstName, result);
	}
	
	private String selectOneColumnFromQuery (Statement st, String query, String columnName) {
		String result = null;
		try {
			ResultSet r = st.executeQuery(query);
			if (! r.next())  return null;
			return r.getString(columnName);
		}
		catch (Exception e) {
		}
		return result;
	}
	

	

}
