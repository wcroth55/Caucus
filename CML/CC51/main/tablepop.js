# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#

"function tablepop (name) {
"   var tw = window.open ('$(mainurl)/tableedit.cml?$(nch)+' + name, 'tw', 
"         'menubar=1,toolbar=1,location=1,resizable=1,scrollbars=1,status=1');
#   tw.opener = self;
"   return false;
"}
