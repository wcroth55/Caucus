
/*** FUNC_SQL.   Handle $sql_xxx() functions.
/
/    func_sql (result, what, arg, vars);
/ 
/    Parameters:
/       Chix       result;  (append resulting value here)
/       char      *what;    (function name)
/       Chix       arg;     (function argument)
/       Vartab     vars;    (CML global variables)
/
/    Purpose:
/       Implement CML SQL related functions.  There are two
/       sets of functions.  The first set returns the entire
/       result of a query, and breaks out the individual pieces:
/          $sql_select (statement)
/          $sql_row    (row)
/          $sql_col    (column)
/
/          A simple example:
/             for row in $sql_select (select * from confnames)
/                "<tr>
/                for col in $sql_row ($(row))
/                   "<td>$sql_col ($(col))</td>
/                end
/             end
/
/          $sql_row() "unquotes" a row returned from $sql_select(), and
/          $sql_col() "unquotes" the data for a single column returned
/          from $sql_row().
/
/       The second set acts like a more traditional open/read/close sequence:
/          $sql_query_open  (statement)
/          $sql_query_row   (handle)
/          $sql_query_close (handle)
/
/          A simple example:
/             set handle $sql_select (select name, cnum from confnames)
/             while $sql_query_row ($(handle))
/                "name=$(name), cnum=$(cnum)<br>
/             end
/             eval $sql_query_close($(handle))
/
/          For simple SELECT-only queries, where we want *all* the data
/          at once, we provide $sql_query_select (select-statement).  For
/          example:
/             for a b in $sql_query_select (SELECT a, b FROM x)
/                "$unquote(a), $unquote(b)
/             end
/
/          (Note that each "a" or "b" returned is quoted, in case the
/          actual value is more than one word.)
/
/
/       Sequence generator (for primary keys etc.)
/          $sql_last_insert ()
/             Used to get the value from an INSERT to an auto_increment field.
/             (This is, wherever possible, replacing the old sql_sequence() calls.)
/
/          $sql_sequence (name)
/             Returns the "next" number in sequence name.  Sequences
/             are automatically created as needed, i.e. the 1st call
/             on a particular sequence name creates it.
/
/    How it works:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sweb/funcsql.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/11/03 New function. */
/*: CR  8/13/04 Add $sql_query_select() */
/*: CR 01/28/05 Make sqlbuf global, for use elsewhere. */
/*: CR 10/06/09 Add $sql_last_insert(). */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"
#include "sqlcaucus.h"

extern AsciiBuf  sqlbuf;

#define  CHXLISTMAX 100
#define  SMALLBUF   200

FUNCTION  func_sql (Chix result, char *what, Chix arg, Vartab vars)
{
   static Chix  word1 = nullchix;
   static Chix  word2, stmt, comma, blank, ctrla, dot, work;
   Chix *chxlist;
   char *statement;
   char *asciifull();
   char  word[SMALLBUF], temp[SMALLBUF];
   int   rc, from, vn, slot, is_select, count;
   int4  handle, pos;
   AsciiBuf ascii_buf();
   ENTRY ("func_sql", "");

   if (word1 == nullchix)  {
      word1 = chxalloc (40L, THIN, "func_sql word1");
      word2 = chxalloc (40L, THIN, "func_sql word2");
      work  = chxalloc (40L, THIN, "func_sql work");
      stmt  = chxalloc (40L, THIN, "func_sql stmt");
      comma = chxalsub (CQ(","),    L(0), L(4));
      blank = chxalsub (CQ(" "),    L(0), L(4));
      ctrla = chxalsub (CQ("\001"), L(0), L(4));
      dot   = chxalsub (CQ("."),    L(0), L(4));
   }

   if (streq (what, "sql_query_open")) {

      /*** The coolest part of $sql_query_open() is that, for SELECT's, it
      /    extracts the column names in the query, and uses them as the CML
      /    variable names where we put the results.  We need to do some parsing
      /    of the original query string to make this work, and then save the
      /    target chix for those CML variables in an array.
      /
      /    Here's a sample (completely made up) query:
      /       "SELECT a.userid, b.fullname fn FROM users a, names b"
      /
      /    Start by finding the FROM keyword, and taking everything before it:
      /       "SELECT a.userid, b.fullname fn"
      /
      /    Grab just the columns, "quote" the blanks, turn commas into blanks:
      /       "a.userid b.fullname^fn"    (^ is control-A)
      /
      /    Finally, parse each token, take the last "word" in it, and
      /    remove any leading "dotted" parts.  This leaves us with
      /    "userid" and "fn" as the names of the target CML variables. */

      chxlist = (Chix *) NULL;
      chxtoken (stmt, nullchix, 1, arg);
      chxupper (stmt);
      is_select = chxeq (stmt, CQ("SELECT"));
      if (is_select) {

         /*** Find "FROM" part of SELECT query.  First remove all
         /    parenthesized expressions (notably subqueries!), then
         /    look for the "FROM". */
         chxcpy   (stmt, arg);
         while (pullParens (stmt, word2))  ;
         chxcpy  (work, stmt);
         chxupper(work);
         handle = 0;
         from   = tablefind (work, CQ("FROM"));
  
         /*** If found, get the substring with just the column/variable names.*/
         if (from >= 2) {
            pos = chxtoken (word1, nullchix, from+1, stmt);
            chxclear  (work);
            chxcatsub (work, stmt, 0, pos);
  
            /*** Use commas as token separators to get the variable names. */
            chxalter   (work, 0, blank, comma);
            chxreplace (work, 0, blank, ctrla);
            chxreplace (work, 0, comma, blank);
  
            /*** Prepare an array to hold the chix for the column/variables */
            chxlist = (Chix *) sysmem (CHXLISTMAX * sizeof(Chix *), "chxlist");
  
            /*** Parse out each column variable, one at a time. */
            for (vn=1;   chxtoken (word1, nullchix, vn+1, work) >= 0;   ++vn) {
               chxreplace (word1, 0, ctrla, blank);
               chxclear   (word2);
  
               /*** Get the last word in this token. */
               for (pos=0;   chxnextword (word2, word1, &pos) >= 0; ) ;
  
               /*** If contains a dot, remove the dot & everything before it */
               if ( (pos = chxrevdex (word2, L(2000), dot)) > 0) {
                  chxcpy    (word1, word2);
                  chxclear  (word2);
                  chxcatsub (word2, word1, pos+1, ALLCHARS);
               }
  
               /*** Assign the target chix for this variable to the chxlist */
               chxlist[vn-1] = nullchix;
               if (NOT EMPTYCHX(word2)) {
                  slot = vartab_find (vars, word2);
                  chxlist[vn-1] = vars->value[slot];
               }
            }
            chxlist[vn-1] = nullchix;
         }
      }

      /*** Finally, execute the SQL query. */
      ascii_buf (sqlbuf, chxlen(arg)+10);
      ascofchx  (sqlbuf->str, arg, 0, sqlbuf->max);
      handle = sql_query_open (sqlbuf->str, chxlist);

      sprintf (temp, "%d", handle);
      chxcat  (result, CQ(temp));
   }

   else if (streq (what, "sql_query_row")) {
      rc = 0;
      if (chxnum (arg, &handle)  &&  handle >= 0)
         rc = sql_query_row (handle);
      chxcatval (result, THIN, (rc ? L('1') : L('0')));
   }

   else if (streq (what, "sql_query_close")) {
      if (chxnum (arg, &handle)  &&  handle >= 0)  {
         sql_query_close (handle);
      }
   }

   else if (streq (what, "sql_query_count")) {
      count = -1;
      if (chxnum (arg, &handle)  &&  handle >= 0)
         count = sql_query_count (handle);
      sprintf (temp, "%d", count);
      chxcat  (result, CQ(temp));
   }

   else if (streq (what, "sql_sequence")) {
      chxtoken (word1, nullchix, 1, arg);
      ascofchx (word,  word1, 0, SMALLBUF);
      sql_sequence (word, word2);
      chxcat   (result, word2);
   }

   else if (streq (what, "sql_col")) {
      chxclear (word1);
      quoter   (word1, "unquote", arg);
      if (chxvalue (word1, 0L) == 037)  chxcatsub (result, word1, 1L, ALLCHARS);
      else                              chxcat    (result, word1);
   }

   else if (streq (what, "sql_row")) {
      chxcpy    (word1, arg);
      chxrepval (word1, 0L, THIN, L(036), L(' '));
      chxcat    (result, word1);
   }

   else if (streq (what, "sql_last_insert")) {
      sprintf (temp, "%d", sql_last_insert());
      chxcat  (result, CQ(temp));
   }

   else if (streq (what, "sql_select")) {
      statement = asciifull (arg);
      strtoken (word, 1, statement);
      strlower (word);

      if (NOT streq (word, "select")) { sysfree (statement);    return (0); }

      rc = sql_exec (0, statement);
      if (rc != SQL_SUCCESS)          { sysfree (statement);    return (0); }

      while ( (rc = sql_fetch(0)) != SQL_NO_DATA) {
         while ( (rc = sql_getchardata (0, sqlbuf)) != SQL_NO_DATA) {
            chxofascii (word1, sqlbuf->str);
            chxclear   (word2);
            quoter     (word2, "quote", word1);

            chxcatval  (result, THIN, L(036));
            chxcatval  (result, THIN, L(037));
            chxcat     (result, word2);
         }
         chxcatval (result, THIN, L(' '));
      }
      sysfree (statement);
   }

   else if (streq (what, "sql_query_select")) {
      statement = asciifull (arg);
      strtoken (word, 1, statement);
      strlower (word);

      if (NOT streq (word, "select")) { sysfree (statement);    return (0); }

      rc = sql_exec (0, statement);
      if (rc != SQL_SUCCESS)          { sysfree (statement);    return (0); }

      while ( (rc = sql_fetch(0)) != SQL_NO_DATA) {
         while ( (rc = sql_getchardata (0, sqlbuf)) != SQL_NO_DATA) {
            chxofascii (word1, sqlbuf->str);
            chxclear   (word2);
            quoter     (word2, "quote", word1);
            chxcat     (result, word2);
            chxcatval  (result, THIN, L(' '));
         }
      }
      sysfree (statement);
   }

   RETURN (1);
}
