
/*** FIX_HREF.   "Fix" HREF's to know about special "Caucus" URLs.
/
/    fix_href (str, start, len, conf, cmlargs, vars, form, 
/                   incvars, macs, cfile, tg, aref);
/   
/    Parameters:
/       Chix     str;      (String containing HREF)
/       int4     start;    (Position HREF starts at???)
/       int4     len;      (max length of <A>...</A> tag)
/       SWconf   conf;     (server configuration info)
/       char     *cmlargs; (CML script arguments)
/       Vartab   vars;     (CML variable list)
/       Vartab   form;     (form data variable list)
/       Chix     incvars;  (text of 'include' variables)
/       Vartab   macs;     (defined macros)
/       CML_File cfile;    (original cml file)
/       Target   tg;       (output target)
/       Chix     aref;     (array reference from $array_eval())
/
/    Purpose:
/       Fix HREF strings so that they know about special Caucus URLs,
/       such as http:/caucus/confname/item etc.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  fixhref.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/22/97 15:15 New function. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  int fix_href (
   Chix    str,
   int4    start,
   int4    len,
   SWconf  conf, 
   char   *cmlargs,
   Vartab  vars,
   Vartab  form,
   Chix    incvars,
   Vartab  macs,
   CML_File cfile,
   Target   tg,
   Chix     aref)
{
   static Chix extract = nullchix;
   static Chix href, quote, newurl;
   int4        href_pos, quote_pos;
   int         success, ok;
   ENTRY ("fix_href", "");

   if (extract == nullchix) {
      extract = chxalloc (L(100), THIN, "fix_href extract");
      newurl  = chxalloc (L(100), THIN, "fix_href newurl");
      href    = chxalsub (CQ("href="), L(0), L(10));
      quote   = chxalsub (CQ("\""),    L(0), L(10));
   }

   /*** Pluck out the anchor string, and see if it has an HREF="..." field. */
   chxclear  (extract);
   chxcatsub (extract, str, start, len+2);
   jixreduce (extract);
   href_pos  = chxindex (extract, L(0), href);
   if (href_pos  < 0)   RETURN (0);

   /*** Find the closing quote of the HREF="..." field. */
   quote_pos = chxindex (extract, href_pos+6, quote);
   if (quote_pos < 0)   RETURN (0);

   /*** Extract the URL, and if it is 'special', put the translated
   /    form into NEWURL. */
   chxclear  (newurl);
   chxclear  (extract);
   chxcatsub (extract, str, start+href_pos+6, quote_pos - href_pos - 6);
   ok = caucus_url (newurl, extract, conf, cmlargs, 
                            vars, form, incvars, macs, cfile, tg, aref);

   success = 0;
   if (ok == 2) {
      chxalter (str, start+href_pos+6, extract, newurl);
      success = chxlen(newurl) - chxlen(extract);
   }

   RETURN (success);
}
