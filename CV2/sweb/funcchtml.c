
/*** FUNC_CHTML.  Filter text to "clean" HTML.
/
/    func_chtml (result, arg, conf, cmlargs, vars, form, 
/                        incvars, macs, cfile, tg, aref);
/   
/    Parameters:
/       Chix     result;   (append resulting value here)
/       Chix     arg;      (function argument)
/       SWconf   conf;     (server configuration info)
/       char    *cmlargs;  (CML script arguments)
/       Vartab   vars;     (CML variable list)
/       Vartab   form;     (form data variable list)
/       Chix     incvars;  (text of 'include' variables)
/       Vartab   macs;     (defined macros)
/       CML_File cfile;    (original cml file)
/       Target   tg;       (output target)
/       Chix     aref;     (array reference from $array_eval())
/
/    Purpose:
/       Evaluates: $cleanhtml (taglistname text)
/       Output filter: translate TEXT into "clean" HTML, skipping or
/       ignoring tags so labelled in variable TAGLISTNAME.
/
/       This provides complete control over which tags (and their
/       contents) may be displayed in a conference.
/
/    How it works:
/       TAGLISTNAME contains a string of allowed or prohibited tags,
/       in the form:
/           tag1,{allow|prohibit},{all|tag}  tag2,...
/
/       where "allow" vs "prohibit" controls whether the tag is passed
/       on to the user's browser.  If a tag is prohibited, then "all"
/       means everything between <tag> and </tag> is skipped; "tag"
/       means that just the <tag> and </tag> is skipped, but the 
/       text in-between is still passed through.
/
/       In addition, func_chtml does two other things:
/
/       (1) Look for HTML tags that normally require a "</??>" closing tag.
/           If the closing tag does not appear, we add one at the end.
/
/       (2) Translate all "special" Caucus URLs for this host into
/           the appropriate CML page reference using the current
/           swebd subserver.
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/funcchtml.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/29/97 15:49 New function, based on previous func_rhtml(). */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  func_chtml (
   Chix     result, 
   Chix     arg, 
   SWconf   conf,
   char    *cmlargs,
   Vartab   vars,
   Vartab   form,
   Chix     incvars,
   Vartab   macs,
   CML_File cfile,
   Target   tg,
   Chix     aref)
{
   static Chix skipstart = nullchix;
   static Chix skip_end;
   static Chix noclose;
   static Chix ignore;
   static Chix tagname, tag, allow, which;
   static Chix is_prohib, is_all, is_comma, is_space;
   Chix        closetags, temp, empty;
   int4        marker, pos, start, prop;
   int         skiptag, state, ig_tag, num, tagnum;
   int4        next_tag();
   ENTRY ("func_chtml", "");

   if (skipstart == nullchix) {
      skipstart = chxalloc (L(80), THIN, "func_chtml skipstart");
      skip_end  = chxalloc (L(80), THIN, "func_chtml skip_end ");
      noclose   = chxalloc (L(80), THIN, "func_chtml noclose ");
      ignore    = chxalloc (L(80), THIN, "func_chtml ignore ");
      tagname   = chxalloc (L(80), THIN, "func_chtml tagname ");
      tag       = chxalloc (L(80), THIN, "func_chtml tag ");
      allow     = chxalloc (L(80), THIN, "func_chtml allow ");
      which     = chxalloc (L(80), THIN, "func_chtml which ");
      is_prohib = chxalloc (L(20), THIN, "func_chtml is_prohibit ");
      is_all    = chxalloc (L(20), THIN, "func_chtml is_all ");
      is_comma  = chxalloc ( L(8), THIN, "func_chtml is_comma ");
      is_space  = chxalloc ( L(8), THIN, "func_chtml is_space ");
      chxofascii (is_comma,  ",");
      chxofascii (is_space,  " ");
      chxofascii (is_prohib, "prohibit");
      chxofascii (is_all,    "all");
      chxofascii (noclose,
       "area br base basefont bgsound col colgroup embed hr img input "
       "isindex li link meta nextid p sound spacer tr wbr");
   }

   /*** Find where the actual HTML text begins. */
   pos = chxtoken (tagname, nullchix, 2, arg);
   marker = pos;     /* Start of text we *can* copy to RESULT. */
   if (pos < 0)   RETURN(1);

   closetags = chxalloc (L(80), THIN, "func_chtml closetags");
   temp      = chxalloc (L(80), THIN, "func_chtml temp");
   empty     = chxalloc (L(80), THIN, "func_chtml empty");

   state  = -1;     /* 0 => copyable, anything else is a skip index. */

   /*** Get the string of allowed & prohibited tags. */
   chxtoken (tagname, nullchix, 1, arg);
   for (num=0;   num<vars->used;   ++num) {
      if (chxeq (vars->name[num], tagname))  break;
   }
   if  (num < vars->used)  chxcpy   (tagname, vars->value[num]);
   else                    chxclear (tagname);

   /*** Build the tables of skippable and ignorable tags. */
   chxclear (skipstart);
   chxclear (skip_end);
   chxclear (ignore);
   for (tagnum=1;   chxtoken (temp, nullchix, tagnum, tagname) >= 0;
        tagnum++) {
      chxreplace (temp, L(0), is_comma, is_space);
      chxtoken   (tag,   nullchix, 1, temp);
      chxtoken   (allow, nullchix, 2, temp);
      chxtoken   (which, nullchix, 3, temp);
      if (chxeq (allow, is_prohib)) {
         if (chxeq (which, is_all)) {
            chxcat    (skipstart, tag);
            chxcatval (skipstart, THIN, L(' '));
            chxcatval (skip_end,  THIN, L('/'));
            chxcat    (skip_end,  tag);
            chxcatval (skip_end,  THIN, L(' '));
         }
         else {
            chxcat    (ignore, tag);
            chxcatval (ignore, THIN, L(' '));
            chxcatval (ignore, THIN, L('/'));
            chxcat    (ignore, tag);
            chxcatval (ignore, THIN, L(' '));
         }
      }
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
   RETURN  (1);
}
