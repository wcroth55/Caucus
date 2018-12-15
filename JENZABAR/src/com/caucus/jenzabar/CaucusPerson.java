package com.caucus.jenzabar;

public class CaucusPerson {

	private String userid;
	private String firstName;
	private String lastName;
	private String emailAddress;
	
	public CaucusPerson (String id, String first, String last, String email) {
		this.userid = id;
		this.firstName = first;
		this.lastName = last;
		this.emailAddress = email;
	}
	
	public String getUserid() { return userid; }
	public String getFirstName() { return firstName; }
	public String getLastName() { return lastName; }
	public String getEmailAddress() { return emailAddress; }
}
