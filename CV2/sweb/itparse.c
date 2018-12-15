/*** IT_PARSE.   Handle the parsing for the $it_parse() function.
/
/    it_parse (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $it_parse() function.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  itparse.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/14/96 16:30 New function. */
/*: CR  3/06/97 12:31 Replace isdigit() with MYDIGIT. */

/*
 ----------------------------------------------------------------------
 State Table for parsing item range selection.
 
    code      means
    --------  ----------------------
    num1=     set num1 to input
    str||=    append input to str
    str=0     clear str
    +(x)      add items indicated by "x" to final result
    ->state   change state (->X means exit)
 
         Input
 State    digit        -          ,         "         eos     spaces       a   
 -----   -------    ------    ------    -------    -------   -------     ------
  null    num1=                          str=0      ->X                  str=
          ->num                          ->quote                         ->word
 ------------------------------------------------------------------------------
  num     +(num1)             +(num1)    +(num1)    +(num1)              +(num1)
          num1=                          str=0                           str=
                    ->num2    ->null     ->quote    ->X                  ->word
 ------------------------------------------------------------------------------
  num2    +(num1-num2)        +(num1)    +(num1)    +(num1)              +(num1)
          ->null              ->null     str=0                           str=0
                                         ->quote    ->X                  ->word
 ------------------------------------------------------------------------------
  quote   str||=    str||=    str||=     +(str)    +(str)     str||=     str||=
                                         ->null    ->X
 ------------------------------------------------------------------------------
  word    str||=    str||=    +(str)     +(str)    +(str)     str||=     str||=
                              ->null     ->quote   ->X        if since
                                                               ->since
                                                           else->word2
 ------------------------------------------------------------------------------
  word2   +(str)              +(str)     +(str)    +(str)                str||= 
          num1=                                                       
          ->num               ->null     ->quote   ->X
 ------------------------------------------------------------------------------
  since   str||=    str||=    +(str)     +(str)    +(str)     +(str)     str||=
                              ->null     ->quote   ->X        ->null
 ------------------------------------------------------------------------------
*/

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern Cstat g_cstat;
extern Pstat g_pstat, g_pstat2;

extern FILE *sublog;
extern int   cml_debug;

#define  IS(x)      (cval == x)
#define  MYDIGIT(x) (060 <= (x)  &&  (x) <= 071)   /* isdigit() macro fails! */
#define  ADDN(n,q)  itp_addn (result, cnum, n, q, rl);
#define  ADDS(s)    itp_adds (result, cnum, s,    rl);
#define  RLNULL     ((Rlist) NULL)

#define  ST_NULL   0
#define  ST_NUM    1
#define  ST_NUM2   2
#define  ST_QUOTE  3
#define  ST_WORD   4    /* In the middle of a word */
#define  ST_WORD2  5    /* Possible beginning of 2nd word. */
#define  ST_SINCE  6

FUNCTION  it_parse (Chix result, Chix arg)
{
   Rlist  rl, this;
   int4   pos, pos0, val, cval;
   int    cnum, type, state, num, num1, ival, item;
   Chix   word, word1;
   char   number[100], temp[100];
   ENTRY ("it_parse", "");

   rl    = a_mak_rlist ("it_parse rl");
   word  = chxalloc (L(50), THIN, "it_parse word");
   word1 = chxalloc (L(50), THIN, "it_parse word1");
   pos   =  L(0);
   cnum  = chxint4 (arg, &pos);

   type = chxtype (arg);
   state = ST_NULL;
   while (1) {

      /*** If we got a number (a sequence of digits), get the value VAL,
      /    the string equivalent NUMBER, and set CVAL to -1. */
      for (pos0=pos, val=0;   
           ival = cval = chxvalue (arg, pos), MYDIGIT(ival);    ++pos) {
         val = 10 * val + (ival - '0');
      }
      if  (pos > pos0) {
         --pos;   
         sprintf (number, "%d", val); 
         cval = -1; 
      }
      else {
         /*** Treat any sequences of spaces or tabs as a single space. */
         for (pos0=pos;   cval = chxvalue (arg, pos), 
                          (cval=='\t' || cval==' ');     ++pos)  ;
         if  (pos > pos0) {
            --pos;
            cval = ' ';
         }
      }

      /*** Handle each row in the state table... */

      if (state == ST_NULL) {
         if      (IS(-1 ))  { num1 = val;                   state = ST_NUM; }
         else if (IS('-'))  ;
         else if (IS(','))  ;
         else if (IS('"'))  { chxclear (word);              state = ST_QUOTE; }
         else if (IS( 0 ))    break;
         else if (IS(' '))  ;
         else { 
            chxclear  (word);
            chxcatval (word, type, cval);                   state = ST_WORD;
         }
      }

      else if (state == ST_NUM) {
         /*** In all cases except "-" and ' ', add prev num to item range. */
         if (NOT (IS('-') || IS(' ')))   ADDN (num1,0);

         if      (IS(-1 ))   num1 = val; 
         else if (IS('-'))                                   state = ST_NUM2;
         else if (IS(',')) {                                 state = ST_NULL;  }
         else if (IS('"')) { chxclear (word);                state = ST_QUOTE; }
         else if (IS( 0 ))                                   break;
         else if (IS(' ')) ;
         else              { chxclear (word);                state = ST_WORD;  }
      }

      else if (state == ST_NUM2) {
         if      (IS(-1 )) { ADDN(num1, (int) val);          state = ST_NULL;  }
         else if (IS('-')) ;
         else if (IS(',')) { ADDN(num1,0);                   state = ST_NULL; }
         else if (IS('"')) { ADDN(num1,0);  chxclear(word);  state = ST_QUOTE; }
         else if (IS( 0 )) { ADDN(num1,0);                   break; }
         else if (IS(' ')) ;
         else              { ADDN(num1,0);  chxclear(word);   state = ST_WORD; }
      }

      else if (state == ST_QUOTE) {
         if      (IS(-1 ))   chxcat    (word, CQ(number));
         else if (IS('-'))   chxcatval (word, type, cval);
         else if (IS(','))   chxcatval (word, type, cval);
         else if (IS('"')) { ADDS(word);                    state = ST_NULL; }
         else if (IS( 0 )) { ADDS(word);                    break; }
         else if (IS(' '))   chxcatval (word, type, cval);
         else                chxcatval (word, type, cval);
      }

      else if (state == ST_WORD) {
         if      (IS(-1 ))   chxcat    (word, CQ(number));
         else if (IS('-'))   chxcatval (word, type, cval);
         else if (IS(',')) { ADDS(word);                    state = ST_NULL; }
         else if (IS('"')) { ADDS(word);                    state = ST_QUOTE;}
         else if (IS( 0 )) { ADDS(word);                    break; }
         else if (IS(' ')) { 
            chxcatval (word, type, cval);
            chxcpy    (word1, word);
            jixreduce (word1);
            if (chxeq (word1, CQ("since ")))                state = ST_SINCE;
            else                                            state = ST_WORD2;
         }
         else                chxcatval (word, type, cval);
      }

      else if (state == ST_WORD2) {
         if      (IS(-1 )) { ADDS(word);  num1 = val;       state = ST_NUM; }
         else if (IS('-')) ;
         else if (IS(',')) { ADDS(word);                    state = ST_NULL; }
         else if (IS('"')) { ADDS(word);                    state = ST_QUOTE;}
         else if (IS( 0 )) { ADDS(word);                    break; }
         else if (IS(' ')) ;
         else              { chxcatval (word, type, cval);  state = ST_WORD; }
      }

      else if (state == ST_SINCE) {
         if      (IS(-1 ))   chxcat    (word, CQ(number)); 
         else if (IS('-'))   chxcatval (word, type, cval);
         else if (IS(',')) { ADDS(word);                    state = ST_NULL; }
         else if (IS('"')) { ADDS(word);                    state = ST_QUOTE;}
         else if (IS( 0 )) { ADDS(word);                    break; }
         else if (IS(' ')) { ADDS(word);                    state = ST_NULL;}
         else              { chxcatval (word, type, cval);  state = ST_WORD; }
      }

      ++pos;
   }

   /*** Scan through the accumulated Rlist of items, and generate
   /    the triplet list of items and append it to RESULT. */
   for (this=rl->next;   this != RLNULL;   this=this->next) {
      for (item=this->i0;   item<=this->i1;   ++item) {
         sprintf (temp, "%d %d %d ", cnum, item, this->r0);
         chxcat  (result, CQ(temp));
      }
   }

   a_fre_rlist (rl);
   chxfree     (word);
   RETURN (1);
}


FUNCTION itp_addn (Chix result, int cnum, int num0, int num1, Rlist rl)
{
   static Rlist range = RLNULL;
   Rlist        new, temp, or_rlist();
   ENTRY ("itp_addn", "");

   /*** Make up a static one-data-node Rlist, for adding a specific
   /    item or item range. */
   if (range == RLNULL) {
      range       = a_mak_rlist ("itp_addn range");
      range->next = a_mak_rlist ("itp_addn rangenext");
   }
   if (num1 == 0)  num1 = num0;
   range->next->i0 = num0;
   range->next->i1 = num1;
   range->next->r0 =  0;
   range->next->r1 = -1;

   /*** RL = RL "or" RANGE. */
   new = or_rlist (rl, range);
   temp      = new->next;
   new->next = rl->next;
   rl->next  = temp;
   a_fre_rlist (new);

   RETURN (1);
}


FUNCTION itp_adds (Chix result, int cnum, Chix word, Rlist rl)
{
   static Namex  namex = (Namex) NULL;
   static People people;
   static Chix   word1;
   Rlist         new, temp, or_rlist();
   Namelist      nl;
   int           this, property, ok;
   ENTRY ("itp_adds", "");

   /*** Initialization. */
   if (namex == (Namex) NULL)  {
      namex  = a_mak_namex  ("func_it namex");
      people = a_mak_people ("func_it people");
      word1  = chxalloc (L(40), THIN, "itp_adds word1");
   }

   chxtoken  (word1, nullchix, 1, word);
   jixreduce (word1);

   if (chxeq (word1, CQ("since"))) {
      property = P_RSINCE;
      chxtoken (nullchix, namex->namepart, 2, word);
   }

   else if (chxeq (word1, CQ("author"))) {
      chxtoken (nullchix, people->namepart, 2, word);
      people->cnum = cnum;
      ok = a_get_people (people, P_EVERY, A_WAIT);
      chxclear (namex->namepart);
      for (nl=people->ids->next;   nl != NLNULL;  nl = nl->next) {
         chxcat    (namex->namepart, nl->str);
         chxcatval (namex->namepart, THIN, L(' '));
      }
      property = P_NAME;
   }

   else {
      property = P_TITLE;
      chxcpy (namex->namepart, word);
   }
   
   namex->cnum = cnum;
       
   if (a_get_namex (namex, property, A_WAIT) == A_OK) {
      /*** RL = RL "or" NAMEX->RLIST. */
      new       = or_rlist (rl, namex->rlist);
      temp      = new->next;
      new->next = rl->next;
      rl->next  = temp;
      a_fre_rlist (new);
   }

   RETURN (1);
}
