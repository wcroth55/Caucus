/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** ENTRY_TEST.  Function invoked by ENTRY macro in all Caucus code. */

/*: AA  7/29/88 17:54 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

#define  MAXLEVEL   300    /* Max number of stack levels */
#define  MAXWIDE     16    /* Width of char string containing func. name */

extern struct unit_template   units[];
extern int                    debug;

static int  level = 0;
char cstack[MAXLEVEL][MAXWIDE];

static FILE *entryfile = NULL;

FUNCTION  entry_test (ets, t)
   char   *ets, *t;
{
#if ENTRYEXIT
   char   temp[200];
   int    i;

#if ENTRYLOG
   if (entryfile == NULL) {
      sysprv (0);
      sprintf (temp, "/tmp/ent%06d.log", getpid());
      nt_chkfile (temp, 0700);
      entryfile = fopen (temp, "a");
      sysprv (1);
   }

   if (entryfile == NULL) fprintf (stderr, "Couldn't open entry log.\n");
   else {
      for (i=0;  i<level;   ++i)  fprintf (entryfile, " ");
      fprintf (entryfile, "%s\n", ets);
      fflush  (entryfile);
   }
#endif

   if (level >= MAXLEVEL) {
      printf ("ENTRY_TEST: level has reached %d!\n", level);
      for (i=0;   i<MAXLEVEL;   ++i)
         printf (" %3d %s\n", i, cstack[i]);
      exit (1);
   }

   strcpy (temp, ets);
   temp[MAXWIDE] = '\0';
   strcpy (cstack[level], temp);
   cstack[level+1][0] = '\0';
   ++level;
#endif

/* mtest  ("entering"); */
}


FUNCTION  exit_test()
{
/* mtest ("leaving"); */

#if ENTRYEXIT
   int  i;

   if (level < 0) {
      printf ("ENTRY_TEST: stack level < 0!\n");
      for (i=0;   i<20;   ++i)
         printf (" %3d %s\n", i, cstack[i]);
      exit (1);
   }

   cstack[level][0] = '\0';
   --level;
   cstack[level][0] = '\0';
#endif
}


#define  MTEST   0
#if MTEST

FUNCTION mtest (direction)
   char  *direction;
{
   char   str[200];
   int    i;

   if (master.items    < 0  ||  master.items    > MAXITEMS  ||
       master.nochange < 0  ||  master.nochange > 1     ||
       master.noenter  < 0  ||  master.noenter  > 1) {
      sprintf (str, "Change to master struct %s %s: %d %d %d\n",
                     direction, cstack[level], 
                     master.items, master.nochange, master.noenter);
      master.items = master.nochange = master.noenter = 0;
      buglist (str);
      unit_lock   (XSBG, WRITE, L(0), L(0), L(0), L(0), "");
      unit_append (XSBG);
      for (i=1;   i<=level;   ++i)  {
         unit_write (XSBG, cstack[i]);
         unit_write (XSBG, "\n");
      }
      unit_close  (XSBG);
      unit_unlk   (XSBG);
   }
}
#endif
