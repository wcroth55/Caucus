/*** SYSESCQUOTE.   "Escape" or remove double-quotes in a string.
/
/   ok = sysescquote (string)
/
/   Parameters:
/     int   ok;       (1=removed them; 0=none)
/     Chix  string;
/
/   Purpose:
/      A Caucus e-mail message being sent out through the CMI gateway
/      may need to have its subject line altered so that the O.S.
/      can handle double-quotes properly.
/
/   How it works:
/      For Unix, it escapes double-quotes by putting a back-slash before
/      them.
/      For VMS, it removes double-quotes.
/
/   Returns: 1 if it changed any, 0 otherwise.
/
/   Related functions: 
/
/   Called by: add_cmimess()
/
/   Operating System Dependencies:
/
/   Home: sys/sysescqu.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  6/23/93 14:40 Created function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "systype.h"

FUNCTION  sysescquote (string)
   Chix   string;
{
   int    success=0;
   int4   value, pos;

   ENTRY ("sysescquote","");

#if UNIX
   for (pos=0L;   value = chxvalue (string, pos);   pos++)
      if (value == '"') {
         chxalter  (string, pos, CQ("\""), chxquick("\\\"", 1));
         pos++;                                    /* Yes, *two* increments. */
         success=1;
      }
#endif

   RETURN  (success);
}
