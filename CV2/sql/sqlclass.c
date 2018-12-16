
/*** SQL_CLASS.   Psuedo "class" of SQL handling functions.
/
/    Purpose:
/       Pseudo "Class" for handling SQL queries from inside Caucus.
/       Version 2: uses MySQL C API directly, instead of ODBC.
/
/    How it works:
/       sql_init() initializes the connection.  Only the connectstr argument
/       is used.  It contains semicolon-separated "name=value" sets of the
/       following:
/          DB=database_name           (required)
/          UID=user_name              (required)
/          PWD=password               (required)
/          HOST=server_host_name      (not yet implemented)
/          SOCKET=socket_file         (optional)
/          PORT
/
/       sql_done() closes down the connection.
/
/       There are two sets of functions for actually executing queries and getting
/       data back.  The first set is for "internal" use in other parts of the code.
/       The second set implements the CML-level function $sql_query_open() and
/       its related functions.
/
/       Both sets operate on "handle" arguments, which are just integer indices
/       into internal arrays.  A handle is really a MySQL result set.
/
/   SET 1 -- INTERNAL
/       The first set typically follows this sequence:
/          rc = sql_exec (handle, sql_statement);
/               (execute the query)
/          rc = sql_fetch(handle);
/               (get the next row of data, SQL_NO_DATA if none left)
/          rc = sql_getchardata(handle, asciibuf);
/               (get the next column of data from current row, SQL_NO_DATA if row finished)
/
/          rc = sql_getcharlimited(handle, buf, bufmax);
/               (same as sql_getchardata, but forcibly truncates data)
/
/        Notice that there is no "close" or release of the handle, either the last (failed)
/        call to sql_fetch() or the next call to sql_exec() closes (really, releases 
/        memory used by) the previous call.
/ 
/        Additionally, sql_quick() provides two versions of a "quick" call
/        to sql_exec(), sql_fetch(), and one call to sql_getchardata(), for cases
/        where we are getting exactly one data element.
/           rc = sql_quick         (query, asciibuf, is_select);
/           rc = sql_quick_limited (query, buf, bufmax, is_select);
/
/        Also, the utility function sql_last_insert() is provided, to return
/        the value of the last auto-increment id field INSERTed.  (Do not
/        use SELECT LAST_INSERT_ID, it has a bug in MySQL 5.1.)
/
/   SET 2 -- CML-level functions
/       The next four functions implement the CML functions of the same
/       name.  They operate on a pool of statement handles that operate
/       on chix:
/          handle = sql_query_open (stmt, chixlist)
/             execute a query, with list of result vars
/
/          sql_query_row  (handle);
/             fetch the next row of data for SELECT, put column data into matching Chix
/
/          sql_query_count(handle)
/             return # of rows affected by SELECT, INSERT, DELETE, UPDATE
/
/          sql_query_close(handle)  close the handle (release the memory)
/
/    Returns: values from sqlcaucus.h
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sqlinit.c
/ */

/*** Copyright (C) 1995-2007 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/23/03 New function. */
/*: CR  7/23/03 Replace setenv() with putenv(). */
/*: CR 11/12/03 Add ENABLE macro, to allow building on hosts w/o ODBC */
/*: CR 10/17/05 Add extensive diagnostic logging in sql_init(). */
/*: CR 11/28/05 Track status of open cursors and only close when needed. */
/*: CR 04/20/07 Major rewrite for MySQL C API. */
/*: CR 05/13/08 Add sql_last_insert(). */

#include <stdio.h>
#include <stdlib.h>
#include "sweb.h"
#include "sqlcaucus.h"
#include <mysql.h>
#include <sys/time.h>

#define  SMALLBUF  300
static char sql_dbname[SMALLBUF];
static char sql_dbuid [SMALLBUF];
static char sql_dbpwd [SMALLBUF];
static char sql_dbhost[SMALLBUF];
static char sql_dbsock[SMALLBUF];

#define MAX_RESULTSETS 25
#define INT_RESULTSETS  4

static MYSQL      connection;
static int        sql_connected = 0;
static MYSQL_RES *resultSet [MAX_RESULTSETS];   /* result sets */
static MYSQL_ROW  resultRow [MAX_RESULTSETS];   /* row for each result set */
static int        resultCols[MAX_RESULTSETS];   /* max cols for result set */
static int        resultPos [MAX_RESULTSETS];   /* current pos (column) in result set */
static Chix *     resultChx [MAX_RESULTSETS];   /* target chix vars per result set */
static int        isSelect  [MAX_RESULTSETS];   /* Is this query a SELECT? */
static long *     resultLen [MAX_RESULTSETS];   /* Length of fields (columns) in current row */

static struct timeval startTime;

static AsciiBuf limitBuf = NULL;

char *strcpy();
char *strncpy();
char *strcat();
char *strtoken();
AsciiBuf ascii_buf();

FUNCTION int sql_init (char *connectstr, char *odbcini, char *odbcsysini) {
   char *s;
   char *socket;
   char  temp[SMALLBUF], word[SMALLBUF];
   int   token, i;
   char *sysmem();

   /*** Parse out the DB=, UID=, PWD= parameters from the connect string. */
   sql_dbname[0] = '\0';
   sql_dbuid [0] = '\0';
   sql_dbsock[0] = '\0';
   strcpy (sql_dbhost, "localhost");
   socket = NULL;
   safecpy (temp, connectstr, SMALLBUF-1);
   for (s=temp;   *s;   ++s)  if (*s == ';')  *s = ' ';
   for (token=1;   strtoken (word, token, temp) != NULL;   ++token) {
      if      (strindex (word, "DB=")     == 0)  strcpy (sql_dbname, word+3);
      else if (strindex (word, "UID=")    == 0)  strcpy (sql_dbuid,  word+4);
      else if (strindex (word, "PWD=")    == 0)  strcpy (sql_dbpwd,  word+4);
      else if (strindex (word, "HOST=")   == 0)  strcpy (sql_dbhost, word+5);
      else if (strindex (word, "SOCKET=") == 0) {
         strcpy (sql_dbsock,  word+7);
         socket = sql_dbsock;
      }
   }

   if (mysql_init(&connection) == NULL)  return SQL_NOMEM;

   if (mysql_real_connect (&connection, sql_dbhost, sql_dbuid, 
          sql_dbpwd, sql_dbname, 3306, socket, 0) == NULL)  return SQL_NOCONNECT;

   for (i=0;   i<MAX_RESULTSETS;   ++i)  resultSet[i] = NULL;

   sql_connected = 1;
   return 1;
}

FUNCTION int sql_exec (int h, char *stmt) {
   int rc;

   if (! sql_connected)                 return (SQL_SUCCESS);
   if (h < 0  ||  h >= INT_RESULTSETS)  return (SQL_BADSTM);

   if (resultSet[h] != NULL)  mysql_free_result(resultSet[h]);

   logQueryStatement (stmt);
   rc = mysql_query (&connection, stmt);
   logQueryTime(rc);

   resultSet[h] = mysql_store_result (&connection);
   
   rc = SQL_SUCCESS;
   if (resultSet[h] == NULL  &&  mysql_errno(&connection) > 0) rc = SQL_BADSTM;

   return (rc);
}

FUNCTION logQueryStatement (char *stmt) {
   gettimeofday(&startTime, NULL);
   logger (0, LOG_FILE, stmt);
}

FUNCTION logQueryTime(int rc) {
   struct timeval stopTime;
   char   text[200];

   gettimeofday(&stopTime, NULL);
   long deltaSec = stopTime.tv_sec  - startTime.tv_sec;
   long deltaMic = stopTime.tv_usec - startTime.tv_usec;
   if (deltaMic < 0) {
      --deltaSec;
      deltaMic += 1000000;
   }
   sprintf (text, "MySQL rc=%d, %s: %6d.%06d", rc, (deltaSec <= 0 ? "FAST" : "SLOW"), deltaSec, deltaMic);
   logger (0, LOG_FILE, text);
}

FUNCTION int sql_fetch (int h) {
   if (! sql_connected)                                           return (SQL_NO_DATA);
   if (h < 0  ||  h >= INT_RESULTSETS  ||  resultSet[h] == NULL)  return (SQL_NO_DATA);

   resultRow[h] = mysql_fetch_row(resultSet[h]);
   if (resultRow[h] == NULL) {
      mysql_free_result (resultSet[h]);
      resultSet[h] = NULL;
      return SQL_NO_DATA;
   }
   resultCols[h] = mysql_num_fields    (resultSet[h]);
   resultLen [h] = mysql_fetch_lengths (resultSet[h]);
   resultPos [h] = 0;
   return SQL_SUCCESS;
}

FUNCTION int sql_getchardata (int h, AsciiBuf abuf) {
   char **row;
   long *len;
   int   pos;

   abuf->str[0] = '\0';
   if (h < 0  ||  h >= INT_RESULTSETS  ||  resultSet[h] == NULL)  return (SQL_NO_DATA);
   if (resultPos[h] >= resultCols[h])                             return (SQL_NO_DATA);

   row = resultRow[h];
   if (row != NULL) {
      pos = resultPos[h];
      len = resultLen[h];
      ascii_buf (abuf, (int) (len[pos]) + 10);
      if (row[pos] != NULL)  strcpy (abuf->str, row[pos]);
   }

   ++resultPos[h];
   return (SQL_SUCCESS);
}

FUNCTION int sql_getcharlimited (int h, char *buf, int bufmax) {
   int    rc;

   limitBuf = ascii_buf (limitBuf, bufmax+20);
   rc = sql_getchardata (h, limitBuf);
   limitBuf->str[bufmax-1] = '\0';
   strcpy (buf, limitBuf->str);
   return rc;
}

FUNCTION int sql_quick (int h, char *query, AsciiBuf abuf, int is_select) {
   int rc;

   abuf->str[0] = '\0';
   rc = sql_exec (h, query);
   if (rc != SQL_SUCCESS)                   return(0);
   if (! is_select)                         return(1);

   rc = sql_fetch(h);
   if (rc != SQL_SUCCESS)                   return (0);
   rc = sql_getchardata (h, abuf);

   return (1);
}

FUNCTION int sql_quick_limited (int h, char *query, char *buf, 
                                int bufmax, int is_select) {
   int   rc;
   limitBuf = ascii_buf (limitBuf, bufmax+20);

   rc = sql_quick (h, query, limitBuf, is_select);
   limitBuf->str[bufmax-1] = '\0';
   strcpy (buf, limitBuf->str);

   return rc;
}

FUNCTION int sql_query_open (char *stmt, Chix *chixlist) {
   int   i, h, t, rc;
   char  temp[SMALLBUF], word1[SMALLBUF];

   /*** Check arguments, make sure we're connected! */
   if (stmt == NULL  ||  !stmt[0])   return (-1);
   if (! sql_connected)              return (-1);

   /*** Find an allocated, unused statement handle H */
   for (h=INT_RESULTSETS;   h<MAX_RESULTSETS;   ++h) {
      if (resultSet[h] == NULL)  break;
   }
   if (h >= MAX_RESULTSETS) return (-1);

   /*** At this point we have a good statement handle.  Mark it as "in use",
   /    and save the list of chix target variables, if any. */
   resultChx[h] = chixlist;

   /*** Now decide if this was a SELECT statement. */
   strncpy (temp, stmt, SMALLBUF);
   temp[SMALLBUF-1] = '\0';
   strtoken (word1, 1, temp);
   strlower (word1);
   isSelect[h] = streq (word1, "select");

   /*** Execute the statement. */
   logQueryStatement (stmt);
   rc = mysql_query (&connection, stmt);
   logQueryTime(rc);

   resultSet[h] = mysql_store_result (&connection);
   if (resultSet[h] == NULL  &&  mysql_errno(&connection) > 0) h = -1;

   return (h);
}

FUNCTION sql_query_row (int h) {
   int    col;
   char **row;
   char  *data;
   Chix  *chxlist;

   /*** Check arguments */
   if (h < INT_RESULTSETS   ||  h >= MAX_RESULTSETS || 
       resultSet[h] == NULL ||  ! isSelect[h])           return (0);

   /*** Fetch the row (advance the cursor), find out # of columns */
   resultRow[h] = mysql_fetch_row(resultSet[h]);
   if (resultRow[h] == NULL) {
      mysql_free_result (resultSet[h]);
      resultSet[h] = NULL;
      return (0);
   }
   resultCols[h] = mysql_num_fields(resultSet[h]);

   chxlist = resultChx[h];
   row = resultRow[h];
   for (col=0;   col < resultCols[h];   ++col) {
      data = (row[col] == NULL ? "" : row[col]);
      if (chxlist[col] != NULL)   chxofascii (chxlist[col], data);
   }

   return (1);
}

FUNCTION int sql_query_count (int h) {
   if (h < INT_RESULTSETS  ||  h >= MAX_RESULTSETS || resultSet[h] == NULL)  return (-1);

   return mysql_affected_rows (&connection);
}

FUNCTION sql_sequence (char *seqname, Chix result) {
   char  name[SMALLBUF], seqtab[SMALLBUF+20], stmt[SMALLBUF+100];
   char  buf [SMALLBUF];
   int   h;
   Chix *chixlist;

   /*** Note: this version is specific to MySQL.  When other
   /    RDBM's are supported, we'll need different versions of this code. */

   /*** Assemble the name of the sequence table. */
   strcpy  (seqtab, "sequence_");
   safecpy (name,   seqname, SMALLBUF);
   strcat  (seqtab, name);

   /*** Does the named sequence exist?  If not, create it. */
   if (! sql_sprintf ("select id from %s", seqtab)) {
      sql_sprintf ("create table %s (id INT NOT NULL)",  seqtab);
      sql_sprintf ("insert into  %s values (0)",         seqtab);
   }

   /*** Increment the sequence.   Then get its value. */
   sql_sprintf ("update %s set id=last_insert_id(id+1)", seqtab);
   chixlist = (Chix *) sysmem (2 * sizeof(Chix *), "chixlist");
   chixlist[0] = result;
   chixlist[1] = nullchix;
   chxclear (result);
   h = sql_query_open ("select last_insert_id()", chixlist);
   if (h >= 0) {
      sql_query_row   (h);
      sql_query_close (h);
   }
   return (1);
}

FUNCTION sql_sprintf (char *format, char *value) {
   char stmt[SMALLBUF+SMALLBUF];
   int  h;

   sprintf (stmt, format, value);
   h = sql_query_open (stmt, (Chix *) NULL);
   if (h >= 0)  sql_query_close (h);

   return (h >= 0);
}


FUNCTION sql_query_close (int h) {
   if (resultSet[h] !=          NULL)  mysql_free_result (resultSet[h]);
   resultSet[h] = NULL;
   if (resultChx[h] != (Chix *) NULL)  sysfree ((char *) resultChx[h]);
   resultChx[h] = NULL;
}


/*** Completely disconnect and clean up when finished with ODBC. */
FUNCTION void sql_done() {
   mysql_close (&connection);
}


FUNCTION safecpy (char *target, char *source, int size) {
   if (target != NULL  &&  source != NULL) strncpy (target, source, size);
   target[size-1] = '\0';
}

FUNCTION long sql_last_insert() {
   return mysql_insert_id (&connection);
}
