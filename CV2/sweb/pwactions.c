
/*** PW_ACTIONS.   Perform add, change, delete, verify actions on 
/                  NCSA-style "flat" password file.
/
/    code = pw_actions (action, passfile, id, pw, lock);
/   
/    Parameters:
/       int      code;      (return code)
/       char    *action;    ("add", "change", "delete", "verify")
/       char    *passfile;  (full pathname of password file)
/       char    *id;        (userid)
/       char    *pw;        (password)
/       int      lock;      (lock passfile?)
/
/    Purpose:
/       Implement 4 actions on an "NCSA-style" or "flat" userid
/       and password file.  The actions are "add" an id & pw,
/       "change" the pw on an existing id, "delete" an id, and
/       "verify" the pw for an id.
/
/       This function also is meant to serve as a prototype for 
/       anyone writing a new version of pw_actions() for some other
/       password file format or access method.
/
/    How it works:
/       ACTION controls which action is performed.  
/
/       PASSFILE contains the full pathname of the password file.  The 
/       program that calls pw_actions() must be able to write in
/       the directory that contains PASSFILE.
/
/       ID is the userid in question.  PW is the password.
/
/       LOCK means use advisory locking to enforce one-user write
/       locking and multi-user read (shared) locking on the
/       password file.  If LOCK is 0, no locking is performed.
/
/    Returns: 0 on success
/             one of the positive error codes #define'd below
/             (BAD_xxxx) on error.
/
/    Known bugs:
/
/    Home:  pwactions.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/14/97 11:56 New function. */
/*: CR 10/07/01 15:00 Allow userid to have "@", max size of MAX_USERID chars. */
/*: CR  4/11/03 Use <errno.h> */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#ifndef _WNT40
#include <unistd.h>
#include <termios.h>
#include <pwd.h>
#endif

#include "sweb.h"

#define  FUNCTION
#define  NOT         !
#define  MAXTRIES    30    /* For locking the passfile... */

#define  ascii_to_bin(c) ((c)>='a'?(c-59):(c)>='A'?((c)-53):(c)-'.')
#define  bin_to_ascii(c) ((c)>=38?((c)-38+'a'):(c)>=12?((c)-12+'A'):(c)+'.')

#define  SUNOS4       0    /* Define as 1 for SunOS 4.x */

#define  SUCCESS      0
#define  BAD_NOARG    1    /* no arguments! */
#define  BAD_EXISTS   2    /* userid already exists (add a user) */
#define  BAD_NOENTRY  3    /* userid does not exist (chg password) */
#define  BAD_OLDPW    4    /* bad old password */
#define  BAD_LENGTH   6    /* userid too int4. */
#define  BAD_CHARS    7    /* bad chars in userid. */
#define  BAD_BUSY     8    /* temp password file busy */
#define  BAD_NOREAD   9    /* can't read  password file */
#define  BAD_NOWRITE 10    /* can't write to temp password file */
#define  BAD_CLOSE   11    /* error on close, disk could be full */
#define  BAD_PROG    12    /* can't read from external program. */

FUNCTION int pw_actions (char *action, char *passfile, char *id, char *pw,
                         int   lock)
{
   time_t tm;
   char   salt[2];
   char   cryptstr[200], user[100], line[300], crypt_found[200];
   char   tempfile[300], old_file[300];
   int    lockfd, code, failed, user_found;
   FILE  *fp, *tp;

   char  *crypt();    /* Let's try this for all systems... */

   /*** ADD id and pw. */
   if (action[0] == 'a') {

      /*** Restrict new userids to lower case letters, numbers, ".",
      /    "-", "@", and "_", and to a max length of 80 chars. */
      if (is_outside (id, "abcdefghijklmnopqrstuvwxyz0123456789._-@")) 
                                                      return (BAD_CHARS);
      if (strlen(id) >= MAX_USERID-1)                 return (BAD_LENGTH);

      /*** Create new encrypted password. */
      time(&tm);
      salt[0] = bin_to_ascii( tm       & 0x3f);
      salt[1] = bin_to_ascii((tm >> 5) & 0x3f);
      strcpy (cryptstr, crypt(pw, salt));

      /*** Lock the password file. */
      if ((lockfd = locker (passfile, 1, lock)) < 0)   return (BAD_BUSY);

      /*** If this ID already exists, don't add it! */
      code = pw_scan (passfile, id, crypt_found);
      if (code < 0)           { unlocker (lockfd);     return (BAD_NOREAD); }
      if (code > 0)           { unlocker (lockfd);     return (BAD_EXISTS); }

      /*** Append the new id/pw to it. */
      if ( (fp = fopen (passfile, "a")) == NULL)  
                              { unlocker (lockfd);     return (BAD_NOWRITE);}
      fprintf  (fp, "%s:%s\n", id, cryptstr);
      failed = fclose (fp);
      unlocker (lockfd);
      if (failed)                                      return (BAD_CLOSE);
      return   (SUCCESS);
   }


   /*** DELETE id. */
   if (action[0] == 'd') {

      sprintf (tempfile, "%s.TMP", passfile);
      sprintf (old_file, "%s.OLD", passfile);

      /*** Lock the password file. */
      if ((lockfd = locker (passfile, 1, lock)) < 0)   return (BAD_BUSY);

      if ( (fp = fopen (passfile, "r")) == NULL)
                                { unlocker (lockfd);   return (BAD_NOREAD); }
      if ( (tp = fopen (tempfile, "w")) == NULL)
                 { fclose (fp);   unlocker (lockfd);   return (BAD_NOWRITE); }

      /*** Read users (from the real file) and write them out (to a
      /    temp file), all except ID. */
      user_found = 0;
      while (get_ent (fp, user, crypt_found, line)) {
         if (strcmp (user, id) == 0) { user_found = 1;   continue; }

         if (line[0] == '#' || line[0] == '+') 
                fprintf (tp, "%s\n",    line);
         else   fprintf (tp, "%s:%s\n", user, crypt_found);
      }
      fclose (fp);
      failed = fclose (tp);
      if (NOT user_found)       { unlocker (lockfd);   return (BAD_NOENTRY); }
      if (failed)               { unlocker (lockfd);   return (BAD_CLOSE); }

      /*** Save the old file, and put the temp in its place. */
      move_it  (passfile, old_file);
      move_it  (tempfile, passfile);
      chmod    (passfile, 0644);
      unlocker (lockfd);
      return   (SUCCESS);
   }


   /*** CHANGE id & pw. */
   if (action[0] == 'c') {

      sprintf (tempfile, "%s.TMP", passfile);
      sprintf (old_file, "%s.OLD", passfile);

      /*** Create new encrypted password. */
      time(&tm);
      salt[0] = bin_to_ascii( tm       & 0x3f);
      salt[1] = bin_to_ascii((tm >> 5) & 0x3f);
      strcpy (cryptstr, crypt(pw, salt));

      /*** Lock the password file. */
      if ((lockfd = locker (passfile, 1, lock)) < 0)   return (BAD_BUSY);

      if ( (fp = fopen (passfile, "r")) == NULL)
                                { unlocker (lockfd);   return (BAD_NOREAD); }
      if ( (tp = fopen (tempfile, "w")) == NULL)
                 { fclose (fp);   unlocker (lockfd);   return (BAD_NOWRITE); }

      /*** Read users (from the real file) and write them out (to a
      /    temp file), changing ID's password only. */
      user_found = 0;
      while (get_ent (fp, user, crypt_found, line)) {
         if (strcmp (user, id) == 0)  {
            strcpy (crypt_found, cryptstr);
            user_found = 1;
         }

         if (line[0] == '#' || line[0] == '+') 
                fprintf (tp, "%s\n",    line);
         else   fprintf (tp, "%s:%s\n", user, crypt_found);
      }
      fclose (fp);
      failed = fclose (tp);
      if (NOT user_found)       { unlocker (lockfd);   return (BAD_NOENTRY); }
      if (failed)               { unlocker (lockfd);   return (BAD_CLOSE); }

      /*** Save the old file, and put the temp in its place. */
      move_it  (passfile, old_file);
      move_it  (tempfile, passfile);
      chmod    (passfile, 0644);
      unlocker (lockfd);
      return   (SUCCESS);
   }


   /*** VERIFY id and pw. */
   if (action[0] == 'v') {

      /*** Lock the password file. */
      if ((lockfd = locker (passfile, 0, lock)) < 0)   return (BAD_BUSY);

      /*** Find the entry for this ID. */
      code = pw_scan (passfile, id, crypt_found);
      unlocker (lockfd);
      if (code <  0)                                   return (BAD_NOREAD); 
      if (code == 0)                                   return (BAD_NOENTRY); 

      /*** Did the supplied password encrypt the same way as the
      /    "real" password?  If so, verify succeeds. */
      if (strncmp (crypt_found, crypt (pw, crypt_found), 13))
                                                       return (BAD_OLDPW);
      return (SUCCESS);
   }

   return (BAD_NOARG);
}


/*** PW_SCAN.  Is this ID in the password file?
/
/    Returns: -1 if PASSFILE cannot be opened.
/              0 if ID was not found
/              1 if ID was found.  Puts encrypted pw in CRYPT.
/ */
FUNCTION  pw_scan (char *passfile, char *id, char *crypt)
{
   FILE *fp;
   char  line[300], user[200];

   if ( (fp = fopen (passfile, "r")) == NULL)  return (-1);
  
   while (get_ent (fp, user, crypt, line)) {
      if (strcmp (user, id) == 0)   { fclose (fp);   return ( 1); }
   }

   fclose (fp);
   return ( 0);
}


/*** MOVE_IT.   Rename file A to file B. 
/ */
FUNCTION  move_it (char *a, char *b)
{
#ifndef _WNT40
   unlink (b);
   link   (a, b);
   unlink (a);
#else
   rename (a, b);
#endif
}


/*** LOCKER.  Lock passwd file NAME.
/
/    If WRITE, lock for writing, else reading (shareable).
/    Only simulate locking if LOCK is 0. 
/ */
FUNCTION  locker (char *name, int write, int lock)
{
#ifndef _WNT40
   struct flock alock = { 0, SEEK_SET, 0, 1 };
   char   lockname[256];
   int    lockflag, i, fd;

   if (NOT lock)  return (0);

   /*** Open the "lockfile", which is NAME plus a trailing "_l". */
   strcpy (lockname, name);
   strcat (lockname, "_l");
   if ( (fd = open (lockname, 2)) < 0)  fd = creat (lockname, 0700);
   if (fd < 0)  return (-1);

   alock.l_type   = (write ? F_WRLCK : F_RDLCK);
   alock.l_start  = 0;

   for (i=0;   (lockflag = fcntl (fd, F_SETLK, &alock)) < 0;   i++) {
/*    fprintf (stderr, "errno=%d\n", errno);   */
      if (i > MAXTRIES)  { close (fd);   return (-2); }
      sleep (2);
   }

   return (fd);
#else
   return (1);
#endif
}


/*** UNLOCKER.   Unlock file locked by locker().
/ */
FUNCTION  unlocker (int fd)
{
#ifndef _WNT40
   struct flock alock = { 0, SEEK_SET, 0, 1 };

   if (fd == 0)  return (1);

   alock.l_type   = F_UNLCK;
   alock.l_start  = 0;
   fcntl  (fd, F_SETLK, &alock);
   close  (fd);
#endif
   return (1);
}


/*** Are any chars in STR outside the set of those in LIST?
/ */
FUNCTION  is_outside (char *str, char *list)
{
   short  found;
   char  *p;

   for (  ;   *str;   ++str) {
      for (found=0, p=list;   *p;   ++p) {
         if (*str == *p)  { found = 1;  break; }
      }

      if (NOT found)  return (1);
   }

   return (0);
}
