#
#---quiz_graphrow.i   Show an individual row in a quiz graph.
#
#   Input:
#      q5           query to fetch number of hits for this row
#      pkey         problem key
#      akey         answer-row key
#      type         type of answer-row
#      text1        left-text of answer-row
#      text2        right-text of answer-row
#      inc(1)       rowspan for this row of text1/text2
#      inc(2)       optional text (for True/False, for example)
#      graphColors  set of graph colors
#
#:CR 02/28/06 New file.
#-------------------------------------------------------------------
   set row $plusmod ($(row) 1 6)
   "<tr>
   if $inc(1)
      "<td rowspan="$inc(1)"
      "   ><div class="grLabel">$(text1) $(text2)&nbsp;</div></td>
   end
   "<td align=right><div class="grLabel">$inc(2)&nbsp;&nbsp;</div></td>

   set color $word ($plus($(row) 1) $(graphColors))

   set numerator -1
   if $not_empty ($(q5))
      set numerator $sql_query_select($(q5))
   end
   set pixels    $divide   ($mult ($(numerator) 400) $(divisor))
   set fraction  $quotient ($mult ($(numerator) 100) $(divisor) 1)
   set fraction  $plus ($(fraction) 0.0)

#  "<td>$(numerator)</td>
#  "<td>$plus ($(fraction) 0.0)</td>
#  "<td>$(pixels)</td>
   "<td class="$(class)"
   if $greater ($(pixels) 0)
      "><div class="grBar" 
      "      style="background-color: $(color); width: $(pixels)px;">&nbsp;</div
   end
   if $gt_equal ($(pixels) 0)
      "><div class="grBarText" 
      "      style="margin-left: $plus($(pixels) 3)px;">$(fraction)%\
                    &nbsp;n=$(numerator)</div>&nbsp;</td>
   end
   else
      ">&nbsp;</td>
   end
   count i 1 9
      "<td class="$(class)">&nbsp;</td>
   end
   "<td class="grCell">&nbsp;</td>
#-------------------------------------------------------------------
