#
#---ebox_ta.i   Edit box, using <textarea>.
#
#  Input:
#   inc(1)   0 = new text (new item or response)
#            1 = old text (existing item or response, specified by
#                          arg(3), arg(4), arg(5) triplet)
#            2 = text in progress (arg(10) has type, arg(9) is name of
#                          var that contains text)
#
#   inc(2)   Allow for "your response will be anonymous". (0 or 1)
#   inc(3)   1=>allow all text types, 0=>only allow permitted types
#   inc(4)   basename of help file for "get Help" link.  E.g. i_item, i_resp.
#   inc(5)   0=>item, 1=>response
#   inc(6)   1=>don't show "use richtext" link!
#   prop     (text property)
#   text     actual text
#
#   Assumes textbox appears inside a table (the famous new curved-blue-box!)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 12/11/02 New file.
#: CR  1/15/03 Add editor-choice pop-up when user clicks in <textarea>.
#: CR  1/25/03 ...but only if alert_ebox is on!
#: CR  8/08/03 Put color background on "anonymous" line.
#: CR  3/27/04 Add "symbols" button.
#: CR  4/27/04 Modify to use popup file attach dialog.
#: CR  9/01/04 Add ebox_submit definition.
#: CR 10/13/04 Add inc(5), disable symbols for netscape4, test can_use_richtext
#: CR 01/10/05 Changes to support mathsyms in both plaintext and richtext.
#: CR 06/08/06 Rewrite for C5.1
#: CR 11/05/07 Remove javascript dependencies.
#: CR 05/12/08 Add word-counter.
#: CR 08/19/09 Do not apply wordLimit for instructors or organizers
#----------------------------------------------------------------------------

if $(use_javascript)
   "<script type="text/javascript" language="javascript">
   "  function ebox_onload (keywords) {
   "     setTimeout ("limitWords()", 20000);
   "  }

   "  var itemLimit = 0;
      if $inc(5)
         if $less ($(itemAccess) $priv(instructor))
   "        itemLimit = $plus(0 $site_data($arg(3) $arg(4) wordLimit));
         end
      end
   
   "  function ebox_submit() {
   "     if (itemLimit > 0) {
   "        count = countWords(document.ebox.tx_resp.value);
   "        if (count > itemLimit) {
   "           confirm ("Your response must be limited to " + itemLimit + " words.\n"
   "                    + "Please remove " + (count-itemLimit) + ".");
   "           return false;
   "        }
   "     }
   "     return true;
   "  }

   "  var prevCount = 0;
   "  function limitWords() {
   "     if (itemLimit == 0)  return;
   "     count = countWords(document.ebox.tx_resp.value);
   "     if (count > itemLimit  &&  count >= prevCount) {
   "        confirm ("Your response must be limited to " + itemLimit + " words.\n"
   "                 + "Please remove " + (count-itemLimit) + ".");
   "     }
   "     prevCount = count;
   "     setTimeout ("limitWords()", 20000);
   "  }
   "  function countWords(usertext) {
   "     var count = 0;
   "     var state = 0;
   "     var c;
   "     var space;
   "     for (i=0;   i<usertext.length;   ++i) {
   "        c = usertext.charAt(i);
   "        space = (c==' ' || c=='\t' || c=='\n');
   "        if (state==0 && ! space) {
   "           ++count;
   "           state = 1;
   "        }
   "        else if (state==1 && space)  state = 0;
   "     }
   "     return count;
   "  }

   "  function ebox_addtext (theText) {
   "     document.ebox.tx_resp.value += theText;
   "     return false;
   "  }
   "  function ebox_addnewline () {
   "     document.ebox.tx_resp.value += '\n';
   "  }
   "  function ebox_texttype (theType) {
   "     document.ebox.tp_resp.selectedIndex = 2;
   "  }
   "</script>

   #---Define javascript func for "you have new editor choices!" pop-up.
   if $and (0$(alert_ebox) $empty ($user_data ($userid() ebox)))
      if $(is_explorer55)
         "<script type="text/javascript" language='javascript'>
         "var saw_choice = 0;
         "function new_editor_choice() {
         "   if (saw_choice)  return;
         "   window.open ("$(mainurl)/editorchoice.cml?$unique()",\
                "myeditor", "resizable=1,scrollbars=1,width=400,height=300");
         "   saw_choice = 1;
         "}
         "</script>
      end
   end
end

   #---Enter your (whatever) text, or upload a file.
   "<tr $(eboxbg)><td>&nbsp;</td>
   "<td colspan=2><table cellspacing=0 cellpadding=0 border=0>
      "<TR>
      "<TD><img $(src_bullet)>&nbsp;$word ($plus ($inc(1) 1) Enter Edit Enter)\
        &nbsp;your&nbsp;</td>
      "<TD>
         if   $equal ($inc(1) 0)
            set tp_resp
         end
         elif $equal ($inc(1) 1)
            set textprop $plus ($(prop) 1)
            set tp_resp  $word ( $(textprop) format literal html cml )
         end
         else
            set tp_resp  $arg(10)
         end

         include $(main)/resptype.i   tp_resp $inc(3) $(tp_resp)
      "</TD>
      "<TD>text&nbsp;below&nbsp;&nbsp;&nbsp;&nbsp;</TD>
   "</table></td>

   "<td><table cellspacing=0 cellpadding=0 border=0>
      "<tr valign=middle>
      "<TD>&nbsp;&nbsp;<img $(src_bullet)>&nbsp;</TD>

      "<TD valign=bottom><a href="$(mainurl)/$(uploadPage)$(xuploadLicense).cml?$(nch)+$(nxt)+\
            $arg(3)+$arg(4)+$arg(5)+$inc(1)"
      "   onClick="openNearby('attach', 500, 330, \
             'resizable=1,scrollbars=1,toolbar=0');  return true;"
      "    target="attach"><img $(src_upload) border=0></a></td>

      "<TD>&nbsp;a&nbsp;file&nbsp;&nbsp;&nbsp;</TD>
      if $and ($not ($and ($(is_netscape4) $not($(is_netscape5)))) $(use_javascript))
         "<TD>&nbsp;&nbsp;<img $(src_bullet)>&nbsp;</TD>
         "<td valign=middle>
         include $(main)/mathsym.i 0
         "<a href="#" onClick="return flipSymbols();"
         "    ><img src="/~$caucus_id()/GIF50/bsymbols.gif" border=0
         "  ></a><br><img $(src_clear) width=1 height=1></td>
      end
      "</table></td>

   "<td colspan=2><table cellspacing=0 cellpadding=0 border=0>
      "<tr valign=top>
      "<TD><nobr>&nbsp;&nbsp;<img $(src_bullet)
      "   >&nbsp;Get&nbsp;<A
      if $(use_javascript)
         " HREF="javascript: \
           pop_up('help', '$(helpurl)/$inc(4).cml?x+x+1');"
      end
      else
         " href="$(helpurl)/$inc(4).cml?x+x+1" target="_blank"
      end
      ">Help</A></nobr></TD>
      if $and ($not (0$inc(6))  $(can_use_richtext))
         "<TD><nobr>&nbsp;&nbsp;<img $(src_bullet)
         "   >&nbsp;<A HREF="$(mainurl)/ebox_switch.cml?$(nch)+$(nxt)+1"
         "   >Use richtext</a></td>
      end
      "</table></td>

#  "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\
#       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
   "<td>&nbsp;</td>

   if $inc(2)
      set bits $sql_query_select (\
                SELECT r.bits FROM resps r WHERE r.items_id=$(vi_inum) AND r.rnum=0 AND r.version= \
                   (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) \
               )
      if $bit_and (32 $(bits))
         "<TR $(eboxbg)><td></td>
         "<TD COLSPAN=5>&nbsp;&nbsp;&nbsp;Your response will be
         "<B>anonymous</B>.</TD>  <td>&nbsp;</td>
      end
   end

   #---Actual textbox.
   "<tr $(eboxbg)>
   "<td>&nbsp;</td>
   "<td colspan=5><textarea WRAP=virtual NAME="tx_resp"
   "                ROWS=$(rows) COLS=$(cols)

   #---If user clicks in <textarea> box, but doesn't know about
   #   other editing choices, inform them!
   if $and (0$(alert_ebox) $empty ($user_data ($userid() ebox)))
      if $(is_explorer55)
         " onFocus='new_editor_choice();'
      end
   end

   if   $equal ($inc(1) 0)
      "></textarea>
   end
   elif $equal ($inc(1) 1)
      ">$t2esc($(text))</textarea>
   end
   else
      ">$t2esc($($arg(9)))</textarea>
   end


   "<INPUT TYPE="hidden" NAME="unique_box" VALUE="$unique()">
   "</td>
   "<td>&nbsp;</td>
#----------------------------------------------------------------------------
