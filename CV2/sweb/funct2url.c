
/*** FUNC_T2URL.  Detect URL's and turn them into appropriate HTML.
/
/    func_t2url (arg, conf, cmlargs, vars, form, incvars, macs, cfile, 
                 tg, aref);
/   
/    Parameters:
/       Chix     arg;     (function argument)
/       SWconf   conf;    (server configuration info)
/       char    *cmlargs; (CML script arguments)
/       Vartab   vars;    (CML variable list)
/       Vartab   form;    (form data variable list)
/       Chix     incvars; (text of 'include' variables)
/       Vartab   macs;    (defined macros)
/       CML_File cfile;   (original cml file)
/       Target   tg;      (output target)
/       Chix     aref;    (array reference from $array_eval())
/
/    Purpose:
/       Output filter: detect URL's in text, and turn them into HTML.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funct2url.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/30/95 17:28 New function. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR  4/30/02 Bring call to funct2hbr inside here on non-url chunks. */
/*: CR 12/22/03 When examining URL for file extensions, stop at ? or # part */
/*: CR 11/09/04 Fix bug about detecting *all* URL protocols. */
/*: CR 01/24/05 Force end of URL at &lt; or &gt; */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  PN   6
#define  NEW_WINDOW  1    /* Should hyperlinks open a new window? */

FUNCTION  func_t2url (
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
   static Chix protocol [PN] = 
      {nullchix, nullchix, nullchix, nullchix, nullchix, nullchix};
   static Chix cctp, result, url, newurl, word, final, ampgt, amplt;
   int4   cval, start, cv, scan, confnum, pos, length, startpos, len;
   char   work[200], target[200];
   int    ui, replace, slash, html, wc, qmark, hmark, stopurl;
   ENTRY ("func_t2url", "");

   if (protocol[0] == nullchix) {
      protocol[0] = chxalsub (CQ("http:/"),       L(0), L(10));
      protocol[1] = chxalsub (CQ("gopher:/"),     L(0), L(10));
      protocol[2] = chxalsub (CQ("telnet:/"),     L(0), L(10));
      protocol[3] = chxalsub (CQ("ftp:/"),        L(0), L(10));
      protocol[4] = chxalsub (CQ("mailto:"),      L(0), L(10));
      protocol[5] = chxalsub (CQ("https:/"),      L(0), L(10));
      cctp        = chxalsub (CQ("http:/caucus"), L(0), L(20));
      ampgt       = chxalsub (CQ("&gt;"),         L(0), L(20));
      amplt       = chxalsub (CQ("&lt;"),         L(0), L(20));
      result      = chxalloc (L(100),  THIN, "func_t2url result");
      newurl      = chxalloc (L(100),  THIN, "func_t2url newurl");
      url         = chxalloc ( L(80),  THIN, "func_t2url url");
      word        = chxalloc ( L(80),  THIN, "func_t2url word");
      final       = chxalloc (L(1000), THIN, "func_t2url final");
   }


   /*** Now, look for URL's, so we can automatically convert them. 
   /    We process chunks of non-url-bearing and url-bearing text
   /    separately, then append each to FINAL. */
   chxclear (final);
   for (scan=0L;   (1);   ) {

      /*** Find start of next URL (if any). */

      /*** Old code... which erroneously picks a "later" URL if it's
      /    earlier in the protocol array.
      for (ui=0;   ui<PN;   ++ui)
         if ( (start = chxindex (arg, scan, protocol[ui])) >= 0)    break;
      / */

      startpos = len = chxlen(arg);
      for (ui=0;   ui<PN;   ++ui) {
         start = chxindex (arg, scan, protocol[ui]);
         if (start >= 0  &&  start < startpos)  startpos = start;
      }
      start = (startpos == len ? -1 : startpos);

      /*** Extract non-url-bearing text from SCAN up to START of this
      /    url (if any), convert HTML special characters, and append to FINAL.*/
      length = (start < 0 ? ALLCHARS : start - scan);
      chxclear   (word);
      chxcatsub  (word, arg, scan, length);
      func_t2hbr (final, word);

      if (start < 0)  break;

      /*** Find forced end of URL, then backup over any "trim" characters. */
      for (cv=start;   cval = chxvalue (arg, cv);   ++cv) {
         if (cval >= 256  ||  conf->url_end[cval]  ||
             chxindex (arg, cv, amplt) == cv       ||
             chxindex (arg, cv, ampgt) == cv )  break;
      }
      while ( (cval = chxvalue(arg, cv-1)), conf->url_trim[cval])  --cv;

      /*** Pluck out URL, transform into HTML version, making name a link. */
      chxclear  (url);
      chxcatsub (url, arg, start, cv - start);
      replace = 1;

      /*** "http:/caucus" strings get special handling. */
      if (chxindex  (url, L(0), protocol[0]) == 0)
         replace = caucus_url (result, url, conf, cmlargs, 
                                   vars, form, incvars, macs, cfile, tg, aref);

      /*** Regular URLs. */
      if (replace == 1)  chxcpy (result, url);

      if (replace > 0) {
         chxcpy (newurl, CQ("<A HREF=\""));
         chxcat (newurl, result);
         chxcat (newurl, CQ("\""));
#if NEW_WINDOW
         if (replace == 1) {

            html = url_window (url, conf);
            if (html) {
               /*** Old code explicitly used javascript to open a window. 
               sprintf (target, "%swidth=%d,height=%d%s",
                  " onClick=\"win = window.open ('', 'fromcau',"
                  " 'resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,",
                  conf->new_win_x, conf->new_win_y,
                  "');\"  TARGET=\"fromcau\" ");
               chxcat (newurl, CQ(target));
               */

               /*** New code calls a macro, which could do anything. */
               sprintf (target, " %%_new_target_window (%d %d)", 
                        conf->new_win_x, conf->new_win_y);
               chxcat  (newurl, CQ(target));
            }
         }
#endif

         /*** Append actual <A HREF etc> text to FINAL. */
         chxcat (final, newurl);

         /*** Now append the link text, which must undergo conversion
         /    for any special HTML chars. */
         chxcat     (final, CQ(">"));
         func_t2hbr (final, url);
         chxcat     (final, CQ("</a>"));

         /*** Resume scanning for URLs right after the end of the old url. */
         scan = cv;
      }
      else scan = start + chxlen (url);
   }
   chxcpy (arg, final);

   RETURN (1);
}
