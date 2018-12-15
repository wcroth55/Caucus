#
#---abox_rt.i   Edit box, using richtext editor, for assignments.
#
#   inc(1)   type of response text (format literal html cml, etc)
#   inc(2)   name of variable containing (existing) text
#   inc(3)   basename of help file for "get Help" link.  E.g. i_item, i_resp.
#   inc(4)   base for uploaded files directory.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 12/11/02 New file.
#: CR 01/17/03 Add ebox_onload() handler.
#: CR 06/21/03 Add optional inc(5) javascript check function.
#: CR 04/26/04 Hook in new pop-up attach-file dialog.
#: CR 01/14/05 Rewrite to use newer richtext editor.
#: CR 02/20/05 Use inc(1) for tp_resp!
#: CR 09/21/05 comment out submitpv, not sure why it's there.
#: CR 02/10/06 Add inc(4) for generic (not just item/resp) file uploads.
#----------------------------------------------------------------------------

#"<tr $(eboxbg)><td></td>
#"<td colspan=5
#"   ><img $(src_bullet)>&nbsp; Edit your text below and press <b>Post</b>.
#"</td><td></td>

"<INPUT TYPE="hidden" NAME="unique_box" VALUE="$unique()">
#"<INPUT TYPE="hidden" NAME="submitpv"   VALUE="1">

"<input type=hidden name="tp_resp" value="$inc(1)">


"<tr $(eboxbg)><td></td>
"<td colspan=5 class="ebox_text">
   "<input type="hidden" id="hdnrte1" name="rte1" value="">
   "<script type="text/javascript" language="javascript">
"  function ebox_onload (keywords) {
"  }
"  function ebox_upload() {
"     var w = openNearby('attach', 500, 330, 'resizable=1,scrollbars=1,toolbar=0');
"     w.location.href = 
"      '$(mainurl)/$(uploadPage)$(xuploadLicense).cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+$arg(5)+\
                                 $inc(1)+$inc(4)';
"     w.focus();
"     w.setTimeout("self.focus();", 500);
"  }
"  function ebox_symbol() {
"     flipSymbols();
"  }
"  function ebox_addtext (theText) {
"     addToRTE ('rte1', theText);
"     return false;
"  }
"  function ebox_addnewline() {
"     insertHTML('<p/>', 'caucusRte');
"  }

"  function ebox_switch() {
"     location.href = "$(mainurl)/ebox_switch.cml?$(nch)+$(nxt)+0";
"     return false;
"  }
"  function ebox_submit() {
"     updateRTEs();
"     var originalText = htmlDecode(document.ebox.caucusRte.value);
"     var c = originalText.substring(0,1);
"     document.ebox.tx_resp.value = 
"        (c == "\n" ? originalText.substring(1) : originalText);
"     return true;
"  }

   "initRTE("$(http)://$host()/~$caucus_id()/CB51/images/",
   "        "$(http)://$host()/~$caucus_id()/CB51/",
   "        "$(http)://$host()/~$caucus_id()/CB51/caucusRte.css", true);

   "var caucusRte = new richTextEditor('caucusRte');
   "caucusRte.html = '$escsingle($(text))';
   "caucusRte.cmdInsertImage = false;
   "caucusRte.cmdSpellcheck  = false;
   "caucusRte.cmdStrikethrough = true;
   "caucusRte.cmdInsertSpecialChars = false;
   "caucusRte.height = '$(wordhigh)';
   "caucusRte.width =  '$(wordwide)';
   "caucusRte.build();
   "</script>
   "<textarea name="tx_resp" style="display:none" rows="1" cols="20"></textarea>

   include $(main)/mathsym.i 1
"</td>
#"</td>
"<td></td>

#----------------------------------------------------------------------------
