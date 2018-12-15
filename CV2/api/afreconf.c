/*** A_FRE_CONF.   Free (deallocate) an Conf object.
/
/    a_fre_conf (conf);
/
/    Parameters:
/       Conf     conf;   (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_conf().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if conf is null or not an Conf.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_conf()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afreconf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/15/92 17:47 New function. */
/*: CR 12/03/92 12:53 sysfree (conf), as well! */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  a_fre_conf (Conf conf)
{
   if (conf == (Conf) NULL)  return (0);
   if (conf->tag != T_CONF)  return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) conf);
   conf->tag = 0;
   chxfree    (conf->lname);
   chxfree    (conf->owner);
   a_fre_text (conf->intro);
   a_fre_text (conf->greet);
   a_fre_text (conf->userlist);

   sysfree ( (char *) conf);

   return  (1);
}
