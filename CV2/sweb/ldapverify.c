
/*** LDAP_VERIFY.   Verify that a uid/pw combination is valid in an LDAP
/                   database.
/
/    ok = ldap_verify (uid, pw, hostname, portnum, opt, bindas, userdn, pwname);
/   
/    Parameters:
/       uid       userid
/       pw        password
/       hostname  LDAP server host
/       portnum   LDAP server port number
/       opt       option string
/       bindas    how to bind to server
/       userdn    dn of desired user
/       pwname    name of attribute containing password
/
/    Purpose:
/       Given a userid and a password, and the necessary info about
/       an LDAP server and its schema, determine if the userid and
/       password are valid (i.e. the userid is properly authenticated
/       by that password) in the LDAP database.
/
/    How it works:
/       There are two possible types of schemas that ldap_verify understands:
/       (1) Authentication by binding.  If we can bind to the LDAP server
/           *as* uid/pw, the userid is considered authenticated.  In this
/           case, bindas contains a string something like:
/              "uid=%s,ou=people,dc=myhost,dc=com %s"
/           into which is substituted 'uid' and 'pw'.  The first word
/           is the binding, the second word is the password.  'userdn'
/           and 'pwname' must be empty(!)
/
/       (2) Authentication by lookup.  We bind to the server, and then
/           lookup a particular user and see if the encryption of the
/           passwords match.  'bindas' is either empty (unauthenticated
/           aka public aka anonymous bind), or a specific dn that
/           has access to read passwords.  (Again, the first word is
/           the binding, the second word is the password to bind with.)
/
/           'userdn' is the dn of each user; it will be a string like:
/              "uid=%s,ou=people,dc=myhost,dc=com"
/           and 'pwname' is the name of the attribute that contains the
/           encrypted password (typically "userPassword").
/
/       Opt contains option keywords.  Currently supports:
/          (empty)   do normal LDAP connection
/          "ssl"     use SSL connection to LDAP server.
/
/    Returns: 
/        0  success
/       -1  cannot connect to 'hostname'
/       -2  cannot bind to ldap server
/       -3  cannot find 'dn'
/       -4  cannot find 'pwname'
/       -5  wrong uid/pw combination
/       -6  LDAP server's encryption mode not supported
/       -7  LDAP disabled in Caucus
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/ldapget.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/18/02 New function. */
/*: CR 01/28/03 Add ldap_opt arg. */

#include <stdio.h>
#include "caucusldap.h"

#if ! DISABLED
#include <ldap.h>
#include <unistd.h>
#endif
#include "logger.h"
#define  FUNCTION
#define  EMPTY(x)  (!x[0])


FUNCTION int ldap_verify (char *uid, char *pw, char *hostname, int portnum, 
                          char *opt, char *bindas, char *userdn, char *pwname) {
#if DISABLED
   return (-7);
#else
   LDAP   *ld;
   LDAPMessage *result, *e;
   int     version = LDAP_VERSION3;
   char   *attrs[2];
   char  **vals;
   int     i;

   char    temp[200], bind_dn[200], bind_pw[200], dn[400], ldap_uri[400];
   int     bind_rc, rc;
   char    pw_recvd[200], pw_actual[200];
   char    junk[200];
   char   *crypt();

   logger (0, LOG_FILE, "entered ldap_verify");

   /*** There must be *some* password! */
   if (! pw[0])                                             return (-5);

   /*** Initialize the connection to the LDAP server, either for SSL
   /    or "plain" connections. */
   logger (0, LOG_FILE, opt);
   if (streq (opt, "ssl")) {
      if (portnum == 0)  portnum = 636;
      sprintf (ldap_uri, "ldaps://%s:%d", hostname, portnum);
   
      rc = ldap_initialize (&ld, ldap_uri);
      sprintf (temp, "ldap_initialize(&ld,'%s') = %d", ldap_uri, rc);
      logger  (0, LOG_FILE, temp);
      if (rc != LDAP_SUCCESS)                               return (-1);

      rc = ldap_set_option (NULL, LDAP_OPT_PROTOCOL_VERSION, (void *) &version);
      sprintf (temp, "ldap_set_option=%d", rc);
      logger (0, LOG_FILE, temp);
      if (rc != LDAP_SUCCESS)                               return (-1);
   }
   else {
      if (portnum == 0)  portnum = LDAP_PORT;
      if ( (ld = ldap_init (hostname, portnum)) == NULL)    return (-1);
   }

   /*** Bind to the server. */
   sprintf  (temp, bindas, uid, pw);
   for (i = strlen (temp) - 1;   i > 0  &&  temp[i]!=' ';   --i) ;
   temp[i] = '\0';
   strcpy (bind_dn, temp);
   strcpy (bind_pw, temp+i+1);
   bind_rc = ldap_simple_bind_s (ld, bind_dn, bind_pw);
   sprintf (junk, "ldap_simple_bind (ld, '%s', '%s') = %d",
                   bind_dn, bind_pw, bind_rc);
   logger (0, LOG_FILE, junk);

   /*** Case 1: authentication by binding. */
   if (EMPTY (userdn)) {
      ldap_unbind  (ld);
      if (bind_rc == LDAP_SUCCESS)                         return ( 0);
      if (bind_rc == LDAP_INVALID_CREDENTIALS   ||
          bind_rc == LDAP_INAPPROPRIATE_AUTH)              return (-5);
      return (-2);
   }

   if (bind_rc != LDAP_SUCCESS)   { ldap_unbind (ld);      return (-2); }
      
   /*** Rest is case 2, authentication by lookup.   Build the dn for
   /    user uid, and search for attribute 'pwname'. */
   sprintf (dn, userdn, uid); /* e.g. "uid=%s,dc=myhost,dc=com" */
   attrs[0] = pwname;
   attrs[1] = NULL;
   if ( ldap_search_s (ld, dn, LDAP_SCOPE_BASE, "(objectclass=*)",
                       attrs, 0, &result) != LDAP_SUCCESS)  {
      ldap_unbind  (ld);
      return (-3);
   }
   
   /*** Go through the result, and get the password. */
   pw_recvd[0] = '\0';
   if ( (e = ldap_first_entry (ld, result)) != NULL) {
      if ( (vals = ldap_get_values (ld, e, pwname)) != NULL) {
         for (i=0;   vals[i] != NULL;    ++i) {
            if (i > 0)  strcat (pw_recvd, " ");
            strcat (pw_recvd, vals[i]);
         }
         ldap_value_free (vals);
      }
   }
   ldap_msgfree (result);
   ldap_unbind  (ld);
   if (EMPTY (pw_recvd))   return (-4);

   /*** {CRYPT} encryption. */
   if (strindex (pw_recvd, "{CRYPT}") == 0) {
      strcpy (pw_actual, pw_recvd+7);
      return (strncmp (pw_actual, crypt (pw, pw_actual), 13) == 0 ? 0 : -5);
   }

   return (-6);
#endif
}
