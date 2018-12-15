
/*** RUN_PROMPT_MACRO.   Execute the "prompt macro" associated with a string.
/
/    run_prompt_macro (prompt_name, dictionary, cname);
/
/    Parameters:
/       char  *prompt_name;   (name of prompt string)
/       int    dictionary;    (user's dictionary number)
/       Chix   cname;         (name of current conference)
/
/    Purpose:
/       Execute the "prompt macro" that is associated with the dictionary
/       string named in PROMPT_NAME.
/
/    How it works:
/       See if the first word of string dic_Aproperties is '1'.  (This means
/       that prompt macros should be executed).  Add ".pm" onto the end of
/       PROMPT_NAME.  Fetch and execute the resulting string, if possible.
/
/    Returns: 1 if prompt macro is executed.
/             0 otherwise
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  inout/runpromp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CK  9/20/93 12:23 New function. */
/*: CK  9/23/93 17:18 Long last arg to chxsetval(). */

#include <stdio.h>
#include "caucus.h"

FUNCTION  run_prompt_macro (prompt_name, dictionary, cname)
   char  *prompt_name;
   int    dictionary;
   Chix   cname;
{
   static Chix dicprop = nullchix;
   static Chix s_dicprop, temp, ignore;
   char   promptmac[100];
   int    status;
   int4   pos, val;

   ENTRY  ("run_prompt_macro", "");

   /*** On first call, get (and keep) working Chix. */
   if (dicprop == nullchix) {
      dicprop = chxalloc ( L(20), THIN, "mdprompt dicprop");
      ignore  = chxalloc ( L(20), THIN, "mdprompt ignore");
      temp    = chxalloc (L(200), THIN, "mdprompt temp");
   }

   /*** Find out (and cache) this dictionary's properties. */
   if (NOT mdstr (dicprop, "dic_Aproperties", &s_dicprop))    RETURN(0);
   chxtoken (temp, nullchix, 1, dicprop);

   /*** Should we go get (and execute) the prompt macro for MDINDEX? */
   if (chxvalue (temp, L(0)) != '1')                            RETURN(0);

   /*** Figure the name of the prompt macro string, and try to fetch it. */
   strconc (promptmac, prompt_name, ".pm");
   if (NOT sysmdstr (promptmac, dictionary, temp, &status))   RETURN(0);

   /*** Execute the prompt macro.  (Turn newlines into ';'s, first.) */
   for (pos=0;   (val = chxvalue  (temp, pos));   ++pos)
      if (val == '\n')  chxsetval (temp, pos, THIN, L(';'));
   interpret (temp, cname, ignore, nullchix);
   RETURN (1);
}
