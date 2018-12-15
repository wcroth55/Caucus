
/*** WPWCOPY.   Web Password Copy utility.
/
/    Invoked as:
/       wpwcopy  web_passwd  etc_passwd  [etc_shadow]  [options]
/
/    Arguments:
/       web_passwd   resulting "web" password file
/       etc_passwd   system password file, e.g. /etc/passwd
/       etc_shadow   optional system "shadow" password file, e.g. /etc/shadow
/       options
/          -u xxx:yyy  only include userids in this numeric range
/          -g xxx:yyy  only include ids with group #s in this numeric range
/
/    Purpose:
/       Create a World-Wide-Web "access authorization" password file
/       by extracting some or all of the userids and (encrypted)
/       passwords from the usual Unix system password file(s).
/
/       Context: a particular Unix host has a list of people (userids)
/       who are allowed to access some services on that host.  When that
/       same host also serves Web pages, it may be desireable to limit access 
/       to certain pages to the same list of userids.
/
/       WPWCOPY provides the capability to copy some or all of the
/       userid & password entries from the system password file(s)
/       to a web-style password file that may in turn be used to
/       restrict access to a Web page or directory of pages.
/       Typically WPWCOPY would be run on a regular schedule (such as
/       daily), or every time the system password file(s) were modified.
/       
/    How it works:
/       Reads lines from etc_passwd, writes userid and encrypted password
/       fields only to web_passwd.
/
/       If etc_shadow file is named, gets password field from it instead.
/
/       If -u or -g option is supplied, only includes userids
/       that fall inside the specified userid or group range.  If both
/       are supplied, both conditions apply.
/
/       WARNING!  Locking the web_passwd file (in case of multiple
/       attempts to run wpwcopy simultaneously) is the responsbility
/       of the user.
/
/       For systems that use /etc/shadow, wpwcopy may have to run
/       as set-uid root.
/
/    Exit status: 0 on success
/                 1 if one of the files could not be opened.
/
/    Error Conditions:
/  
/    Known bugs:
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/04/96 17:37 New program. */

#include <stdio.h>

#define  FUNCTION
#define  NOT         !
#define  MAXLEN      256

char  *field (char **lp);

FUNCTION int main (int argc, char *argv[])
{
   FILE  *web, *pwd, *sha;
   int    ai, pi, uid0, uid1, gid0, gid1, userid, group;
   char   pwfile[3][MAXLEN];
   char   line[MAXLEN], *lp;
   char   username[MAXLEN], pass[MAXLEN], shapass[MAXLEN];

   /*** Initialization. */
   pwfile[0][0] = pwfile[1][0] = pwfile[2][0] = '\0';
   uid0 = gid0 =     0;
   uid1 = gid1 = 32000;

   /*** Parse arguments. */
   for (pi=0, ai=1;   ai<argc;   ++ai) {

      if      (strncmp (argv[ai], "-u", 2) == 0) 
         sscanf (argv[++ai], "%d:%d", &uid0, &uid1);

      else if (strncmp (argv[ai], "-g", 2) == 0) 
         sscanf (argv[++ai], "%d:%d", &gid0, &gid1);

      else if (pi < 3) strcpy (pwfile[pi++], argv[ai]);
   }

   /*** Usage message if not enough arguments. */
   if (NOT pwfile[1][0]) {
      fprintf (stderr, 
        "Usage: wpwcopy web_passwd etc_passwd [etc_shadow] [-u|-g x:y]\n");
      exit    (1);
   }
/*
   printf ("0='%s'\n", pwfile[0]);
   printf ("1='%s'\n", pwfile[1]);
   printf ("2='%s'\n", pwfile[2]);
   printf ("g %d:%d\n", gid0, gid1);
   printf ("u %d:%d\n", uid0, uid1);
*/

   /*** Open files. */
   if ( (web = fopen (pwfile[0], "w")) == NULL) {
      fprintf (stderr, "Can't open web password file '%s'\n", pwfile[0]);
      exit    (1);
   }

   if ( (pwd = fopen (pwfile[1], "r")) == NULL) {
      fprintf (stderr, "Can't open system password file '%s'\n", pwfile[1]);
      exit    (1);
   }

   sha = NULL;
   if (pwfile[2][0]){
      if ( (sha = fopen (pwfile[2], "r")) == NULL) {
         fprintf (stderr, "Can't open shadow password file '%s'\n", pwfile[2]);
         exit    (1);
      }
   }
   
   /*** For each line in etc_passwd... */
   while (fgets (line, MAXLEN, pwd) != NULL) {
      line [strlen(line)-1] = '\0';

      /*** Pluck out the relevant fields. */
      lp = line;
      strcpy (username, field(&lp));
      strcpy (pass,     field(&lp));
      sscanf (field(&lp), "%d", &userid);
      sscanf (field(&lp), "%d", &group);

      /*** Exclude users outside uid/gid ranges. */
      if (userid < uid0  ||  userid > uid1)  continue;
      if (group  < gid0  ||  group  > gid1)  continue;

      /*** Get password from etc_shadow, if relevant. */
      if (sha != NULL  &&  shadow_pass (shapass, username, sha))
         strcpy (pass, shapass);

      /*** Write web_passwd entry. */
      fprintf (web, "%s:%s\n", username, pass);
   }
   fclose (web);
   fclose (pwd);
   fclose (sha);

   exit   (0);
}


/*** Return a pointer to the "next" field in the line. */

FUNCTION char *field (char **lp)
{
   static char buf[MAXLEN];
   char  *bp;

   buf[0] = '\0';
   bp = buf;

   while (**lp != ':')  *bp++ = *(*lp)++;
   *bp = '\0';
   *(*lp)++;
   return (buf);
}

/*** Find the shadow password SHAPASS for this USERNAME. */

FUNCTION  int shadow_pass (char *shapass, char *username, FILE *sha)
{
   char  line[MAXLEN], *lp;
   int   eof = 0;

   /*** Scan the etc_shadow file up to twice (once from current position,
   /    then once from the top) for USERNAME. */
   for (eof=0;   eof < 2;   ++eof) {
      while (fgets (line, MAXLEN, sha) != NULL) {
         lp = line;

         /*** If found, put the password in SHAPASS and return. */
         if (strcmp (username, field(&lp)) == 0) {
             strcpy (shapass,  field(&lp));
             return (1);
         }
      }
      rewind (sha);
   }

   /*** Twice through, no luck.  User is not in file. */
   return (0);
}
