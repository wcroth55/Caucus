#
#---CHECKALL.JS   Javascript for checkall() boxes function.
#
#   To stay compatible with IE 3.02, looks for a "LAST" element
#   of theform.elements[], since theform.elements.length is
#   not available (or doesn't work right).
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 02/06/06 Assume that theform.elements.length exists!
#----------------------------------------------------------------------

   "  var all_checked = 0;
   "  function checkall (theform) {
   "     all_checked = 1 - all_checked;
#  "     for (i=0;   theform.elements[i].value != "LAST";   ++i) {
   "     for (i=0;   i < theform.elements.length;   ++i) {
   "        if (theform.elements[i].name != "ibox")  continue;
   "        theform.elements[i].checked = all_checked;
   "     }
   "     return false;
   "  }
