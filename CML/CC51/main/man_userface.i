#
#---man_userface.i   Individual row in man_userface user reg info page.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 02/21/05 New file.
#:CR 08/20/06 Add "accept" registration option.
#:CR 01/03/11 Allow cml-box & cml-line to appear on Mgr/Self/Pub/Reg pages
#----------------------------------------------------------------

#---Note: {delete} must always be last option!!

"<td class="userface$(p)"><select name="name$(pos)">
for x in $(fieldNames) ------ {none} {CML-line} {CML-box} {Password} \
          {Verify-PW} {Error-msg} {Toolbar} {delete}
   if $not ($tablefind ($(x) active))
      "<option %selected($(name))>$(x)
   end
end
"</select><input type="hidden" name="position$(pos)" value="$(pos)"</td>

if $and ($tablefind ($(name) $(fieldNames) {none}) \
         $not_equal ($(name) laston))
   "<td class="userface$(p)"><select name="format$(pos)">
      for x in {none} text-line text-box checkbox pull-down upload accept
         "<option %selected($(format))>$(x)
      end
   "</select></td>
end
else
   "<td class="userface$(p)"></td>
end

"<td class="userface$(p)"
if $tablefind ($(format) text-line text-box pull-down upload accept)
   "><input type="checkbox" name="required$(pos)" value="1" 
   "              $ifthen ($(required) checked)
end
"></td>

"<td class="userface$(p)"
if $or ($tablefind ($(format) text-line text-box) \
        $tablefind ($(name)   {Password} {Verify-PW}))
   "><input type="text" name="width$(pos)" size=3 value="$(width)"
end
"></td>

set showCheckboxes 1
if $equal ($(name) {CML-box})
   "<td class="userface$(p)" colspan=2><div class="userface$(p)"
   "   ><textarea wrap="virtual" name="label$(pos)" rows="3" cols="45"
   "   >$escquote($t2esc($(label)))</textarea></td>
#  "<td class="userface$(p)"><img $(src_check)></td>
#  "<input type="hidden" name="on_reg$(pos)" value="1">
end
elif $equal ($(name) {CML-line})
   "<td class="userface$(p)" colspan=2><div class="userface$(p)"
   "   ><input type="text" name="label$(pos)" size=45 
   "          value="$(label)"></td>
#  "<td class="userface$(p)"><img $(src_check)></td>
#  "<input type="hidden" name="on_reg$(pos)" value="1">
end
elif $tablefind ($(name) {Error-msg} {Toolbar})
   "<td class="userface$(p)" colspan=5></td>
   "<td class="userface$(p)"><img $(src_check)></td>
   "<input type="hidden" name="on_reg$(pos)" value="1">
   set showCheckboxes 0
end
elif $tablefind ($(name) {Password} {Verify-PW})
   "<td class="userface$(p)"><div class="userface$(p)"
   "   ><input type="text" name="label$(pos)" size=20 value="$(label)"
   "   ></td>
   "<td class="userface$(p)" colspan=4></td>
   "<td class="userface$(p)"><img $(src_check)></td>
   "<input type="hidden" name="on_reg$(pos)"   value="1">
   "<input type="hidden" name="required$(pos)" value="1">
   set showCheckboxes 0
end


else
   "<td class="userface$(p)"><div class="userface$(p)"
   "   ><input type="text" name="label$(pos)" size=20 value="$(label)"
   "   ></td>
   
   "<td class="userface$(p)">
   if $tablefind ($(name) $(fieldNames) {none})
      "<input type="text" name="macro$(pos)" size=15 value="$(macro)">
   end
   "</td>
end

if $(showCheckboxes)
   for field in on_mgr on_self on_public on_reg
      "<td class="userface$(p)"
      if $tablefind ($(name) $(fieldNames) {none} {CML-box} {CML-line})
         "><input type="checkbox" name="$(field)$(pos)" value="1"
         "              $ifthen ($($(field)) checked)
      end
      "></td>
   end
end


if $equal ($(format) pull-down)
   "<tr valign=top class="userface_row$(parity)">
   "<td class="userface$(p)" colspan=5>&nbsp;</td>
   "<td class="userface$(p)" colspan=2 align=right
   "   >Choices:&nbsp;<br>One per line</td>
   "<td class="userface$(p)" colspan=5><textarea wrap="virtual" 
   "    name="choices$(pos)" rows="3"
   "    cols="45">$escquote($t2esc($(choices)))</textarea></td>
   "<td class="userface$(p)"></td>
end

#---------------------------------------------------------------------
