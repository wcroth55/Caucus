
/*** SYSPARMAIL.  Parse incoming CMI mail, send as Caucus message to user.
/
/    ok = sysparmail (file);
/
/    Parameters:
/       int     ok;        (success?)
/       int     file;      (unit number of file containing incoming CMI mail)
/
/    Purpose:
/       File unit FILE contains the text of incoming Unix or Vax/VMS mail.
/       Read and parse it.  Send each message in FILE to the current
/       user as a Caucus message, inserting the proper data for author and
/       subject.
/
/    How it works:
/       FILE must be locked (and unlocked) by the caller.
/
/    Returns: 1 if any mail was imported.
/             0 otherwise
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by: join()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  sys/sysparma.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  1/06/90 01:04 Created function. neither sysparmail or getfield done.*/
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "derr.h"

/*** State table values for parsing mail. */
#define NO_MESS  0
#define IN_HDR   1
#define IN_MSG   2

#define FIELDS   8  /* Number of header field types that Caucus imports. */

extern Userreg          thisuser;
extern Chix             ss_cmiesc;
extern union null_t     null;

FUNCTION  sysparmail (file)
   int   file;
{
   int   success = 0;

#if UNIX
   static Chix fieldstab, s_from;
   short  state, first_line, i;
   int    error, ctrla, sub_index;
   Chix   prevline, presline, author, subject, from_id, date, time,
          from, the_header[FIELDS], temp, prevtext, newtext;
   Chix   left_br, rite_br;
   int4   systime();

   ENTRY ("sysparmail","");

   date     = chxalloc ( L(40), THIN, "sysparma date");
   time     = chxalloc ( L(40), THIN, "sysparma time");
   temp     = chxalloc ( L(80), THIN, "sysparma temp");
   from     = chxalloc ( L(40), THIN, "sysparma from");
   author   = chxalloc (L(100), THIN, "sysparma author");
   from_id  = chxalloc (L(100), THIN, "sysparma from_id");
   subject  = chxalloc (L(100), THIN, "sysparma subject");
   presline = chxalloc (L(100), THIN, "sysparma presline");
   prevline = chxalloc (L(100), THIN, "sysparma prevline");
   prevtext = chxalloc (L(200), THIN, "sysparma prevtext");
   newtext  = chxalloc (L(200), THIN, "sysparma newtext");

   for (i=0;   i<FIELDS;   i++)
      the_header[i] = chxalloc (L(40), THIN, "sysparma presline");

   /*** The mail parser goes through the following steps:
   /     1) <FF> or "From id text" marks a new msg.
   /     2) Get Subject and Caucus_Name, if available.
   /     3) Copy rest of text into chix newtext.
   /     4) Call add_dmess() to actually send message. */

   /*** Get necessary strings from dictionary. */
   mdstr    (from, "mes_Kfrom", &s_from);
   mdtable  ("mes_A822maiflds", &fieldstab);   /* table of mail hdr fields */

   if (NOT unit_view (file))   FAIL;

   /*** Most of SYSPARMAIL is a big loop that parses the text in FILE
   /    one line at a time.  The initial state is NO_MESS, i.e. no
   /    message.  A <FF> (VV) or a line starting with "From" (UNIX)
   /    starts a new message, changing state to IN_HDR.  After the
   /    header is processed, the state changes to IN_MSG, until the
   /    next header is found. */
   state = NO_MESS;
   ctrla = 0;

   while (unit_read (file, presline, L(0))) {

      /*** "From" followed by at least two words starts a new message. */
      if (ctrla  ||  (chxindex (presline, L(0), from) == 0  &&
                      chxtoken (temp, nullchix, 3, presline) != -1 ) ) {

         /*** If we were already in a message, finish it and send it off. */
         if (state == IN_MSG) {
            sysdaytime (date, time,  0, systime());
            add_dmess (author, from_id, nullchix, subject, MSEND, date, time,
                       prevtext, 0, -1, thisuser->id, newtext,
                       (Attachment) NULL, &error);
            if (error == DBOXFULL)  mdwrite (XWER, "mes_Trecipfull", null.md);
         }

         if (ctrla)  { state = NO_MESS;   continue; }

         /*** Starting a new message.  Empty all header fields. */
         for (i=0;   i<FIELDS;   ++i)  chxclear (the_header[i]);

         /*** The_Header is an array of strings, one per possible message
         /    field.  Other fields may be added in the future: at the moment,
         /    the only defined fields are:
         /       0    sender address  ("Reply-To:")
         /       1    sender address  ("From:")
         /       2    sender address  ("Sender:")
         /       3    sender address  ("From")
         /       4    message subject ("Subject:")
         /       5    message subject ("Subj:")
         /       6    "to" field  (unused)
         /       7    "cc" field  (unused)  */

         /*** Read and process all the header lines.  Header stops at
         /    the first blank line. */
         state = IN_HDR;
         getfield (fieldstab, the_header, presline);
         while (unit_read (file, presline, L(0))) {
            chxtrim (presline);
            if (EMPTYCHX (presline))  break;
            getfield (fieldstab, the_header, presline);
         }

         /*** Starting a new message.  Get address of sender, and put
         /    it in FROM_ID (after the CMI prefix characters).  Prepare
         /    the SUBJECT and AUTHOR. */

         /*** Check, in order, the header fields "Reply-To:", "From:",
         /    "Sender:", and "From" for the RFC822 sender field,
         /    which is either "phrase <address>" or "address phrase".  */
         left_br = chxquick ("<", 2);
         rite_br = chxquick (">", 3);
         for (i=0;   i<=3;   ++i) if (NOT EMPTYCHX (the_header[i])) {
            if (chxinside (the_header[i], left_br, from_id, rite_br) >= 0)  break;
            chxtoken (from_id, nullchix, 1, the_header[i]);
            break;
         }
         /* What if there's no addressat all?  Is that possible? */
         chxconc   (from_id, ss_cmiesc, from_id);
         chxcpy    (author, from_id);

         /*** Check the possible subject fields, in order. */
         sub_index = (EMPTYCHX (the_header[4]) ? 5 : 4);
         chxcpy (subject, the_header[sub_index]);

         state = IN_MSG;
         first_line = 1;
         chxclear (newtext);
      }

      /*** Processing regular message text lines.
      /    UNIX reads a line and writes the previous line, so that it
      /    skips the nasty "Message #" line that preceeds each new message.
      /    But it means that after reading the first text line, we must do
      /    nothing.
      /
      /    Vax/VMS just reads and writes each line, as it depends on the
      /    formfeed to start a new message. */
      else if (state == IN_MSG) {
         if (first_line)  first_line = 0;
         else {
            chxcatval (newtext, THIN, (int4) ' ');
            chxcat    (newtext, prevline);
            chxcatval (newtext, THIN, (int4) '\n');
         }
         chxcpy (prevline, presline);
      }
   }

   /*** Message processing over, clean up.  If we're at the end of the
   /    last message, send it out too. */
   success = (state == IN_MSG  ||  state == IN_HDR);
   if (success) {
      sysdaytime (date, time,  0, systime());
      add_dmess (author, from_id, nullchix, subject, MSEND, date, time,
                 prevtext, 0, -1, thisuser->id, newtext,
                 (Attachment) NULL, &error);
      if (error == DBOXFULL)  mdwrite (XWER, "mes_Trecipfull", null.md);
   }

   unit_close (file);

done:
   chxfree (date);       chxfree (time);
   chxfree (temp);       chxfree (from);
   chxfree (author);
   chxfree (from_id);    chxfree (subject);
   chxfree (presline);   chxfree (prevline);
   chxfree (newtext);    chxfree (prevtext);
   RETURN (success);
#else
   return (1);
#endif

}


#if UNIX

/*** GETFIELD extracts message field information from LINE.
/
/    If LINE begins with one of the field names in FIELDTAB, and
/    the matching field number in HEADER is empty, GETFIELD
/    extracts the field information, and places it in HEADER.
/
/    Returns 1 on success, and 0 otherwise. */

FUNCTION  getfield (fieldtab, header, line)
   Chix fieldtab, header[], line;
{
   int    success;
   short  fieldnum;
   Chix   field, t0, t1;

   field = chxalloc (L(160), THIN, "getfield field");

   /*** Get the field label off of this line.  
   /    Get the field index in the master table of message fields. */
   t0 = chxquick ("\t", 0);
   t1 = chxquick (" ",  1);
   while (chxalter (line, L(0), t0, t1)) ;
   chxtoken  (field, nullchix, 1, line);
   jixreduce (field);

   fieldnum = tablefind (fieldtab, field);

   /*** If this is not a valid field, or one we already have data for,
   /    return failure. */
   if (fieldnum < 0)                     FAIL;
   if (NOT EMPTYCHX (header[fieldnum]))  FAIL;

   /*** Copy the text of the field into this slot in HEADER. */
   chxtoken (nullchix, header[fieldnum], 2, line);
   SUCCEED;

done:
   chxfree (field);
   return  (success);
}
#endif
