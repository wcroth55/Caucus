/*** PARSE2_TOKENS.   Parse a command line instance range into tokens.
/
/    parse2_tokens (line, token, rangewords);
/
/    Parameters:
/       Chix   line;        (command line to be parsed)
/       Token  token[];     (array of empty tokens, supplied by caller)
/       Chix   rangewords;  (table of words that demand a following argument)
/
/    Purpose:
/       Parse2_token parses a LINE of item/response instances into an array
/       of tokens, which can then be used for higher-level parsing.
/
/    How it works:
/       Parse2_tokens() produces an array of tokens of the following types:
/          T_NUM     a decimal number, positive or negative
/          T_DASH    a dyadic dash
/          T_COLON   a colon (item:resp separator)
/          T_WORD    an arbitrary word
/          T_DATA    a word that *must* be data for the previous word
/          T_END     last token (no other meaning)
/
/       The caller must supply a sufficiently large array of empty tokens
/       in TOKEN.  Parse2_token() allocates chix for tokens of type T_WORD 
/       and T_DATA; the caller must free them when it is finished.
/
/       RANGEWORDS is a table of words that demand a following data word.
/       (E.g., "AUTHOR", "TITLE", "SINCE", etc.)
/       
/       Parse2_token() operates as a finite-state machine, parsing one 
/       character at a time from LINE.  The state table, actions and
/       transitions are described below.  The actions are:
/          ->      "go to state"
/          new     "create a new token"
/          add     "add this digit or character to current token"
/          type    "set type of this token to"
/          backup  "go back one character in LINE"
/
/        ST_NEW: (starting state) About to start a new token
/           comma, space:                 -> ST_NEW
/           dash:  new, type T_DASH,      -> ST_NEW
/           colon: new, type T_COLON,     -> ST_NEW
/           digit: new, type T_NUM, add,  -> ST_NUM
/           quote: new, type T_DATA,      -> ST_QUOTED
/           other: new, type T_WORD, add, -> ST_WORD
/
/        ST_NUM: in the middle of a number
/           comma, space:  -> ST_NEW
/           digit: add,    -> ST_NUM
/           other: backup, -> ST_NEW
/
/        ST_QUOTED: in the middle of a quoted string
/           quote:      -> ST_NEW
/           other: add, -> ST_QUOTED
/
/        ST_WORD: in the middle of a word
/           comma, space, colon, dash: backup, -> ST_REST
/           quote:                     backup, -> ST_NEW
/           other:                     add,    -> ST_WORD
/
/        ST_REST: check, should rest of LINE become data word?
/           comma, space:         -> ST_REST
/           quote:        backup, -> ST_NEW
/           other: if current word in RANGEWORDS
/                     then new, add rest of LINE, exit
/                     else backup, -> ST_NEW
/
/    Returns: 1
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
/    Known bugs:
/       This version used hard-coded character constants for the
/       values that determine comma, dash, and colon.  These should
/       probably become dictionary strings.
/
/    Home:  gen/parse2to.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CT  5/27/93 14:11 New function. */
/*: CP  6/13/93 23:44 Use delete_token(). */
/*: CK 10/06/93 15:12 Fill in position member of each new token. */
/*: CR 10/17/93 17:47 Remove unused 'found'. */

#include <stdio.h>
#include "caucus.h"
#include "token.h"

#define  ST_NEW    1
#define  ST_NUM    2
#define  ST_QUOTED 3
#define  ST_WORD   4
#define  ST_REST   5

FUNCTION  parse2_tokens (line, token, rangewords)
   Chix   line, rangewords;
   Token  token[];
{
   int4   pos, val, b, blank;
   int    this, state, valtype, which;
   int    space, digit, dash, colon, comma, quote;
   Chix   blankstr;

   ENTRY ("parse2_tokens", "");

   blankstr = chxalloc (L(20), THIN, "parse2to blankstr");
   chxspecial (SC_GET, SC_BLANK, blankstr);
   valtype  = chxtype (line);

   this  = -1;
   state = ST_NEW;
   for (pos=0L;   (val = chxvalue (line, pos));   ++pos) {

      /*** Determine type of this character. */
      space = 0;
      for (b=0L;   (blank = chxvalue (blankstr, b));   ++b)
         if (val == blank) { space = 1;   break; }
      digit = (val>='0'  &&  val<='9');
      dash  = (val=='-');
      colon = (val==':');
      comma = (val==',');
      quote = (val=='"');

      if (state == ST_NEW) {
         if      (comma | space)   ;

         else if (dash)
            { token[++this].type = T_DASH;    token[this].pos = pos; }

         else if (colon) 
            { token[++this].type = T_COLON;   token[this].pos = pos; }

         else if (digit) {
            token[++this].type = T_NUM;
            token[  this].num0 = val - '0';
            token[  this].pos  = pos;
            state = ST_NUM;
         }

         else if (quote)  {
            token[++this].type = T_DATA;
            token[  this].str  = chxalloc (L(40), valtype, "parse2to token");
            token[  this].pos  = pos + 1;
            state = ST_QUOTED;
         }

         else {
            token[++this].type = T_WORD;
            token[  this].str  = chxalloc (L(40), valtype, "parse2to token");
            token[  this].pos  = pos;
            chxcatval (token[this].str, valtype, val);
            state = ST_WORD;
         }
      }

      else if (state == ST_NUM) {
         if      (comma | space)  state = ST_NEW;
         else if (digit)          token[this].num0 =
                                     10 * token[this].num0 + val - '0';
         else                     { --pos;   state = ST_NEW; }
      }

      else if (state == ST_QUOTED) {
         if (quote)  state = ST_NEW;
         else        chxcatval (token[this].str, valtype, val);
      }

      else if (state == ST_WORD) {
         if      (comma | space | colon | dash) { --pos;   state = ST_REST; }
         else if (quote)                        { --pos;   state = ST_NEW; }
         else             chxcatval (token[this].str, valtype, val);
      }

      else if (state == ST_REST) {
         if      (comma | space)  ;
         else if (quote)          { --pos;   state = ST_NEW; }
         else {
            which = tablematch (rangewords, token[this].str);
            if (which < 0)        { --pos;   state = ST_NEW; }
            else {
               token[++this].type = T_DATA;
               token[  this].str  = chxalloc (L(80), valtype, "parse2to token");
               token[  this].pos  = pos;
               chxcatsub (token[this].str, line, pos, ALLCHARS);
               break;
            }
         }
      }
   }
   ++this;
   token[this].type = T_END;
   if (this >= 1)  token[this].pos  = token[this-1].pos + 1;

   /*** For each token, copy NUM0 to NUM1. */
   for (this=0;   token[this].type != T_END;   ++this)
      token[this].num1 = token[this].num0;

   chxfree (blankstr);

   RETURN  (1);
}
