
/*** FUNC_PRIV.   Return privilege level values for roles.
/
/    func_priv     (result, arg);
/    func_privName (result, arg);
/
/    int code   = privValue (role);
/    char *role = privName  (code);
/   
/    Purpose:
/       Contains the fundamental definition and mapping of the privilege
/       levels assigned to specific roles.
/
/    How it works:
/       privValue() is for internal code.  It translates a role name
/       to the matching privilege code value.  Privilege values are always multiples of 10.
/
/       privName() is the reverse; it translates the privilege code
/       to the role name.
/
/       func_priv provides the $priv() CML function.  It calls privValue()
/       to do the actual mapping.
/
/       func_privName() does the reverse mapping in CML.
/
/    Home:  funcpriv.c
/ */

/*** Copyright (C) 2008 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/28/08 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"
#include "sqlcaucus.h"
#include "sql.h"

#define  SITE_CLEARED  -100
#define  QUERY_MAX     2000
#define  USER_MAX       256

extern int       cml_debug;
extern AsciiBuf  sqlbuf;

#define CODE_PRIMARY  100
#define CODE_ORGANIZER  50
#define CODE_INSTRUCTOR 40
#define CODE_INCLUDE    30
#define CODE_READONLY   20
#define CODE_WRITEONLY  10
#define CODE_MINIMUM    10
#define CODE_EXCLUDE     0
#define CODE_INHERIT    -1

#define TEMP_SIZE 20
FUNCTION  int privValue (char *role) {
   char temp[TEMP_SIZE];
   if (role == NULL)            return   0;

   strncpy (temp, role, TEMP_SIZE);
   temp[TEMP_SIZE-1] = '\0';
   strlower (temp);

   if (roleMatch (temp, "primary"))    return CODE_PRIMARY;
   if (roleMatch (temp, "organizer"))  return CODE_ORGANIZER;
   if (roleMatch (temp, "instructor")) return CODE_INSTRUCTOR;
   if (roleMatch (temp, "include"))    return CODE_INCLUDE;
   if (roleMatch (temp, "readonly"))   return CODE_READONLY;
   if (roleMatch (temp, "writeonly"))  return CODE_WRITEONLY;
   if (roleMatch (temp, "minimum"))    return CODE_MINIMUM;
   if (roleMatch (temp, "exclude"))    return CODE_EXCLUDE;
   if (roleMatch (temp, "inherit"))    return CODE_INHERIT;
   return 0;
}

FUNCTION char *privName (int code) {
   if (code == CODE_PRIMARY)    return "primary";
   if (code == CODE_ORGANIZER)  return "organizer";
   if (code == CODE_INSTRUCTOR) return "instructor";
   if (code == CODE_INCLUDE)    return "include";
   if (code == CODE_READONLY)   return "readonly";
   if (code == CODE_WRITEONLY)  return "writeonly";
   if (code == CODE_EXCLUDE)    return "exclude";
   if (code == CODE_INHERIT)    return "inherit";
   return CODE_EXCLUDE;
}

FUNCTION int roleMatch (char *a, char *b) {
   if (a[0] != b[0])  return 0;
   if (a[1] != b[1])  return 0;
   if (a[2] != b[2])  return 0;
   return 1;
}

#define ROLE_SIZE 10
FUNCTION  int func_priv (Chix result, Chix arg) {
   char role[ROLE_SIZE];
   int  code;

   ascofchx (role, arg, 0, ROLE_SIZE);
   code = privValue (role);
   sprintf (role, "%d", code);
   chxcat  (result, CQ(role));
   return 0;
}

FUNCTION int func_privName (Chix result, Chix arg) {
   int4 code;
   char *privName();

   if (NOT chxnum (arg, &code))  code = CODE_EXCLUDE;
   chxcat (result, CQ(privName(code)));
}
