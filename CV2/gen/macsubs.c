/*** MAC_SUBS.  CV2.2 Macro variable substitution.
/
/    MAC_SUBS takes a string COMMAND, and performs
/    macro variable substitution on it.  (E.G., $(userid) is
/    expanded into the user's userid, and so forth.)  The command
/    line arguments $1, $2, etc. are expanded using the words in
/    the string COMLINEARGS.
/
/    The expanded form of COMMAND is placed in a new string
/    that is allocated off the heap.  MAC_SUBS returns a pointer
/    to this new string.  The original COMMAND string is left
/    unmodified.  It is the responsbility of the caller to sysfree()
/    the new string when the caller is finished with it.
/
/    MAC_SUBS calls var_substitute(), which does most of the
/    actual work.
/
/    The caller is responsible for deallocating the RESULT.
/
/    Note: argument COMLINEARGS may be a nullchix.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/11/89 10:40 New function to replace mac_expand(). */
/*: CR  9/25/89 11:14 Mods to do expansion during interpret(). */
/*: CR  4/18/90 23:03 Expand esc_backsl, esc_dollar. */
/*: JV  8/06/90 20:25 Add more docs, Site-Dependent Reg Info. */
/*: JX  5/21/92 13:56 Chixify. */
/*: CX  6/22/92 10:14 Remove 3rd arg from var_subs() call. */
/*: CR 10/11/92 22:06 Add comments. */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CP  8/06/93 15:28 Add 'master' arg. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  Chix mac_subs (command, comlineargs, master)
   Chix   command, comlineargs;
   Master master;
{
   static Chix esc_backsl;
   static Chix esc_dollar;
   Chix   result;

   ENTRY ("mac_subs", "");

   result     = chxalloc (L(80), THIN, "macsubs result");

   if (NOT chxtype(esc_backsl)) {
      esc_backsl   = chxalloc (L(5), THIN, "macsubs esc_backsl");
      chxofascii (esc_backsl, "\001\001\001");
   }

   if (NOT chxtype(esc_dollar)) {
      esc_dollar   = chxalloc (L(5), THIN, "macsubs esc_dollar");
      chxofascii (esc_dollar, "\002\002\002");
    }

   /*** "Hide" escaped characters for the moment. Must hide two bkslashes 
   /    before hiding "\$" to prevent "\\$(u1)" from being  */
   while (chxalter (command, L(0), CQ("\\\\"), esc_backsl)) ;
   while (chxalter (command, L(0), CQ("\\$"),  esc_dollar)) ;

   /*** Copy the original string into it. */
   chxcpy  (result, command);

   /** Now substitute in the variables, and replace the escaped characters. */
   var_substitute  (result,  comlineargs, master);
   while (chxalter (result,  L(0), esc_dollar, CQ("$") )) ;
   while (chxalter (command, L(0), esc_dollar, CQ("$") )) ;
   while (chxalter (result,  L(0), esc_backsl, CQ("\\"))) ;
   while (chxalter (command, L(0), esc_backsl, CQ("\\"))) ;

   RETURN (result);
}
