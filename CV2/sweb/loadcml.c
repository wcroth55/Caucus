
/*** LOADCML.   Load an entire CML file (and included files) into memory.
/
/    newsize = loadcml (cml_orig, conf, size, cfile, vars, form, 
/                                query, macs, otab, tg, aref, found);
/   
/    Parameters:
/       int        newsize;  (lines used after loading CMLNAME)
/       char      *cml_orig;  (name of CML file)
/       SWconf    *conf;     (server configuration info)
/       int        size;     (current lines used in CFILE)
/       CML_File   cfile;    (put text of cml file here)
/       Vartab     vars;     (list of CML variables)
/       Vartab     form;     (forms data)
/       char      *query;    (?)
/       Vartab     macs;     (defined macros)
/       Obtab      otab;     (object table)
/       Target     tg;       (output target)
/       Chix       aref;     (array reference from $array_eval())
/       int       *found;    (did we really find the file?)
/
/    Purpose:
/       Load an entire CML file (and by recursive calls, any $include()
/       files) into memory in CFILE.
/
/    How it works:
/       Allows directory overriding.  E.g. if cml_orig is "dir/file.cml",
/       loadcml() looks for "dir/file.cml", "dir.1/file.cml", "dir.2/file.cml",
/       etc. up through "dir.9/file.cml".  Highest existing file "wins",
/       and the actual name is stored in a static table for the duration
/       of the process.  Subsequent calls for the same cml_orig get the
/       file named in the static table.
/
/    Returns: new size of CFILE.
/       Sets found to 1, else 0 if file not found.
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  loadcml.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/03/95 15:29 New function. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR  2/05/02 10:45 Add overrideable version feature. */
/*: CR  2/20/02 11:00 Make override feature cache highest level of each dir */
/*: CR  3/01/02 14:00 Add 'found' argument. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  CMAX  250

extern int   cml_debug, cml_errors;

FUNCTION  loadcml (cml_orig, conf, size, cfile, vars, form, 
                   query, macs, otab, tg, aref, found)
   char    *cml_orig;
   SWconf   conf;
   int      size;
   CML_File cfile;
   Vartab   vars, form, macs;
   char    *query;
   Obtab    otab;
   Target   tg;
   Chix     aref;
   int     *found;
{
   static Chix   lparen = nullchix;
   static Chix   rparen;
   static Vartab cmlnametab;
   static Vartab dirtab;
   Chix          cmlname, cmlvalue, word, result;
   int           slot, levelmax, is_dir, junk;
   char          cmlfull[CMAX], cmldir[CMAX], cmlfile[CMAX], cmlsave[CMAX],
                 cmltemp[CMAX];
   char          temp[1000];
   int           lnum, level;
   FILE   *fp;
   ENTRY ("loadcml", "");

   /*** Initialize static elements on very first pass. */
   if (lparen == nullchix) {
      lparen     = chxalsub (CQ("("), L(0), L(1));
      rparen     = chxalsub (CQ(")"), L(0), L(1));
      cmlnametab = a_mak_vartab ("cmlnametab");
      dirtab     = a_mak_vartab ("dirtab");
   }
   cmlname  = chxalloc (L(40), THIN, "loadcml cmlname");
   cmlvalue = chxalloc (L(40), THIN, "loadcml cmlvalue");
   word     = chxalloc (L(50), THIN, "loadcml word");
  *found    = 1;

   /*** Have we found this file before?  (Is it in our cache of file names)? */
   chxofascii (cmlname, cml_orig);
   slot = vartab_is (cmlnametab, cmlname);

   /*** No -- try to find the "overrideable" location of the CML file,
   /    and add it to CMLNAMETAB. */
   if (slot < 0) {
      /*** E.g. try cml_path/dir.2/file,  cml_path/dir.1/file,
      /             cml_path/dir/file, etc. 
      /    Name of "highest" successful result is put into cmlsave. */

      /*** If we can split the original name into a dir and a filename... */
      strcpy (cmlsave, cml_orig);
      if (splitcml (cml_orig, cmldir, cmlfile)) {

         /*** We keep a cache of the cmldir's (should be just a few, 
         /    e.g. 3 or 4) with the level # of the highest dir.N that
         /    exists.  That way we only have to scan a limited number
         /    of directories to find a file.  So start by
         /    finding the dir in our cache of dirs and level numbers.  If it's
         /    not in the cache, find the highest numbered version of that
         /    dir, and put it into the cache. */
         chxofascii (word, cmldir);
         if ( (slot = vartab_is (dirtab, word)) < 0) {
            for (level=9;   level>0;   --level) {
               sprintf (cmltemp, "%s/%s.%d", conf->cml_path, cmldir, level);
               is_dir = sysisdir (cmltemp);
               if (is_dir)  break;
            }
            slot = vartab_add (dirtab, word, word);
            dirtab->code[slot] = level;
         }
         levelmax = dirtab->code[slot];

         for (level=levelmax;  level>0;   --level) {
            sprintf (cmltemp, "%s.%d/%s", cmldir, level, cmlfile);
            sprintf (cmlfull, "%s/%s", conf->cml_path, cmltemp);
            if (syscheck (cmlfull)) {
               strcpy (cmlsave,  cmltemp);
               break;
            }
         }
      }

      /*** OK, found the "highest" version of the file, put it in the
      /    filename cache. */
      chxofascii (cmlvalue, cmlsave);
      slot = vartab_add (cmlnametab, cmlname, cmlvalue);
   }

   /*** Now that we've got the (highest version # of the) file either way,
   /    build the full pathname. */
   sprintf (cmlfull, "%s/%s", conf->cml_path, 
                              ascquick(cmlnametab->value[slot]));

   if ( (fp = fopen (cmlfull, "r")) == NULL) {
      if (cml_debug) {
/*       fprintf (sublog, "Failed to open: %s\n", cmlfull); */
/*       fflush  (sublog); */
      }
/*    if (cml_errors)  fprintf (stderr, "Failed to open: %s\n", cmlfull); */
      if (cfile->lines[size] == (Chix) NULL)
          cfile->lines[size] = chxalloc (L(200), THIN, "process load");
      chxclear (cfile->lines[size]);
      chxfree  (cmlname);
      chxfree  (cmlvalue);
      chxfree  (word);
     *found = 0;
      RETURN   (size);
   }

   result = chxalloc (L(80), THIN, "loadcml result");

   /*** Load the entire CML file into memory. */
   strcpy (cfile->filename, cml_orig);
   for (lnum=0;   (1);   ++size) {
      if (cfile->lines[size] == (Chix) NULL)
          cfile->lines[size] = chxalloc (L(200), THIN, "process load");

      if (size > MAXCML - 5) {
         sprintf (temp, "\"<h2>CML File '%s' too Large!</h2>\n", cmlname);
         chxofascii (cfile->lines[size], temp);
         cfile->lnums[size] = lnum;
         ++size;
         break;
      }

      if (NOT readcml (fp, conf, cfile->lines[size], 0, &lnum))  break;
      cfile->lnums[size] = lnum;

      /*** Test for $include(file)... */
      chxtoken (word, nullchix, 1, cfile->lines[size]);
      ascofchx (temp, word, L(0), L(9));
      if (NOT streq (temp, "$include"))  continue;

      /*** Got one.  Recursively load it. */
      if (cml_debug) {
/*       fprintf (sublog, "\n$include(%s)\n", temp); */
/*       fflush  (sublog); */
      }
      /*** Evaluate any functions inside $include(...) */
      chxclear  (result);
      chxclear  (word);
      func_scan (result, cfile->lines[size], conf, query, vars, form, 
                         word, macs, cfile, otab, tg, aref);
      if (chxinside (result, lparen, word, rparen) < 0)  continue;
      ascofchx (temp, word, L(0), ALLCHARS);
      while (stralter (temp, " ", "")) ;
      size = loadcml (temp, conf, size, cfile, vars, form, 
                      query, macs, otab, tg, aref, &junk);
   }
   fclose  (fp);

   chxfree (word);
   chxfree (result);
   chxfree (cmlname);
   chxfree (cmlvalue);
   cfile->size = size;
   RETURN  (size);
}


/*** Split CMLPATH into directory CMLDIR, filename CMLFILE. */
FUNCTION  int splitcml (char *cmlpath, char *cmldir, char *cmlfile) {
   int    cp;
   char   name[250];

   cmldir[0] = '\0';
   strcpy (cmlfile, cmlpath);
   if (strindex (cmlpath, "/") < 0)  return (0);

   strcpy (name, cmlpath);
   for (cp=strlen(name)-1;   name[cp] != '/';   --cp) ;
   name[cp] = '\0';
   strcpy (cmldir,  name);
   strcpy (cmlfile, name+cp+1);
   return (1);
}
