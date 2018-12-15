#
#---grade_assign.i   "Assignment" view of gradebook.
#
#   Lists assignments in the gradebook in an editable table.
#   Acts as both gradebook display, *and* as a form submission
#   handler for changes to the "assignment" view of the gradebook.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/19/04 New page.
#------------------------------------------------------------------------

#---List of columns that can be displayed.
eval $mac_define (grade_col_list   type due points weight avg hi low)

#---%grade_col_visible (colname)
#   Is this column marked as "visible"?
eval $mac_define (grade_col_visible \
                     \$word (1 \$site_data (\$(cnum) - grade_vis_@1) 1))

#---%showcol (colname text)
#   Display "text" if column 'colname' is visible.
eval $mac_define (showcol  \
   \$ifthen (\$or (\$(is_instructor) \$mac_expand (\%grade_col_visible(@1))) \
       \$rest (2 @@)))


"<input type="hidden" name="is_submit" value="1">

if $and ($(is_instructor) 0$form(is_submit))
   #---Form submission from delete (trashcan icon) button.
   #   deletetk contains taskkey's of assignments to be deleted.
   if $not_empty ($form(deletetk))
      set query DELETE FROM grade_tasks WHERE  taskkey = $form(deletetk)
      set h $sql_query_open ($(query))
      eval  $sql_query_close($(h))
   end
   
   #---Form submission from Enter press or "Save Changes" click.
   #   If this was a form submission (and not a reload) and we had any changes, 
   #   UPDATE every row with a taskkey in 'changelist'.  INSERT new rows
   #   that have a taskkey of 0.  Data is stored in fields with names
   #   made of column name (e.g. title, due, etc.) combined with taskkey,
   #   e.g. title234 is the title column for assignment 234.
   for tk in $ifthen ($not ($reloaded()) $form(changelist))
      if $equal ($(tk) 0)
         #---Give weight a default value of 1.00.
         set weight $word (1 $form(weight$(tk)) 1.00)
         set query INSERT INTO grade_tasks \
             (taskkey, cnum, title, type, due, points, weight) VALUES \
             ( NULL, '$arg(3)', '$form(title$(tk))',  \
              '$form(type$(tk))',    '%yyyymmddOf($form(due$(tk)))',  \
              '$form(points$(tk))',  $(weight))
      end
      else
         set query UPDATE grade_tasks \
               SET title='$form(title$(tk))', type='$form(type$(tk))', \
                   due='%yyyymmddOf($form(due$(tk)))', \
                   points='$form(points$(tk))', weight='$form(weight$(tk))' \
             WHERE taskkey=$(tk)
      end
      set h $sql_query_open ($(query))
      eval  $sql_query_close($(h))
   end
   
   #---Update which columns should be visible to students.
   for col in %grade_col_list()
      eval $set_site_data ($(cnum) - grade_vis_$(col) 0$form(vis$(col)))
   end
end

#---Hidden field for list of deleted rows.
"<input type="hidden" name="deletetk"   value="">


#---Table column headers
"<table class="gradetable">
"<tr class="graderowh" valign=top>
"<td class="gradetext">Title</td>
"%showcol (type   <td class="gradetype">Type</td>)
"%showcol (due    <td class="gradedate">Due Date</td>)
"%showcol (points <td class="gradenum" >Points</td>)
"%showcol (weight <td class="gradenum" >Weight</td>)
"%showcol (avg    <td class="gradenum" >Class Avg</td>)
"%showcol (hi     <td class="gradenum" >Hi</td>)
"%showcol (low    <td class="gradenum" >Low</td>)

if $(is_instructor)
   #---Checkboxes for "should this column be visible to students?
   "<tr class="graderowh" valign=top>
   "<td class="gradetext"
   "   >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Visible to students?</td>
   
   "<td class="gradetype"><input type="checkbox" name="vistype" 
   "   $ifthen (%grade_col_visible (type) CHECKED)
   "   value="1" style="width: 1em;"></td>
   for col in $rest (2 %grade_col_list())
      "<td class="gradenum"><input type="checkbox" name="vis$(col)" 
      "   $ifthen (%grade_col_visible ($(col)) CHECKED)
      "   value="1" style="width: 1em;"></td>
   end
end


#---Query to fill (rest of) editable table of assignments.
set query \
        SELECT taskkey tk, title, type, due, points, weight FROM grade_tasks \
         WHERE cnum=$arg(3) \
         ORDER BY taskkey
set h $sql_query_open ($(query))

#---%gradecell (class key valuevarname reqNumber)
#   Detailed layout of a grade cell, editable if instructor, not if student.
if $(is_instructor)
   eval $mac_define (gradecell \
            <td class="@1"><input type="text" name="@3@2" value="\$(@3)" \
                onChange="gradechange(document.grades.@3@2, @2, @4);" \
                \$mac_expand (%tab()) \
               ></td>)
end
else
   eval $mac_define (gradecell <td class="@1">\$(@3)</td>)
end


#---Main loop, one row per assignment.
eval %tab(0)
set parity 1
while $sql_query_row ($(h))
   set parity $minus (1 $(parity))
   "<tr class="graderow$(parity)">
   "<td><table cellspacing=0 cellpadding=0 width="100%">
      "<tr class="graderow$(parity)">%gradecell (gradetext $(tk) title 0)
      if $(is_instructor)
         "<td align=right><a href="#" 
         "    onClick="return deletetask('$(tk)');"
         "  ><img $(src_trash) border=0></a></td>
      end
   "</table></td>
   "%showcol (type   %gradecell (gradetype $(tk) type 0))
   set due %monthDayYear($(due))
   "%showcol (due    %gradecell (gradedate $(tk) due 0))
   "%showcol (points %gradecell (gradenum  $(tk) points 1))
   "%showcol (weight %gradecell (gradenum  $(tk) weight 1))

   set h1 $sql_query_open (\
           SELECT AVG(scorenumb) average from grade_scores WHERE taskkey=$(tk))
   eval $sql_query_row   ($(h1))
   "%showcol (avg <td class="gradenum">$round (0$(average) 1)</td>)
   eval $sql_query_close ($(h1))

   set h1 $sql_query_open (\
           SELECT MAX(scorenumb) score from grade_scores WHERE taskkey=$(tk))
   eval $sql_query_row   ($(h1))
   "%showcol (hi <td class="gradenum">$(score)</td>)
   eval $sql_query_close ($(h1))

   set h1 $sql_query_open (\
           SELECT MIN(scorenumb) score from grade_scores WHERE taskkey=$(tk))
   eval $sql_query_row   ($(h1))
   "%showcol (low <td class="gradenum">$(score)</td>)
   eval $sql_query_close ($(h1))
end
eval $sql_query_close ($(h))

#---Empty row (to add a new assignment)
if $(is_instructor)
   set title
   set type
   set due
   set points
   set weight
   set parity $minus (1 $(parity))
   set tk 0
   "<tr class="graderow$(parity)">
   "%gradecell (gradetext $(tk) title 0)
   "%gradecell (gradetype $(tk) type 0)
   "%gradecell (gradedate $(tk) due 0)
   "%gradecell (gradenum  $(tk) points 1)
   "%gradecell (gradenum  $(tk) weight 1)
   "<td class="gradenum"></td>
   "<td class="gradenum"></td>
   "<td class="gradenum"></td>
end

"</table>
#------------------------------------------------------------------------
