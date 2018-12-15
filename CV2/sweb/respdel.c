
/*** RESP_DEL.   CML function of same name.
/
/    resp_del (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $resp_del(), return error code < 0, or else 0 = success.
/
/    How it works:
/       $resp_del (cnum item_id response-number)
/
/    Returns:
/
/    Error Conditions:
/
/    Notes: worry about attachments someday!
/  
/    Home:  respdel.c
/ */

/*** Copyright (C) 1995-2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 04/03/06 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "sqlcaucus.h"
#include "sql.h"
#include "caucus5.h"

#define  SITE_CLEARED  -100
#define  QUERY_MAX     2000
#define  USER_MAX       256

#define TEMP(n)   chxtoken (temp, nullchix, n, arg)

extern int       cml_debug;
extern AsciiBuf  sqlbuf;

FUNCTION  resp_del (Chix result, Chix arg) {
   static Chix   temp = nullchix;
   int    error, rc, ok, i, item_id;
   int4   cnum, rnum;
   char   user[USER_MAX], nothing[100], magic[USER_MAX];
   AsciiBuf ascii_buf();
   ENTRY ("resp_add", "");

   /*** Initialization. */
   if (temp == nullchix) {
      temp  = chxalloc     (L(40), THIN, "func_var temp");
   }

   /*** Parse all of the arguments. */
   error = 0;
   TEMP(1);   if (! chxnum (temp, &cnum))    cnum    =  0;
   TEMP(2);   if (! chxnum (temp, &item_id)) item_id =  0;
   TEMP(3);   if (! chxnum (temp, &rnum))    rnum    = -1;

   /*** Validate arguments and access. */
   if      (cnum    == 0)  error = RESP_DEL_NOCNUM;
   else if (item_id <  0)  error = RESP_DEL_NOITEM;
   else if (rnum    <  0)  error = RESP_DEL_NORESP;

   /*** Does user have access to delete this response? */
   if (error == 0) {
      ok  = (conf_access(cnum) >= privValue("organizer"));
      if (NOT ok) {
         sprintf (sqlbuf->str, 
            "SELECT userid FROM resps WHERE rnum=%d AND items_id=%d AND version=0",
                  rnum, item_id);
         rc = sql_quick_limited (0, sqlbuf->str, magic, 100, 1);
         sysuserid (temp);
         ok = (streq (ascquick(temp), magic));
      }
      if (NOT ok)        error = RESP_DEL_NOWRITE;
   }

   /*** Mark the response as deleted, and empty the text. */
   if (error == 0) {
      sprintf (sqlbuf->str, 
         "UPDATE resps SET deleted=1, text='' "
         " WHERE rnum=%d AND items_id=%d AND version=0", rnum, item_id);
      rc = sql_quick_limited (0, sqlbuf->str, nothing, 100, 0);
      if (rc == 0)  error = RESP_DEL_FAILED;
   }

   /*** And return the error code, or else the new response number. */
   sprintf (magic, "%d", error);
   chxcat  (result, CQ(magic));

   /*** Update items.lastresp for this item, just in case. */
   if (error == 0) {
      sprintf (sqlbuf->str, 
         "UPDATE items SET lastresp = "
         "   (SELECT MAX(rnum) FROM resps WHERE items_id = %d AND deleted=0 AND version=0) "
         " WHERE id = %d ", item_id, item_id);
      rc = sql_quick_limited (0, sqlbuf->str, nothing, 100, 0);
   }

   return (error);
}
