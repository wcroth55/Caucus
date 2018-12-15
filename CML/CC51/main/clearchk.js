#
#---CLEARCHK.JS   Clear all check boxes in parent viewitem.cml window.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#---------------------------------------------------------------------

#---New code for "one form per response" in viewitem.cml
"   for (i=0;   i<window.opener.document.forms.length;   ++i) {
"      if (window.opener.document.forms[i].bx != null)
"          window.opener.document.forms[i].bx.checked = false;
"   }

#---Old code
#"  for (i=0;   window.opener.document.scroll.elements[i].value != "LAST";  
#"       ++i) {
#"     if (window.opener.document.scroll.elements[i].name == "bx") {
#"         window.opener.document.scroll.elements[i].checked = false;
#"     }
#"  }
#---------------------------------------------------------------------
