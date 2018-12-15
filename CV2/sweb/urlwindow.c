
/*** URL_WINDOW.  Should a given URL get a new window?
/
/    yes = url_window (url, conf);
/   
/    Parameters:
/       Chix     url;     (URL in question)
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
/       Detect <A HREF...> links, and add a target to them if there isn't one.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funchaddtar.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/09/04 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  int url_window (
   Chix     url,
   SWconf   conf)
{
   static Chix question = nullchix;
   static Chix hash, htmllist, turl, word;
   int    ui, replace, slash, html, wc, qmark, hmark, stopurl;
   ENTRY ("url_window", "");

   if (question == nullchix) {
      question    = chxalsub (CQ("?"),            L(0), L( 4));
      hash        = chxalsub (CQ("#"),            L(0), L( 4));
      turl        = chxalloc ( L(80),  THIN, "url_window turl");
      word        = chxalloc ( L(80),  THIN, "url_window word");
      htmllist    = chxalloc ( L(80),  THIN, "url_window htmllist");
      chxcpy (htmllist, CQ(conf->new_win_for));
   }


   /*** Attempt to determine if this likely to be an HTML file.
   /    If so, we'll want to open a new window, and otherwise not.
   /    Heuristics to determine if a URL is an HTML file are:
   /    (after removing ?... or #... from a URL)
   /    (a) last slash is last char
   /    (b) last slash is part of "http://"
   /    (c) .htm, .html, or .shtml follows last slash
   /    (d) otherwise, no "." after last slash.  */
   html    = 0;
   stopurl = 1000000;
   qmark   = chxindex (url, L(0), question);
   hmark   = chxindex (url, L(0), hash);
   if (qmark > 0)                      stopurl = qmark;
   if (hmark > 0  &&  hmark < stopurl) stopurl = hmark;
   chxclear  (turl);
   chxcatsub (turl, url, L(0), stopurl);
   jixreduce (turl);
   slash = chxrevdex (turl, L(2000), CQ("/"));

   /*** Any URL that ends in a "/" is assumed to point to an
   /    HTML file (and thus deserves a new window). */
   if      (slash == chxlen (turl) - 1)                       html = 1;
   else if (slash > 0  &&  chxvalue (turl, slash-1) == '/')   html = 1;

   /*** Any URL that ends in an extension in the swebd.conf
   /    parameter NEW_WIN_FOR gets a new window. */
   else for (wc=1;   chxtoken (word, nullchix, wc, htmllist) >= 0;
             wc++) {
      if (chxrevdex (turl, L(2000), word) > slash) { html = 1;  break;}
   }

   if (NOT html  &&  chxrevdex (turl, L(2000), CQ(".")) < slash) html = 1;
   return (html);
}
