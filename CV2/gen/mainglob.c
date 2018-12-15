/*** MAINGLOBALS.   Global data structures used by Caucus main program.
/
/    mainglobals();
/
/    Parameters: None.
/
/    Purpose:
/       Mainglobals() defines the set of global variables which are
/       used (primarily) by the regular Caucus main program.
/
/       See utilglobals() for the definition of other global variables
/       which are used in common by other Caucus-related programs.
/
/    How it works:
/
/    Returns: 1
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus main program.
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  gen/mainglob.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/02/91 15:22 New function. */
/*: CR  7/26/91 18:25 Remove flags2[], never used. */
/*: CR  9/16/91 13:04 Allocate pdir,master,oldmaster members from the heap. */
/*: CR 11/14/91 11:24 Call make_master() to allocate MASTER memory. */
/*: JV 12/16/91 16:37 Change pdir.lnum to int4. */
/*: CR  1/10/92 21:17 Remove global onofftable. */
/*: JX  5/13/92 17:10 Chixify. */
/*: CX  5/28/92 18:42 Move ss_pause, ss_abort to unit_newline(). */
/*: CX  6/05/92 11:51 Replace com_unit with u_keybd, u_com. */
/*: CX  8/06/92 12:18 Allocate memvar & sitevar here. */
/*: JV 10/13/92 09:44 Change items_flagged[] size from 1050 to MAXITEMS+10. */
/*: TP 10/19/92 15:59 Remove u_com, u_keybd. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: CR  1/13/93 13:39 Call make_partic(). */
/*: JV  3/09/93 09:43 Add attach_def. */
/*: CN  5/15/93 12:54 Add selected[] (to replace items_flagged). */
/*: CP  7/08/93 16:11 Integrate 2.5 changes. */
/*: CP 10/12/05 Rip out mdstr() calls for C5. */
 
#include <stdio.h>
#include "caucus.h"

Chix  servername;      /* Name of server program, client/server model only */
int   menu_level=0;    /* 0==not in menus, others=depth in menu system. */

/*** Master file information */
struct  master_template  master, oldmaster;

/*** User participation information */
struct  partic_t         thisconf;
struct  fresh_template   fresh;
struct  namlist_t       *sysmgrprint = NULL;

/*** User's message list record. */
struct  message_t        mess;

/*** Response directory. */
struct  pdir_t           pdir = {-1, 0, (short *) NULL, (int4 *) NULL };

/*** List of items chosen by user after parsing of item range. */
short  selected[MAXITEMS+10];

/*** Debugging switch. */
int    debug = 0;

/*** Attributes of current item. */
struct this_template thisitem;

/*** CV2.2 memory variables V0 -> V9. */
Chix memvar[10];

/*** CV2.4 site-dependent variables U0 -> U9. */
Chix sitevar[10];

/*** Boundaries for sorted tables, such as namelist files. */
Chix  sorthigh;
Chix  sortlow;

/*** Some useful tables for answering questions from PROMPTER. */
Chix   yesnotable;
Chix   objectable;
/*** Global saved character strings. */
Chix   ss_dotnl, ss_caucus, ss_inprom;
Chix   ss_inred, ss_outred, ss_appred;
Chix   ss_qt1, ss_qt2;
Chix   ss_lparen, ss_rparen;
Chix   ss_all, ss_colon;
Chix   ss_cmiesc;
Chix   ss_luid, ss_ruid;
Chix   afm_name;                   /* Auto-Fire Macros name-buffer */

/*** Linked list of definitions of file attachment formats. */
Attach_Def attach_def;

FUNCTION  mainglobals()
{
   extern union null_t       null;
   static short first_call = 1;
   short i;
   char *sysmem();
   int  index, success;

   ENTRY ("mainglobals", "");

   /*** On first call,
   /      allocate memory for master, oldmaster, pdir, and thisconf.
   /      initialize XCMA record "master".   */
   if (first_call) {
      first_call = 0;
      pdir.resp = (short *) sysmem (sizeof(short) * L(PDIRMAX  + 3), "maing1");
      pdir.lnum = (int4  *) sysmem (sizeof(int4 ) * L(PDIRMAX  + 3), "maing2");
      oldmaster.responses = 
                  (short *) sysmem (sizeof(short) * L(MAXITEMS + 3), "maing3");
      make_master (&master,   MAXITEMS);
      make_partic (&thisconf, MAXITEMS);

      attach_def = make_def_attach();

      for (index=0;   index<10;   index++) {
         memvar [index] = chxalloc (L(100), THIN, "memvar");
         sitevar[index] = chxalloc (L(100), THIN, "sitevar");
       }

      sorthigh = chxalloc (L(20), THIN, "mainglob sorthigh");
      chxofascii (sorthigh, "~~~~~~~~~~");

      sortlow  = chxalloc (L(5),  THIN, "mainglob sortlow");
      chxofascii (sortlow,  "!");

      servername = chxalloc (L(80),  THIN, "mainglob servername");
      ss_dotnl   = chxalsub (CQ("'.\n$"),  L(0), L(20));
      ss_colon   = chxalsub (CQ(":"),      L(0), L(20));
      ss_inred   = chxalsub (CQ("<"),      L(0), L(20));
      ss_outred  = chxalsub (CQ(">"),      L(0), L(20));
      ss_appred  = chxalsub (CQ(">>"),     L(0), L(20));
      ss_caucus  = chxalsub (CQ("caucus"), L(0), L(20));
      ss_inprom  = chxalsub (CQ(">"),      L(0), L(20));
      ss_qt1     = chxalsub (CQ("'"),      L(0), L(20));
      ss_qt2     = chxalsub (CQ("\""),     L(0), L(20));
      yesnotable = chxalsub (CQ("no yes ok"), L(0), L(20));
      objectable = chxalsub (CQ("items responses messages/send "
                    "osubjects/subjects psubjects persons/participants"),
                                           L(0), L(100));
      ss_all     = chxalsub (CQ("all"),    L(0), L(20));
      ss_lparen  = chxalsub (CQ("("),      L(0), L(20));
      ss_rparen  = chxalsub (CQ(")"),      L(0), L(20));
      ss_cmiesc  = chxalsub (CQ("="),      L(0), L(20));
      ss_luid    = chxalsub (CQ("("),      L(0), L(20));
      ss_ruid    = chxalsub (CQ(")"),      L(0), L(20));

      afm_name   = chxalloc (L(200), THIN, "mainglob afm_name");
   }

   /*** Init Site-Dependent Reg Info. */
   for (i=0;   i<10;   ++i)  if (NOT chxtype (sitevar[i])) {
      sitevar[i] = chxalloc (L(40), THIN, "sitevar");
      if (sitevar[i] != nullchix) chxclear(sitevar[i]);
   }
  
   SUCCEED;

 done:

   RETURN ( success );
}
