
/*** FUNC_H2URL.  Detect URL's in HTML text and make them active links.
/
/    func_h2url (arg, conf, cmlargs, vars, form, incvars, macs, cfile, 
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
/       Output filter: detect URL's in HTML text, and make them active links.
/       Do not mess up existing, proper HTML links.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funch2url.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/08/04 New function. */
/*: CR 01/24/05 Force end of URL at &lt; or &gt; */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  PN   6
#define  NEW_WINDOW  1    /* Should hyperlinks open a new window? */

FUNCTION  func_h2url (
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
   static Chix cctp, result, url, newurl, word, final,
               upper, nbsp, amplt, ampgt;
   int4   cval, start, startpos, cv, scan, confnum, pos, length, space;
   char   work[200], target[200];
   char  *map, *sysmem();
   int    ui, replace, slash, html, wc, qmark, hmark, stopurl;
   int    c1, c2, c3, len, state;
   ENTRY ("func_h2url", "");

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
      nbsp        = chxalsub (CQ("&nbsp;"),       L(0), L(10));
      result      = chxalloc (L(100),  THIN, "func_h2url result");
      newurl      = chxalloc (L(100),  THIN, "func_h2url newurl");
      url         = chxalloc ( L(80),  THIN, "func_h2url url");
      word        = chxalloc ( L(80),  THIN, "func_h2url word");
      final       = chxalloc (L(1000), THIN, "func_h2url final");
      upper       = chxalloc (L(1000), THIN, "func_h2url upper");
   }

   /*** Section I.  Build a 'map' of which chars in 'arg' could
   /    possibly contain a URL that should be make into a hot link.
   /    0 => could be a link, 1 => must not be made into a link. */

#define ST_TEXT 0
#define ST_HTML 1
#define ST_Q1   2
#define ST_Q2   3
   
   chxcpy   (upper, arg);
   chxupper (upper);
   len = chxlen (upper);
   map = sysmem (len, "func2url-len");  

   /*** Find everything between < and > and mark them as '1' in the map. */
   state = ST_TEXT;
   for (scan=0;   scan < len;   ++scan) {
      c1 = chxvalue (upper, scan);

      if (state == ST_TEXT) {
         if (c1 == '<')    state = ST_HTML;
      }

      else if (state == ST_HTML) {
         map[scan] = 1;
         if (c1 == '"' )   state = ST_Q2;
         if (c1 == '\'')   state = ST_Q1;
         if (c1 == '>' )   state = ST_TEXT;
      }

      else if (state == ST_Q1) {
         c2 = chxvalue (upper, scan+1);
         map[scan] = 1;
         if      (c1 == '\\'  &&  c2 == '\'')  map[++scan] = 1;
         else if (c1 == '\'')                  state = ST_HTML;
      }

      else if (state == ST_Q2) {
         c2 = chxvalue (upper, scan+1);
         map[scan] = 1;
         if      (c1 == '\\'  &&  c2 == '"')   map[++scan] = 1;
         else if (c1 == '"')                   state = ST_HTML;
      }
   }

   /*** Now find everything between "<A" and "</A", and mark them as '1'. */
   state = ST_TEXT;
   for (scan=0;   scan < len;   ++scan) {
      c1 = chxvalue (upper, scan);
      c2 = chxvalue (upper, scan+1);
      c3 = chxvalue (upper, scan+2);

      if (state == ST_TEXT) {
         if (c1 == '<'  &&  c2 == 'A')    state = ST_HTML;
      }

      else if (state == ST_HTML) {
         map[scan] = 1;
         if (c1 == '"' )   state = ST_Q2;
         if (c1 == '\'')   state = ST_Q1;
         if (c1 == '<'  &&  c2 == '/'  &&  c3 == 'A')   state = ST_TEXT;
      }

      else if (state == ST_Q1) {
         map[scan] = 1;
         if      (c1 == '\\'  &&  c2 == '\'')  map[++scan] = 1;
         else if (c1 == '\'')                  state = ST_HTML;
      }

      else if (state == ST_Q2) {
         map[scan] = 1;
         if      (c1 == '\\'  &&  c2 == '"')   map[++scan] = 1;
         else if (c1 == '"')                   state = ST_HTML;
      }
   }

#if 1

   /*** Now, look for URL's, so we can automatically convert them. 
   /    We process chunks of non-url-bearing and url-bearing text
   /    separately, then append each to FINAL. */
   chxclear (final);
   for (scan=0L;   (1);   ) {

      /*** Find start of next URL (if any). */
      startpos = len;
      for (ui=0;   ui<PN;   ++ui) {
         start = chxindex (arg, scan, protocol[ui]);
         if (start >= 0  &&  start < startpos)  startpos = start;
      }

      /*** Skip over URLs that may not be made a hot link, based on the map. */
      if (startpos < len  &&  map[startpos])  { 
         chxcatsub (final, arg, scan, 5);
         scan += 5;   
         continue;
      }

      /*** Extract non-url-bearing text from SCAN up to START of this
      /    url (if any), and append to FINAL.*/
      length = (startpos == len ? ALLCHARS : startpos - scan);
      chxcatsub  (final, arg, scan, length);

      if (startpos == len)  break;

      /*** Find forced end of URL, then backup over any "trim" characters. 
      /    Watch out for &nbsp; at the end of a URL, too. */
      for (cv=startpos;   cval = chxvalue (arg, cv);   ++cv) {
         if (cval >= 256  ||  conf->url_end[cval]  ||
             chxindex (arg, cv, amplt) == cv       ||
             chxindex (arg, cv, ampgt) == cv )          break;
      }
      space = chxindex (arg, startpos, nbsp);
      if (space >= startpos  &&  space < cv)  cv = space;
      while ( (cval = chxvalue(arg, cv-1)), conf->url_trim[cval])  --cv;

      /*** Pluck out URL, transform into HTML version, making name a link. */
      chxclear  (url);
      chxcatsub (url, arg, startpos, cv - startpos);
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
               sprintf   (target,
                  " target=\"fromcau\" onClick=\"%%_target_fromcau (%d %d)\"", 
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
      else scan = startpos + chxlen (url);
   }
   chxcpy (arg, final);
#endif

#if 0
   chxclear (arg);
   for (scan=0;   scan < len;   ++scan) 
      chxcatval (arg, THIN, (map[scan] ? L('.') : L(' ')));
#endif

   sysfree (map);

   RETURN (1);
}
