#
#---ebox_rt.i   Edit box, using richtext editor
#
# Input:
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
#   prop     (text property)
#   text     actual text
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 12/11/02 New file.
#: CR 01/17/03 Add ebox_onload() handler.
#: CR 06/21/03 Add optional inc(5) javascript check function.
#: CR 04/26/04 Hook in new pop-up attach-file dialog.
#: CR 01/10/05 Changes to support mathsyms in both plaintext and richtext.
#: CR 09/21/05 comment out submitpv, not sure why it's there.
#: CR 06/08/06 Rewrite for C5.1
#: CR 08/19/09 Do not apply wordLimit for instructors or organizers
#----------------------------------------------------------------------------

if $inc(2)
   set bits $sql_query_select (\
             SELECT r.bits FROM resps r WHERE r.items_id=$(vi_inum) AND r.rnum=0 AND r.version= \
                (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) \
            )
   if $bit_and (32 $(bits))
      "<tr $(eboxbg)><td></td>
      "<td colspan=5>
         "<img $(src_bullet)>&nbsp;&nbsp;&nbsp;Your response 
         "   will be <B>anonymous</B>.
         "<td></td>
      end
   end
"</td><td></td>

"<INPUT TYPE="hidden" NAME="unique_box" VALUE="$unique()">
#"<INPUT TYPE="hidden" NAME="submitpv"   VALUE="1">

if $equal ($inc(1) 1)
   set textprop $plus ($(prop) 1)
   set tp_resp  $word ( $(textprop) format literal html cml )
   "<input type=hidden name="tp_resp" value="$(tp_resp)">
end
else
   "<input type=hidden name="tp_resp" value="cml">
end


"<tr $(eboxbg)><td></td>
"<td colspan=5 class="ebox_text">
#  "<input type="hidden" id="hdnrte1" name="rte1" value="">
   "<script type="text/javascript" language="javascript">

   "initRTE("$(http)://$host()/~$caucus_id()/CB51/images/",
   "        "$(http)://$host()/~$caucus_id()/CB51/", 
   "        "$(http)://$host()/~$caucus_id()/CB51/caucusRte.css", true);
   "var caucusRte = new richTextEditor('caucusRte');
   if $equal ($inc(1) 1)
      "caucusRte.html = '$escsingle($(text))';
   end
   "caucusRte.cmdInsertImage = false;
   "caucusRte.cmdSpellcheck  = false;
   "caucusRte.cmdStrikethrough = true;
   "caucusRte.cmdInsertSpecialChars = false;
   "caucusRte.height = '$(wordhigh)';
   "caucusRte.width =  '$(wordwide)';
   "caucusRte.build();

"  function ebox_onload (keywords) {
"     setTimeout ("limitWords()", 20000);
"  }
"  function ebox_upload() {
"     var w = openNearby('attach', 500, 330, 'resizable=1,scrollbars=1,toolbar=0');
"     w.location.href = 
"      '$(mainurl)/$(uploadPage)$(xuploadLicense).cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+$arg(5)+$inc(1)';
"     w.focus();
"     w.setTimeout("self.focus();", 500);
"  }
"  function ebox_symbol() {
"     flipSymbols();
"  }
"  function ebox_addtext (theText) {
"     insertHTML(theText, 'caucusRte');
"     return false;
"  }
"  function ebox_addnewline() {
"     insertHTML('<p/>', 'caucusRte');
"  }
"  function ebox_switch() {
"     location.href = "$(mainurl)/ebox_switch.cml?$(nch)+$(nxt)+0";
"     return false;
"  }

"  var itemLimit = 0;
   if $inc(5)
      if $less ($(itemAccess) $priv(instructor))
"        itemLimit = $plus(0 $site_data($arg(3) $arg(4) wordLimit));
      end
   end

"  function ebox_submit() {
"     updateRTEs();
"     document.ebox.tx_resp.value = htmlDecode(document.ebox.caucusRte.value);
"     if (itemLimit > 0) {
#        count = countWords(document.ebox.caucusRte.value);
"        count = countWords(document.ebox.tx_resp.value);
"        if (count > itemLimit) {
"           confirm ("Your response must be limited to " + itemLimit + " words.\n"
"                    + "Please remove " + (count-itemLimit) + ".");
"           return false;
"        }
"     }
"       
"     return true;
"  }

"  var prevCount = 0;
"  function limitWords() {
"     if (itemLimit == 0)  return;
"     updateRTEs();
"     document.ebox.tx_resp.value = htmlDecode(document.ebox.caucusRte.value);
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

   
#  "writeRichText('rte1', mytext, $(wordwide), $(wordhigh), true, false);
   "</script>
   "<textarea name="tx_resp" style="display:none" rows="1" cols="20"></textarea>

   include $(main)/mathsym.i 1
"</td>

"</td>
"<td></td>

#----------------------------------------------------------------------------
