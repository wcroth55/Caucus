#
#---INDVRESP.JS   Definition of Javascript functions needed to support
#   (a) the manipulation of the checkboxes next to individual
#       responses in viewitem.cml
#
#   (b) the check for non-empty response box when user clicks on Pass.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 01/12/03 Add ebox stuff (myform.pass.value).
#: CR 06/10/04 press_pass() must check myform.tx_resp != null.
#----------------------------------------------------------------------

"function reloader() {
"   location.href = "$(mainurl)/viewitem.cml?$unique()+$arg(2)+$arg(3)+$arg(4)+\
                     $arg(5)+$arg(6)+$arg(7)+$arg(8)";
"   return true;
"}

"function go_to (c,i,r) {
"   location.href = "$(mainurl)/viewitem.cml?$unique()+$(nxt)+" + 
"                   c + "+" + i + "+" + r + "+x+1+x#here";
"   return true;
"}

"function press_pass(myform) {
"   if (myform.tx_resp != null  &&  myform.tx_resp.value != "") {
"      if (! confirm (
"         "There is text in the response box.\n\n" +
"         "  -  press OK       to discard it and go on, or\n" +
"         "  -  press Cancel to remain here.") ) {
"         return;
"      }
"   }
"   myform.pass.value = '1';
"   myform.submit();
"}
#----------------------------------------------------------------------
