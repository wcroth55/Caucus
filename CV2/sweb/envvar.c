
/*** ENV_VAR.   Get an environment variable value.
/
/    env_var (value, name);
/   
/    Parameters:
/       char  *value;               (put returned value here)
/       char  *name;                (name of environment variable)
/
/    Purpose:
/       Get the value of an environment variable.
/
/    How it works:
/       If the variable doesn't exist, clear VALUE.
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  envvar.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/28/96 18:19 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  void env_var (char *value, char *name)
{
   char  *ptr, *getenv();

   ptr = getenv (name);
   if (ptr == NULL)  value[0] = '\0';
   else              strcpy (value, ptr);

   return;
}
