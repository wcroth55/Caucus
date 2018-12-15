
/*** BUILD_NAME_CASCADE.   Name cascade builder prototype. 
/
/    build_name_cascade(optsize);
/   
/    Parameters:
/       int  optsize;   (optimum size of pull-downs)
/
/    Purpose:
/       Completely repopulate the name_cascade table, with the list
/       of the name breaks of groups of user first names, last names, 
/       userids; and conference names.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/
/    Notes:
/  
/    Home:  buildnamecas.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/23/05 New function. */
/*: CR 12/19/05 Handle large numbers of identical names (expands blockhigh) */
/*: CR 12/22/05 Return properly if very few names total! */
/*: CR 02/28/06 Bug: never returned if last chunk always had row < blocklow */

#include <stdio.h>
#include <math.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"
#include "sqlcaucus.h"
#include "sql.h"

#define  STRMAX 600
#define  QMAX  2000

extern AsciiBuf  sqlbuf;
static AsciiBuf  nameBuf = NULL;

FUNCTION  build_name_cascade (int optsize) {

   char        strtemp[STRMAX];
   char        name[STRMAX], minName[STRMAX], prvName[STRMAX];
   char        query[QMAX],  nothing[10];
   int         rc0, count;
   AsciiBuf    ascii_buf();

   ENTRY ("cascade", "");

   if (nameBuf == NULL) nameBuf = ascii_buf (NULL, STRMAX);

   /*** If the entries for the non-existent id "_" do not exist, create them. */
   strcpy (query, "SELECT count(*) FROM user_info WHERE userid='!'");
   rc0   = sql_quick_limited (0, query, strtemp, STRMAX, 1);
   strcat (strtemp, "0");
   count = atoi(strtemp);
   if (count < 1) {
      strcpy (query, "INSERT INTO user_info (userid, active, fname, lname) "
                     "VALUES ('!',     0, '!',     '!')");
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);
      strcpy (query, "INSERT INTO user_info (userid, active, fname, lname) "
                     "VALUES ('~~~~~', 0, '~~~~~', '~~~~~')");
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);
      strcpy (query, "INSERT INTO confs (cnum, name) VALUES ('-1', '!')");
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);
      strcpy (query, "INSERT INTO confs (cnum, name) VALUES ('-2', '~~~~~')");
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);
   }
   

   strcpy (query, "SELECT count(*) FROM user_info");
   rc0   = sql_quick_limited (0, query, strtemp, STRMAX, 1);
   count = atoi(strtemp);

   cascade_fill ("lname", optsize, 
      "SELECT upper(lname)  FROM user_info %s ORDER BY lname  LIMIT %d", count);
   cascade_fill ("fname", optsize, 
      "SELECT upper(fname)  FROM user_info %s ORDER BY fname  LIMIT %d", count);
   cascade_fill ("userid", optsize, 
      "SELECT upper(userid) FROM user_info %s ORDER BY userid LIMIT %d", count);

   strcpy (query, "SELECT count(*) FROM confs");
   rc0   = sql_quick_limited (0, query, strtemp, STRMAX, 1);
   count = atoi(strtemp);
   cascade_fill ("name", optsize, 
      "SELECT upper(name)    FROM confs %s ORDER BY name LIMIT %d", count);

   strcpy (query, "DELETE FROM name_cascade WHERE active=1");
   rc0 = sql_quick_limited (0, query, nothing, 9, 0);
   strcpy (query, "UPDATE name_cascade SET active=1");
   rc0 = sql_quick_limited (0, query, nothing, 9, 0);
}

FUNCTION diffPos (char *name, char *low) {
   int   i;
   int   result = -1;

   for (i=0;   name[i]  &&  low[i]  &&  name[i] == low[i];   ++i) ;
   if  ( (! name[i])  &&  (! low[i]))  return (-1);
   return (i);
}

FUNCTION char *cascadeEscape (char *s) {
   static char temp[STRMAX];
   char       *t;

   for (t=temp;   *s;   ++s)  {
      if (*s == '\'')  *t++ = '\\';
      *t++ = *s;
   }
   *t = '\0';
   return (temp);
}

FUNCTION cascade_fill (char *field, int optsize, char *queryTemplate, 
                       int count) {
   char   lowClause[STRMAX], minBreak[STRMAX];
   char   name[STRMAX], minName[STRMAX], prvName[STRMAX], lowName[STRMAX];
   char   query[QMAX], nothing[10];
   int    blocksize, blocklow, blockhigh, row, pos, diff, minDiff;
   int    rc0, rc1, hasData;
   float  sqrtf(float x);
   char  *cascadeEscape();

   /*** We're trying to find blocks of names somewhere between blocksize/2
   /    and 1.5 * blocksize, where the border entries between blocks have
   /    the minimum # of leading characters needed to be different.  (E.g.
   /    "azure" and "baby" differ in the 1st char position, which is good.) */
   blocksize = (int) sqrt((float) count) + 1;
   if (blocksize < optsize)  blocksize = optsize;
   blocklow  = blocksize / 2;
   blockhigh = 1.5 * ((float) blocksize);

   /*** Each outer loop gets one block. */
   lowClause[0] = '\0';
   lowName  [0] = '\0';
   while (1) {
      sprintf (query, queryTemplate, lowClause, blockhigh+2);
/*    fprintf (stderr, "query='%s'\n", query); */

      /*** Each inner block iterates over *all* the names, trying to find
      /    the ideal "border" entry as described above.   We also save the
      /    very lowest and very highest to be the absolute top and bottom
      /    borders of the entire set. */
      rc0 = (sql_exec (0, query) == SQL_SUCCESS);
      minDiff = 1000000;
      prvName[0] = '\0';
      for (row=0;   rc0  &&  row < blockhigh  &&  
                    (hasData = (sql_fetch(0) != SQL_NO_DATA));     row++) {
         sql_getchardata (0, nameBuf);
         nameBuf->str[STRMAX-1] = '\0';
         strcpy (name, nameBuf->str);
         if (! lowName[0])  strcpy (lowName, name);
   
         if (row > blocklow) {
            if ( (pos = diffPos (name, prvName)) >= 0) {
               diff = pos * 1000 + abs(row-blocksize);
               if (diff < minDiff) { 
                  minDiff = diff;   
                  strcpy (minName,  name);
                  strcpy (minBreak, name);
                  minBreak[pos+1] = '\0';
               }
            }
         }
/*       fprintf (stderr, 
           "   name='%s', min='%s', minDiff=%d, row=%d, blocklow=%d\n", 
           name, minName, minDiff, row, blocklow); */

         strcpy (prvName, name);
      }
      if (minDiff == 1000000  &&  blocksize > optsize  &&  hasData) {
         blockhigh = 1.5 * ((float) blockhigh);
         continue;
      }
      if (row < blockhigh)  break;
      sprintf (lowClause, " WHERE %s >= '%s' ", field, cascadeEscape(minName));
      cascade_insert (field, minBreak);
   }
   cascade_insert (field, lowName);
   cascade_insert (field, prvName);
}

FUNCTION cascade_insert (char *type, char *name) {
   char query[QMAX], nothing[10];
   int  rc1;

   sprintf (query, "INSERT INTO name_cascade (type, name, active) "
                   "VALUES ('%s', '%s', 0)", type, name);
   rc1 = sql_quick_limited (1, query, nothing, 9, 0);
}
