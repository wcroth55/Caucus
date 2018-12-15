
/*** MV_CONF.  Rename a conference.
/
/    ok = mv_conf (cnum, name2, lib)
/
/    Parameters:
/       int   cnum;           conf number
/       Chix  name2;          New full name of the conference.
/       char *lib;            root path of caucus (web) file library.
/
/    Purpose:
/
/    How it works:
/       If LIB is non-null, rename the lib directory for this
/       conference as well.
/
/    Returns: 0 cnum doesn't exist
/            -1 name2 does exist (rename fails)
/             1 success
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  conf/mvconf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/16/97 15:32 New function, based on makeconf(). */
/*: CR 10/04/05 Major rewrite for C5. */
/*: CR 04/07/11 Allow renaming to a *deleted* conf name. */

#include <stdio.h>
#include "unitcode.h"
#include "handicap.h"
#include "derr.h"
#include "conf.h"
#include "chixuse.h"
#include "namelist.h"
#include "null.h"
#include "done.h"

#define  MAXNAME 300

extern union null_t     null;

FUNCTION  mv_conf (int cnum, Chix name2, char *lib)
{
   int      success, rc;
   char     dir1[MAXNAME], dir2[MAXNAME];
   char     query[MAXNAME], temp[MAXNAME], oldname[MAXNAME], newname[MAXNAME], 
            nothing[10];
   ENTRY ("mv_conf", "");

   /*** Make sure 'old' conference exists. */
   cl_name (oldname, MAXNAME, cnum);
   if (NULLSTR(oldname))   RETURN(0);

   /*** Make sure 'new' name doesn't already exist! */
   ascofchx  (newname, name2, 0, MAXNAME);
   sprintf   (query, "SELECT cnum FROM confs WHERE name='%s' AND deleted=0 ", newname);
   sql_quick_limited (0, query, temp, MAXNAME, 1);
   if (NOT NULLSTR(temp))  RETURN(-1);

   /*** Change name. */
   sprintf (temp, "UPDATE confs SET name='%s' WHERE cnum='%d'", newname, cnum);
   rc = sql_quick_limited (0, temp, nothing, 9, 0);

   /*** Update the TrueConf file. */
   unit_lock  (XCTC, WRITE, L(cnum), L(0), L(0), L(0), nullchix);
   unit_make  (XCTC);
   unit_write (XCTC, name2);   unit_write (XCTC, CQ("\n"));
   unit_close (XCTC);  
   unit_unlk  (XCTC);

   /*** Clear the trueconf cache. */
   get_trueconf (-1, nullchix, nullchix, nullchix);

   /*** Move the (web) conference library. */
/*
   if (lib != NULL  &&  lib[0]) {
      strlower  (oldname);
      strlower  (newname);
      sprintf   (dir1, "%s/%s", lib, oldname);
      sprintf   (dir2, "%s/%s", lib, newname);
      if (syscheck (dir1))  sysrename (dir1, dir2);
   }
*/
   DONE (1);

done:
   RETURN  (success);
}
