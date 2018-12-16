
/*** PROCESS_REQUEST.  Process a SWEB request from a validated client.
/
/    process_request (cd, conf, lice, ip_addr);
/   
/    Parameters:
/       Hose                cd;       (connection descriptor)
/       struct sweb_conf_t *conf;     (server configuration info)
/       int                 lice;     (license code)
/       char               *ip_addr;  (client's ip address)
/
/    Purpose:
/       Read a SWEB request (to parse and evaluate a CML file) from
/       a validated client, and send the results back to the client.
/
/    How it works:
/       If LICE indicates an invalid license, return an appropriate
/       warning page to the client, rather than the page they requested.
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  process.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/09/95 17:33 New function. */
/*: CR  3/25/95 13:13 Convert to chix. */
/*: CR  4/27/00 13:24 Add httpd.accept element, raise sizes to 200. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR  3/01/02 14:00 Add 'found' argument loadcml(). */
/*: CR  6/28/02 change http.accept to http.xaccept to placate AIX. */
/*: CR  5/04/03 read size of http.request_uri dynamically. */

#include <stdio.h>
#include "systype.h"
#include "chixuse.h"
#include "api.h"
#include "sweb.h"

#if UNIX | NUT40
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#endif

#define  CMAX  256

static CML_File        cfile;
static Obtab           otab;
static Chix            errtext;

Vartab        vars, form, macs;
struct        http_t   http;

extern FILE *sublog;
extern int   cml_debug;

static int fail_count = 0;

FUNCTION  process_request (
   Hose                cd, 
   struct sweb_conf_t *conf,
   int                 lice,
   char               *ip_addr )
{
   static   int    first_call = 1;
   static   int4   request1;
   static   Target tg;
   static   Chix   pageOutput;
   static   Chix   contentType;
   static   Chix   location;
   static   Chix   htmlOpen;
   static   Chix   htmlClose1;
   static   Chix   htmlClose2;
   char     query[CMAX], cmlname[CMAX], temp[CMAX];
   char    *tempOut;
   int      num, size, broke, contd, found, urilen;
   Chix     empty;
   Chix     makeChix();
   CML_File make_cml();
   Obtab    make_obtab();
   Target   target();
   ENTRY ("process_request", "");

   /*** On first call, initialize the array of CML lines, 
   /    and the forms-data variable list. */
   if (first_call) {
      errtext = chxalloc (L(256), THIN, "process errtext");
      pageOutput = chxalloc (L(10000), THIN, "process pageOutput");
      contentType = makeChix ("Content-type:");
      htmlOpen    = makeChix ("<HTML");
      htmlClose1   = makeChix ("</HTML");
      htmlClose2   = makeChix ("</html");
      location    = makeChix ("Location:");
      cfile = make_cml(errtext);
      otab  = make_obtab(100);
      vars  = a_mak_vartab ("process vars");
      form  = a_mak_vartab ("process form");
      macs  = a_mak_vartab ("process macs");
/*    tg    = target (cd, nullchix); */
      tg    = target (NULL, pageOutput);
      first_call = 0;
      request1   = time(NULL);
      http.referer0[0] = '\0';
   }
   empty   = chxalloc (  L(4), THIN, "process empty");


   /*** Get the name of the CML file to be processed.   Make
   /    sure it exists. */
   hose_read (cd, cmlname,  200, 0);
   hose_read (cd,  query,   200, 0);
   fix_query (query);

   /*** Determine if this is a "reload". */
   conf->reload = reload_check (cmlname, query);

   strcpy  (conf->cmlname, cmlname);
   sprintf (temp, "   page %s?%s... reload=%d", cmlname, query, conf->reload);
   logger  (0, LOG_FILE, temp);
   if (lice == LICE_NODISK)     strcpy (cmlname, "baddisk.cml");

   /*** Get specific CGI environment variables. */
   hose_read (cd, &http.user_agent[0],     200, 0);
   hose_read (cd, &http.referer1[0],       200, 0);
   hose_read (cd, &http.xaccept[0],        200, 0);
   hose_read (cd, temp,                      6, 0);
   urilen = atoi (temp);
   hose_read (cd, &http.request_uri[0], urilen, 0);
   if (NOT http.referer0[0])  strcpy (http.referer0, http.referer1);

   /*** Check for 'spoofing'. */
   if (conf->check_client_ip  &&  NOT streq (ip_addr, conf->ip_addr)) { 
      strcpy (cmlname, "syserr1.cml");
      conf->timeout = 10;
   }

   /*** Read forms data. */
   read_form_data (cd, form, conf);

   /*** Include the globals. */
   cml_globals();

   /*** Load the entire CML file into memory. */
   size = loadcml (cmlname, conf, 0, cfile, vars, form, query, macs, otab, 
                   tg, nullchix, &found);

   /*** If the file does not exist, complain! */
   if (size == 0)  say_no_file (cd, cmlname, conf);

   /*** If it does, parse the entire CML file. */
   else  {
      parse_cml (tg, cfile, size, 0, conf, vars, form, query, 
                       &broke, empty, macs, otab, &contd, nullchix);
      convert_and_write (cd, tg->po, 0, conf);

      if (chxindex (tg->po, 0, contentType) < 0  &&
          chxindex (tg->po, 0, location)    < 0) {
         logger (1, LOG_FILE, "<<<NO CONTENT-TYPE<<<<<<<<<<<<<<<<<<");
         tempOut = asciifull (tg->po);
         logger (1, LOG_FILE, tempOut);
         logger (1, LOG_FILE, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
         free (tempOut);
      }
      else if (chxindex (tg->po, 0, htmlOpen)   >= 0   &&
               chxindex (tg->po, 0, htmlClose1) <  0   &&
               chxindex (tg->po, 0, htmlClose2) <  0) {
         logger (1, LOG_FILE, "<<<NO CLOSE_HTML<<<<<<<<<<<<<<<<<<");
         tempOut = asciifull (tg->po);
         logger (1, LOG_FILE, tempOut);
         logger (1, LOG_FILE, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
         free (tempOut);
      }
    
      chxclear (tg->po);
   }

   logger (0, LOG_FILE, " done");
   chxfree (empty);

   RETURN (1);
}

FUNCTION Chix makeChix (char *text) {
   Chix result;

   result = chxalloc (L(20), THIN, "process makeChix");
   chxofascii (result, text);
   return result;
}
