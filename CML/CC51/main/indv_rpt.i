#
#---INDV_RPT.I   Report results of a MOVE or COPY response set.
#
#  $inc(1)  "Copied" or "Moved"
#  $inc(2)  1=>warn that item proper was not deleted
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/18/02 %displayed_conf_name() calculates displayable conference name.
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#: CR 10/13/04 Use CSS.
#-----------------------------------------------------------------------

"Content-type: text/html
"
"<HTML>
"<HEAD>
"<TITLE>$inc(1) Responses</TITLE>
"<script type="text/javascript" language="javascript">
   include $(main)/clearchk.js
"</SCRIPT>
include $(main)/css.i 1
"</HEAD>

"<BODY $(body_bg) >
"<CENTER><H2>Copied Responses</H2></CENTER>

"<P>
"$inc(1) $sizeof($form(checklist)) responses to
"the <u>%displayed_conf_name ($(to_cnum))</u> conference,
"<A HREF="#" onClick="window.opener.go_to($(to_cnum),$(to_id),$(to_rd)); \
                      window.opener.pop_gone('indv');  window.close();">
"<NOBR>item $item_label($(to_cnum) $(to_id))</NOBR></A>.

if $inc(2)
   "<P>
   "(The item itself was not deleted.&nbsp; 
   "You must do that manually, since deleting an item also
   "deletes <b>all</b> of its sub-items.)
end

if $equal ($inc(1) Moved)
   if $less (%access($arg(2)) $priv(instructor))
      "<P>
      "(Since you are not an organizer, only your own responses will be 
      "moved; any other responses will just be copied.)
   end
end

"<P>
"<FORM>
"<CENTER>
"<INPUT TYPE="submit" NAME=submit VALUE="Close" 
"     onClick="window.opener.pop_gone('indv'); window.close(); return false;">
"</CENTER>
"</FORM>

"<P>
"<HR>
"</div>
"</body>
"</HTML>
#------------------------------------------------------------------
