
/*** RESPLOOP.   Ask the user to RESPOND or (PASS) to an item.
/
/    ? = resploop (item, seen, high, back, attach);
/
/    Parameters:
/       int    item;        (item number)
/       int   *seen;        (resploop sets to highest resp # actually seen)
/       int   *high;        (resploop sets to high water mark for user)
/       int   *back;        (resploop sets to "# to go back")
/       int    attach;      (0=>no attach, 1=>show attach, 2=>askme)
/
/    Purpose:
/       Handle interaction with user at "RESPOND, PASS, etc." prompt.
/       after displaying (part of) an item.
/
/    How it works:
/       RESPLOOP handles the "RESPOND or (PASS)" loop after the user
/       has seen responses up through response number SEEN to ITEM.
/       At that point, the user may enter:
/          STOP    to examining items and RETURN to DO NEXT?
/          PASS    to pass on to the next item requested
/          RESPOND to add a response to this item
/          CHANGE  to change one of his responses
/          NEW     to leave new responses marked as "NEW"
/          FORGET  to forget this item
/          n       to see responses beginning with # n
/          LIFO    to see the responses in reverse order
/   
/       RESPLOOP will continue to loop and prompt the user until either
/       PASS or STOP is selected.  RESPLOOP also modifies SEEN, HIGH,
/       and BACK according to what the user has seen.  (See SHOWITEM
/       for details.)
/
/       If a particular response has an attachment, use the ATTACH
/       argument to decide what to do.
/
/    Returns: 1 if user finished viewing responses normally
/             0 if user indicated done viewing *all* responses
/               (e.g., typed STOP or pressed <CANCEL>.)
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  showitem()
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  resp/resploop.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CW  5/18/89 15:43 Add XW unit codes. */
/*: CR  9/05/89 15:14 Add code for LIFO. */
/*: CR  9/05/89 22:48 Add handling for SET MYTEXT options. */
/*: CR  9/15/89 17:11 Check thisuser.read_only. */
/*: CR 10/30/89 12:10 Remove backup_seen. */
/*: CR  3/04/90 22:40 Add context argument to enterresp() call. */
/*: CR  3/21/90 22:29 Write NEWPAGE before each show_response(). */
/*: CR  7/14/90 12:10 Use thisuser.is_organizer. */
/*: CX 10/20/91 12:23 Chixify. */
/*: CR  1/20/92 15:49 Add confnum to frozen() call. */
/*: DE  4/21/92 11:38 Matched call params for min & max */
/*: CX  6/05/92 16:23 XKEY --> u_keybd. */
/*: JX  6/19/92 12:15 Clear prompt after using it for PASS. */
/*: CR 10/08/92 15:22 Add logfeature() stuff. */
/*: TP 10/19/92 16:02 Use input_control(). */
/*: CR  1/07/93 16:24 Allow explicitly asking for response "0". */
/*: CP  6/27/93 19:02 Use new form of chg_rloop(), add header comments. */
/*: CP  8/24/93 15:45 Add 'attach' arg, pass to show_resp(). */
/*: CK  9/15/93 15:49 Use new get_resp_text()/enterres() calls. */
/*: CK  9/19/93  0:07 Add 'onlyattach' arg to show_resp() call. */
/*: CK  9/20/93 12:38 Add run_prompt_macro() for res_Fitemp. */
/*: CR 10/22/93 15:21 New rule for add new resp, SET MYTEXT NEVER. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"

#define  STOP  1
#define  PASS  2

extern struct master_template master;
extern struct screen_template used;
extern struct fresh_template  fresh;
extern struct partic_t        thisconf;
extern union  null_t          null;
extern Userreg                thisuser;
extern short                  confnum;
extern Chix                   confname;

FUNCTION  resploop (item, seen, high, back, attach)
   int    item, *seen, *high, *back, attach;
{
   static Chix rlooptab = nullchix;
   Chix        prompt, format, pass, reply, temp;
   int4        number;
   int         saw, choice, is_number, done, first, last, shown, got_text;

   ENTRY ("resploop", "");
   
   prompt = chxalloc (L(20), THIN, "resploop prompt");
   format = chxalloc (L(20), THIN, "resploop format");
   reply  = chxalloc (L(20), THIN, "resploop reply");
   pass   = chxalloc (L(20), THIN, "resploop pass");
   temp   = chxalloc (L(20), THIN, "resploop temp");

   /*** The default choice is "pass".  Get it from res_Arloop into PASS. */
   mdtable  ("res_Arloop", &rlooptab);
   chxtoken (prompt, nullchix, 3, rlooptab);
   chxbreak (prompt, pass, format, CQ("/"));

   frozen (confnum, item, 1);   /* Warn user if item is frozen. */

   /*** Prepare the RESP, PASS, etc. prompt. */
   mdstr (format, "res_Fitemp", null.md);
   chxclear  (prompt);
   chxformat (prompt, format, (int4) item, L(0), nullchix, nullchix);

   for (done=0;   NOT done;   ) {

      run_prompt_macro ("res_Fitemp", thisuser->dict, confname);
      choice = prompter (prompt, rlooptab, 1, "res_Hitemp", 40, reply, pass);
      sysbrkclear();

      is_number  = chxnum (reply, &number) /* &&  number */ ;
      used.lines = used.width = 0;

      /*** <CANCEL> and STOP mean stop looking at *any* responses. */
      if      (choice<=-3  ||  choice==0)     done = STOP;

      /*** A number means re-examine responses. */
      else if (choice==-1  &&  is_number)  {
         if   (number >= 0) first = min ( (int) master.responses[item],  
                                          (int) number);
         if   (number <  0) first = max ( (int) (master.responses[item] + 
                                          number+1), 1);
         unit_write (XWTX, NEWCPAGE);
         saw  = show_responses (XWTX, confnum, item, first, 
                     master.responses[item], 0, 0, attach, &shown, 0);
         if (shown > 0)
            logfeature (CQ("respread"), confname, L(item), L(0), L(shown), L(0));
        *seen = max (*seen, saw);
         if (*seen > *high)        { *back = 0;             *high = *seen; }
         if (*seen > *high - *back)  *back = *high - *seen;
      }

      /*** RESPOND means add a response to this item. */
      else if (choice== 1  &&  frozen (confnum, item, 1)) ;
      else if (choice== 1  &&  thisuser->read_only)
         mdwrite (XWTX, "ent_Ereadonly", null.md);
      else if (choice== 1) {
         got_text = 0;
         if (get_resp_text(1)  &&  
                 unit_lock (XITX, WRITE, L(0), L(0), L(0), L(0), CQ(""))) {
            got_text = enterresp (item, 0, XITX);
            unit_unlk (XITX);
         }
         if (NOT got_text)   mdwrite (XWTX, "ent_Tcancel", null.md);
         else {
            if (thisuser->mytext == NEVER) {
               if (*high == master.responses[item] - 1) 
                   *high =  master.responses[item];
               *back = 0;
            }
            if (thisuser->mytext == LATER) {
               if (*high == master.responses[item]-1) {
                  ++*high;
                  ++*back;
               }
               if (fresh.responses[item] == master.responses[item]-1) {
                  fresh.responses[item]++;
                  fresh.backresp [item]++;
               }
            }
         }
      }

      /*** PASS or <RETURN> means continue on to the next item. */
      else if (choice== 2)     done = PASS;

      /*** CHANGE means do CHANGE RESPONSE on this item. */
      else if (choice== 3  &&  thisuser->read_only)
         mdwrite (XWTX, "ent_Ereadonly", null.md);
      else if (choice== 3) {
         if (NOT thisuser->is_organizer  &&  master.nochange) {
            mdwrite (XWER, "chg_Tnochgr", null.md);
            continue;
         }
         chxtoken (temp, nullchix, 2, reply);
         first = 1;    last = master.responses[item];
         if (chxnum (temp, &number))  first = last = number;
         chg_rloop  (item, first, last, 1, XKEY);
      }

      /*** NEW means do not mark as "seen" any new responses to this item. */
      else if (choice== 4) {
         mdwrite (XWTX, "res_Tkeepnew", null.md);
         *seen = -1;
         *high = thisconf.responses[item];
         *back = thisconf.responses[item];
      }

      /*** FORGET means mark the item as forgotten. */
      else if (choice== 5)  {
         mdwrite (XWTX, "res_Tforgot", null.md);
         thisconf.responses[item] = -2;
         thisconf.backresp [item] =  0;
      }

      /*** LIFO means display responses in reverse order. */
      else if (choice == 6) {
         unit_write (XWTX, NEWCPAGE);
         saw  = show_responses (XWTX, confnum, item, 1, master.responses[item],
                                      1, 0, attach, &shown, 0);
         if (shown > 0)
            logfeature (CQ("respread"), confname, L(item), L(0), L(shown), L(0));
        *seen = max (*seen, saw);
         if (*seen > *high)        { *back = 0;             *high = *seen; }
      }

      if (master.responses[item] < 0)  done = PASS;
   }

   chxfree (prompt);
   chxfree (format);
   chxfree (reply);
   chxfree (pass);
   chxfree (temp);

   RETURN  (done==STOP ? 0 : 1);
}
