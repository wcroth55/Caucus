
/*** READ_FORM_DATA.   Read any "forms" data from the client.
/
/    read_form_data (cd, form, conf);
/ 
/    Parameters:
/       Hose                cd;     (connection descriptor)
/       Vartab              form;   (variable list for forms data)
/       struct sweb_conf_t *conf;   (server configuration info)
/
/    Purpose:
/       Read any "forms" data supplied by the client, parsing it
/       and storing it in the variable list FORM.
/
/    How it works:
/       Note that if a field appears more than once in the forms data,
/       the values are concatenated (with separating spaces).
/
/    Returns:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  readform.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/03/95 22:56 New function. */
/*: CR  9/01/95 17:04 Allow multiple values for a field. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "diction.h"
#include "api.h"

#define  TEMPMAX    1024
#define  ST_NAME       1
#define  ST_VALUE      2

extern int   cml_debug;

FUNCTION  read_form_data (cd, form, conf)
   Hose   cd;
   Vartab form;
   struct sweb_conf_t *conf;
{
   static Chix work = (Chix) NULL;
   char   temp[TEMPMAX+2];
   char   mheader[200];
   int4   content_length;
   int    num;
   int    begin, ok;
   ENTRY ("read_form_data", "");

   /*** Always clear old forms data variables. */
   for (num=0;   num < form->used;   ++num)  {
      chxclear (form->name [num]);
      chxclear (form->value[num]);
   }

   /*** Get the CONTENT_LENGTH from the client, and see if we need to
   /    actually do anything. */
   hose_read (cd, temp, 8, 0);
   temp[8] = '\0';

   sscanf (temp, "%d", &content_length);
/* if (cml_debug) fprintf (sublog, "content_length = %d\n", content_length); */
   if (content_length == 0)  RETURN(1);

   /*** OK, we do have forms data, allocate/clear some working chix. */
   if (work == (Chix) NULL) {
      work  = chxalloc (L(400), THIN, "read_form work");
   }
   chxclear (work);


   /*** Read all the forms data from the client, and append it to
   /    the WORKing chix.   Append a terminating '&' to force closure
   /    of the last forms data field. */

   
   /*** Read the first "line" of forms data from the client.  */
   ok = read_fline (cd, 1, content_length, temp, TEMPMAX, &begin);

   /*** If it starts with dashes, this is a multipart form.  Otherwise
   /    it is a single-part form. */
   if (temp[0] == '-')  {
      strcpy (mheader, temp);    /* Save the original multipart header. */
      form_multi (cd, content_length, temp, TEMPMAX, mheader, conf, form);
   }

   /*** Otherwise, this is a single-part form. */
   else {
      /*** Read all of the data into WORK. */
      do {
         chxcat (work, CQ(temp));
/*       if (cml_debug)  fprintf (sublog, "%s", temp); */
      } while (read_fline (cd, 0, content_length, temp, TEMPMAX, &begin) >= 0) ;

      /*** Close it with a final "&", and process it into form variables. */
      chxcatval   (work, THIN, '&');
      form_single (work, form, conf);
   }

/* if (cml_debug) {
      fprintf (sublog, "\n");
      for (num=0;   num < form->used;   ++num) {
         fprintf (sublog, "FORM: %s = (%d)",
                  ascquick(form->name[num]), chxtype(form->value[num]));
         fprintf (sublog, " %s\n",      ascquick(form->value[num]));
      }
      fflush (sublog);
   } */


   RETURN (1);
}
