
/*** PARSE_FIELDS.  Parse fields out of a command line string.
/
/    ok = parse_fields (str, startword, quote, fieldnames, havedata, 
/                            fieldnum, data);
/
/    Parameters:
/       int    ok;          (Success?)
/       Chix   str;         (command line string to be parsed)
/       int    startword;   (start with this word of STR)
/       Chix   quote;       (quoting char)
/       Chix   fieldnames   (table of field names)
/       int    havedata;    (do these fields take a data string?)
/       int   *fieldnum;    (returned field number of field found)
/       Chix   data;        (returned data found)
/
/    Purpose:
/       Parse_fields() is used to parse (and remove) "fields" in
/       a command line string.  A "field" is either a single word
/       (such as BRIEF), or a word that takes an argument
/       (such as TITLE "ABC XYZ").
/
/    How it works:
/       Parse_fields starts scanning STR at word STARTWORD.  It examines
/       each word in turn, to see if it is a member of FIELDNAMES.  If
/       so, it is removed from STR, and FIELDNUM set to the index of that
/       word in FIELDNAMES.
/
/       If HAVEDATA is true, parse_fields removes the "argument" that
/       immediately followed the word, and places it in DATA.  If the
/       argument was quoted, the entire argument is removed from STR
/       and placed in DATA (the quotes are not carried over into DATA).
/       If there were no quotes, the remainder of STR is removed and 
/       placed in DATA.
/
/       QUOTE contains the string that defines what "quoted" means.
/       (usually it is a 1-byte string of the double-quote character,
/        but it could be anything.)
/
/       Parse_fields() is usually called repeatedly (in a loop) until
/       it fails.
/
/    Returns: 1 on success.
/             0 (sets FIELDNUM to word #) if a word is ambiguous.
/             0 (sets FIELDNUM to  0) if no words in FIELDNAMES were found.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  gen/parsefi.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/28/92 17:15 New function. */
/*: CR  8/23/92 21:52 Correct comments. */
/*: JX 10/08/92 12:10 Chixify. */
/*: CR 10/08/92 23:06 chxsub() -> chxcatsub(). */
/*: CR 10/12/92 11:53 Fixiy. */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CR 12/10/92 15:06 chxalter arg int4. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  parse_fields (str, startword, quote, fieldnames, havedata, 
                             fieldnum, data)
   Chix   str,  quote,  fieldnames,  data;
   int    startword, havedata, *fieldnum;
{
   int    this, next, match, success;
   int4   pos;
   Chix   word;

   ENTRY ("parse_fields", "");

   word = chxalloc (L(80), THIN, "parsefi word");

   for (this=startword;   (pos = chxtoken (word, nullchix, this, str)) != -1;  
          ++this) {
      match = tablematch (fieldnames, word);
      if (match == -2)  { *fieldnum = this;   RETURN (0); }
      if (match == -1)     continue;

      /*** Remove the matching word, & set FIELDNUM.  If this field
      /    does not take an argument, we're done. */
      chxalter (str, pos, word, CQ(""));
      *fieldnum = match;
      if (NOT havedata)   SUCCEED;

      /*** Now that we've removed WORD from STR, word number THIS is
      /    where the argument begins.  Is it a quoted argument?  If so, 
      /    NEXT is where the argument ends.   If not, take the
      /    rest of the line as the argument and return. */
      pos = chxtoken (nullchix, data, this, str);
      if (pos == -1)  { chxclear (data);   SUCCEED; }
      if (chxindex (str, pos, quote) != pos  ||
          (next = chxindex (str, pos+1, quote)) < 0) {
         chxalter (str, pos, data, CQ(""));
         SUCCEED;
      }

      /*** Quoted argument.  Pluck out the initial QUOTE and the data. */
      chxclear  (data);
      chxcatsub (data, str, pos, next - pos + 1);
      chxalter  (str,  pos, data, CQ(""));

      /*** Remove both quotes from DATA. */
      chxalter (data, L(0), quote, CQ(""));
      chxalter (data, L(0), quote, CQ(""));
      SUCCEED;
   }

  *fieldnum = 0;
   FAIL;

done:
   chxfree (word);
   RETURN  (success);
}
