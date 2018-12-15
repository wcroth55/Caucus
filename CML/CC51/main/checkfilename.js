#
#---CHECKFileName.JS   Javascript for checkFileName() boxes function.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/23/02 Add filesafe to CC44.
#--------------------------------------------------------------

"var all_checked = 0;
"function checkFileName (theform) {
"   var fName = "";
"   var userid = theform.userid.value;

"   for (i=theform.upload.value.length-1;  i>=0; i--) {
"      var ttt = theform.upload.value.charAt(i);
"      var zzz = "\\";
"      if (theform.upload.value.charAt(i) == zzz)  break;
"      fName = theform.upload.value.charAt(i) + fName;
"   }

"   var found = 0
"   var ok = null;

"   for (i=0;   theform.elements[i].value != "url"; ++i) {
"      if (theform.elements[i].name == fName) {
"         found = 1;
"         var fileUserid = theform.elements[i].value;
"         break;          
"      } 
"   }
"   if (found == 1) {
"      ok = confirm("Do you want to update the current version of: " + 
"                    fName + " ?")
"      if (ok == true){
#         alert(fileUserid);
#         alert(userid);
"         if (fileUserid != userid) {
"            alert ("You are not the owner of this file, contact: " + 
"                    fileUserid);                
"            return false;
"         }
"         return true;
"      }
"      else  return false;
"   }
"   else  return true;
"}
