
/*** NEXT_TAG.   Find next HTML tag in STR.
/
/    more = next_tag (tag, str, pos, conf, cmlargs, 
/                          vars, form, incvars, macs, cfile, tg, aref);
/   
/    Parameters:
/       int      more;     (tag found?)
/       Chix     tag;      (tag found)
/       Chix     str;      (scanning through STR...)
/       int4    *pos;      (starting at this position)
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
/       Scan STR for the next HTML tag.  Return the string and
/       update the scan position POS.
/
/    How it works:
/
/    Returns: -1 if no more tags found.
/             position (in STR) where tag was found.
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  nexttag.c
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


#define   ST_TEXT   0  
#define   ST_TAG    1
#define   ST_COM    2

FUNCTION  int4 next_tag (
   Chix     tag, 
   Chix     str, 
   int4    *pos,
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
   static Chix temp     = nullchix;
   static Chix closetag, dblquote, opentag, closeopen;
   int4   cval, start;
   int    state, type, quote, templen;
   char   tempend[12];
   ENTRY ("next_tag", "");

   if (temp == nullchix)  {
      temp      = chxalloc (L(80), THIN, "next_tag temp");
      closetag  = chxalsub (CQ(  ">"), L(0), L(4));
      opentag   = chxalsub (CQ(  "<"), L(0), L(4));
      dblquote  = chxalsub (CQ("\">"), L(0), L(4));
      closeopen = chxalsub (CQ( "><"), L(0), L(4));
   }

   state = ST_TEXT;
   type  = chxtype (str);
   chxclear (tag);

   do {
      cval = chxvalue (str, *pos);

      if (state == ST_TEXT) {
         if (cval == '<')  {
            state   = ST_TAG;  
            quote   = 0;
            templen = 0;
            chxclear (temp);
            start   = *pos; 
         }
         if (cval == '\\')  {
            ++*pos;
            if (chxvalue (str, *pos) == 0)  RETURN (-1);
         }
      }

      /*** Inside a tag begun with "<"... */
      else if (state == ST_TAG) {

         /*** End of string!  Add a ">" to close it. */
         if (cval == 0) { cval = '>';   chxcat (str, closetag); }

         /*** Beginning of new tag w/o closing old one.  Add a ">". */
         if (cval == '<') {
            chxalter (str, *pos, opentag, closeopen);
            cval = '>';
         }

         /*** Everything inside the tag gets added to TEMP. */
         if (cval != '>') {
            chxcatval (temp, type, cval);
            if (cval == '"')  quote = 1 - quote;
            if (++templen == 3  &&  streq (ascquick(temp), "!--")) {
               state = ST_COM;
               tempend[0] = tempend[1] = tempend[2] = tempend[3] = 0;
            }
         }

         /*** Handle end of tag. */
         else {
            if (quote)  chxalter (str, *pos, closetag, dblquote);
            ++*pos;
            chxtoken  (tag, nullchix, 1, temp);
            jixreduce (tag);
            if (streq (ascquick(tag), "a"))  
               *pos += fix_href (str, start, chxlen(temp), conf, cmlargs, 
                                 vars, form, incvars, macs, cfile, tg, aref);
            RETURN  (start);
         }
      }

      else if (state == ST_COM) {
         if (cval == 0) { chxcat (str, CQ("-->"));  RETURN (-1);}
         else {
            chxcatval (temp, type, cval);
            tempend[0] = tempend[1];
            tempend[1] = tempend[2];
            tempend[2] = cval;
            if (streq (tempend, "-->"))  state = ST_TEXT;
         }
      }

      ++*pos;
   } while (cval != 0) ;

   RETURN  (-1);
}
