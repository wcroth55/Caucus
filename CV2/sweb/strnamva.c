/*** STR_NAM_VALUE.   Pluck 'name="value"' pairs out of a string.
/
/    found = str_nam_value (value, name, text);
/ 
/    Parameters:
/       int    found;      (Was NAME found in TEXT?)
/       char  *value;      (place "value" string here)
/       char  *name;       (NAME we're looking for)
/       char  *text;       (text to look through)
/
/    Purpose:
/       Find a 'name="value"' string inside TEXT, and extract VALUE.
/
/    How it works:
/
/    Returns: 1 if 'name=' appears in TEXT
/             0 otherwise
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  strnamva.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/06/96 20:26 New function, derived from old readform.c */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"


FUNCTION  str_nam_value (char *value, char *name, char *text)
{

   char   find[200];
   int    pos, findlen, len;

   value[0] = '\0';

   strcpy (find, name);
   strcat (find, "=\"");
   findlen = strlen(find);

 
   if ( (pos = strindex (text,             find)) < 0)  return (0);

   if ( (len = strindex (text+pos+findlen, "\"")) < 0)  return (0);

   strsub (value, text, pos+findlen, len);

   return (1);
}
