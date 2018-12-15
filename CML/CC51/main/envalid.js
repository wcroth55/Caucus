#
#---ENVALID.JS.   Define javascript is_valid() function for
#                 cus_engrp.cml and per_engrp.cml.
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#

"  function is_valid() {
"     if ($(is_explorer3))  return true;
"     var hours = false;
"     for (i=0;   i<24;   ++i)  hours = hours || document.enote.hd[i].checked;
"     if  (! hours)
"        return confirm ("No hours were selected -- will not send e-mail.");
"     var mnths = false;
"     for (i=0;   i<31;   ++i)  mnths = mnths || document.enote.md[i].checked;
"     var weeks = false;
"     for (i=0;   i<7;    ++i)  weeks = weeks || document.enote.wd[i].checked;
"     if ( (! mnths) && (! weeks))
"        { alert ("You must check at least one day.");   return false; }
"     if  (mnths && weeks) {
"        confirm ("Check days of the month OR days of the week, not both.");
"        return false;
"     }
"     return true;
"  }
