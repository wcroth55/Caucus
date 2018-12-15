/*** A_INIT.   Initialize the Caucus API.
/
/    ok = a_init (initapi);
/
/    Parameters:
/       int       ok;       (initialization successful?)
/       Initapi   initapi;  (object containing initialization values)
/
/    Purpose:
/       Initialize the API so that the other API functions may be used.
/
/    How it works:
/       Caller must fill in initapi->confid to be the pathname of the
/       Caucus database directory.
/
/       If this is a text-terminal, caller must set initapi->terminal to 1.
/
/       Caller *may* fill in tbufs and masters members with size
/       of respective caches.
/
/       Caller *may* fill in userid member.  In that case, Caucus acts
/       as if it is running "as" that userid.  Warning: it is up to
/       the calling application to determine that the real user is
/       allowed to "act as" that userid!
/
/       A_init() calls/loads a considerable amount of the Caucus command
/       line interface code & data structures.  Some of these may be
/       trimmed off someday.
/
/       If a_init() is called more than once, just change the stored userid
/       to match what is in 'initapi' -- don't do anything else. 
/
/    Returns: A_OK on success
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/       Japanese compatibility has not yet been tested... although
/       the code is all there.
/
/    Home:  api/ainit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/12/92 17:47 New function. */
/*: CR 11/16/92 13:03 Call unitglobals(), mainglobals(). */
/*: CR 11/27/92 18:49 Needs xfpglobals(). */
/*: CR 11/29/92 18:05 Initialize screen, used. */
/*: CR 12/04/92 14:35 Make systurn a no-op. */
/*: JV 12/07/92 13:50 Use sysglobals. */
/*: CR 12/10/92 20:43 Add sysainit() calls. */
/*: CR 12/13/92 21:55 Add comments. */
/*: CR  1/19/93 13:10 Add license stuff. */
/*: CR  2/23/93  0:11 Add chx_init() call. */
/*: CR  2/28/93 21:56 Add a_cch_tbuf initialization. */
/*: CK  3/01/93 16:50 Comment out a_cch_master() for now. */
/*: JV  6/09/93 11:57 Add support for text terminals. */
/*: CP  8/01/93 10:32 Add api_debug. */
/*: CR  3/28/95  8:03 Add initapi.userid stuff. */
/*: CR  8/27/95 20:06 Add japanese language handling. */
/*: CR  4/02/02 12:40 Allow multiple calls. */
/*: CR 12/11/02 Always call last_on(), even in a second call to a_init(). */

#include <stdio.h>
#include "chixuse.h"
#include "caucus.h"
#include "api.h"
#include "text.h"
#include "units.h"

extern int                     debug;
extern Chix                    confid, confname;
extern struct screen_template  screen, used;
extern struct termstate_t      term_in_caucus;
extern struct flag_template    flag;

int                            api_debug;  /* API internal debugging flag. */

FUNCTION  a_init (Initapi initapi)
{
   static   int first_time = 1;
   Textbuf  a_cch_tbuf();

   if (first_time) {
      debug     = 0;
      api_debug = 0;
      screen.lines =  0;    used.lines = 0;
      screen.width = 80;    used.width = 0;
   
      /*** Initialize system-dependent stuff. */
      sysainit (0);
   
      confid = chxalloc (L(80), THIN, "a_init confid");
      chxcpy (confid, initapi->confid);
   
      confname = chxalloc (L(32), THIN, "a_init confname");
   }

   if (NOT NULLSTR(initapi->userid)) {
      strcpy (flag.userid, initapi->userid);
      flag.login = 1;
   }

   if (first_time) {
      first_time = 0;

      /*** If not a text terminal, don't fiddle with terminal settings. */
      if (NOT initapi->terminal) systurn (&term_in_caucus, IS_AFILE, ON);
   
      /*** This may be needed to initialize flag.  Then again, I hope not. */
      /* argchk  (argc, argv, flagstart);  */
   
      utilglobals ();
   
      /*** Handle ascii vs. japanese character sets for disk data. */
      unit_control (0, UNIT_SET + initapi->diskform + 1);
      if (initapi->diskform >= 2) {
         chx_init (JIX);
         flag.japan = 1;
      }
      else  chx_init (ASCII);
   
      /*** System dependent stuff should be pulled out of sysexit() and
      /    placed into sysainit.  Some of this (Unix umask) has already
      /    been done. */
      /* sysexit (-1); */
   
      unitglobals();
   
      if (NOT mainglobals()) return (A_DBERR);
   
      sysglobals(); 
      xfpglobals();
   
      /*** Initialize size of API tbuf cache. */
      a_cch_tbuf   (V_SIZE, initapi->tbufs, 0, 0, 0, 0, nullchix);
      /* a_cch_master (V_SIZE, initapi->masters); */
   }

   last_on (1);
   return (A_OK);
}
