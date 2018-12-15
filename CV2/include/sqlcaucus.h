
/*** SQLCAUCUS.H.   Include file for SQL functions. */
 
/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/23/03 New file. */
/*: CR  3/14/04 Add SQLCAU_HANDLES */

#ifndef  SQLCAUCUS
#define  SQLCAUCUS 1

/*** Define common "readability" macros. */
#define  FUNCTION
#define  NOT              !
#define  EMPTYSTR(x)     (!(x[0]))

/*** New stuff */
#define SQL_SUCCESS      1
#define SQL_NO_DATA     -1
#define SQL_NOMEM       -2
#define SQL_NOCONNECT   -3
#define SQL_BADSTM      -4

/*** Function prototypes. */
FUNCTION  int   sql_init (char *connstr, char *odbcini, char *odbcsysini);

#endif

