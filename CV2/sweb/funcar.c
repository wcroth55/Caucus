
/*** FUNC_AR.   Return the resulting value of a array_xxx() function.
/
/    func_ar (result, what, arg, 
/             conf, cmlargs, vars, form, protect, 
/             incvars, macs, cfile, otab, tg, aref);
/   
/    Parameters:
/       Chix     result;  (append resulting value here)
/       char    *what;    (function name)
/       Chix     arg;     (function argument)
/       SWconf   conf;    (server configuration info)
/       char    *cmlargs; (CML script arguments)
/       Vartab   vars;    (CML variable list)
/       Vartab   form;    (form data variable list)
/       int      protect; (only allow "safe" functions)
/       Chix     incvars; (text of 'include' variables)
/       Vartab   macs;    (defined macros)
/       CML_File cfile;   (original cml file)
/       Obtab    otab;    (object table)
/       Target   tg;      (output target)
/       Chix     aref;    (array reference from $array_eval())
/
/    Purpose:
/       Append value of array_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/
/    Returns: 1 on successful evaluation
/             0 on error
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funcar.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/09/01 17:55 New function. */
/*: CR  7/14/01 12:00 array_recurs: func_value -> hold, append to result. */
/*: CR  3/12/04 $array() (empty args) returns empty str, no errors;
/*              array_load() must not crash on empty array name!          */
/*: CR 12/22/04 new maxdim arg for array_args(), to prevent overrun.  */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"
#include "unitcode.h"

extern FILE *sublog;
extern int   cml_debug;

#define AR_ROWS  50
#define AR_NAME 100
#define AR_DIMS  10

typedef struct array_t * Array;
struct array_t {
   char   name[AR_NAME];
   int    dims;
   int    d[AR_DIMS];
   int    ncell;
   int    changed;
   Chix  *cell;
   long   used;
};

static Array  ars[AR_ROWS];

FUNCTION  func_ar (Chix result, char *what, Chix arg, 
                   SWconf conf, char *cmlargs,
                   Vartab vars, Vartab form,
                   int protect, Chix incvars, Vartab macs, CML_File cfile,
                   Obtab otab, Target tg, Chix aref)
{
   static int  first_call = 1;
   static Chix word1, word2, word3;
   char        name[AR_NAME];
   char        temp[100];
   int         i, oldest, ai, di, dinsert, iinsert, dremove, iremove, dincr;
   int         d[20], v[20], dims, exists, coords[20], coends[20];
   int4        pos;
   long        x, y;
   Array       ap;
   Array       array_make(), array_load();
   
   ENTRY ("func_ar", "");

   if (first_call) {
      for (i=0;   i<AR_ROWS;   ++i)  ars[i] = NULL;
      word1 = chxalloc (L(30), THIN, "func_ar word1");
      first_call = 0;
   }

   if (streq (what, "array_size")) {
      array_args (arg, name, d, &dims, AR_DIMS);

      ai = array_in_mem (ars, name);
      if (ai < 0)
         { chxcatval (result, THIN, L('0'));  RETURN(0); }

      ars[ai]->used = time(NULL);
      if (d[0] < 0)  sprintf (temp, "%d", ars[ai]->dims);
      else           sprintf (temp, "%d", ars[ai]->d[d[0]-1]);
      chxcat (result, CQ(temp));
   }

   else if (streq (what, "array_set")) {
      array_args (arg, name, d, &dims, AR_DIMS);

      ai = array_in_mem (ars, name);
      if (ai < 0)
         { array_err (cfile, name, "no array");         RETURN (0); }
      if (ars[ai]->dims > dims)  
         { array_err (cfile, name, "bad dimensions");   RETURN (0); }

      chxtoken (nullchix, word1, ars[ai]->dims + 2, arg);
      i = array_index (ars[ai], d);
      if (i < 0) 
         { array_err (cfile, name, "out of bounds");    RETURN (0); }
      ars[ai]->used    = time(NULL);
      ars[ai]->changed = 1;
      chxcpy  (ars[ai]->cell[i], word1);
      sprintf (temp, "%d", i);
      chxcat  (result, CQ(temp));

   }

   else if (streq (what, "array")) {
      if (chxtoken (word1, nullchix, 1, arg) < 0)       RETURN (0);
      array_args (arg, name, d, &dims, AR_DIMS);

      ai = array_in_mem (ars, name);
      if (ai < 0)
         { array_err (cfile, name, "no array");         RETURN (0); }
      if (ars[ai]->dims > dims)  
         { array_err (cfile, name, "bad dimensions");   RETURN (0); }

      i = array_index (ars[ai], d);
      if (i < 0) 
         { array_err (cfile, name, "out of bounds");    RETURN (0); }
      ars[ai]->used = time(NULL);
      chxcat (result, ars[ai]->cell[i]);
   }

   /*** $cell()  evaluate cell reference(s) relative to array/coordinate 
   /    in aref */
   else if (streq (what, "cell")) {
      if (aref == nullchix  ||  chxvalue (aref, 0L) == 0)  RETURN (0);
      array_args (aref, name, d, &dims, AR_DIMS);

      ai = array_in_mem (ars, name);
      if (ai < 0)
         { array_err (cfile, name, "no array");            RETURN (0); }

      for (pos=0;   cellparse (arg, &pos, dims, d, coords);   ) {
         if (pos > 0)  chxcatval (result, THIN, L(' '));
         i = array_index (ars[ai], coords);
         if (i < 0)
            { array_err (cfile, name, "cell: range err");     RETURN (0); }

         array_recurs (result, ars[ai]->cell[i], name, dims, coords, 
                       conf, cmlargs, vars, form, protect, incvars, macs, 
                       cfile, otab, tg, aref);
      }
   }

   

   /*** $cells()  evaluate cell range reference(s) relative to array/coordinate 
   /    in aref */
   else if (streq (what, "cells")) {
      if (aref == nullchix  ||  chxvalue (aref, 0L) == 0)  RETURN (0);
      array_args (aref, name, d, &dims, AR_DIMS);

      ai = array_in_mem (ars, name);
      if (ai < 0)
         { array_err (cfile, name, "no array");            RETURN (0); }

      /*** Parse the starting (coords) and ending (coends) cells. */
      pos = 0;
      if (NOT cellparse (arg, &pos, dims, d, coords)  ||
          NOT cellparse (arg, &pos, dims, d, coends)) 
         { array_err (cfile, name, "cells: bad args");     RETURN (0); }
         /* Something in errors?! */

      /*** Find the (one!) dimension that differs. */
      for (di=(-1),i=0;   i<dims;   ++i) {
         if (coords[i] != coends[i]) {
            if (di < 0)  di = i;
            else         RETURN (0);
         }
      }
      dincr = (coends[di] - coords[di] >= 0 ? 1 : -1);

      /*** Evaluate all the cells between coords and cooends, inclusive. */
      i = array_index (ars[ai], coords);
      array_recurs (result, ars[ai]->cell[i], name, dims, coords, 
                    conf, cmlargs, vars, form, protect, incvars, macs, 
                    cfile, otab, tg, aref);
      while (coords[di] != coends[di]) {
         chxcatval (result, THIN, L(' '));
         coords[di] += dincr;
         i = array_index (ars[ai], coords);
         if (i < 0) 
            { array_err (cfile, name, "cells: range err");     RETURN (0); }

         array_recurs (result, ars[ai]->cell[i], name, dims, coords, 
                       conf, cmlargs, vars, form, protect, incvars, macs, 
                       cfile, otab, tg, aref);
      }
   }

   else if (streq (what, "array_create")) {
      array_args (arg, name, d, &dims, AR_DIMS);
      if (d[0] < 0  ||  d[1] < 0)
         { chxcatval (result, THIN, L('0'));  RETURN(0); }

      /*** Does array exist? */
      ai = array_find (ars, name);
      exists = (ai >= 0);
      if (NOT exists) {
         chxtoken (word1, nullchix, 1, arg);
         exists = 1;
         if (unit_lock (XSAY, READ, L(0), L(0), L(0), L(0), word1)) {
            exists = unit_check (XSAY);
            unit_unlk  (XSAY);
         }
      }
      if (exists) {
         chxcatval (result, THIN, L('0'));
         array_err (cfile, name, "already exists");
         RETURN (0);
      }
 
      ai = array_slot (ars);
      ap = array_make (name, d);
      if (ap == NULL) {
         chxcatval (result, THIN, L('0'));  
         array_err (cfile, name, "out of memory");
         RETURN(0);
      }
      ars[ai] = ap;
      chxcatval (result, THIN, L('1'));
      ars[ai]->used = time(NULL);
      array_store (ap);
   }

   else if (streq (what, "array_insert")) {
      array_args (arg, name, v, &dims, AR_DIMS);
      dinsert = v[0] - 1;
      iinsert = v[1];

      ai = array_in_mem (ars, name);
      if (ai < 0)
         { array_err (cfile, name, "no array");         RETURN (0); }

      /*** Make an empty array ap with the inserted "row". */
      for (i=0;   i<ars[ai]->dims;   ++i)  d[i] = ars[ai]->d[i];
      d[i] = -1;
      d[dinsert]++;
      ap = array_make (name, d);

      copy_insert (ap, ars[ai], 0, coords, dinsert, iinsert); 

      /*** Ditch the old array, put the new one in its place. */
      array_free (ars[ai]);
      ars[ai] = ap;
      ars[ai]->used    = time(NULL);
      ars[ai]->changed = 1;
   }

   else if (streq (what, "array_remove")) {
      array_args (arg, name, v, &dims, AR_DIMS);
      dremove = v[0] - 1;
      iremove = v[1];

      ai = array_in_mem (ars, name);
      if (ai < 0)
         { array_err (cfile, name, "no array");         RETURN (0); }

      /*** Make an empty array ap with the removed "row". */
      for (i=0;   i<ars[ai]->dims;   ++i)  d[i] = ars[ai]->d[i];
      d[i] = -1;
      d[dremove]--;
      ap = array_make (name, d);

      copy_remove (ap, ars[ai], 0, coords, dremove, iremove); 

      /*** Ditch the old array, put the new one in its place. */
      array_free (ars[ai]);
      ars[ai] = ap;
      ars[ai]->used    = time(NULL);
      ars[ai]->changed = 1;
   }

   else if (streq (what, "array_store")) {
      array_args (arg, name, d, &dims, AR_DIMS);

      ai = array_find (ars, name);
      if (ai < 0)
         { array_err (cfile, name, "no loaded array");  RETURN (0); }

      ars[ai]->used    = time(NULL);
      ars[ai]->changed = 0;
      array_store (ars[ai]);
   }

   else if (streq (what, "array_clear")) {
      array_args (arg, name, d, &dims, AR_DIMS);
      ai = array_find (ars, name);
      if (ai < 0)
         { array_err (cfile, name, "no array");         RETURN (0); }
      array_free (ars[ai]);
      ars[ai] = NULL;
   }

   else if (streq (what, "array_delete")) {
      array_args (arg, name, d, &dims, AR_DIMS);
      ai = array_find (ars, name);
      if (ai >= 0) {
         array_free (ars[ai]);
         ars[ai] = NULL;
      }
      chxtoken (word1, nullchix, 1, arg);
      if (unit_lock (XSAY, WRITE, L(0), L(0), L(0), L(0), word1)) {
         unit_kill  (XSAY);
         unit_unlk  (XSAY);
      }
   }

   RETURN(1);
}

/*** Parse a cell reference such as '$cell(5 R-2)' */
FUNCTION  cellparse (Chix args, int4 *pos, int dims, int *d, int *coords) {
   static Chix word = nullchix;
   int   i, val;
   char *ascptr;

   if (word == nullchix)  word = chxalloc (L(30), THIN, "cellparse word");

   for (i=0;   i<dims;   ++i) {
      if (chxnextword (word, args, pos) < 0)  return (0);
      ascptr = ascquick (word);

      /*** Interpret coordinate as absolute or relative. */
      if (ascptr[0] == 'R')  {
         sscanf (ascptr+1, "%d", &val);
         coords[i] = val + d[i];
      }
      else {
         sscanf (ascptr,   "%d", &val);
         coords[i] = val;
      }
   }

   return (1);
}

FUNCTION array_recurs (Chix result, Chix cellvalue, char *name, int dims, 
                       int *coords, 
                       SWconf conf, char *cmlargs,
                       Vartab vars, Vartab form,
                       int protect, Chix incvars, Vartab macs, CML_File cfile,
                       Obtab otab, Target tg, Chix aref) {
   int   recurs, val0, val1, i;
   Chix  ar_eval, newargs, hold;
   char  temp[100];
   
   /*** Recurse only if cellvalue contains a parseable '$'. */
   for (recurs=val0=i=0;   (val1 = chxvalue (cellvalue, i));   ++i) {
      if (val1 == '$'  &&  val0 != '\\')  { recurs = 1;   break; }
   }
   if (NOT recurs) {
      chxcat (result, cellvalue);
      return;
   }

   /*** Prepare arguments to recursively do $array_eval() by calling
   /    func_value(). */
   newargs   = chxalloc (L(100), THIN, "array_recurs newargs");
   hold      = chxalloc (L(200), THIN, "array_recurs hold");
   ar_eval   = chxalsub (CQ("array_eval"), L(0), L(20));
   chxofascii (newargs, name);
   for (i=0;   i<dims;   ++i) {
      sprintf (temp, " %d", coords[i]);
      chxcat  (newargs, CQ(temp));
   }

   /*** Do recursive call and replace value.   Stick the result in 'hold'
   /    (7/14/01) and *then* append to result. */
   func_value (hold, ar_eval, newargs,
               conf, cmlargs, vars, form, protect, incvars, macs,
               cfile, otab, tg, aref);
   chxcat (result, hold);

   chxfree (hold);
   chxfree (ar_eval);
   chxfree (newargs);
}

/*** Make sure array NAME is in memory in ARS. */
FUNCTION array_in_mem (Array *ars, char *name) {
   int   ai;
   Array ap, array_load();

   ai = array_find (ars, name);

   /*** If not found in memory, load it, and add to table of arrays. */
   if (ai < 0) {
      ap = array_load (name);
      if (ap == (Array) NULL)  return (-1);
      ai = array_slot (ars);
      ars[ai] = ap;
   }
   return (ai);
}

/*** Convert typical $arrayxxx() args into name and array of numbers. 
/    d[n] == -1 means n dimensions. */
FUNCTION array_args (Chix arg, char *name, int *d, int *dims, int maxdim) {
   Chix  word;
   int   i;
   int4  v;

   word = chxalloc (L(30), THIN, "array_args word");
   chxtoken (word, nullchix, 1, arg);
   ascofchx (name, word, 0, AR_NAME);
   for (i=2;   chxtoken (word, nullchix, i, arg) >= 0;   ++i) {
      if (NOT chxnum (word, &v))  break;
      if (i-2 >= maxdim)          break;
      d[i-2] = v;
   }
   d[i-2] = -1;
   *dims = i-2;
   chxfree (word);
}

/*** Error log: array error. */
FUNCTION array_err (CML_File cfile, char *name, char *err) {
   char  temp[1000];
   sprintf (temp, "\n%s:%d array %s, error: %s\n",
                  cfile->filename, cfile->lnums[cfile->cdex], name, err);
   chxcat  (cfile->errtext, CQ(temp));
}

/*** Find an array in array of Arrays. */
FUNCTION array_find (Array *x, char *name) {
   int  i;

   for (i=0;   i<AR_ROWS;   ++i)
      if (x[i] != NULL  &&  streq (x[i]->name, name))  return (i);

   return (-1);
}

/*** Find/make a usable slot in array of Arrays. */
FUNCTION array_slot (Array *x) {
   int  i, oldest;

   for (oldest=i=0;   i<AR_ROWS;   ++i) {
      if      (x[i] == NULL)                  return (i);
      else if (x[i]->used < x[oldest]->used)  oldest = i;
   }

   if (ars[oldest]->changed)  array_store (ars[oldest]);
   array_free (ars[oldest]);
   ars[oldest] = NULL;
   return (oldest);
}

/*** Create/allocate a new array. */
FUNCTION Array array_make (char *name, int *d) {
   Array a;
   int   i, ncell;

   a = (Array) sysmem (sizeof (struct array_t), "array_make");
   if (a == (Array) NULL)  return (a);

   strcpy (a->name, name);
   a->used = time(NULL);

   for (i=0;               i<AR_DIMS;   ++i)  a->d[i] = d[i];
   for (ncell=d[0], i=1;   d[i] > 0;    ++i)  ncell = ncell * d[i];
   a->ncell = ncell;
   a->dims  = i;
   a->cell  = (Chix *) sysmem (ncell * sizeof (Chix *), "a->cell");
   if (a->cell == (Chix *) NULL)      return ((Array) NULL);
   a->changed = 0;

   for (i=0;   i<ncell;   ++i)
      a->cell[i] = chxalloc (L(20), THIN, "array_make");
   if (a->cell[ncell-1] == nullchix)  return ((Array) NULL);

   return (a);
}

FUNCTION array_free (Array ap) {
   int  i;
   for  (i=0;   i<ap->ncell;   ++i)  chxfree (ap->cell[i]);
   sysfree (ap->cell);
   sysfree (ap);
}

FUNCTION array_index (Array ap, int *d) {
   int  i, nc;

   for (nc=0, i=0;   i<ap->dims;   ++i) {
      if (d[i] >= ap->d[i])  return (-1);
      nc =  (nc * ap->d[i]) + d[i];
   }

   return (nc);
}

FUNCTION array_store (Array ap) {
   static Chix word = nullchix;
   static Chix blank, newl;
   char  temp[100];
   int   i, how0, how1;
   int4  pos;

   if (word == nullchix) {
      word  = chxalloc (L(80), THIN, "array_store word");
      newl  = chxalsub (CQ("\n"), L(0), L(2));
      blank = chxalsub (CQ(" "),  L(0), L(2));
   }
      
   chxcpy (word, CQ(ap->name));
   if (NOT unit_lock (XSAY, WRITE, L(0), L(0), L(0), L(0), word))  return (0);
   if (NOT unit_make (XSAY))                 { unit_unlk (XSAY);   return (0); }

   sprintf (temp, "%d %d\n", ap->dims, ap->ncell);
   unit_write (XSAY, CQ(temp));
   for (i=0;   i<ap->dims;   ++i) {
      sprintf (temp, "%d ", ap->d[i]);
      unit_write (XSAY, CQ(temp));
   }
   unit_write (XSAY, newl);

   /*** Reserve space (3 lines) for future options. */
   unit_write (XSAY, newl);
   unit_write (XSAY, newl);
   unit_write (XSAY, newl);

   for (i=0;   i<ap->ncell;   ++i) {
      if (chxvalue (ap->cell[i], 0L) == 0)  continue;

      sprintf (temp, "%d\n", i);
      unit_write (XSAY, CQ(temp));

      for (pos=0;  (how1 = chxnextline (word, ap->cell[i], &pos)) > 0; ) {
         unit_write (XSAY, blank);
         unit_write (XSAY, word);
         unit_write (XSAY, newl);
         how0 = how1;
      }
      if (how0 == 2) { unit_write (XSAY, blank);   unit_write (XSAY, newl); }
   }
   unit_close (XSAY);
   unit_unlk  (XSAY);
   ap->used = time(NULL);
   return (1);
}

FUNCTION Array array_load (char *name) {
   static Chix word = nullchix;
   short       nums[AR_DIMS];
   int         dims, d[AR_DIMS], i, first;
   int4        nc;
   Array       ap, array_make();

   if (word == nullchix) word  = chxalloc (L(80), THIN, "array_load word");

   if (name == NULL  ||  !name[0])                               return(NULL);
   chxcpy (word, CQ(name));
   if (NOT unit_lock (XSAY, READ, L(0), L(0), L(0), L(0), word)) return(NULL);
   if (NOT unit_view (XSAY))                { unit_unlk (XSAY);  return(NULL); }
      
   /*** Get dimensions, allocate array. */
   unit_short (XSAY, nums, 2);
   dims = nums[0];
   unit_short (XSAY, nums, dims);
   for (i=0;   i<dims;   ++i)  d[i] = nums[i];
   d[dims] = -1;
   ap = array_make (name, d);

   /*** Reserved space for future options. */
   unit_read (XSAY, word, 0);
   unit_read (XSAY, word, 0);
   unit_read (XSAY, word, 0);

   nc = 0;
   while (unit_read (XSAY, word, 0)) {
      if (chxvalue (word, L(0)) != ' ') {
         chxnum (word, &nc);
         first = 1;
      }
      else {
         if (NOT first) chxcatval (ap->cell[nc], THIN, L('\n'));
         first = 0;
         chxcatsub (ap->cell[nc], word, L(1), ALLCHARS);
      }
   }

   unit_close (XSAY);
   unit_unlk  (XSAY);
   ap->used = time(NULL);
   return (ap);
}

/*** Copy array SRC to TRG, inserting a "row" in dimension DINSERT,
/    at index IINSERT.  Implemented as a recursive copy of the next-lower
/    dimension, until we get to a simple vector. */
FUNCTION copy_insert (Array trg, Array src, int dthis, int *coords,
                       int dinsert, int iinsert) {
   int   i, j, offset, sn, tn;
       
   /*** Vector case. */
   if (dthis == src->dims-1) {
      for (i=0;   i<src->d[dthis];   ++i) {
         coords[dthis] = i;
         for (sn=0, j=0;   j<src->dims;   ++j)
            sn =  (sn * src->d[j]) + coords[j];
         for (tn=0, j=0;   j<src->dims;   ++j) {
            offset = (j == dinsert && coords[j] >= iinsert ? 1 : 0);
            tn =  (tn * trg->d[j]) + offset + coords[j];
         }
/*       printf ("[%d %d]=%d <- [%d %d]=%d\n", coords[0], coords[1], tn,
                  coords[0], coords[1], sn); */
         chxcpy (trg->cell[tn], src->cell[sn]);
      }
   }

   /*** Recursive call to next dimension. */
   else {
      for (i=0;   i<src->d[dthis];   ++i) {
         coords[dthis] = i;
         copy_insert (trg, src, dthis+1, coords, dinsert, iinsert);
      }
   }
}

/*** Copy array SRC to TRG, removing a "row" in dimension DREMOVE,
/    at index IREMOVE.  Implemented as a recursive copy of the next-lower
/    dimension, until we get to a simple vector. */
FUNCTION copy_remove (Array trg, Array src, int dthis, int *coords,
                       int dremove, int iremove) {
   int   i, j, offset, sn, tn;
       
   /*** Vector case. */
   if (dthis == src->dims-1) {
      for (i=0;   i<src->d[dthis];   ++i) {
         if (dthis == dremove  &&  i==iremove)  continue;

         coords[dthis] = i;
         for (sn=0, j=0;   j<src->dims;   ++j)
            sn =  (sn * src->d[j]) + coords[j];
         for (tn=0, j=0;   j<src->dims;   ++j) {
            offset = (j == dremove && coords[j] > iremove ? -1 : 0);
            tn =  (tn * trg->d[j]) + offset + coords[j];
         }
/*       printf ("[%d %d]=%d <- [%d %d]=%d\n", coords[0], coords[1], tn,
                  coords[0], coords[1], sn); */
         chxcpy (trg->cell[tn], src->cell[sn]);
      }
   }

   /*** Recursive call to next dimension. */
   else {
      for (i=0;   i<src->d[dthis];   ++i) {
         if (dthis == dremove  &&  i==iremove)  continue;
         coords[dthis] = i;
         copy_remove (trg, src, dthis+1, coords, dremove, iremove);
      }
   }
}

