/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** ADDXMESSAGE.  Create a NEW MESSAGE xaction.
/
/   int n = addxmessage (author_name, author_id, subject, type,
/                 corig_text, corig_parts, rrsize, mailctrl,
/                 recip_name, recip_id, recip_chn, ctext, date, time, error)
/
/   Parameters:
/      Chix  author_name;   (name of author of message)
/      Chix  author_id;     (id of author, or "id@host")
/      Chix  subject;       (subject of message)
/      int   type;          (MSEND, MREPLY, MFORWARD)
/      Chix  corig_text;    ("original" text)
/      int   corig_parts;   (# of message parts in corig_text)
/      int   rrsize;        (size of return receipt requested; -1 for none)
/      short mailctrl;      (indication of exceptional mail)
/      Chix  recip_name;    (recipient name)
/      Chix  recip_id;      (recipient id)
/      Chix  recip_chn;     (recipient host)
/      Chix  ctext;         ("new" text of message)
/      Chix  date;          (date message sent)
/      Chix  time;          (time message sent)
/      int  *error;         (place for returned error code)
/ 
/   Purpose:
/      Caucus_x should call this to generate a xaction and append it
/      to a neighbor's XHML file so that this message gets routed to
/      its destination.
/      It does not affect the local Caucus DB in any way.
/
/   How it works:
/      addxmessage is very simple: it fills in a XACTION structure
/      and then calls putnextxact() to write out the xaction.
/
/      Some non-obvious arguments:
/         AUTHOR_ID: this is mostly obvious: for new messages sent from
/            this host, AUTHOR_ID is just a userid.  For messages created
/            on another host, however, AUTHOR_ID is already in the form
/            userid@CHN.  ADDXMESSAGE detects these by looking for an "@".
/         TYPE: one of MSEND, MREPLY, MFORWARD (in mess.h)
/            This indicates to the XFU at the destination what type
/            of message this is aint4 this axis.
/         MAILCTRL: one of 0 (zero), MNONE, MONE, MMANY, MFULL, MNOCHN.
/            This is used with the xaction type-modifier REJECT.  It
/            indicates to the XFU at the destination why this message
/            was rejected.  (0 & MONE aren't actually used for this purpose.)
/           MNONE indicates that there was no recipient that matched the
/            RECIP_NAME, RECIP_ID tuple at the host RECIP_CHN.
/           MFULL indicates that the recipient was found but the mailbox
/            was full.
/           MMANY indicates that two or more recipients were found that
/            matched the RECIP_NAME, RECIP_ID tuple at the host
/            RECIP_CHN.  In this case the original message should be put
/            into the CORIG_TEXT Chix and the list of the matching people
/            should be put into the CTEXT Chix, one per line.
/           MNOCHN usually indicates a DB problem.  A message gets routed
/            from one host to the next by each host's local routing table.
/            Anthropomorphically, the message "sniffs out" its destination,
/            discovering how to get there one clue at a time.  If it ever
/            "loses the scent", it heads back to its sender with MNOCHN set.
/            (This has not actually been implemented yet, and may not ever.)
/
/   CHIX Warning: we actually use "@" in this code.
/
/   Returns:  1 on success.  (Sets ERROR to NOERR.)
/             0 on error. (See below)
/
/   Error Conditions: logs error in bugfile, and sets ERROR to:
/      DNOCHN    (No CHN supplied?)
/      DLOCK     (database locking error?)
/      DFILE     (couldn't open some file)
/      DMISC     (couldn't get XSNF.)
/
/   Related Functions:
/
/   Called by: mess_forward(), mess_parse(), mess_send()
/
/   Home: xaction/addxmess.c
/ */

/*: JV  6/05/91 12:32 Create this. */
/*: JV  6/10/91 18:16 Change arg list, add code */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 18:59 Cleaned, changed get_chn call. */
/*: JV  7/04/91 19:10 Add mailctrl arg, move get_chn. */
/*: JV  7/09/91 13:42 Rewrite completely to use putnextxact() */
/*: CR  7/11/91 17:03 Added error conditions and documentation.  Fix header!*/
/*: JV  7/15/91 19:43 Add multiple text fields. */
/*: JV  7/19/91 12:25 Clean lint filter. */
/*: JV  7/22/91 16:31 Add ENTRY */
/*: CR  7/27/91 14:16 Include "goodlint.h". */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: JV  7/30/91 08:52 De-Lintify */
/*: JV 10/08/91 15:00 Don't ReFree chixen. */
/*: JV 10/24/91 14:33 Fix multiple-hop sends. */
/*: JV 11/06/91 10:29 Change common.h to null.h. */
/*: JV 12/03/91 13:25 Allow initial substring CHN matches. */
/*: JV 12/07/91 19:56 Declare chxalloc. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: CR 12/12/91 12:39 Add header comments for arguments. */
/*: CR  8/24/92 17:24 Fix "nested" comments. */
/*: JX  5/29/92 16:50 Chixify. */
/*: JX  8/01/92 12:30 Change empty_xact to make_xaction(). */
/*: CI 10/06/92 15:58 Chix Integration. */
/*: CP  8/18/93 13:18 Change double ? comments to singles.  (Trigraph.  Ugh.) */

#include <stdio.h>
#include "handicap.h"
#include "done.h"
#include "derr.h"
#include "chixuse.h"
#include "null.h"
#include "unitcode.h"
#include "mess.h"
#include "xaction.h"
#include "goodlint.h"

FUNCTION addxmessage (author_name, author_id, subject, type,
            corig_text, orig_parts, rrsize, mailctrl,
            recip_name, recip_id, recip_chn, ctext, date, time, error)
   int   type, orig_parts, rrsize, *error;
   Chix  author_name, author_id, subject, recip_name, recip_id, recip_chn,
         date,  time, corig_text, ctext;
   short mailctrl;
{
   int  lhnum, success;
   Chix mychn, auth_id, auth_chn, carray[2];
   Xactionptr xact, make_xaction();

   ENTRY ("addxmessage", "");

   mychn    = chxalloc (L(80), THIN, "addxmess mychn");
   auth_id  = chxalloc (L(20), THIN, "addxmess auth_id");
   auth_chn = chxalloc (L(80), THIN, "addxemss auth_chn");
  
   *error = NOERR;

   if (NOT get_chn (mychn, XSNF, 0)) {
      bug ("ADDXMESS: couldn't get XSNF.", 0);  *error = DMISC;  FAIL; }

   xact = make_xaction();

   xact->typenum = TT_MESSAGE;
   xact->actnum  = (mailctrl == 0) ? XT_NEW : XT_REJECT;

   /*** Concatenate mychn to author_id for msgs sent from here.
   /    Msgs sent from other hosts already have a CHN! */
   if (chxbreak (author_id, auth_id, auth_chn, CQ("@")))
      chxcpy (xact->uid, author_id);
   else
      chxformat (xact->uid, CQ("%s@%s"), L(0), L(0), author_id, mychn);
   chxcpy (xact->uname, author_name);
   chxcpy (xact->rechost, recip_chn);
   chxcpy (xact->recid,   recip_id);
   chxcpy (xact->recname, recip_name);
   chxcpy (xact->title,   subject);
   chxformat (xact->date,   CQ("%s %s"), L(0), L(0), date, time);
   xact->mtypenum = type;
   xact->m_oparts = orig_parts;
   xact->rrlength = rrsize;
   xact->mailctrl = mailctrl;
   xact->datasize[0] = chxlen (ctext);
   xact->datasize[1] = chxlen (corig_text);

   if (NOT get_course (recip_chn, &lhnum)) {
      bug ("ADDXMESS: host not found.", 0);
      logx ("ADDXMESS: CHN not in database.", xact);
      *error=DNOCHN; FAIL; }

   /*** If a CHN's initial substring was specified, GET_COURSE will
   /    fill in RECIP_CHN. */
   chxcpy (xact->rechost, recip_chn);

   if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), nullchix)) {
      bug ("ADDXMESS: can't lock XHML.", 0); *error=DLOCK;  FAIL; }
   if (NOT unit_append (XHML)) {
      bug ("ADDXMESS: can't open XHML.", 0); *error=DFILE;
      unit_unlk (XHML);  FAIL; }

   if (xact->datasize[0]) carray[0] = ctext;
   else                   carray[0] = chxalloc (L(0), THIN, "addxmess carray[0]");
   if (xact->datasize[1]) carray[1] = corig_text;
   else                   carray[1] = chxalloc (L(0), THIN, "addxmess carray[1]");
   putnextxact (XHML, xact, carray);
 
   unit_close (XHML);  unit_unlk (XHML);

   if (NOT xact->datasize[0]) chxfree (carray[0]);
   if (NOT xact->datasize[1]) chxfree (carray[1]);

   SUCCEED;

done:
   chxfree (mychn);
   chxfree (auth_id);
   chxfree (auth_chn);
   free_xaction (xact);
   RETURN (success);
} 
