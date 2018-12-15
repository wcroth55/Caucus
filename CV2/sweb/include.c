
/*** INCLUDE_CML.   Handle "include" directive to include another file.
/
/    include_cml (tg, cmlname, vars, form, conf, query, broke, incvars, 
/                     macs, cfile, otab, contd, aref);
/   
/    Parameters:
/       int       cd;      (connection descriptor)
/       Target    tg;      (output target)
/       Chix      cmlname; (name of file to include)
/       Vartab    vars;    (CML variable list)
/       Vartab    form;    (form data variable list)
/       SWconf    conf;    (server configuration info)
/       char     *query;   (CML arguments from client)
/       int      *broke;   (was a 'break' encountered?)
/       Chix      incvars; (rest of include line)
/       Vartab    macs;    (defined macros)
/       CML_File  cfile;   (original cml file)
/       Obtab     otab;    (object table)
/       int      *contd;   (continued line?)
/       Chix      aref;    (array reference from $array_eval())
/
/    Purpose:
/       Handle CML "include" directive.  This (recursively) includes
/       another CML file in the one currently being processed.
/
/    How it works:
/
/    Returns: number of last line (in included file) parsed.
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  include.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/05/95 14:02 New function. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR 10/30/01 11:36 Allow protected functions to work underneath includecml */
/*: CR  3/01/02 14:00 Add 'found' argument loadcml(). */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include <sys/types.h>
#include "api.h"
#define  MAXINC 100

extern FILE *sublog;

FUNCTION  include_cml (tg, cmlname, vars, form, conf, query, broke, 
                           incvars, macs, cfile, otab, contd, aref)
   Target   tg;
   Chix     cmlname, incvars;
   Vartab   vars, form, macs;
   SWconf   conf;
   char    *query;
   int     *broke, *contd;
   CML_File cfile;
   Obtab    otab;
   Chix     aref;
{
   static   int      ic = 0;
   static   CML_File icache[MAXINC];
   CML_File newfile, make_cml();
   char     filename[256];
   int      num, size, lnum, i, found;
   ENTRY ("include_cml", "");

   ascofchx (filename, cmlname, L(0), 255);

   /*** Protect functions should work "underneath" $includecml even if
   /    $protect() was called "above". */
   prot_push (0);

   if (conf->cache_include) {

      /*** Is this include file already in the cache? */
      for (i=0;   i<ic;   ++i) {
         if (streq (filename, icache[i]->filename)) {
            newfile = icache[i];
            break;
         }
      }

      /*** No.  Load it, and put it in the cache (if room). */
      if (i >= ic) {
         newfile = make_cml(cfile->errtext);
         size = loadcml (filename, conf, 0, newfile, vars, form, 
                         query, macs, otab, tg, aref, &found);
         if (i < MAXINC) icache[ic++] = newfile;
      }

      /*** Parse the entire CML file. */
      if (newfile->size > 0) {
         lnum = parse_cml (tg, newfile, newfile->size, 0, conf, 
                  vars, form, query, broke, incvars, macs, otab, contd, aref);
      }

      if (i >= MAXINC)  free_cml (newfile);
   }

   else {
      newfile = make_cml(cfile->errtext);

      /*** Load the entire included CML file into memory. */
      size = loadcml (filename, conf, 0, newfile, vars, form, 
                      query, macs, otab, tg, aref, &found);

      if (size > 0) {
         /*** Parse the entire CML file. */
         lnum = parse_cml (tg, newfile, size, 0, conf, vars, form, 
                           query, broke, incvars, macs, otab, contd, aref);
      }
      else if (NOT found) {
         char temp[512];
         sprintf (temp, "\n%s:%d Can't open include file '%s'\n",
                         cfile->filename, cfile->lnums[cfile->cdex],
                         filename);
         chxcat  (cfile->errtext, CQ(temp));
      }

      /*** Free the temporary structure... */
      free_cml (newfile);
   }

   prot_pop();

   RETURN (lnum);
}
