
/*** SWEB.H.   Include file for SWEB programs. */
 
/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/27/95 22:25 New file. */
/*: CR  4/05/00 14:49 Add config to sweb_conf_t. */
/*: CR  4/27/00 13:25 Add http_t.accept, referer0, referer1. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR 10/07/01 15:00 Raise userid length. */
/*: CR 10/16/01 13:50 Add SWconf groupsearch, grouppermnames. */
/*: CR 01/25/02 12:00 Add pw_can_reset. */
/*: CR 03/05/02 13:00 Add robot_reject. */
/*: CR  4/02/02 12:40 Cookie login support. */
/*: CR  6/28/02 change http.accept to http.xaccept to placate AIX. */
/*: CR  9/18/02 add ldap members to sweb_conf_t. */
/*: CR  l/28/03 add ldap_opt. */
/*: CR  5/04/03 raise size of http_t.request_uri. */
/*: CR  5/23/03 add sweb_conf_t.sqlconnectstr, odbcini, odbcsysini. */
/*: CR 01/30/05 add sweb_conf_t.auth3tag. */
/*: CR 02/01/05 Add AsciiBuf */
/*: CR 02/15/05 add sweb_conf_t.stderr. */
/*: CR 08/24/05 add CAUCUS4. */

#include "systype.h"
#include "entryex.h"
#include "object.h"
#include "maxparms.h"

#ifndef  SWEB
#define  SWEB  1

/*** Include Caucus4-only code?  "Vanilla" Caucus 5 should make this 0. */
#define  CAUCUS4           1

/*** Error codes from 5.1 functions. */
#define  ERR_NOCNUM       -1
#define  ERR_NOITEM       -2
#define  ERR_NOWRITE      -3
#define  ERR_FAILED       -4
#define  ERR_INVALID      -5 

/*** Define common "readability" macros. */
#define  FUNCTION
#define  NOT               !
#define  S_INFINITY        1000000

#ifndef  L
#define  L(x)              ( (int4) x )
#endif

#ifndef  NULLSTR
#define  NULLSTR(x)        (!(x[0]))
#endif

/*** Server port number for swebd: used for NT only.   Should eventually
/    become a configurable number in swebd.conf. */
#define  SERVER_PORT  8023

/*** Define codes for logger() calls. */
#include "logger.h"

/*** Define pipe buffer size, and read and write size. */
#define  PIPEBUF   8192
#define  PIPEWRITE 4096
#define  PIPEREAD  8192

#if LNX12
#undef   PIPEWRITE
#define  PIPEWRITE  512
#endif

/*** Define "hose" types & info. */
#if UNIX | NUT40
typedef int     Hose;
#define BADHOSE  (-1)
#endif
#if WNT40
#include <windows.h>
#include <iostream.h>
typedef HANDLE  Hose;
#define BADHOSE  ((HANDLE) NULL)
#define USE_OVERLAP    0
#endif

/*** Max # of attempts by swebsock to connect to swebd. */
#define  MAX_ATTEMPT   3

/*** Define "socket message" stuff. */
#define  SOCKMSGMAX 4096 - 4 - 4 - 1
struct sockmsg_t {
   int   len;
   int   eof;
   char  buf[SOCKMSGMAX+1];
};

/*** For use in "func_xxx()" calls; must use arg RESULT. */
#define  BAD(x)            chxcat (result, CQ("<I>"x"</I>"))

/*** Name of swebfail file. */
#define  SWEBFAIL          "swebfail.html"

/*** Max size of CML file. */
#define  MAXCML     2000

/*** Minimum required amount of free disk space */
#define  MIN_DISK_SPACE  7000    /* KB */
#define  MIN_DISK_FILES   500
#define  LICE_NODISK       10

/*** Bit masks for item/response property values. */
#define  M_TEXTTYPE   0x000F
#define  M_BITS       0xFFF0
#define  M_ISORG      0x0010

/*** Bit masks for manager features. */
#define  MGR_SET      0x001    /* set list of managers! */
#define  MGR_CRCONF   0x002    /* create conference */
#define  MGR_RMCONF   0x004    /* remove conference */
#define  MGR_BEORG    0x008    /* become organizer */
#define  MGR_IS       0x010    /* is a manager */
#define  MGR_CHGPASS  0x020    /* can change passwords */
#define  MGR_MKID     0x040    /* can add ids */
#define  MGR_RMID     0x080    /* can remove ids */
#define  MGR_RMCOORG  0x100    /* can remove conference if organizer. */
#define  MGR_RESERV   0x100    /* 001 -> 100 reserved */

/*** Max number of scripts to auto-run from swebd on start-up. */
#define  MAX_SCRIPTS     20

/*** Max len of string of file extensions that open a new window. */
#define  MAXNWF        240

/*** Max number of Config parameters. */
#define  MAXCFG        100

typedef  struct sweb_conf_t * SWconf;
struct sweb_conf_t {

   /*** This information is in the initial, primary server daemon. */
   char  hostname   [200];      /* Name of this host.  */
   char  filename   [200];      /* Configuration file. */
   char  caucus_name[ 20];      /* Caucus userid (string). */
   int   caucus_id;             /* Run as effective uid caucus_uid. */
   int   real_id;               /* Run as "real" id real_id. */
   int   caucus_group;          /* Run as "effective" group. */
   int   real_group;            /* Run as "real" group. */
   char  caucus_path[200];      /* pathname of caucus directory. */
   char  cml_path   [200];      /* pathname of CML directory. */
   char  pw_prog    [200];      /* password-changing program. */
   char  pw_path    [256];      /* password file, if passprog is "internal". */
   int   pw_can_add;            /* passprog can add uid/pw? */
   int   pw_can_change;         /* passprog can change uid/pw? */
   int   pw_can_delete;         /* passprog can delete uid/pw? */
   int   pw_can_reset;          /* passprog can change uid/pw if run w/o uid */
   int   pw_can_verify;         /* passprog can verify uid/pw? */
   int   pw_lock;               /* passfile should be locked? */
   char  ldap_host  [200];      /* ldap server hostname */
   int   ldap_port;             /* ldap server port #, 0 if not supplied. */
   char  ldap_opt   [100];      /* ldap options. Currently empty or "ssl". */
   char  ldap_bindas[300];      /* bind as this id & pw to ldap server */
   char  ldap_userdn[300];      /* format string for constructing searches */
   char  ldap_pwname[100];      /* name of user password attribute */
   int   ldap_can_add;          /* ldap can add uid/pw? */
   int   ldap_can_change;       /* ldap can change uid/pw? */
   int   ldap_can_delete;       /* ldap can delete uid/pw? */
   int   ldap_can_reset;        /* ldap can change uid/pw if run w/o uid */
   int   ldap_can_verify;       /* ldap can verify uid/pw? */
   int   diskform;              /* Disk_Format code (character set) */
   int   browserform;           /* Browser_Format code (character set) */
   int   check_client_ip;       /* Test client ip for consistency? */
   int   timeout;               /* sub-server timeout after no requests. */
   FILE *verbose;               /* debugging output? */
   char  caucuslib  [200];      /* Caucus library directory. */
   char  httplib    [200];      /* URL of Caucus library directory. */
   char  adminmail  [200];      /* E-mail of administrator */
   int   pagetime;              /* Timeout for swebsock. */
   char *scripts[MAX_SCRIPTS];  /* Scripts to run on swebd start-up. */
   char  url_end [256];         /* Chars that mark the end of a URL. */
   char  url_trim[256];         /* Chars that should be trimmed from URL end. */
   char  file_chars[256];       /* Replacement values for bad file name chars */
   Chix  config[MAXCFG];        /* $config() parameters. */
   int   groupsearch;           /* 1=>all, 0=>first match */
   Chix  grouppermnames;        /* group permission names & bit value list */
   int   robot_reject;          /* # seconds to reject new swebs for same uid */
   char  sqlconnectstr[200];    /* SQL database connect string. */
   char  odbcini[200];          /* database driver manager ODBCINI location */
   char  odbcsysini[200];       /* database driver manager ODBCSYSINI location*/
   char  auth3tag  [200];       /* authmethod 3 hash tag */
   char  stderrTo  [200];       /* what to do with output to stderr? */

   /*** The rest is specific to each instance of the subserver. */
   char  remote [MAX_USERID+2]; /* remote client's userid. */
   char  ip_addr    [100];      /* remote client's ip address */
   char  cmlname    [200];      /* cml file name and dir. */
   int   secure;                /* security code to identify this subserver. */
   int   output;                /* redirected output file descriptor. */
   int   new_win_x;             /* new_win width */
   int   new_win_y;             /* new_win height */
   int   reload;                /* is current page a reload of imm prev page? */
   int   cache_timeout;         /* some caches expire after this much time. */
   char  new_win_for[MAXNWF];   /* open new window for these file extensions.*/
   char  cache_include;         /* cache include'd files? */
   int   socket_delay;          /* seconds to delay read/write to new socket.*/
   char  auth_method;           /* 0=>none, 1=>http_remote, 2=>cookie */
   char  caucus_ver[12];        /* cookie verification string */
   char  password[40];          /* stored password (see auth_pw()) */
};


/*** A CML file, as read in by loadcml(). */
typedef  struct cmlfile_t * CML_File;
struct cmlfile_t {
   char  filename[200];         /* original filename. */
   int   cdex;                  /* index to current line in lines[], lnums[] */
   int   size;                  /* number of lines loaded. */
   Chix  errtext;               /* accumulated error text */
   Chix  lines[MAXCML];         /* Chix text of each line. */
   short lnums[MAXCML];         /* original line numbers */
};

/*** Output "target" -- either a Hose or a Chix. */
typedef struct target_t * Target;
struct target_t {
   Hose  cd;
   Chix  po;
};


/*** HTTP cgi environment variables. */
struct http_t {
   char  user_agent  [220];
   char  referer0    [220];
   char  referer1    [220];
   char  xaccept     [220];
   char  request_uri[2000];
};

/*** Expandable ascii buffer. */
typedef struct asciibuf_t * AsciiBuf;
struct         asciibuf_t {
   char *str;
   int   max;
};

/*** Function prototypes. */
FUNCTION  int   cml_globals (void);
FUNCTION  void  clean_newlines (Chix x);
FUNCTION  int   fix_query   (char *query);
FUNCTION  int   touch_cl    (int4 cnum,   SWconf conf);
FUNCTION  int   func_cl     (Chix result, char *what, Chix arg, SWconf conf);
FUNCTION  int   func_my     (Chix result, char *what, Chix arg);
FUNCTION  int   func_peo    (Chix result, char *what, Chix arg);
FUNCTION  int   func_re     (Chix result, char *what, Chix arg, int reload);
FUNCTION  char *upperBoundary();
FUNCTION  char *lowerBoundary();

FUNCTION  int   stralter    (char *a, char *b, char *c);

#endif
