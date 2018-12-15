
/*** FUNC_RHTML.  Filter text to reduced HTML.
/
/    func_rhtml (result, arg, use_prop, conf, cmlargs, vars, form, 
/                        incvars, macs, cfile, tg, aref);
/   
/    Parameters:
/       Chix    result;   (append resulting value here)
/       Chix    arg;      (function argument)
/       int     use_prop; (if true, use PROP in $safehtml().)
/       SWconf  conf;     (server configuration info)
/       char   *cmlargs;  (CML script arguments)
/       Vartab  vars;     (CML variable list)
/       Vartab  form;     (form data variable list)
/       Chix    incvars;  (text of 'include' variables)
/       Vartab  macs;     (defined macros)
/       CML_File cfile;   (original cml file)
/       Target   tg;      (output target)
/       Chix     aref;    (array reference from $array_eval())
/
/    Purpose:
/       Output filter: translate text into "reduced" HTML.
/       Evaluates:
/         $rhtml   (     text)
/         $safehtml(prop text)
/
/    How it works:
/       Reduced or "safe" HTML is a subset of HTML.  The notion is that we 
/       want users to be able to enter HTML fragments for their responses, 
/       etc., but don't want them to screw up the rest of the containing HTML
/       page.
/
/       PROP controls specific properties of the "safing" of HTML.
/       PROP & 0x01 means "allow" <FORM>s.
/       PROP & 0x02 means "allow" <SCRIPT>s
/       PROP & 0x04 means "allow" <APPLET>s
/
/       This means we do four things:
/
/       (1) Anything inside
/             <head>   ... </head>
/             <title>  ... </title>
/             <form>   ... </form>    (depending on PROP)
/             <script> ... </script>  (depending on PROP)
/             <applet> ... </applet>  (depending on PROP)
/           is removed entirely
/
/       (2) Any of the tags:
/             <html>   </html>
/             <body>   </body>
/           are simply ignored (removed).
/
/       (3) Look for HTML tags that normally require a "</??>" closing tag.
/           If the closing tag does not appear, we add one at the end.
/
/       (4) Translate all "special" Caucus URLs for this host into
/           the appropriate CML page reference using the current
/           swebd subserver.
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funcrhtml.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/19/95 16:28 New function. */
/*: CR  6/13/97 10:45 Add <SCRIPT>, <APPLET> removal. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  PR_FORM   1
#define  PR_SCRIPT 2
#define  PR_APPLET 4

FUNCTION  func_rhtml (
   Chix    result, 
   Chix    arg, 
   int     use_prop,
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
   static Chix skipstart = nullchix;
   static Chix skip_end;
   static Chix noclose;
   static Chix ignore;
   Chix        closetags, temp, empty, tag;
   int4        marker, pos, start, prop;
   int         skiptag, state, ig_tag;
   int4        next_tag();
   ENTRY ("func_rhtml", "");

   if (skipstart == nullchix) {
      skipstart = chxalloc (L(80), THIN, "func_rhtml skipstart");
      skip_end  = chxalloc (L(80), THIN, "func_rhtml skip_end ");
      noclose   = chxalloc (L(80), THIN, "func_rhtml noclose ");
      ignore    = chxalloc (L(80), THIN, "func_rhtml ignore ");
      chxofascii (ignore,    "html /html body /body");
      chxofascii (noclose,
       "area br base basefont bgsound col colgroup embed hr img input "
       "isindex li link meta nextid p sound spacer tr wbr");
   }
   closetags = chxalloc (L(80), THIN, "func_rhtml closetags");
   temp      = chxalloc (L(80), THIN, "func_rhtml temp");
   empty     = chxalloc (L(80), THIN, "func_rhtml empty");
   tag       = chxalloc (L(80), THIN, "func_rhtml tag");

   state  = -1;     /* 0 => copyable, anything else is a skip index. */

   /*** Handle the text PROPerties. */
   pos  = L(0);
   prop = 0;
   if (use_prop)  prop = chxint4 (arg, &pos);
   marker = pos;     /* Start of text we *can* copy to RESULT. */

   chxofascii (skipstart, " head  title");
   chxofascii (skip_end,  "/head /title");
   if (NOT (prop & PR_FORM)) {
      chxcat  (skipstart, CQ("  form"));
      chxcat  (skip_end,  CQ(" /form"));
   }
   if (NOT (prop & PR_SCRIPT)) {
      chxcat  (skipstart, CQ("  script"));
      chxcat  (skip_end,  CQ(" /script"));
   }
   if (NOT (prop & PR_APPLET)) {
      chxcat  (skipstart, CQ("  applet"));
      chxcat  (skip_end,  CQ(" /applet"));
   }
    

   /*** Scan and filter the text. */
   while ( (start = next_tag (tag, arg, &pos, conf, cmlargs, 
                        vars, form, incvars, macs, cfile, tg, aref)) >= 0) {

      /*** If we're not skipping anything (yet?)... */
      if (state == -1) {

         /*** Did we find an ignorable <tag> ? */
         ig_tag  = tablefind (ignore, tag);
         if (ig_tag >= 0) {
            chxcatsub (result, arg, marker, start-marker);
/*          marker = pos + 1; */
            marker = pos;
            continue;
         }

         /*** Did we find a "skippable" <tag> ? */
         skiptag = tablefind (skipstart, tag);
         if (skiptag >= 0) {
            state = skiptag;
            chxcatsub (result, arg, marker, start-marker);
            continue;
         }
 
         /*** No, did we find a closing tag?  If so, remove it from CLOSETAGS.*/
         else if (chxvalue (tag, L(0)) == '/') {
            chxclear  (temp);
            chxformat (temp, CQ("<%s> "), L(0), L(0), tag, nullchix);
            chxalter  (closetags, L(0), temp, empty);
         }
 
         /*** If we found an opening tag, add its closer to CLOSETAGS. */
         else if (tablefind (noclose, tag) < 0) {
            chxformat (closetags, CQ("</%s> "), L(0), L(0), tag, nullchix);
         }
      }

      /*** Here if we've been skipping over stuff... */
      else {
         if (state == tablefind (skip_end, tag)) {
            state  = -1;
            marker = pos + 1;
         }
      }
   }

   if (state == -1)  chxcatsub (result, arg, marker, ALLCHARS);
   chxcat (result, closetags);

   chxfree (closetags);
   chxfree (temp);
   chxfree (empty);
   chxfree (tag);
   RETURN  (1);
}
