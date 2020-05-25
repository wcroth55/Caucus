#
#---VIEWITEM.I     Response box area at end of item.
#
#
#---At this point, we are at the "bottom" of the item.  If the item
#   is not frozen/readonly, we'll put up the text box for their response.
#
#   In either case, we have to put up one or more "submit" buttons
#   for the user to press.  Some of them go "on" to the next page,
#   which is typically the next item in arg(8), or (if there
#   are no more items), the conf home page or the caucus center page.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/18/02 %displayed_conf_name() calculates displayable conference name.
#: CR 01/12/03 Add ebox stuff (lots).
#: CR 05/14/03 Rearrange ebox blue table, so that no splits occur.
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#: CR  8/08/04 Add restricted date range for responding.
#: CR  9/01/04 Add ebox_submit() call.
#: CR 10/13/04 Layout changes.
#: CR 06/08/06 Rewrite for C5.1
#: CR 05/13/08 Add limits on # responses, size of responses.
#: CR 07/12/08 Add 'who has seen' control.
#: CR 01/23/2020 iPhone richtext, add explicit link to change to plaintext
#----------------------------------------------------------------

include $(main)/getNextPageTarget.i

#---Remember last response, in case additional responses get added
#   while this user is thinking & writing.
set vi_rlast $(lastresp)


"<form METHOD=POST NAME="ebox" 
"      onSubmit="return ebox_submit();"
"   ACTION="viewf.cml?$(nch)+$(nxt)+\
    $(vi_cnum)+$(vi_inum)+$(lastresp)+x+$arg(7)+$arg(8)">
"<input type="hidden" name="pass" value="0">

#---Should there be a response box on this page?
#   (No if frozen, readonly, or not organizer and current time outside 
#    restricted date range.)
set rdate0 $site_data ($(vi_cnum) $(vi_inum) rdate0)
set rdate1 $site_data ($(vi_cnum) $(vi_inum) rdate1)
set date_is_lo  $and ($not($(is_org))  $less ($time() 0$(rdate0)))
set date_is_hi  $and ($not($(is_org))  $greater (0$(rdate1) 0) \
                       $greater ($time() $plus ($(rdate1) 86400)))
#set no_rbox $or ($(frozen) $(date_is_lo)  $(date_is_hi) \
#                 $gt_equal ($priv(readonly) $(access)))
set no_rbox $or ($(frozen) $(date_is_lo)  $(date_is_hi) \
                 $equal($(itemAccess) $priv(readonly)) )

#---Check for limit on number of responses by this (non-organizer) user.
set respCount
if $and ($not ($(no_rbox))  $not($(is_org)))
   set  respLimit $plus (0 $site_data ($(vi_cnum) $(vi_inum) respLimit))
   if $(respLimit)
      set respCount $sql_query_select (SELECT count(*) FROM resps r \
          WHERE r.items_id=$(vi_inum) AND r.userid='$userid()' AND r.deleted=0 AND r.version= \
              (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) )
      set no_rbox $gt_equal ($(respCount) $(respLimit))
   end
end

#---Build entire response/toolbox inside a pretty blue curved box.
if $(isBlog)
   "<p/>
   "<div class="response_body">
   "<div style="$ifthen ($not_empty ($(lmargin))  margin-left: $(lmargin);) \
                $ifthen ($not_empty ($(rmargin)) margin-right: $(rmargin);)">
   "<b>Note:</b> Responses to this item will <b>not</b> appear in your published
   "blog.&nbsp;
   "<p/>
   "Any responses that are posted here are only visible to the
   "registered members of this conference.
   "</div>
   "</div>
   "<p/>
end

"<table cellspacing=0 cellpadding=0 border=0 >
"<tr>
"<td><a name="ebox"><img $(src_eboxtl)></a></td>
"<td $(eboxbg) colspan=5><b>Choose from:</b></td>
"<td align=right><img $(src_eboxtr)></td>

"<tr $(eboxbg)>
"<td colspan=5><img $(src_clear) width=1 height=3></td>

#---Try and detect case where user entered text, but then pressed Pass!
#   Warn user if possible.
if $and ($(use_javascript) $not ($(no_rbox)) \
         $not ($and ($(is_macintosh) $(is_explorer4))))
   "<TD rowspan=2><A HREF="javascript:press_pass(document.ebox)"
   "   ><IMG BORDER=0 $(src_pass) HSPACE=0></A></TD>
end
else
   "<TD rowspan=2><INPUT TYPE="image" NAME="pass" BORDER=0 $(src_pass) HSPACE=0></TD>
end
"<td rowspan=2>&nbsp;</td>




#"<td><img $(src_clear) width=1 height=3></td>

"<tr valign=top $(eboxbg)><td>&nbsp;</td>
#"<TD>
#  if $not (0$site_data (0 - if_$(iface)_hide_acton))
#     include $(main)/manipulate.i 1
#  end
#"</TD>

"<TD align=left>
   if $not (0$site_data (0 - if_$(iface)_hide_forget))
"    <table cellspacing=0 cellpadding=0 border=0>
"    <tr valign=top>
"    <td><img $(src_bullet)>&nbsp;</td>
"    <td>Forget&nbsp;item&nbsp;</td>
"    <td><INPUT TYPE=checkbox NAME=forget
"        $ifthen ($(forgot)  CHECKED)></td>
"    </table>
   end
"</td>

"<TD>
   if $not (0$site_data (0 - if_$(iface)_hide_whoseen))
      if $word (1 $site_data ($(vi_cnum) - enableWhoHasSeen) 1)
   "    <table cellspacing=0 cellpadding=0>
   "    <tr valign=top>
   "    <td>&nbsp;&nbsp;<img $(src_bullet)>&nbsp;</td>
   "    <td><A HREF="whoseen.cml?$(nch)+$(nxt)+\
              $(vi_cnum)+$(vi_inum)+%item_lastresp($(vi_inum))+x+$arg(7)+$arg(8)"
   "       >Who&nbsp;has&nbsp;seen&nbsp;this&nbsp;item?</A>&nbsp;</TD>
   "    </table>
      end
   end
"</td>
"<td></td>

"<TD>
   "<table CELLSPACING=0  CELLPADDING=0 border=0>
   "<tr valign=top>
   "<TD>&nbsp;&nbsp;<img $(src_bullet)>&nbsp;</td>
   "<td><B>Go to $(target_name):</B>&nbsp;</TD>
   "</table></td>

"<tr $(eboxbg)>
"<td colspan=7><img $(src_clear) width=1 height=3></td>

#---Frozen or 'readonly' items...
if $(no_rbox)
   "<tr $(eboxbg)><td colspan=7>&nbsp;</td>
   "<tr $(eboxbg)>
   "<td>&nbsp;</td>
   "<td colspan=5 align=center>
   if $(frozen)
      "<IMG $(src_ice)>  This item is <B>frozen</B>.&nbsp;
      "No more responses may be added.
   end
   elif $(date_is_lo)
      "This item is closed for responses until $word (1 $dateof($(rdate0))).
   end
   elif $(date_is_hi)
      "This item was closed for responses at the 
      "end of $word (1 $dateof($(rdate1))).
   end
   elif $not_empty($(respCount))
      "You have reached your maximum of $(respLimit) responses.
   end
   else
      "You have been given "read only" access to this conference.&nbsp;
      "Contact the organizer, %person($co_org($(vi_cnum))),
      " for more information.
   end
   "</TD>
   "<td>&nbsp;</td>

   "<tr valign=bottom>
   "<td><img $(src_eboxbl)></td>
   "<td colspan=5 $(eboxbg)>&nbsp;</td>
   "<td align=right><img $(src_eboxbr)></td>
   "</table>

   "<script type="text/javascript" language="javascript">
   "   function ebox_onload() {}
   "   function ebox_submit() { return true; }
   "</script>
end


#---Writeable items -- put up text box, etc.
else
   #---Actual textarea editing box.
   include $(main)/ebox_type.i
   if $(ebox_rt)
      include $(main)/ebox_rt.i $if ($empty($arg(9)) 0 2) 1 0 i_resp 1
   end
   elif $(ebox_ek)
      include $(main)/ebox_ek.i $if ($empty($arg(9)) 0 2) 1 0 i_resp
   end
   elif $(ebox_ta)
      include $(main)/ebox_ta.i $if ($empty($arg(9)) 0 2) 1 0 i_resp 1
   end

"<tr $(eboxbg)>
"<td>&nbsp;</td>
"<td colspan=5>
   #---Buttons below editing box (post+view, etc.)
   if $(ebox_ta)
      "<table border=0>
#     "<TR valign=top>
      "<TR>
      "<TD><INPUT TYPE=SUBMIT NAME="submitpv" 
      "          VALUE="Post &amp; View">&nbsp; </TD>
      "<TD>it&nbsp;now.</TD>
   
      "<TD>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
   
      "<TD><INPUT TYPE=SUBMIT NAME="submitpg" 
      "       VALUE="Post &amp; Go&nbsp;&nbsp;&nbsp;">&nbsp; </TD>
      "<TD>to $(target_name)</TD>
   
      "<TD>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
      "<TD>
          include $(main)/boxsize.i 0
      "</TD>
      "</table>
   end

   if $(ebox_rt)
      "<table border="0" width="100%">
      "<TR>
      "<TD colspan='2'><INPUT TYPE=SUBMIT NAME="submitpv"
      "          VALUE="Post &amp; View">&nbsp;
      "  it&nbsp;now.&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
      if $greater ($str_index(iPhone $http_user_agent()) 0)
         "<td align='right'><a href="#" onClick="return ebox_switch();">Use plaintext editor</a></td>
      end
      "</table>
   end

"</td>
"<td>&nbsp;</td>
"<tr>
"<td><img $(src_eboxbl)></td>
"<td colspan=5 $(eboxbg)>&nbsp;</td>
"<td align=right><img $(src_eboxbr)></td>
"</table>

end

"</FORM>
#----------------------------------------------------------------
