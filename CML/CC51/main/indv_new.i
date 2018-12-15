#
#---INDV_NEW.I   Handle results of MARK from indvresp.cml.
#                Then close current (pop-up) window.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#-----------------------------------------------------------------------

if $not_empty ($form(checklist))
#  set r $minus ($word (1 $form(checklist)) 1)
#  eval  $set_it_seen ($arg(2) $arg(3) $(r))
end

set vi_markseen 0

"Content-type: text/html
"
"<HTML>
"<HEAD>
"<TITLE> ...Working... </TITLE>
"<script type="text/javascript" language="javascript">
   "   window.opener.reloader();
   "   window.opener.pop_gone('indv');
   "   window.close();
"</SCRIPT>
"</HEAD>

"<BODY $(body_bg) >
"<CENTER><H2>...Working...</H2></CENTER>
"</div>
"</body>
"</HTML>
