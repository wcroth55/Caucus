
/*** CAUCUS_URL.   Special handling of "http:/caucus" URLs.
/
/    ok = caucus_url (result, url, conf, cmlargs, vars, form, 
/                             incvars, macs, cfile, tg, aref);
/   
/    Parameters:
/       int       ok       (success?)
/       Chix      result   (resulting real URL)
/       Chix      url;     (original "special" URL)
/       SWconf    conf;    (server configuration info)
/       char     *cmlargs; (CML script arguments)
/       Vartab    vars;    (CML variable list)
/       Vartab    form;    (form data variable list)
/       Chix      incvars; (text of 'include' variables)
/       Vartab    macs;    (macros defined)
/       CML_File  cfile;   (original cml file)
/       Target    tg;      (output target)
/       Chix      aref;    (array reference from $array_eval())
/
/    Purpose:
/       Translate special "caucus" URLs into an appropriate CML file 
/       reference, using the current swebd subserver.
/
/       Handles the generic special URL form:
/         [http:] [//hostname] /caucus [/conference [/item [/response] ] ]
/
/    How it works:
/
/    Returns: 2 on success (special URL)
/             1 if valid URL (but not a special URL)
/             0 if invalid, should not be a URL at all
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  caucusurl.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/24/96 12:14 New function. */
/*: CR  1/22/97 15:15 Expand to cover all forms of special URL. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR  8/14/10 14:09 Handle items with breakouts (e.g. http://host/caucus/confName/1.1.1/15) */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  PN   5

FUNCTION  int caucus_url (
   Chix       result,
   Chix       url, 
   SWconf     conf, 
   char      *cmlargs,
   Vartab     vars,
   Vartab     form,
   Chix       incvars,
   Vartab     macs,
   CML_File   cfile,
   Target     tg,
   Chix       aref)
{
   static Chix  slash = nullchix;
   static Chix  blank, temp, word, colon, respWord;
   static Obtab otab;
   Obtab        make_obtab();
   int4   pos, confnum, item, resp;
   int    success, pword;
   char   work[300], ascword[200];
   ENTRY ("caucus_url", "");

   /*** Initialization. */
   if (slash == nullchix) {
      slash  =  chxalsub (CQ("/"),            L(0), L(20));
      colon  =  chxalsub (CQ(":"),            L(0), L(20));
      blank  =  chxalsub (CQ(" "),            L(0), L(20));
      temp   =  chxalloc ( L(80), THIN, "caucus_url temp");
      word   =  chxalloc ( L(80), THIN, "caucus_url word");
      respWord = chxalloc (L(20), THIN, "caucus_url respWord");
      otab   =  make_obtab(5);
   }


   /*** Replace all /'s and :'s with spaces to make parsing easy. */
   chxcpy     (temp, url);
   chxreplace (temp, L(0), slash, blank);
   chxreplace (temp, L(0), colon, blank);
   success = 2;
   pword   = 1;
   chxtoken  (word, nullchix, pword, temp);
   jixreduce (word);

   /*** Skip "http:" part. */
   if (streq (ascquick(word), "http")) 
      chxtoken  (word, nullchix, ++pword, temp);

   /*** If "//hostname" part is there, make sure it matches current hostname! */
   strcpy (ascword, ascquick(word));
   if (NOT streq (ascword, "caucus")) {
      if (NOT streq (ascword, conf->hostname))   RETURN (1);
      chxtoken  (word, nullchix, ++pword, temp);
   }
      
   /*** Insist on "/caucus" part. */
   if (NOT streq (ascquick(word), "caucus"))     RETURN (1);

   /*** If no conf name beyond "http:/caucus", go to Caucus Center. */
   if (chxtoken (word, nullchix, pword+1, temp) < 0)
/*    strcpy (work, "$(href)/center.cml?$(nch)+$(nxt)+x+x+x+x+x+x"); */
      strcpy (work, "%_caucus_url()");

   /*** Everything else has at least "http:/caucus/conference_name". 
   /    Determine the conference number. */
   else {
      chxclear (result);
      func_cl  (result, "cl_list", word, conf);
      pos = L(0);
      confnum = chxint4 (result, &pos);

      /*** Must have valid conf num, and be able to join it. */
      if (confnum <= 0  ||  conf_access (confnum) < 1)   success = 0;

      /*** Just conference name, goes to confhome.cml. */
      if (chxtoken (word, nullchix, pword+2, temp) < 0)
         sprintf (work, 
/*            "$(href)/confhome.cml?$(nch)+$(nxt)+%d+x+x+x+x+x", confnum); */
              "%%_caucus_url(%d)", confnum);

      /*** Everything else has "http:/caucus/conf_name/item[/resp]". */
      else {
         pos  = L(0);
         item = chxint4 (word, &pos);
         if (item == 0)  success = 0;
         chxtoken (respWord, nullchix, pword+3, temp);
         pos  = L(0);
         resp = chxint4 (respWord, &pos);
         sprintf (work,
/*          "$(href)/viewitem.cml?$(nch)+$(nxt)+%d+%d+%d+x+x+x#here", */
/*           confnum, item, resp); */
            "%%_caucus_url(%d %s %d)", confnum, ascquick(word), resp);
      }
   }

   /*** Run the result through func_scan() to evaluate all the variable
   /    references. */
   chxcpy (temp, CQ(work));
   chxclear  (result);
   func_scan (result, temp, conf, cmlargs, vars, form, incvars, 
                            macs, cfile, otab, tg, aref);

   RETURN(success);
}
