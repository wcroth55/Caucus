
/*** OKNEWCONF.  Is this new conference name OK? 
/
/    ok = oknewconf (name);
/  
/    Parameters:
/       int   ok;     (ok name?)
/       Chix  name;   (name of new conference)
/  
/    Purpose:
/      Is this an OK name for a conference?
/  
/    How it works:
/       warning: do we really want to check if the first character is
/       alphanumeric?
/  
/    Returns: 1 if conference name is ok and conference doesn't exist.
/             0 otherise.
/  
/    Error Conditions:
/  
/    Side Effects:
/  
/    Related to:
/  
/    Called by: 
/  
/    Home: conf/oknewcon.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:59 Source code master starting point */
/*: JV  8/21/89 19:03 Reflect change in def of return from cnumber. */
/*: CW  7/20/89 14:28 Add XW unit codes. */
/*: CR  2/12/91 17:39 Do not allow reserved words (LIST, STOP, HELP). */
/*: CR  4/09/91 13:30 Remove confnum. */
/*: JV 10/28/91 14:50 Added 2nd arg to cnumber().*/
/*: WC 11/29/91 21:14 Add 3rd arg to cnumber(). */
/*: JX  3/10/92 09:35 Chixify, add header. */
/*: JX  5/16/92 14:50 Remove unnecessary SUCCESS. */
/*: CR 10/13/92 12:51 Some cleanup, comments. */
/*: CR 11/24/92 13:37 Allow "." as part of conf name. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "chixuse.h"

#define  MAX_CONFNAME  80

extern union null_t null;

FUNCTION  oknewconf (name)
   Chix  name;
{
   int    good, v, len, i;
   static Chix  jointab, helptab;
   Chix   str;

   ENTRY ("oknewconf", "");
   good = 1;

   str = chxalloc (L(100), THIN, "oknew str");

   /*** Is the name too int4, empty, or not start with a letter? */
   len = chxlen (name);
   if (len > MAX_CONFNAME  ||  EMPTYCHX (name)  ||
                     NOT cf_lower((char) chxvalue (name, L(0))))  good = 0;

   /*** Allow only letters, numbers, ".", and _'s. */
   for (i=0;   i<len;   i++) {
      v = chxvalue (name, L(i));
      if (cf_lower (v))  continue;
      if (cf_digit (v))  continue;
      if (v == '_')      continue;
      if (v == '.')      continue;
      good = 0;
      break;
   }

   if (NOT good) {
      mdwrite (XWTX, "sys_Ebadconf", null.md);
      goto done;
   }

   /*** Do not allow reserved words like LIST, STOP, HELP. */
   mdtable ("mai_Ajoin",    &jointab);
   mdtable ("gen_Aprmhelp", &helptab);
   if (tablefind  (jointab, name) >= 0  ||
       tablematch (helptab, name) >= 0) {
      mdwrite (XWTX, "sys_Ereserved", null.md);
      good = 0;
      goto done;
   }

   /*** Does this conference already exist? */
   good = 1;
   chxcpy (str, name);
   if (cnumber (str, LOCALCONF, &v) > 0  &&  chxeq (str, name)) {
      good = 0;
      mdwrite    (XWTX, "sys_Econfexist", null.md);
   }

done:
   chxfree (str);
   RETURN  (good);
}
