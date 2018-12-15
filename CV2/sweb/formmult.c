/*** FORM_MULTI.    Process result of a multi-part form.
/
/    form_multi (cd, con_len, line, linemax, mheader, conf, form);
/ 
/    Parameters:
/      Hose                cd;       (connection descriptor)
/      int4                con_len;  (content length of form data)
/      char               *line;     (temporary data assembly area)
/      int                 linemax;  (max size of LINE)
/      char               *mheader;  (multi-part form header)
/      Vartab              form;     (forms data)
/      struct sweb_conf_t *conf;     (server configuration info)
/
/    Purpose:
/       Process the resultant data from a multi-part form, 
/       and store the results in the forms variables in FORM.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  formmult.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/06/96 20:26 New function, derived from old readform.c */
/*: CR  3/28/00 15:25 Replace "bad" chars in uploaded file name. */

#include <stdio.h>
#include <fcntl.h>
#include "fcntl2.h"
#include "chixuse.h"
#include "sweb.h"
#include "diction.h"
#include "api.h"

#define   DONE   goto done

FUNCTION  form_multi (cd, con_len, line, linemax, mheader, conf, form)
   Hose                cd;
   int                 linemax;
   int4                con_len;
   char               *line, *mheader;
   Vartab              form;
   struct sweb_conf_t *conf;
{
   Chix   name, name2;
   char   namestr[300], filestr[300], savefile[300];
   char   eheader[200];
   char   temp[1000];
   char  *ps;
   int    lang, ok, begin, slot, first, fd, eof, fn, trim, ptr;
   char  *form_temp();
   ENTRY ("form_multi", "");

   strcpy (eheader, mheader);
   strcat (eheader, "--");

   name  = chxalloc (L(30), THIN, "form_mult name");
   name2 = chxalloc (L(30), THIN, "form_mult name2");
   lang = conf->browserform+1;

   /*** For each part of a multi-part form... */
   while (1) {
      
      /*** Read all content description lines, up through a blank line,
      /    and parse out field name (and file name, if a file). */
      savefile[0] = '\0';
      chxclear (name);
      while (1) {
         ok = read_fline (cd, 0, con_len, line, linemax, &begin);
         if (ok <  0)  DONE;
         if (ok == 0)  break;
         if (str_nam_value (namestr, "name",     line))  
             chxofascii (name, namestr);
         if (str_nam_value (filestr, "filename", line)) {
            if (NOT NULLSTR(filestr))    strcpy (savefile, filestr);
         }
      }

      /*** Prepare a FORM variable for this. */
      slot  = vartab_find (form, name);
      if (slot < 0)  DONE;

      /*** If this is a file form...
      /       Set form var "name.formname" to original file name.
      /       Set form var to name of temporary file where data resides.
      /       Create & open the temporary file. */
      if (NOT NULLSTR(savefile)) {
         chxconc (name2, CQ("name."), name);
         fn = vartab_find  (form, name2);

         /*** Trim down original file name 
         /    (remove all leading subdirectories or drive names), 
         /    and save it as $form(name.formname). */
         logger (0, LOG_FILE, savefile);
         for (trim=(-1), ptr=0;   savefile[ptr];   ++ptr) {
            if (savefile[ptr] == '/'  ||  savefile[ptr] == '\\') trim = ptr;
         }
         if (trim >= 0) {
            strcpy (filestr, savefile+trim+1);
            strcpy (savefile, filestr);
         }

         /*** Replace all "bad" characters in file name. */
         for (ps=savefile;   *ps;   ++ps)
            if (conf->file_chars[*ps])  *ps = conf->file_chars[*ps];
         chxofascii (form->value[fn],   savefile);

         strcpy (filestr, form_temp(1) );
         chxofascii (form->value[slot], filestr);
/*       fd = creat (filestr, 0755); */
         fd = open (filestr, O_WRONLY | O_CREAT | O_BINARY, 0755);
      }

      /*** Read through and accumulate the data in this part. */
      for (first=1;   (1);   first=0) {
         ok  = read_fline (cd, 0, con_len, line, linemax, &begin);
         if (first) begin = 0;
         eof = (ok < 0  ||  streq (line, eheader));
         if (eof        ||  streq (line, mheader))   break;

         /*** Add this "line" to file or string, as appropriate. */
         if (NOT NULLSTR(savefile)) {
            if (begin)  write (fd, "\r\n", 2);
            write (fd, line,  ok);
         }
         else {
            if (begin) {  /* Unsure about using both \r\n ? */
               to_jix (form->value[slot], "\r\n", 1-first, lang);
               first = 0;
            }
            to_jix (form->value[slot], line, 1-first, lang);
         }
      }

      if (NOT NULLSTR(savefile)) {
         close (fd);
         if (first)  chxclear (form->value[slot]);  /* Empty file. */
      }

      if (eof)      break;
   }

done:
   chxfree (name);
   chxfree (name2);
   RETURN  (1);
}

FUNCTION  mypr (head, str, tail, conf)
   char  *head, *str, *tail;
   struct sweb_conf_t *conf;
{
   char  *s;

   fprintf (conf->verbose, "%s", head);
   for (s=str;  *s;  ++s) {
      if (*s >= 040  &&  *s < 0177)   fputc   (*s, conf->verbose);
      else                            fprintf (conf->verbose, "\\%03o", *s);
   }
   fprintf (conf->verbose, "%s", tail);
   return  (1);
}
