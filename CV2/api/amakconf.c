/*** A_MAK_CONF.   Make (allocate) an Conf object.
/
/    conf = a_mak_conf (name);
/
/    Parameters:
/       Conf   conf;    (returned/created object)
/       char  *name;    (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize a Conf object.
/
/    How it works:
/
/    Returns: newly created object
/             NULL on error
/
/    Error Conditions: insufficient heap memory, or heap catastrophe.
/  
/    Side effects: 
/
/    Related functions:  a_fre_conf()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakconf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/15/92 17:47 New function. */
/*: JV  2/24/93 17:08 Add ATTACH. */
/*: CP  8/01/93  9:37 Add 'name' argument, debugging stuff. */
/*: CP  5/04/94  0:35 Initialize 'library' member! */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  Conf  a_mak_conf (char *name)
{
   Conf  conf;

   conf = (Conf) sysmem (sizeof (struct Conf_t), "Conf_t");
   if (conf == (Conf) NULL)  return (conf);

   conf->tag      = T_CONF;
   A_SET_NAME (conf->dname, name);
   conf->lname    = chxalloc (L(20), THIN, "a_mak_co lname");
   conf->owner    = chxalloc (L(20), THIN, "a_mak_co owner");
   conf->intro    = a_mak_text();
   conf->greet    = a_mak_text();
   conf->userlist = a_mak_text();
   conf->cnum     = 0;
   conf->change   = 0;
   conf->add      = 0;
   conf->visible  = 0;
   conf->subject  = 0;
   conf->ready    = 0;
   conf->attach   = 0;
   conf->library  = 0;

   if (api_debug) a_debug (A_ALLOC, (Ageneric) conf);

   return (conf);
}
