#
#---HT.I   Help file Front include.  Takes care of two things:
#
#   (a) resizes of the current window -- reloads current page with new
#       window sizes as args, so it can store the new sizes as user vars.
#
#   (b) putting up the title and back/close buttons.
#
#   inc(1)   including file name (e.g., "h_center.cml")
#   inc(2)   $quote()'d help page title
#   inc(3)   1 => don't show Back button  (0 or empty shows button)
#
#   arg(1)   'x' or new window width
#   arg(2)   'x' or new window height
#   arg(3)   passed on
#   arg(4)   passed on
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/13/04 Use CSS.
#------------------------------------------------------------------------

"Content-type: text/html
"
"<HTML>
"<HEAD>
"<TITLE>$unquote($inc(2))</TITLE>

"<SCRIPT LANGUAGE="JavaScript">
"   started = new Date();
"   function resizer() {
"      thistime = new Date();
"      if (thistime.getTime() - started.getTime() < 3000)    return;
"      xv = $if ($(is_netscape4) window.innerWidth  document.body.clientWidth);
"      yv = $if ($(is_netscape4) window.innerHeight document.body.clientHeight);

#      ---Set size of this window in our opener so he knows, too!
"      window.opener.xs_help = xv;
"      window.opener.ys_help = yv;

#      ---Reload ourself... so that we can write new sizes into user vars.
"      location.href = "$(helpurl)/$inc(1)?" + xv + "+" + yv + "+$arg(3)+$arg(4)";
"   }
"</SCRIPT>
include $(main)/css.i 1
"</HEAD>

"<BODY $(body_bg) $ifthen ($(use_resize) onResize="resizer();")>

"<FORM NAME="helpform">
   "<TABLE CELLSPACING=0 CELLPADDING=0 WIDTH="100%">
   "<TR>
   "<TD><FONT SIZE=+2><B>$unquote($inc(2))</B></FONT></TD>
   "<TD ALIGN=right
   if $not(0$inc(3))
      "><INPUT TYPE="button" VALUE="Back" onClick="history.back();"
   end
   ">&nbsp;<INPUT TYPE="button" VALUE="Close" 
   "   onClick="window.opener.pop_gone('help'); window.close();"></TD>
   "</TABLE>
"</FORM>
"<P>

#---If arg(1) and arg(2) are non-empty, and are numbers, save
#   the resized value as persistent variables.
if $and ($not_empty ($arg(1))   $equal ($is_num($arg(1)) $strlen($arg(1))) \
         $not_empty ($arg(2))   $equal ($is_num($arg(2)) $strlen($arg(2))) )
   eval $set_user_data ($userid() xs_help $arg(1))
   eval $set_user_data ($userid() ys_help $arg(2))
end

#------------------------------------------------------------------------
