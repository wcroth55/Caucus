/* forwardCaucusMail -- Inbound Caucus email filter.
/  
/  Purpose:
/     Email may be sent directly to a Caucus item, by addressing it to:
/        caucus-Site:Conference_Name:ItemNumber@caucushost.com
/     or
/        caucus-Site.-Conference_Name.-ItemNumber@caucushost.com
/
/        (substituting proper values for Site, Conference_Name,
/         ItemNumber, and caucushost.com.)
/
/     Such email is received by postfix, resent via the 'luser' interface,
/     picked up by the luser's .forward file, and filtered through here.
/     ForwardCaucusMail then rewrites the subject line so that the existing
/     Caucus-email interface can understand it, and tinkers with the From
/     and To fields accordingly.
/
/     Thus, for example: 
/        caucus-coe.-CCEP-GL_Mgt_Team_Room.-14.1@coexploration.org
/
/     means breakout 1 from item 14 in conference CCEP-GL_Mgt_Team_Room in the Caucus
/     site installed under unix userid "coe" on the server coexploration.org.  The
/     email address gets rewritten to coe_mail@localhost, with the conference name
/     and item label in the subject field, from whence it is picked up by the old
/     subject-line-based Caucus script.
/
/     The "-d" argument may be specified on the command line, in which case
/     diagnostic information is written to stderr in case of errors.
/
/:CR 04/10/2009 First version.
/:CR 10/21/2010 Add -d debug flag, comments.
/ */

#include <stdio.h>
#define  MAXLINE 2000
#define  MAXWORD  400
#define  ST_START      0
#define  ST_COPY_TEXT  3
#define  NOT           !
#define  FUNCTION
#define  EMPTY(x)      (!x[0])

char *strtoken();

static int debug = 0;

FUNCTION main(int argc, char *argv[]) {
   char  line[MAXLINE];
   char  subject[MAXLINE];
   char  address[MAXWORD], from[MAXWORD];
   int   state = ST_START;
   int   atPos, i;

   for (i=0;   i<argc;   ++i) {
      if (strcmp ("-d", argv[i]) == 0)  debug = 1;
   }

   address[0] = '\0';
   while (fgets (line, MAXLINE-1, stdin) != NULL) {
      line [strlen(line)-1] = '\0';
      if (state == ST_START) {

         if (strindex (line, "To:") == 0) {
            if (EMPTY(address)) parseAddressAndSubject (address, subject, line);
         }

         else if (strindex (line, "Delivered-To:") == 0) {
            parseAddressAndSubject (address, subject, line);
         }

         else if (strindex (line, "From ") == 0) {
            strtoken (from, 2, line);
         }

         else if (line[0] == '\0') {
            printf ("To: %s\n",      address);
            printf ("Subject: %s\n", subject);
            printf ("From: %s\n",    from);
            printf ("\n");
            state = ST_COPY_TEXT;
         }
      }

      else if (state == ST_COPY_TEXT) {
         printf ("%s\n", line);
      }
   }
}

FUNCTION die (char *msg, char *text) {
   if (debug) {
      fprintf (stderr, "Error: %s\n", msg);
      fprintf (stderr, "input=\n%s\n", text);
   }
   exit(0);
}

FUNCTION parseAddressAndSubject (char *address, char *subject, char *input) {
   int   atPos, atColon;
   char  line[400];
   char  site[MAXWORD], conf[MAXWORD], item[MAXWORD];
   char *postCaucus;

   atColon = strindex (input, ":");
   if (atColon < 0)  die (": pos < 0", input);
   strcpy (line, input+atColon+2);

   atPos = strindex (line, "@");
   if (atPos < 0)    die  ("@ pos < 0", line);

   line[atPos] = '\0';
   while (stralter (line, "<",  "")) ;
   while (stralter (line, ">",  "")) ;
   while (stralter (line, " ",  "")) ;
   while (stralter (line, "\"", "")) ;
   while (stralter (line, ":",  " ")) ;
   while (stralter (line, ".-", " ")) ;

   if (strindex (line, "caucus-") != 0)  die ("caucus- pos < 0", line);
   postCaucus = line + strlen("caucus-");
   strtoken (site, 1, postCaucus);
   strtoken (conf, 2, postCaucus);
   strtoken (item, 3, postCaucus);

   sprintf (subject, "Re: ::Caucus: %s: %s", conf, item);
   sprintf (address, "%s_mail@localhost", site);
}

/*
To: "caucus-china:Test:1"@localhost.localdomain, undisclosed-recipients:;
    china:Test:1
Subject: Re: ::Caucus: Test: 1:2-3  Test Item
*/

int strindex (s, t)
   char  s[], t[];
{
   register int  i, j, k;

   for (i=0;  s[i] != '\0';  i++) {
      for (j=i, k=0;  t[k] != '\0' && s[j]==t[k];  j++, k++) ;
      if (t[k] == '\0')  return (i);
   }
   return (-1);
}

/*** STRALTER.  Alter 1st occurence of OLD in SSS to NEW. */

int stralter (sss, old, new)
   char  sss[], old[], new[];
{
   int   ssslen, oldlen, newlen, pos, delta;
   register int i;
   int   strlen(), strindex();

   ssslen = strlen (sss);   oldlen = strlen (old);   newlen = strlen (new);
   if (ssslen==0 || oldlen==0)         return (0);

   if ((pos = strindex (sss, old)) < 0)  return (0);

   delta = newlen - oldlen;
   if (delta > 0) {
      for (i=ssslen;  i>= pos+oldlen;  --i)  sss[i+delta] = sss[i]; }
   if (delta < 0) {
      for (i=pos+oldlen;  i<=ssslen;   ++i)  sss[i+delta] = sss[i]; }

   for (i=0;  i<newlen;  ++i)  sss[i+pos] = new[i];

   return (1);
}

/*** STRTOKEN.  Find the POS'th token in a string.
/
/    STRTOKEN finds the POS'th token in the string LINE, and copies it
/    into WORD.  A token is a sequence of nonblank characters.  The first
/    token in a string is number 1.
/
/    STRTOKEN returns a pointer to the beginning of the found token in 
/    LINE.  If no such token was found, STRTOKEN returns NULL, and
/    sets WORD to the empty string.
/
/ */

#include <stdio.h>

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

char *strtoken (word, pos, line)
   char    word[], line[];
   int     pos;
{
   int   between, i, len;
   char  buf[300];
   char  *w, *start;

   /*** BETWEEN means we are between tokens (in the white space).  */
   between  = TRUE;
   len = strlen (line);

   for (i=0;   i<=len;   i++) {

      if (line[i]==' '  ||  line[i]=='\0') {
         if (NOT between) {
            *w = '\0';
            if ((--pos) <= 0) { strcpy (word, buf);    return (start); }
         }
         between = TRUE;
      }

      else {
         if (between)  {
            w       = buf;
            start   = line + i;
            between = FALSE;
         }
         *w++ = line[i];
      }
   }

   word[0] = '\0';
   return (NULL);
}
