
/*** SQL_INIT.   Initialize SQL connection.
/
/    ok = sql_init (connectstr, odbcini, odbcsysini);
/    
/    rc = sql_exec (sql_statement);
/
/    rc = sql_fetch();
/
/    sql_done();
/   
/    Parameters:
/       connectstr   database connect string
/          DSN=<data source name>
/          HOST=<server host name>
/          SVT=<database server type>
/          DATABASE=<database path>
/          OPTIONS=<db specific opts> 
/          UID=<user name>        
/          PWD=<password>         
/          READONLY=<N|Y>         
/          FBS=<fetch buffer size>  
/          Typically "DSN=xxx;DATABASE=caucus_userid;UID=userid;PW=password"
/
/    Purpose:
/
/    How it works:
/
/    Returns: values from sqlcaucus.h
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sqlinit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/23/03 New function. */
/*: CR  7/23/03 Replace setenv() with putenv(). */
/*: CR 11/12/03 Add ENABLE macro, to allow building on hosts w/o ODBC */

#define  ENABLE    1

#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include "sweb.h"
#include "sqlcaucus.h"

#if ENABLE
SQLHENV  sql_h_env   = NULL;
SQLHDBC  sql_h_conn  = NULL;
SQLHSTMT sql_h_stmt  = NULL;
static   int    sql_connected = 0;
static   int    sql_colnum    = 0;
static   int    sql_columns   = 0;
#endif

FUNCTION int sql_init (char *connectstr, char *odbcini, char *odbcsysini) {
  static char env_odbcini[300], env_odbcsysini[300];
  short buflen;
  char  buf[257];
  int   status;

#if ENABLE
  if (NOT EMPTYSTR(odbcini))  {
     sprintf (env_odbcini, "ODBCINI=%s", odbcini);
     putenv  (env_odbcini);
  }
  if (NOT EMPTYSTR(odbcsysini))  {
     sprintf (env_odbcsysini, "ODBCSYSINI=%s", odbcsysini);
     putenv  (env_odbcsysini);
  }

  if (SQLAllocHandle (SQL_HANDLE_ENV, NULL, &sql_h_env) != SQL_SUCCESS)
     return SQLCAU_BADENV;

  SQLSetEnvAttr (sql_h_env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3,
      SQL_IS_UINTEGER);

  if (SQLAllocHandle (SQL_HANDLE_DBC, sql_h_env, &sql_h_conn) != SQL_SUCCESS)
    return SQLCAU_BADCON;

  status = SQLDriverConnect (sql_h_conn, 0, (UCHAR *) connectstr, SQL_NTS,
      (UCHAR *) buf, sizeof (buf), &buflen, SQL_DRIVER_NOPROMPT);

  if (status != SQL_SUCCESS  && 
      status != SQL_SUCCESS_WITH_INFO)  return SQLCAU_NOCONN;

  /* Deprecated, replace with SQLSetConnectAttr(). ?? */
  /* Probably don't even need, trace must be turned on first anyway. */
  SQLSetConnectOption (sql_h_conn, SQL_OPT_TRACEFILE, (UDWORD) "\\SQL.LOG");

  sql_connected = 1;

  if (SQLAllocHandle (SQL_HANDLE_STMT, sql_h_conn, &sql_h_stmt) != SQL_SUCCESS)
     return SQLCAU_BADSTM;
#endif

  return SQLCAU_OK;
}


/*** Completely disconnect and clean up when finished with ODBC. */
FUNCTION void sql_done() {
#if ENABLE
   if (sql_h_stmt != NULL) {
      int sts;
      sts = SQLCloseCursor (sql_h_stmt);
      SQLFreeHandle (SQL_HANDLE_STMT, sql_h_stmt);
   }

   if (sql_connected)      SQLDisconnect (sql_h_conn);
   if (sql_h_conn != NULL) SQLFreeHandle (SQL_HANDLE_DBC, sql_h_conn);
   if (sql_h_env  != NULL) SQLFreeHandle (SQL_HANDLE_DBC, sql_h_env);

#endif
   return;
}

FUNCTION int sql_exec (char *stmt) {
   int rc = SQL_SUCCESS;

#if ENABLE
   if (! sql_connected)  return (SQL_SUCCESS);

   rc = SQLExecDirect (sql_h_stmt, (UCHAR *) stmt, SQL_NTS);
#endif

   return (rc);
}

FUNCTION int sql_fetch() {
   int         rc = SQL_SUCCESS;

#if ENABLE
   SQLSMALLINT numcols;

   if (! sql_connected)  return (SQL_NO_DATA);

   rc = SQLFetch (sql_h_stmt);
   if (rc == SQL_NO_DATA)  SQLCloseCursor (sql_h_stmt);
   SQLNumResultCols (sql_h_stmt, &numcols);
   sql_columns = numcols;
   sql_colnum  = 0;
#endif

   return (rc);
}

FUNCTION int sql_getchardata (char *buf, int bufmax) {
   int        rc = SQL_NO_DATA;
#if ENABLE
   SQLINTEGER indicator;
#endif

   buf[0] = '\0';

#if ENABLE
   if (sql_colnum >= sql_columns)  return (SQL_NO_DATA);

   rc = SQLGetData (sql_h_stmt, ++sql_colnum, SQL_C_CHAR, buf, bufmax,
                    &indicator);
   if (rc        == SQL_NO_DATA)    return (rc);
   if (indicator == SQL_NULL_DATA)  indicator = -1;
   if (indicator == SQL_NO_TOTAL)   indicator = -1;
   buf[indicator+1] = '\0';
#endif
   return (rc);
}
