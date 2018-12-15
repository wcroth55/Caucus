/*** DECODE_SETOPTS.   Parse & decode SET options.
/
/    decode_setopts (reg, str, sys, defstr);
/
/    Parameters:
/       struct  userreg_t *reg;   (user registration info struct)
/       Chix    str;              (string of set options, on one line)
/       Chix    sys;              (string of system dependent set options)
/       Chix    defstr;           (default string of set options)
/
/    Purpose:
/       A user's SET options are stored as two lines of information
/       in their XURG file.  Decode_setopts() parses those lines and
/       fills in the appropriate fields in REG.
/
/    How it works:
/       Decode_setopts() works with both the new (all fields with a
/       restricted set of keyword values are stored as numbers) and
/       old (keywords themselves were stored in the XURG file) formats.
/       It automatically converts old format data to new format.
/
/       If STR does not contain an option, it is filled in from DEFSTR.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  loadreg()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  chg/decodese.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/04/92 13:49 New function. */
/*: CR  1/09/92 13:31 Add sys opt stuff. */
/*: DE  2/26/92 14:45 Chixified */
/*: JX  5/13/92 11:26 Minor chix fixes. */
/*: CX  6/02/92 13:33 Handle systurn(...LINE_EDIT). */
/*: JX  6/11/92 12:18 Fix to actually *use* DEFSTR, allow SYS == null. */
/*: JX  6/11/92 13:52 Fix to get numeric values correctly. */
/*: JX  8/04/92 18:04 Chixify header. */
/*: JX 10/09/92 13:16 Parse numbers correctly. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR  2/26/93  0:18 Scan all 18 fields. */
/*: CP  8/17/93 14:22 Handle SHOWATTACH, TERMINAL. */
/*: CP  8/24/93 13:59 Changes to SHOWATTACH. */
/*: CP  9/02/93 12:27 Both chxtoken()'s must use w+1. */
/*: JV  9/07/93 11:13 Add SET TRANSFER. */

#include <stdio.h>
#include "caucus.h"

#define ON_OFF_NUM(x)  (is_on ? x : (is_off ? 0 : numvalue))

extern struct termstate_t term_in_caucus;

FUNCTION  decode_setopts (reg, str, sys, defstr)
   Userreg  reg;
   Chix  str, sys, defstr;
{
   int    w, is_num, is_on, is_off, numvalue, chs, success;
   Chix   word, tempchx;
   int4   lpos;

   ENTRY ("decode_setopts", "");

   word    = chxalloc (L(16), THIN, "decodese word");
   tempchx = chxalloc (L(80), THIN, "decodese tempchx");

   if (str == nullchix) str = defstr;

   for (w=0;   w<=20;   ++w) {
      chxtoken(word, tempchx, w+1, str);
      if (EMPTYCHX (word))  chxtoken (word, nullchix, w+1, defstr);
      jixreduce (word);

      lpos     = L(0);
      numvalue = chxint4  (word, &lpos);
      is_num   = chxvalue (word,  lpos) == '\0';
      is_on    = chxeq    (word, CQ("on"));
      is_off   = chxeq    (word, CQ("off"));

      switch (w) {

         /* DEBUG */
         case ( 0): reg->debug     = ON_OFF_NUM (-1);    break;

         /* SCREENSIZE */
         case ( 1): reg->lines     = numvalue;           break;

         /* SCREENWIDTH */
         case ( 2): reg->width     = numvalue;           break;

         /* FORMAT */
         case ( 3): reg->truncate  = 1 - ON_OFF_NUM (1); break;

         /* EDITOR */
         case ( 4): chxcpy (reg->editor, word);          break;

         /* TAB */
         case ( 5): reg->tablength = numvalue;           break;

         /* PAGE */
         case ( 6): reg->newpage   = numvalue;           break;

         /* PRINT */
         case ( 7):
            chs = tablematch (CQ("default edit"), word);
            reg->printcode = (is_num ? numvalue : (chs >= 0 ? chs : 0));
            break;

         /* VERBOSE */
         case ( 8): reg->verbose   = ON_OFF_NUM (1);     break;

         /* DICTIONARY */
         case ( 9): reg->dict      = numvalue;           break;

         /* EOT */
         case (10):
            chs = tablematch (CQ("1eof 2return"), word);
            chxcpy (reg->eot, (chs==1 ? chxquick("", 0 ) : 
                              (chs==0 ? chxquick("$endfile$", 1 ) : word)));
            break;

         /* EXPIRATION */
         case (11): reg->expwarn   = ON_OFF_NUM(1);      break;

         /* TEXT_ENTRY */
         case (12):
            chs = tablematch (CQ("terminal editor wordwrap window"), word);
            reg->texten = (is_num ? numvalue : (chs >= 0 ? chs : 0));
            break;

         /* RECEIPT_SIZE */
         case (13): reg->receiptsize = numvalue;         break;

         /* MYTEXT */
         case (14):
            chs = tablematch (CQ("later now never"), word);
            reg->mytext = (is_num ? numvalue : (chs >= 0 ? chs : 0));
            break;

         /* STARTMENU */
         case (15): chxcpy (reg->startmenu, (is_off ? CQ("") : word));  break;

         /* IMPORT_MAIL */
         case (16):
            chs = tablematch (CQ("off import copy"), word);
            reg->import_mail = (is_num ? numvalue : (chs >= 0 ? chs : 0));
            break;

         /* EXPORT_MAIL */
         case (17):
            chs = tablematch (CQ("off export"), word);
            reg->export_mail = (is_num ? numvalue : (chs >= 0 ? chs : 0));
            break;

         /* TERMINAL. */
         case (18): chxcpy (reg->terminal, word);       break;

         /* SHOWATTACH. */
         case (19): reg->showattach = numvalue;         break;

         /* TRANSFER. */
         case (20): chxcpy (reg->transfer, word);       break;
      }
   }

   /*** Initialize all the system-dependent fields to their default values.
   /    Then process & parse all the tokens in SYS. */
   reg->sysi1 = 0;
   if (sys != nullchix) 
      for (w=1;   w<=1;   ++w) {
         chxtoken (word, tempchx, w, sys );
         if (EMPTYCHX (word))  break;
         jixreduce (word);

         lpos     = L(0);
         numvalue = chxint4 (word, &lpos);
         is_num = chxvalue (word,   lpos) == '\0';
         is_on  = chxeq  (word, CQ("on"));
         is_off = chxeq  (word, CQ("off"));
   
         switch (w) {
   
            /* SYSI1 (Vax LINE-EDIT) */
            case ( 1):
               reg->sysi1 = ON_OFF_NUM (1);
               systurn (&term_in_caucus, LINE_EDIT, (reg->sysi1 ? ON : OFF));
               break;
         }
      }

   SUCCEED;

done:
   chxfree (word);
   chxfree (tempchx);
   return  (success);
}
