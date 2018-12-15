#
#---ISELECT.I   Display form elements for selecting a list of items
#
#   Input:
#      $inc(1)  Text to place in front of form elements.
#
#   Input: (variables)
#      ikey   current item list "key" (all, new, num, etc.)
#      idata  current item list data  (for keys name, num, author, etc.)
#
#   Output: (form fields)
#      ikey
#      idata
#
#   Notes:
#      Must be enclosed inside a <FORM> and a <TABLE>.  
#      Caller puts up SUBMIT button.
#
#      The results of a submitted form are usually processed by
#      itemListOf.i to produce a triplet list of items.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/13/04 Use CSS.
#----------------------------------------------------------------------

set s.all
set s.new
set s.num
set s.name
set s.author
set s.since
set s.past
set s.mine
set s.forgot
set s.none
set s.$(ikey) SELECTED

if $not_empty($(s.all)$(s.new)$(s.mine)$(s.none)$(s.forgot))
   set idata
end

"<TR class="itemlist_selector">
"<TD ALIGN=RIGHT>$inc(1)&nbsp;&nbsp; </TD>
"<TD>
   "<SELECT NAME="ikey">
   "  <OPTION $(s.all)    VALUE="all"   >All the items
   "  <OPTION $(s.new)    VALUE="new"   >Items with new activity
   "  <OPTION $(s.since)  VALUE="since" >Items active since (date):
   "  <OPTION $(s.past)   VALUE="past"  >Items active in past N days:
   "  <OPTION $(s.num)    VALUE="num"   >Items numbered:
   "  <OPTION $(s.name)   VALUE="name"  >Items titled (keywords):
   "  <OPTION $(s.author) VALUE="author">Items created by (author):
   "  <OPTION $(s.mine)   VALUE="mine"  >Items you created
   "  <OPTION $(s.forgot) VALUE="forgot">Forgotten items
   "  <OPTION $(s.none)   VALUE="none"  >No items
   "</SELECT>&nbsp;
"</TD>

"<TD><INPUT TYPE="text" NAME="idata" SIZE=12 VALUE="$(idata)"></TD>
"<td></td>
#------------------------------------------------------------------
