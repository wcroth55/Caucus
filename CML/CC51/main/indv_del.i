#
#---INDV_DEL.I   Handle results of DELETE from indvresp.cml.
#                Then close current (pop-up) window.
#   arg(1)   unique
#       2    cnum
#       3    inum  (id, not label)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#-----------------------------------------------------------------------

set indvAccess  %access($arg(2))
set indvOrg     $gt_equal ($(indvAccess) $priv(instructor))

set deleteTime $time()
for r in $form(checklist)
   if $not_equal ($(r) 0)
      include $(main)/deleteResponse.i $arg(3) $(r) $(deleteTime) $(indvOrg)
#     eval $resp_del ($arg(2) $arg(3) $(r))
#     include $(main)/rlibdel.i $arg(2) $arg(3) $(r)
   end
end

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
