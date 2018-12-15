
/*** C5_TABLE_CHECK.   Make sure all required Caucus 5.0 tables exist.
/
/    c5_table_check();
/ 
/    Parameters:
/
/    Purpose:
/
/    How it works:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sweb/c5tablecheck.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/28/05 New function. */
/*: CR 10/18/05 Add conforder table. */
/*: CR 03/15/06 Add user_info.registered, regface. */
/*: CR 04/20/07 Replaced by CML scripts.  Now a no-op. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"
#include "sql.h"
#include "sqlcaucus.h"

#define  BUFMAX 30000
static char buf[BUFMAX];

#define  CHXLISTMAX 100
#define  SMALLBUF   200

FUNCTION  c5_table_check ()
{

   return (1);
}
