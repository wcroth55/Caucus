/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

#include <stdio.h>
#include <string.h>

#include <sql.h>
#include <sqlext.h>

#define MAXCOLS		32

SQLHENV henv;
SQLHDBC hdbc;
SQLHSTMT hstmt;
int connected;

int main (int argc, char **argv) {
  if (ODBC_Connect ("DSN=myodbc3;UID=roth") != 0) {
     printf ("Error connecting!\n");
     exit (1);
  }

  printf ("Connected!\n");

  ODBC_Disconnect ();
}

int ODBC_Connect (char *connStr)
{
  short buflen;
  char buf[257];
  SQLCHAR dataSource[120];
  SQLCHAR dsn[33];
  SQLCHAR desc[255];
  SQLCHAR driverInfo[255];
  SWORD len1, len2;
  int status;

  if (SQLAllocHandle (SQL_HANDLE_ENV, NULL, &henv) != SQL_SUCCESS)
    return -1;

  SQLSetEnvAttr (henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3,
      SQL_IS_UINTEGER);

  if (SQLAllocHandle (SQL_HANDLE_DBC, henv, &hdbc) != SQL_SUCCESS)
    return -1;

  status = SQLDriverConnect (hdbc, 0, (UCHAR *) connStr, SQL_NTS,
      (UCHAR *) buf, sizeof (buf), &buflen, SQL_DRIVER_NOPROMPT);

  if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO)
    return -1;

  printf ("buf='%s'\n", buf);

  /* Deprecated, replace with SQLSetConnectAttr(). ?? */
  /* Probably don't even need, trace must be turned on first anyway. */
  SQLSetConnectOption (hdbc, SQL_OPT_TRACEFILE, (UDWORD) "\\SQL.LOG");


  connected = 1;

  status = SQLGetInfo (hdbc, SQL_DRIVER_VER, 
	driverInfo, sizeof (driverInfo), &len1);
  if (status == SQL_SUCCESS)
    printf ("Driver: %s\n", driverInfo);

  status = SQLGetInfo (hdbc, SQL_DRIVER_NAME, 
	driverInfo, sizeof (driverInfo), &len1);
  if (status == SQL_SUCCESS)
    printf ("Driver name: %s\n", driverInfo);

  if (SQLAllocHandle (SQL_HANDLE_STMT, hdbc, &hstmt) != SQL_SUCCESS)
    return -1;

  return 0;
}


/* Disconnect from the database */
int ODBC_Disconnect (void) {
  if (hstmt) {
    int sts;
    sts = SQLCloseCursor (hstmt);
#if 0
       if (sts != SQL_ERROR)
	   ODBC_Errors ("CloseCursor");
#endif
       SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
  }

  if (connected) SQLDisconnect (hdbc);

  if (hdbc) SQLFreeHandle (SQL_HANDLE_DBC, hdbc);

  if (henv) SQLFreeHandle (SQL_HANDLE_ENV, henv);

  return 0;
}
