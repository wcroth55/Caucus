/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** PARSE_OR_ASK.  Parse item list off command line, or ask user for it.
/
/    ok = parse_or_ask (instance, &result, modifier, attach, cnum, thisitem,
/                                  mod1st);
/
/    Parameters:
/       int    ok;          (Success?)
/       Chix   instance;    (presumed item list, from command line)
/       Rlist  result;      (place to put resulting Rlist of items/resps)
/       char  *modifier;    (array of modifier results)
/       Chix   attach;      (file attachment data)
/       int    cnum;        (conference number)
/       This   thisitem;    (this item/resp information)
/       int    mod1st;      (first response follows RESPONSE modifier?)
/
/    Purpose:
/       The Caucus CLI tries to parse item/response lists off the
/       command line.  If for some reason the user did not specify
/       an item/response, we give the user one more chance, and
/       prompt them for an item/response list.
/
/       Parse_or_ask() handles this "try to parse it, or else ask for
/       it one more time and *then* parse it" functionality.
/
/    How it works:
/       Calls new_parse() to actually do the parsing work, and otherwise
/       calls mdprompter() to interact with the user.
/
/       Parse_or_ask() allocates RESULT (well, new_parse() really does),
/       so it is up to the caller to a_fre_rlist() it.
/
/       The MOD1ST argument controls whether the precise selection of
/       the first response in a range follows the use of the RESPONSE
/       keyword.  In most cases, this argument should be 1. See new_parse()
/       for more details.
/
/    Returns: 1 if a parseable item/response instance was supplied (somehow!)
/                  *and* some items/responses were actually found.
/             0 otherwise
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus CLI functions.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  gen/parseora.c
/ */

/*: CP  9/09/93 16:44 New function. */
/*: CK  9/15/93 13:36 Add 'mod1st' arg. */
/*: CK 10/07/93 14:45 Handle error code return from new_parse(). */

#include <stdio.h>
#include "caucus.h"
#include "modifier.h"
#include "api.h"
#include "redirect.h"
#define   RLNULL   ( (Rlist) NULL )

extern struct master_template   master;
extern union  null_t            null;

FUNCTION  parse_or_ask (instance, result, modifier, attach, cnum, thisitem,
                                  mod1st)
   Chix   instance;
   Rlist *result;
   char  *modifier;
   Chix   attach;
   int    cnum;
   This   thisitem;
   int    mod1st;
{
   int    success, item, resp, errpos;

   ENTRY ("parse_or_ask", "");

   /*** Try just parsing what the caller gave us in INSTANCE.   Then
   /    make sure there's really at least one response in it. */
   if (new_parse (instance, result, modifier, attach, cnum, thisitem, 
                            1, mod1st, 1, &errpos)) {
      one_response (&item, &resp, *result, &master);
      if (item < 0)  FAIL;
      SUCCEED;
   }

   /*** Report on errors.  Don't need to complain 'no items selected'. */
   if (errpos >= 0) {
      report_parse_err (instance, errpos);
      modifier[M_NOCOMPLAIN] = 1;
      FAIL;
   }

   /*** If that didn't work, and NOASK was specified, give up immediately. */
   if (modifier[M_NOASK])  FAIL;

   /*** Otherwise, prompt the user "hey, *which* items do you want to see?" */
   if (mdprompter ("res_Pwhichitem", nullchix, 0, "res_Hwhichitem", 200,
                    instance, nullchix) < -1)  FAIL;

   /*** ...and parse the result.   Make sure there's really at least
   /    one response in it. */
   a_fre_rlist (*result);
  *result = RLNULL;
   if (new_parse (instance, result, modifier, attach, cnum, thisitem, 
                                0, mod1st, 1, &errpos)) {
      one_response (&item, &resp, *result, &master);
      if (item < 0)  FAIL;
      SUCCEED;
   }

   /*** Report on errors in 2nd try.  Don't need to complain 'no items 
   /    selected'. */
   if (errpos >= 0) {
      report_parse_err (instance, errpos);
      modifier[M_NOCOMPLAIN] = 1;
   }
   FAIL;

done:
   if (NOT success  &&  NOT modifier[M_NOCOMPLAIN])
      mdwrite (XWER, "chg_Enoitem", null.md);

   RETURN  (success);
}
