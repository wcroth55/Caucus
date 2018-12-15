#
#---ebox_ek.i   Edit box, using Ekit applet
#
#   inc(1)   0 = new text (new item or response)
#            1 = old text (existing item or response, specified by
#                          arg(3), arg(4), arg(5) triplet)
#            2 = text in progress (arg(10) has type, arg(9) is name of
#                          var that contains text)
#
#   inc(2)   Allow for "your response will be anonymous". (0 or 1)
#   inc(3)   1=>allow all text types, 0=>only allow permitted types
#   inc(4)   basename of help file for "get Help" link.  E.g. i_item, i_resp.
#   inc(5)   optional js check function -- only submit if check function is true
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 02/23/03 New file.
#: CR 06/21/03 Add optional inc(5) javascript check function.
#: CR  4/27/04 Modify to use popup file attach dialog. (DOES NOT WORK YET!)
#----------------------------------------------------------------------------

"<tr $(eboxbg)><td></td>
"<td colspan=2
"   ><table cellspacing=0 cellpadding=0 border=0>
"    <tr>
"    <td><img $(src_bullet)>&nbsp;$word ($plus ($inc(1) 1) Enter Edit Enter) 
"         your text below and press&nbsp;</td>
"    <td><input type="button" name="submitpv" value="Post"
"            onClick="document.ebox.tx_resp.value = \
                      document.ebox.Ekit.getDocumentText();  \
                      $ifthen ($not_empty($inc(5)) if ($inc(5))) \
                      document.ebox.submit();">
"    </table>
   if $inc(2)
      if $bit_and (32 $re_bits($(vi_cnum) $(vi_inum) 0))
         "&nbsp;&nbsp;Your response will be <B>anonymous</B>.
         "<td></td>
      end
   end
"</td>
"<td colspan=4><table cellspacing=0 cellpadding=0>
   "<tr>
   "<TD>&nbsp;&nbsp;<img $(src_bullet)>&nbsp;</TD>

   "<TD valign=bottom><a href="$(mainurl)/uploadpop.cml?$(nch)+$(nxt)+\
         $arg(3)+$arg(4)+$arg(5)+$inc(1)"
   "   onClick="openNearby('attach', 500, 300, \
          'resizable=1,scrollbars=1,toolbar=0');  return true;"
   "    target="attach"><img $(src_upload) border=0></a></td>
   "</table></td>

"<tr $(eboxbg)><td colspan=7 $(eboxbg)><img $(src_clear) width=1 height=5></td>

"<INPUT TYPE="hidden" NAME="unique_box" VALUE="$unique()">
"<INPUT TYPE="hidden" NAME="submitpv"   VALUE="1">

if $equal ($inc(1) 1)
   set textprop $plus ($re_prop ($arg(3) $arg(4) $arg(5)) 1)
   set tp_resp  $word ( $(textprop) format literal html cml )
   "<input type=hidden name="tp_resp" value="$(tp_resp)">
end
else
   "<input type=hidden name="tp_resp" value="cml">
end


"<tr $(eboxbg)><td></td>
"<td colspan=5>
   if $(is_explorer4)
      "<OBJECT ID="Ekit" classid="clsid:8AD9C840-044E-11D1-B3E9-00805F499D93"
      "  WIDTH="$(wordwide)" HEIGHT="$(wordhigh)"
      "  codebase="http://java.sun.com/products/plugin/1.3/jinstall-13-win32.\
                   cab#Version=1,3,0,0">
      "   <XMP><APPLET CODEBASE="." CODE="com.hexidec.ekit.EkitApplet.class" 
      "      ARCHIVE="$(ekit)/ekitapplet.jar" NAME="Ekit" WIDTH="$(wordwide)" 
      "       HEIGHT="$(wordhigh)"></XMP>
      "      <PARAM NAME="codebase" VALUE=".">
      "      <PARAM NAME="code" VALUE="com.hexidec.ekit.EkitApplet.class">
      "      <PARAM NAME="archive" VALUE="$(ekit)/ekitapplet.jar">
      "      <PARAM NAME="type" VALUE="application/x-java-applet;version=1.3">
      "      <PARAM NAME="scriptable" VALUE="true">
      "      <PARAM NAME="STYLESHEET" VALUE="$(ekit)/ekit.css">
      "      <PARAM NAME="LANGCODE" VALUE="en">
      "      <PARAM NAME="LANGCOUNTRY" VALUE="US">
      "      <PARAM NAME="TOOLBAR" VALUE="true">
      "      <PARAM NAME="SOURCEVIEW" VALUE="false">
      "      <PARAM NAME="EXCLUSIVE" VALUE="true">
      "      <PARAM NAME="MENUICONS" VALUE="true">
      "   </APPLET>
      "</OBJECT>
   end
   else
      "<EMBED NAME="Ekit" type="application/x-java-applet;version=1.3"
      "  CODE="com.hexidec.ekit.EkitApplet.class" 
      "  ARCHIVE="$(ekit)/ekitapplet.jar"
      "  WIDTH="$(wordwide)" HEIGHT="$(wordhigh)"
      "  pluginspage="\
             http://java.sun.com/products/plugin/1.3/plugin-install.html">
      "</EMBED>
   end
   
   "<textarea name="tx_resp" style="display:none" rows="1" cols="20"></textarea>
"</td>
"<td></td>


"<script type="text/javascript" language="JavaScript">
"  function ebox_onload (keywords) {
   if $equal ($inc(1) 1)
"     document.ebox.Ekit.setDocumentText
"        ('$escsingle($re_text($arg(3) $arg(4) $arg(5)))');
   end
   elif $equal ($inc(1) 2)
"     document.ebox.Ekit.setDocumentText ('$($arg(9))');
   end
"  }
"  function ebox_addtext (theText) {
"     document.ebox.Ekit.setDocumentText (
"        document.ebox.Ekit.getDocumentText() + theText);
"  }
"</SCRIPT>

#---------------------------------------------------------------------
