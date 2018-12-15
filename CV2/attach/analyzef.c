/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** ANALYZE_FILE.  Determine what type of file this is from its magic #.
/
/    def = analyze_file (filename, filetype);
/
/    Parameters:
/       Attach_Def def;        (ptr to definition for this format)
/       Chix       filename;   (file to analyze)
/       Chix       filetype;   (caller thinks this is the file type)
/
/    Purpose:
/       When a user attempts to display a file attachment, it needs to
/       know what type of file it is, so it can use an appropriate
/       application.  Caucus stores the format type in the DB, so we
/       determine the file type when it is being stored in the DB.
/       analyze_file() attempts to determine the file type.
/
/    How it works:
/       analyze_file() looks at the first few bytes of the file and compares
/       them against the ones stored in the linked-list of Format Definitions.
/
/       If FILETYPE is not empty, analyze_file() uses this information
/       as an educated guess by the caller (user) as to what the file type
/       might be.
/
/    Returns: NULL if file doesn't exist or not a recognized format
/             ptr to a Format Definition
/
/    Error Conditions:
/
/    Side effects:
/
/    Related Functions: parse_attach_map()
/                       ok_to_display_attach()
/
/    Called by: UI
/
/    Operating System Dependencies:
/
/    Known Bugs
/       For now, ignores FILETYPE information supplied by caller.
/
/    Home: attach/analyzef.c
/
/ */

/*: JV  2/25/93 12:13 Create this. */
/*: JV  5/25/93 17:47 Bail out if file doesn't exist. */
/*: JV  8/10/93 18:00 Turn off sysprv at all exits. */
/*: CP  8/12/93 14:17 Add 'filetype' arg; always skip empty header node. */
/*: CP  8/30/93 16:15 Remove sysprv() calls, add asc/bin type handling. */
/*: JZ 10/05/93 12:27 Check for tab when determining ascii/binary. */

#include <stdio.h>
#include <sys/types.h>
#include "handicap.h"
#include "unitcode.h"
#include "attach.h"
#define  ADNULL   ( (Attach_Def) NULL)
#define  BUFSIZE  1024

extern Attach_Def attach_def;

FUNCTION Attach_Def analyze_file (filename, filetype)
   Chix filename, filetype;
{
   char       buffer[BUFSIZE+8];
   Attach_Def def;
   int        bytes, pos, datatype;

   if (attach_def == ADNULL)  return (ADNULL);

   if (NOT unit_lock (XIRE, READ, L(0), L(0), L(0), L(0), filename))  return (ADNULL);
   if (NOT unit_b_view (XIRE, L(0), 0)) {
      unit_unlk (XIRE);
      return    (ADNULL);
   }
   bytes = unit_b_read (XIRE, buffer, L(BUFSIZE));
   unit_close  (XIRE);
   unit_unlk   (XIRE);

   /* Look for exact match.   (Skip empty header node.) */
   for (def=attach_def->next;   def != ADNULL;   def = def->next)
      if (def->magic_size && !memcmp (buffer, def->magic, def->magic_size))
         return (def);

   /*** Decide if file is ascii or binary. */
   for (datatype=DATA_ASCII, pos=0;   pos<bytes;   ++pos) {
      if (buffer[pos] == '\n')                          continue;
      if (buffer[pos] == '\r')                          continue;
      if (buffer[pos] == '\t')                          continue;
      if (buffer[pos] >= ' '  &&  buffer[pos] <= '~')   continue;
      datatype = DATA_BINARY;
      break;
   }

   /* Look for matching file extension. */

   /*** No matching definition found so far.  Find remaining 
   /   "magic-number-less" definition, that also has the proper 
   /   type (binary vs ascii). */
   for (def=attach_def->next;   def != ADNULL;   def = def->next)
      if (def->magic_size == 0  &&  def->data_type == datatype)   return (def);

   return (ADNULL);
}
