
/*** SAY_NO_FILE.  Tell browser user this file does not exist! 
/
/    say_no_file (cd, cmlname, conf);
/   
/    Parameters:
/       Hose                cd;      (connection descriptor, output goes here)
/       char               *cmlname; (name of CML file)
/       struct sweb_conf_t *conf;    (server configuration info)
/
/    Purpose:
/       Tell browser user the desired CML file does not exist.
/       (Instead of just giving the cryptic "500 Server error"!)
/
/    How it works:
/
/    Known bugs:
/
/    Home:  sweb/saynofile.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/25/96 15:03 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "diction.h"

extern FILE *sublog;
extern int   cml_debug;

FUNCTION  say_no_file (
   Hose                cd,
   char               *cmlname,
   struct sweb_conf_t *conf )
{
   Chix   a;
   ENTRY ("say_no_file", "");

   a = chxalloc (L(200), THIN, "say_no_file a");

   chxofascii (a, 
       "Content-type: text/html\n"
       "\n"
       "<HTML>\n"
       "<HEAD><TITLE>No Such File</TITLE></HEAD>\n"
       "<BODY BGCOLOR=\"#FFFFFF\">\n"
       "<H2>No Such File</H2>\n"
       "<P>\n"
       "The link you just selected points to a CML file that does not\n"
       "exist.  The file is called <B>");
   chxcat (a, CQ(cmlname));
   chxcat (a, CQ(
       "</B>.\n"
       "<P>\n"
       "Please inform the Caucus manager or system administrator.\n"
       "<P><HR>\n"
       "</BODY>\n"
       "</HTML>\n"));
   
   convert_and_write (cd, a, L(0), conf);

   chxfree (a);
   RETURN (1);
}
