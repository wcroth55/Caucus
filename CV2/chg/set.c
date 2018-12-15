/*** SET.   Set values of the various parameters that control the way
/    CAUCUS works.
/
/    SET without any arguments prints the current values of all of
/    the parameters.  SET with an argument prints the current
/    value of the selected parameter, and prompts the user to change
/    it to something else. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:34 Source code master starting point */
/*: CR  8/22/88 13:27 Change SET DEBUG ON/OFF -> 0/1/2 */
/*: CR  1/17/89 11:12 Add SET EXPIRATION warning option. */
/*: AM  2/07/89 10:25 Jcaucus changes*/
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CW  6/12/89 14:53 Add handling for SET TEXT_ENTRY option. */
/*: CR  9/01/89 15:20 Add handling for SET RECEIPTSIZE option. */
/*: CR  9/05/89 16:13 Add handling for SET MYTEXT option. */
/*: CR  9/28/89 13:24 Add handling for SET STARTMENU option. */
/*: CR  1/08/90 16:25 Add SET IMPORT_MAIL option. */
/*: JV  3/15/90 11:51 Disallow '/' as seperator for equivalent strings. */
/*: CR  4/12/90 16:27 SET DICT must call sysglobals(), too. */
/*: CR  4/16/90 10:58 Add SET EXPORT. */
/*: CR  8/12/90 19:10 Cast editfile/mdprompter arg to make lint happy. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  7/02/91 16:32 globals() is now mainglobals(). */
/*: UL 12/11/91 18:41 Make unit_lock args 3-6 int4.*/
/*: CR  1/07/92 18:38 Revamp, use value_setopts(). */
/*: CR  1/09/92 12:20 Make order of SET display depend on string set_Aorder. */
/*: DE  3/17/92 16:56 Chixified */
/*: JX  5/19/92 20:59 Fix Chix. */
/*: JX  6/09/92 17:21 chxtoken returns -1 on fail. */
/*: JX  6/16/92 17:15 Only allocate answertab on first call! */
/*: JX  6/19/92 16:35 Clear VALUE before value_setopts(). */
/*: JX  6/25/92 11:55 Fixify. */
/*: JV 12/04/92 13:41 Add sitevar arg to storereg(). */
/*: CL 12/10/92 17:06 Long ascofchx args. */
/*: CR  2/17/93 11:07 Replace chxcpy(...,null) with CQ(""). */
/*: CP  8/01/93 13:00 Add api_debug. */
/*: CP  8/17/93 14:14 Add 'terminal', 'showattach' property. */
/*: CP  8/24/93 13:54 SHOWATTACH now has multiple values. */
/*: JV  9/07/93 10:54 Add 'transfer'. */
/*: CK  9/07/93 15:46 Remove duplicate SET EXPORT captive user test. */
/*: JZ 10/08/93 15:45 Put complete name of file trans protocol in TRANSFER. */
/*: CP  2/07/94 14:15 Never let SCREENWIDTH go below 4. */

#include <stdio.h>
#include "caucus.h"

extern struct screen_template screen;
extern Userreg                thisuser;
extern struct namlist_t      *sysmgrprint;
extern struct flag_template   flag;
extern union  null_t          null;
extern int                    debug;
extern int                    api_debug;
extern Chix                   sitevar[];
extern Chix                   att_ui_type;

/*** The set options are:
/    0:DEBUG    1:SCREENSIZE    2:SCREENWIDTH   3:FORMAT       4:EDIT
/    5:TAB      6:PAGE          7:PRINT         8:VERBOSE      9:DICTIONARY
/   10:EOT     11:EXPIRATION   12:TEXT_ENTRY   13:RECEIPTSIZE
/   14:MYTEXT  15:STARTMENU    16:IMPORT_MAIL  17:EXPORT_MAIL 18:TERMINAL
/   19:SHOWATTACH 20:TRANSFER
/
/    For each option, we have the name in OPTIONTAB, the associated
/    help in HELPTAB, the codes for use in 'prompter' in RESTRICTAB,
/    and the tables of allowed answers in ANSWERTAB. */

static char *helptab[] = {
   "set_Hnohelp",  "set_Hscsiz",  "set_Hscwid",    "set_Hformat",
   "set_Hedit",    "set_Htab",    "set_Hpage",     "set_Hprint",
   "set_Hverb",    "set_Hdict",   "set_Heot",      "set_Hexpire",
   "set_Htexten",  "set_Hrecsiz", "set_Hmytext",   "set_Hstmenu",
   "set_Himport",  "set_Hexport", "set_Hterminal", "set_Hshowat",
   "set_Htransfer"
};

static char first=1;
static Chix answertab[21];

/*** Restrict what value user may enter for each option.  0=>anything,
/    1=>table entry or number, 2=>table entry only. */
static char  restrictab[] =
  { 1, 1, 1, 2, 0, 1, 1, 2, 2, 1, 0, 2, 2, 1, 2, 0, 2, 2, 0, 2, 2 };

static Chix optiontab;
static Chix s_print;
static Chix s_eot;
static Chix s_texten;
static Chix s_mytext;
static Chix s_import;
static Chix s_export;
static Chix s_order;
static Chix s_showat;
static Chix s_trans;

FUNCTION  set (command)
   Chix  command;
{
   struct namlist_t  *pcd, *nlnode();
   static Chix        editlist;
   Chix   option, value, means, def, onofftable, rest, newstr, format,
          value_word;
   char   means_str[40];
   int    supplied, number, i, n, choice, c, index, success;
   int4   dumint4;

   ENTRY ("set", "");

   if (first)                             /* FIRST gets set to zero below! */
      for (index=0; index<21; index++ )
         answertab[index] = chxalloc (L(40), THIN, "set answertab[i]");

   option      = chxalloc (L(80),  THIN, "set option");
   value       = chxalloc (L(80),  THIN, "set value");
   means       = chxalloc (L(80),  THIN, "set means");
   def         = chxalloc (L(80),  THIN, "set def");
   rest        = chxalloc (L(80),  THIN, "set rest");
   newstr      = chxalloc (L(40),  THIN, "set newstr");
   format      = chxalloc (L(40),  THIN, "set format");
   onofftable  = chxalloc (L(100), THIN, "set onofftable");
   value_word  = chxalloc (L(100), THIN, "set value_word");
   success     = 0;

   /*** Get the table of options, and on/off keywords. */
   mdtable ("set_Aoption", &optiontab);
   mdstr   (onofftable, "onofftable", null.md);

   /*** Many options have values that must be chosen from a restricted set
   /    of keywords.  Get those sets of keywords and load them into
   /    ANSWERTAB. */
   mdtable ("set_Aprint",  &s_print);
   mdtable ("set_Aeot",    &s_eot);
   mdtable ("set_Atexten", &s_texten);
   mdtable ("set_Amytext", &s_mytext);
   mdtable ("set_Aimport", &s_import);
   mdtable ("set_Aexport", &s_export);
   mdtable ("set_Ashowat", &s_showat);
   mdtable ("set_Atrans",  &s_trans);

   if (first) {
      chxcpy (answertab[0], onofftable);
      chxcpy (answertab[3], onofftable);
      chxcpy (answertab[7], s_print);
      chxcpy (answertab[8], onofftable);
      chxcpy (answertab[10], s_eot);
      chxcpy (answertab[11], onofftable);
      chxcpy (answertab[12], s_texten);
      chxcpy (answertab[14], s_mytext);
      chxcpy (answertab[15], onofftable);
      chxcpy (answertab[16], s_import);
      chxcpy (answertab[17], s_export);
      chxcpy (answertab[19], s_showat);
      chxcpy (answertab[20], s_trans);
      first = 0;
   }

   /*** The command SET without options displays current settings. */
   chxtoken  (option, rest, 2, command);
   if (EMPTYCHX (rest)) {
      unit_write (XWTX, NEWCPAGE);
      mdtable ("set_Aorder", &s_order);
      mdwrite    (XWTX, "set_Theader", null.md);
      for (i=1;   chxtoken (value, nullchix, i, s_order) != -1;   ++i) {
         if (NOT chxnum (value, &dumint4))  continue;
         n = dumint4;
         chxtoken (option, nullchix, n+1, optiontab);
         chxupper (option);
         chxclear (value);
         value_setopts (value, n, thisuser, answertab[n]);
         chxclear   (means);
         chxformat  (means, CQ("set_Tmeans%d"), L(n), L(0), nullchix, nullchix);
         ascofchx (means_str, means, L(0), L(100));
         mdstr    (format, means_str, null.md);
         chxclear   (newstr);
         chxformat  (newstr, CQ("%-12s %10s    "), L(0), L(0), option, value);
         chxformat  (newstr, CQ("%s\n"), L(0), L(0), format, nullchix);
         unit_write (XWTX, newstr);
     }
     sysset (XWTX, command);
     SUCCEED;
   }

   /*** The rest of the code for this function handles the user trying
   /    to choose an option and set its value.   Start by finding out
   /    which option the user chose.   Make sure they made a legal choice. */
   jixreduce (option);
   choice = tablematch (optiontab, option);
   if (choice == -1  &&  (choice = sysset (XWTX, command)) >= 0)  SUCCEED;

   if (choice == -2) {
      ambiguous (option);
      FAIL;
   }

   if (choice == -1) {
      mdstr      (format, "set_Fbadcom", null.md);
      chxclear   (newstr);
      chxformat  (newstr, format, L(0), L(0), option, nullchix);
      unit_write (XWER, newstr);
      FAIL;
   }

   /*** From here on, we know the user selected a valid option.  Now we
   /    handle the user setting a new value for that option.   If a
   /    value was supplied on the command line, e.g. SET OPTION VALUE,
   /    then use that.  Otherwise, prompt the user for a value. */
   chxtoken (value, rest, 3, command);
   supplied = (NOT EMPTYCHX (rest) ? 3 : 0);
   value_setopts (def, choice, thisuser, answertab[choice]);
   chxformat (means, CQ("set_Pprm%d"), L(choice), L(0), nullchix, nullchix );
   ascofchx  (means_str, means, L(0), L(100));
   c = mdprompter (means_str, answertab[choice],
                          (int) (restrictab[choice] + supplied),
                          helptab[choice], 40, value, def);
   if (c < -1)  SUCCEED;     /* Return or CANCEL: no change. */
   chxtoken (value_word, nullchix, 1, value);
     /* Use 1st word only of what user typed. */

   /*** Certain values typed by the user may be constrained by other
   /    circumstances.  Captive-caucus users always have SET EXPORT OFF.
   /    If EDITLIST is ON, SET EDIT selection must be restricted to list
   /    of allosed editors. */
   if (choice == 17  &&  flag.login) {
      mdwrite (XWTX, "set_Enoexport", null.md);
      c = 0;
   }
   if (choice==4  &&  flag.editlist) {
      mdtable ("editnames", &editlist);
      if (tablematch  (editlist, value) < 0) {
         list_editors (editlist);
         FAIL;
      }
   }

   /*** Interpret the VALUE and set the appropriate field in the user's
   /    registration struct. */
   if (NOT chxnum (value, &dumint4)) dumint4 = 0;
   number = dumint4;
   switch (choice) {
      case ( 0): thisuser->debug       = number;         break;
      case ( 1): thisuser->lines       = number;         break;
      case ( 2): thisuser->width       = max(number,4);  break;
      case ( 3): thisuser->truncate    = c;              break;
      case ( 4): chxcpy (thisuser->editor, value);       break;
      case ( 5): thisuser->tablength   = number;         break;
      case ( 6): thisuser->newpage     = number;         break;
      case ( 7): thisuser->printcode   = c;              break;
      case ( 8): thisuser->verbose     = 1-c;            break;
      case ( 9): thisuser->dict        = number;         break;

      case (10): chxcpy (thisuser->eot,
                   (c==1 ? CQ("") : (c==0 ? CQ("$endfile$") : value)));
                   break;

      case (11): thisuser->expwarn     = 1-c;          break;
      case (12): thisuser->texten      = c;            break;
      case (13): thisuser->receiptsize = number;       break;
      case (14): thisuser->mytext      = c;            break;
      case (15): chxcpy (thisuser->startmenu, (c==1 ? CQ("") : value));
                                                       break;
      case (16): thisuser->import_mail = c;            break;
      case (17): thisuser->export_mail = c;            break;
      case (18): chxcpy    (thisuser->terminal, value);
                 jixreduce (thisuser->terminal);       break;
      case (19): thisuser->showattach  = c;            break;
      case (20): chxtoken  (thisuser->transfer, nullchix, c+1, s_trans);
                 jixreduce (thisuser->transfer);       break;
   }

   /*** Once the VALUE has been set, some options must perform additional
   /    actions.  SET DICTIONARY must reload in-memory strings.
   /    SET PRINT EDIT must let the user edit the print codes.
   /    SET PRINT DEFAULT must clear the print codes.
   /    SET TERMINAL  must modify the global att_ui_type for attachment
   /                  display. */
   if (choice ==  9) { mainglobals();   sysglobals(); }
   if (choice == 18)   chxcpy (att_ui_type, thisuser->terminal);
   if (choice ==  7) {
      /*** If edit, copy the printcodes into a XITX temp file, let the user
      /    edit the XITX file, then copy back to the printcodes. */
      if (c == 1) {
         pcd = (thisuser->print->next!=null.nl ? thisuser->print : sysmgrprint);
         unit_lock (XITX, WRITE, L(0), L(0), L(0), L(0), nullchix);
         unit_make (XITX);
         for (pcd = pcd->next;   pcd != null.nl;   pcd = pcd->next) {
            unit_write (XITX, pcd->str);
            unit_write (XITX, CQ("\n"));
         }
         unit_close (XITX);
  
         /*** Edit the temporary file. */
         editfile (XITX, thisuser->editor, (int) thisuser->truncate);
  
         /*** Copy the edited printcode instructions back in. */
         loadprint (&thisuser->print, XITX, 0);
         unit_kill (XITX);
         unit_unlk (XITX);
      }
  
      /*** If set to DEFAULT, reset the printcodes. */
      else {
         nlfree (thisuser->print);
         thisuser->print = nlnode (4);
      }
   }

   /*** Finally, write out the changed information, and set the global
   /    debug and screen parameters according to what the user may have
   /    just set. */
   storereg (thisuser, 1, sitevar);
   debug        = thisuser->debug;
   api_debug    = thisuser->debug;
   screen.lines = thisuser->lines;
   screen.width = thisuser->width;

   SUCCEED;

 done:

   chxfree (option);
   chxfree (value);
   chxfree (means);
   chxfree (def);
   chxfree (rest);
   chxfree (newstr);
   chxfree (format);
   chxfree (onofftable);
   chxfree (value_word);

   RETURN (success);
}
