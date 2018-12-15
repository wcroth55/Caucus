
/*** INTO_ADDRESS.   Turn a string into an Address.
/
/    into_address (str, result);
/
/    Parameters:
/       Chix      str;      (name of person)
/       Address   result;   (resulting Address)
/
/    Purpose:
/       Convert a string name or address of a person into a full-fledged
/       Address, for use by other functions (such as choose_name()).
/
/       For now, into_address() only picks up name and host parts; there
/       is no clear way to parse out a userid.
/
/    How it works:
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions: make_address(), free_address()
/
/    Called by:  Caucus (addmess()...)
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/intoaddr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 11/11/91 23:53 New function*/
/*: CX  6/01/92 11:34 Trim blanks front and rear of result. */
/*: CR 12/10/92 15:01 chxalter arg int4. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CT  5/28/93 12:41 CMI address goes into name and id parts. */
/*: CR 10/12/01 16:40 Ignore @'s!  Replace with "^" for now. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;
extern Chix         ss_cmiesc;

FUNCTION   into_address (str, result)
   Chix    str;
   Address result;
{

   ENTRY ("into_address", "");

   chxclear (result->name);
   chxclear (result->id);
   chxclear (result->host);

   if (str == null.chx)  str = chxquick ("", 2);

   /*** CMI addresses go into NAME *and* ID part. */
   if (chxindex (str, L(0), ss_cmiesc) == 0) {
      chxcpy (result->name, str);
      chxcpy (result->id,   str);
      RETURN (1);
   }

   chxbreak (str, result->name, result->host, CQ("^"));

   chxtrim  (result->name);
   chxtrim  (result->host);
   while    (chxvalue(result->name,0L)==' ')  
      chxalter (result->name, L(0), chxquick(" ", 0), chxquick("", 1));
   while    (chxvalue(result->host,0L)==' ')
      chxalter (result->host, L(0), chxquick(" ", 0), chxquick("", 1));

   RETURN (1);
}
