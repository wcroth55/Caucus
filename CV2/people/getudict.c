/*** GET_U_DICTI.  Get the user's dictionary number.
/
/  int dicti = get_u_dicti (user)
/
/  Parameters:
/     Chix user;     Userid of user
/
/  Purpose:
/     Often we need to know what dictionary a user is using.  This is
/     the object-oriented method. 
/
/  How it works
/     It calls get_person() to load the information and then returns
/     the value.  
/
/  Returns:
/     -1 if couldn't get person (doesn't exist or DB error)
/      n number of dictionary that person uses
/
/  Related Functions: get_person()
/
/  Home: people/getudict.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  7/16/91 10:11 Create function. */
/*: JV  8/13/91 15:00 Add ENTRY, add memory. */
/*: CR  8/22/91 11:19 Declare bugtell(). */
/*: CR  1/07/92 15:21 Remove setdecode(). */
/*: DE  4/10/92 14:56 Chixified */
/*: JV 10/21/92 17:06 Fixify. */
/*: CL 12/10/92 17:06 Long ascofchx args. */

#include <stdio.h>
#include "caucus.h"

extern int debug;

FUNCTION  get_u_dicti (user)
   Chix  user;
{
   static Userreg    ureg = (Userreg) NULL;
   Userreg make_userreg();
   char  userstr[80];
   char  *bugtell();

   ENTRY ("get_u_dicti", "");

   if (ureg == (Userreg) NULL)
      ureg = make_userreg ();

   if (NOT chxeq (user, ureg->id)) {
      if (NOT get_person (user, ureg)) {
         ascofchx (userstr, user, L(0), L(80));
         if (debug) bugtell ("get_u_dicti: Couldn't get_person", 0, L(0),
                             userstr);
         RETURN (-1);
      }
   }
   RETURN (ureg->dict);
}
