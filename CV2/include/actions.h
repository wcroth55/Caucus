/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** ACTIONS.H.  Definitions of server action codes. */

/*: CR 11/16/88  0:17 Initial release of server code. */
/*: CR 11/16/88 13:17 Add ARMUS (remove user) action code. */
/*: JV  2/05/91 14:28 Add ANULL (null message) action code. */
/*: CR  7/01/91 20:24 Add #ifndef stuff. */

#ifndef ACTIONS_H
#define ACTIONS_H

#define  ADONE     0x0100
#define  ALOCK     0x0200

#define  ACMPW     0x1500      /* Caucus Manager: get Password verification. */
#define  ACMCP     0x1600      /*   "      "      change password. */
#define  ARMCN     0x1700      /*   "      "      delete conference. */
#define  AMKCN     0x1800      /*   "      "      make conference. */
#define  ARMUS     0x1900

#define  AMDGT     0x3000      /* Get more of dictionary string. */
#define  ANXCF     0x3200
#define  APDIR     0x3500
#define  AMAKE     0x3A00
#define  ALDME     0x3B00
#define  AAPND     0x3C00
#define  AGTRC     0x3D00
#define  ALDMA     0x3E00
#define  AVIEW     0x3F00
#define  AWRIT     0x4000
#define  AREAD     0x4100
#define  AFNAM     0x5000
#define  ACHEK     0x5100
#define  ACLCH     0x5200      /* System clock check: clock set back? */
#define  AMDPS     0x5500      /* Position in dictionary & get 1st 256 chrs */
#define  AKILL     0x6000
#define  ADRCH     0x6100      /* Check for USER, TEXT directory. */
#define  ACLOS     0x6200
#define  AUNLK     0x6400
#define  ANULL     0x7900  /* This is used to send a msg with no contents. */
                           /* The server replies with an empty msg. */

   /* Action codes must be less than 0x8000 to avoid sign-extension!! */

#endif
