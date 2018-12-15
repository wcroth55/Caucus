
/*** LOAD_CONF.   Load SWEBD configuration file.
/
/    ok = load_conf (conf);
/   
/    Parameters:
/       int    ok;                  (Success loading file?)
/       struct sweb_conf_t *conf;   (configuration struct)
/
/    Purpose:
/       Load the contents of the SWEBD configuration file.
/
/    How it works:
/
/    Returns: 1 on success
/             0 if file cannot be read
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  loadconf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/28/95 23:07 New function. */
/*: CR 03/08/00 15:12 Add SOCKET_DELAY parameter. */
/*: CR 03/28/00 15:23 Add File_Chars parameter. */
/*: CR 04/03/00 16:15 Add Config parameter. */
/*: CR 06/06/00 23:42 Replace tabs with spaces. */
/*: CR 10/16/01 13:50 Add SWconf groupsearch, grouppermnames. */
/*: CR 01/25/02 12:00 Add pw_can_reset. */
/*: CR 03/05/02 13:00 Add robot_reject. */
/*: CR  4/02/02 12:40 Cookie login support (password member). */
/*: CR  1/28/03 Parse [ssl] option in LDAP_HOST. */
/*: CR  5/23/03 Add connectstring, odbcini, odbcsysini. */
/*: CR  6/18/03 make rest[] an actual array, copy if restptr!=NULL. */
/*: CR  2/15/05 Add Stderr parameter. */

#include <stdio.h>
#include <sys/types.h>
#include "chixuse.h"
#include "sweb.h"
#include "diction.h"

#if UNIX | NUT40
#include <pwd.h>
#include <grp.h>
#endif

extern int output_default;

FUNCTION  load_conf (struct sweb_conf_t *conf)
{
   FILE  *fp;
   struct passwd *ent;
   struct group  *grp;
   char          *sysmem(), *strtoken();
   char          *restptr, *lp;
   char   line[200], keyword[200], value[200], rest[300];
   int    sc, pval, rval, uend=0, utrim=0, configs=0;
   ENTRY ("load_conf", "");

   if ( (fp = fopen (conf->filename, "r")) == NULL)  RETURN (0);

   conf->output          = output_default;
   conf->diskform        = conf->browserform = 0;
   conf->pw_prog[0]      = '\0';
   conf->pw_path[0]      = '\0';
   conf->pw_can_add      =   0;
   conf->pw_can_change   =   0;
   conf->pw_can_delete   =   0;
   conf->pw_can_reset    =   0;
   conf->pw_can_verify   =   0;
   conf->ldap_can_add    =   0;
   conf->ldap_can_change =   0;
   conf->ldap_can_delete =   0;
   conf->ldap_can_reset  =   0;
   conf->ldap_can_verify =   0;
   conf->pw_lock         =   1;
   conf->check_client_ip =   1;
   conf->caucus_name[0]  = '\0';
   conf->caucus_id       =  -1;
   conf->real_id         =  -1;
   conf->caucus_group    =  -1;
   conf->real_group      =  -1;
   conf->verbose         = NULL;
   conf->caucuslib[0]    = '\0';
   conf->adminmail[0]    = '\0';
   conf->new_win_x       = 500;
   conf->new_win_y       = 380;
   conf->cache_timeout   = 300;
   conf->pagetime        = 180;
   conf->new_win_for[0]  = '\0';
   conf->cache_include   = '\0';
   conf->socket_delay    =   0 ;
   conf->groupsearch     =   0 ;
   conf->robot_reject    =   0 ;
   conf->grouppermnames  = chxalloc (60L, THIN, "grouppermnames");
   conf->password[0]     = '\0';
   conf->ldap_port       =   0 ;
   conf->ldap_host[0]    = '\0';
   conf->ldap_opt [0]    = '\0';
   conf->ldap_bindas[0]  = '\0';
   conf->ldap_userdn[0]  = '\0';
   conf->ldap_pwname[0]  = '\0';
   conf->sqlconnectstr[0]= '\0';
   conf->odbcini[0]      = '\0';
   conf->odbcsysini[0]   = '\0';
   conf->auth3tag[0]     = '\0';
   conf->stderrTo[0]     = '\0';

   for (sc=0;   sc < MAXCFG;        ++sc)  conf->config [sc] = NULL;
   for (sc=0;   sc < MAX_SCRIPTS;   ++sc)  conf->scripts[sc] = NULL;
   for (sc=0;   sc < 256;           ++sc)  {
      conf->url_end   [sc] = conf->url_trim[sc] = 0;
      conf->file_chars[sc] = 0;
   }

   while (fgets (line, 200, fp) != NULL) {
      if (line[0] == '#')  continue;

      line [ strlen(line)-1 ] = '\0';
      for (lp=line;   *lp;   ++lp)  if (*lp == '\t')  *lp = ' ';

      strtoken (keyword, 1, line);
      rest[0] = '\0';
      restptr = strtoken (value, 2, line);
      if (restptr != NULL)  strcpy (rest, restptr);
      strlower (keyword);

      if (streq (keyword, "caucus_id")) {
#if UNIX | NUT40
         if (isdigit(value[0])) {
            sscanf (value, "%d", &conf->caucus_id);
            if ( (ent = getpwuid(conf->caucus_id)) != NULL)
               strcpy (conf->caucus_name, ent->pw_name);
         }
         else {
            if ( (ent = getpwnam(value)) != NULL)  
               conf->caucus_id = ent->pw_uid;
            strcpy (conf->caucus_name, value);
         }

         if (NULLSTR(conf->caucus_name)  ||  conf->caucus_id < 0) {
            fprintf (stderr, "swebd: bad CAUCUS_ID field in "
               "configuration file '%s'.\nExiting.\n",
               conf->filename);
            exit (1);
         }
#endif
#if WNT40
         strcpy (conf->caucus_name, value);
#endif
      }

      else if (streq (keyword, "real_id")) {
#if UNIX | NUT40
         if (isdigit(value[0]))  sscanf (value, "%d", &conf->real_id);
         else {
            if ( (ent = getpwnam(value)) != NULL)  conf->real_id = ent->pw_uid;
         }
#endif
      }

      else if (streq (keyword, "caucus_group")) {
#if UNIX | NUT40
         if (isdigit(value[0]))  sscanf (value, "%d", &conf->caucus_group);
         else {
            if ( (grp = getgrnam(value)) != NULL) 
                                 conf->caucus_group = grp->gr_gid;
         }
#endif
      }

      else if (streq (keyword, "real_group")) {
#if UNIX | NUT40
         if (isdigit(value[0]))  sscanf (value, "%d", &conf->real_group);
         else {
            if ( (grp = getgrnam(value)) != NULL) 
                                 conf->real_group = grp->gr_gid;
         }
#endif
      }

      else if (streq (keyword, "caucus_path")) {
         strcpy (conf->caucus_path, value);
      }

      else if (streq (keyword, "admin_mail")) {
         strcpy (conf->adminmail, value);
      }

      else if (streq (keyword, "cml_path")) {
         strcpy (conf->cml_path,    value);
      }

      else if (streq (keyword, "hostname")) {
         strcpy (conf->hostname,    value);
      }

      else if (streq (keyword, "pw_prog")) {
         strcpy (conf->pw_prog, rest);
      }

      else if (streq (keyword, "connectstring")) {
         strcpy (conf->sqlconnectstr, rest);
      }

      else if (streq (keyword, "odbcini")) {
         strcpy (conf->odbcini, value);
      }

      else if (streq (keyword, "odbcsysini")) {
         strcpy (conf->odbcsysini, value);
      }

      else if (streq (keyword, "config")) {
         if (configs < MAXCFG) {
            conf->config[configs] = chxalloc (60L, THIN, "config[]");
            chxcpy (conf->config[configs], CQ(rest));
            ++configs;
         }
      }

      else if (streq (keyword, "pw_path")) {
         strcpy (conf->pw_path, value);
      }

      else if (streq (keyword, "pw_can_add")) 
         conf->pw_can_add    = streq (value, "on");

      else if (streq (keyword, "pw_can_change")) 
         conf->pw_can_change = streq (value, "on");

      else if (streq (keyword, "pw_can_delete")) 
         conf->pw_can_delete = streq (value, "on");

      else if (streq (keyword, "pw_can_reset")) 
         conf->pw_can_reset  = streq (value, "on");

      else if (streq (keyword, "pw_can_verify")) 
         conf->pw_can_verify = streq (value, "on");

      else if (streq (keyword, "ldap_can_add")) 
         conf->ldap_can_add    = streq (value, "on");

      else if (streq (keyword, "ldap_can_change")) 
         conf->ldap_can_change = streq (value, "on");

      else if (streq (keyword, "ldap_can_delete")) 
         conf->ldap_can_delete = streq (value, "on");

      else if (streq (keyword, "ldap_can_reset")) 
         conf->ldap_can_reset  = streq (value, "on");

      else if (streq (keyword, "ldap_can_verify")) 
         conf->ldap_can_verify = streq (value, "on");

      else if (streq (keyword, "pw_lock")) 
         conf->pw_lock       = streq (value, "on");

      else if (streq (keyword, "new_win_for")) {
         if (strlen (conf->new_win_for) < MAXNWF - 5 - strlen(value)) {
            strcat  (conf->new_win_for, rest);
            strcat  (conf->new_win_for, " ");
         }
      }

      else if (streq (keyword, "timeout")) {
         sscanf (value, "%d", &conf->timeout);
      }

      else if (streq (keyword, "robot_reject")) {
         sscanf (value, "%d", &conf->robot_reject);
      }

      else if (streq (keyword, "pagetime")) {
         sscanf (value, "%d", &conf->pagetime);
      }

      else if (streq (keyword, "socket_delay")) {
         sscanf (value, "%d", &conf->socket_delay);
      }

      else if (streq (keyword, "disk_format")) {
         conf->diskform = tablematch (chxquick (LANG_STRING, 1),
                                      chxquick (value,       2) );
         if (conf->diskform < 0)  conf->diskform = 0;
      }

      else if (streq (keyword, "browser_format")) {
         conf->browserform = tablematch (chxquick (LANG_STRING, 1),
                                         chxquick (value,       2) );
         if (conf->browserform < 0)  conf->browserform = 0;
      }

      else if (streq (keyword, "check_client_ip")) {
         strlower (value);
         conf->check_client_ip = streq (value, "on");
      }

      else if (streq (keyword, "cache_include")) {
         strlower (value);
         conf->cache_include = streq (value, "on");
      }

      else if (streq (keyword, "verbose")) {
         strcpy   (keyword, value);
         strlower (keyword);
         if      (streq (keyword, "off"))  conf->verbose = NULL;
         else if (streq (keyword, "on" ))  conf->verbose = stderr;
         else {
            nt_chkfile (value, 0700);
            conf->verbose = fopen (value, "a");
         }
      }

      else if (streq (keyword, "caucus_lib"))  strcpy (conf->caucuslib, value);

      else if (streq (keyword, "http_lib"))    strcpy (conf->httplib,   value);

      else if (streq (keyword, "auth3tag"))    strcpy (conf->auth3tag,  value);

      else if (streq (keyword, "stderr"))      strcpy (conf->stderrTo,  value);

      else if (streq (keyword, "temp_dir"))    systmpdir (value, 1);

      else if (streq (keyword, "script")) {
         for (sc=0;   sc<MAX_SCRIPTS;   ++sc) {
            if (conf->scripts[sc] != NULL)  continue;
            conf->scripts[sc] = sysmem (strlen(rest)+4, "script");
            strcpy (conf->scripts[sc], rest);
            break;
         }
      }

      else if (streq (keyword, "url_end")) {
         for (sc=2;   strtoken (value, sc, line) != NULL;   ++sc) {
            sscanf (value, "%d", &pval);
            if (pval >= 0  &&  pval < 256)  conf->url_end[pval] = uend = 1;
         }
      }

      else if (streq (keyword, "url_trim")) {
         for (sc=2;   strtoken (value, sc, line) != NULL;   ++sc) {
            sscanf (value, "%d", &pval);
            if (pval >= 0  &&  pval < 256)  conf->url_trim[pval] = utrim = 1;
         }
      }

      else if (streq (keyword, "file_chars")) {
         for (sc=2;   strtoken (value, sc, line) != NULL;   ++sc) {
            sscanf   (value, "%d", &pval);
            strtoken (value, ++sc, line);
            sscanf   (value, "%d", &rval);
            if (pval >= 0  &&  pval < 256)  conf->file_chars[pval] = rval;
         }
      }

      else if (streq (keyword, "groupsearch")) {
         /*** 'GroupSearch ALL' means search all matches in groups files. */
         strlower (value);
         conf->groupsearch = (value[0] == 'a');
      }

      else if (streq (keyword, "grouppermnames")) {
         strlower (rest);
         chxcat (conf->grouppermnames, CQ(rest));
         chxcat (conf->grouppermnames, CQ(" "));
      }

      else if (streq (keyword, "ldap_host")) {
         strtoken (conf->ldap_host, 2, line);
         if (strtoken (value,           3, line) != NULL)
            conf->ldap_port = atoi (value);
         if (strtoken (value,           4, line) != NULL)
            strcpy (conf->ldap_opt, value);
      }

      else if (streq (keyword, "ldap_bindas")) strcpy (conf->ldap_bindas, rest);
      else if (streq (keyword, "ldap_userdn")) strcpy (conf->ldap_userdn, rest);
      else if (streq (keyword, "ldap_pwname")) strcpy (conf->ldap_pwname, rest);
   }

   if (NULLSTR(conf->pw_prog))
      conf->pw_can_add = conf->pw_can_change = conf->pw_can_delete =
                         conf->pw_can_verify = 0;

   /*** If no URL_End chars are supplied, use defaults. */
   if (NOT uend) {
      conf->url_end[' '] = conf->url_end[')']  = 1;
      conf->url_end['!'] = conf->url_end['\"'] = conf->url_end['\''] = 1;
      conf->url_end['<'] = conf->url_end['&']  = conf->url_end[0240] = 1;
   }

   /*** If no URL_Trim chars are supplied, use defaults. */
   if (NOT utrim)
      conf->url_trim['.'] = conf->url_trim[','] = 1;

   fclose (fp);
   
   RETURN (1);
}
