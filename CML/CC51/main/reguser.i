#
#---REGUSER.I     Put up entire user registration form elements.
#
#   inc(1)  source      source of current data: 0=>form, else inc(1) = userid
#   inc(2)  table?      0=>not inside a table, 1=>inside a table.
#   inc(3)  query       SQL query to get user_iface fields.
#   inc(4)  skipnames   list of name fields that are skipped.
#   inc(5)  onChange    if not empty, event handler name for "onChange" condition.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/30/00 13:03 Add textbox field type "_x".
#: CR 04/21/00 14:06 Add toolbar field type "tb".
#: CR 05/07/00 20:42 Add space for urc macro field.
#: CR 01/21/01 11:26 Changes for interface-based new user reg page.
#: CR 09/27/02 Make 'ht' (CML text) optional (require check mark) for this page
#: CR  9/02/04 Use %urcregnums() to get REG field numbers.
#: CR 02/24/05 Massive rewrite to use SQL.
#: CR 12/16/05 Add checkbox handling.
#: CR 03/06/06 Add delete old?
#: CR 08/20/06 Add "accept" registration option.
#: CR 08/13/07 Display (but no editing of) registeredon field.
#: CR 12/17/07 Add inc(5), onChange event handler.
#: CR 05/16/08 Add latitude_longitude field.
#: CR 07/17/08 Add lat/long "disable" and "recalculate" options.
#---------------------------------------------------------------

set fieldNames $sql_query_select (SELECT name FROM user_columns ORDER BY name)

set onChange $ifthen ($not_empty($inc(5)) onChange="$inc(5)();")

set h $sql_query_open ($inc(3))

set in_table $inc(2)

while $sql_query_row ($(h))
   if $equal ($(name) {none})
      continue
   end

   if $tablefind ($(name) $inc(4))
      continue
   end

   set onChange
   if $and ($not_empty($inc(5)) $not_equal($(name) lname) $not_equal($(name) fname))
      set onChange onChange="$inc(5)();"
   end

   if $equal ($(name) {Toolbar})
      if $(in_table)
         "</table>
         "</blockquote>
      end
      "<table width="100%" border=0>
      "<tr>
      "<td><h2>New User Registration</h2></td>
      "<td><h2>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</h2></td>
      "<td align=right>
      "   <A HREF="http://www.caucuscare.com" TARGET="_blank"><IMG 
      "       SRC="$(icon)/lcaucus0.gif"  ALT="Caucus" 
      "       WIDTH=148 HEIGHT=40 BORDER=0
      "      ></A><A HREF="$(mainurl)/goodbye.cml"><IMG
      "           SRC="$(icon)/bred1quit.gif"
      "           ALT="Quit" BORDER=0></A></td>
      "</table>
      set in_table 0
      continue
   end

   if $not ($(in_table))
      "<blockquote>
      "<table border=0>
      set in_table 1
   end

   "<tr valign=top>

   "<td width=1
   if $(required)
      "><font color=red><b>*</b></font
   end
   "></td>

   if $and ($equal ($(name) {Error-msg})  $not_empty ($(regerrs)))
      "<TD COLSPAN=3>&nbsp;<BR>
         "<table BORDER>
         "<TR><TD><table>
         "<TR VALIGN=top>
         "<TD>$(fontopen)<B><BLINK>Error:</BLINK></B>$(fontclose)</TD>
         "<TD></TD>
         "<TD>$(fontopen)$(regerrs)$(fontclose)</TD>
         "</TABLE>
         "</TD>
         "</TABLE>
         "&nbsp;<P>
      "</TD>
      continue
   end

   if $tablefind ($(name) {CML-box} {CML-line})
      "<td colspan=2>$mac_expand($reval($cleanhtml(_empty $(label))))</td>
      continue
   end

   "<td width="20%">$(label)</td>

   if $equal ($inc(1) 0)
      set value $form($(name))
   end
   else
      set value $user_info ($inc(1) $(name))
   end

   if   $tablefind ($(name) {Password})
      "<td><input type="password" size="$(width)" name="_Password_"
      "     value="$form(_Password_)"></td>
   end
   elif $tablefind ($(name) {Verify-PW})
      "<td><input type="password" size="$(width)" name="_Verify-PW_"
      "     value="$form(_Verify-PW_)"></td>
   end
   elif $equal ($(name) latitude_longitude)
      "<td><table cellspacing=0 cellpadding=0>
      "<tr valign=top>
      "<td><input type="text" size="$(width)" name="$(name)" $(onChange)
      "           value="$escquote($(value))">
      "<script language="javascript">
      "   function openEditMap() {
      "      editWin = window.open ('', 'editMap', 
      "                'resizable=1,toolbar=0,scrollbars=1,menubar=0,location=0,\
                        status=0,width=400,height=400');
      "      editWin.focus();
      "      return true;
      "   }
      "   function disable() {
      "      document.forms[0].$(name).value = 'none';
      "      return false;
      "   }
      "   function recalculate() {
      "      window.open ('resetLatLong.cml?$unique()', 'editMap', 
      "         'resizable=0,toolbar=0,scrollbars=0,menubar=0,location=0,\
                 status=0,width=200,height=150');
      "      return false;
      "   }
      "</script>
      "</td>
      "<td>&nbsp;&nbsp;&nbsp;&nbsp;</td>
      "<td>
      "<a href="editUserLatLongOnMap.cml?$unique()" 
      "   onClick="return openEditMap();"   TARGET="editMap"
      "  >Edit via map</a>, or
      "  <a href="#" onClick="return recalculate();">recalculate</a> my location,
      "  or <a href="#" onClick="return disable();">disable</a>.&nbsp;
      "  Remember to click "Change My Information".
      "</td></table></td>
   end
   elif $equal ($(format) text-line)
      set textType $if ($greater($(width) 0) text hidden)
      "<td><input type="$(textType)" size="$(width)" name="$(name)" $(onChange)
      "           value="$escquote($(value))"></td>
   end
   elif $equal ($(format) text-box)
      "<td><textarea name="$(name)" cols="$(width)" rows="3" $(onChange)
      "   >$t2esc($(value))</textarea></td>
   end
   elif $equal ($(format) checkbox)
      "<td><input type="checkbox" name="$(name)" value="1"   $(onChange)
      "       $ifthen (0$(value) checked)></td>
   end
   elif $equal ($(format) pull-down)
      "<td><select name="$(name)" $(onChange)>
         "<option>(choose one)
         set choices $replace (32 01 $(choices))
         set choices $replace (10 32 $(choices))
         for x in $(choices)
            set xwords $unquote($(x))
            "<option $ifthen ($equal (x$quote($(value)) x$(x)) selected)>$(xwords)</option>
         end
      "</select></td>
   end
   elif $equal ($(format) upload)
      "<td><INPUT NAME="$(name)" TYPE="file" size="1" $(onChange)
      "   >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      "   Delete old $(name)?<input type="checkbox" name="delete-$(name)"
      "                       value="1"></td>
   end
   elif $equal ($(format) accept)
      "<td><input type="checkbox" name="$(name)" value="1"  $(onChange)
      "    $ifthen (0$(value) checked)
      "    onChange="document.register.acceptanceRequired.value = \
               (this.checked ? '1' : '0');"
      ">&nbsp;&nbsp;&nbsp;
      if $not_empty($(macro))
         "$mac_expand(%$(macro))
      end
      "<input type="hidden" name="acceptanceRequired" value="0">
      "</td>
   end
   elif $equal ($(name) registeredon)
      "<td>$(value)</td>
   end
end
eval $sql_query_close ($(h))

#---------------------------------------------------------------
