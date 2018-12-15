#
#---grade_export.i   Exported "Class" view of gradebook.
#
#   Lists the individual student scores (by assignment) in a spreadsheet.
#   table.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/19/04 New page.
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#: CR  9/01/04 Make sure all SQL queries are closed!
#: CR 11/22/04 Use associative array syntax in gradechange call, in
#              case userid has a dash in it!
#: CR 01/24/05 Major rewrite, to be for spreadsheet only.
#------------------------------------------------------------------------


#---Section II: Compute/Display the table column headers

#---%rowclear (n)
#   %rowflush (n)
#   %rowtext  (n text)
eval $mac_define (rowclear \$set(row@1))
eval $mac_define (rowflush \$(row@1)\$set(row@1))
eval $mac_define (rowtext  \$set (row@1 \$(row@1)"\$rest(2 @@)"\$char(9)))

eval %rowclear(0)

#---Get the values for the column headers.
set query SELECT title, taskkey, points, weight FROM grade_tasks \
           WHERE cnum=$(cnum) ORDER BY taskkey
set h $sql_query_open ($(query))
eval %rowtext (0 Student)

set pointrow
set total   0
set dotprod 0

#---Save arrays of weight(i) and weight(i)/points(i), and the sum of
#   all the weights (wsum), for later computations.
set wdivp
set weights
set wsum    0

eval %rowclear(1)
eval %rowtext (1)

while $sql_query_row  ($(h))
   eval %rowtext (0 $(title))

   eval %rowtext (1 $(points))

   set points 0$(points)
   set weight 0$(weight)

   set total $plus ($(total) $(points))
   set dotprod $plus ($(dotprod) $mult($(weight) $(points)))

   if $greater ($(points) 0)
      set wdivp $(wdivp) $quotient ($(weight) $(points))
      set wsum  $plus ($(wsum) $(weight))
   end
   else
      set wdivp $(wdivp) 0
   end
   set weights $(weights)   $(weight)
end
eval $sql_query_close($(h))

eval %rowtext (0 Percentage)
"%rowflush(0)

eval %rowtext (1 So far)
eval %rowtext (1 All)
"%rowflush(1)

#========================================================================
#---Section III: Compute/Display the editable table cells, one row per student.

#---%rowscore (key userid value) is a convenience macro, to make it
#   easier to write the HTML code for each cell.
if $not ($(is_html))
   eval $mac_define (rowscore)
end
elif $(is_instructor)
   eval $mac_define (rowscore \$set (row@1 \$(row@1) \
         <td class="gradenum"><input type="text" name="S@2_@3" value="@4" \
             onChange="gradechange(document.grades['S@2_@3'], 'S@2_@3', 0);" \
             \$mac_expand (%tab()) \
         ></td>))
end
else
   eval $mac_define (rowscore \$set (row@1 \$(row@1) \
                       <td class="gradenum">@4</td>))
end

#---Build the uidlist as all members of the conference (if organizer),
#   or else just self (skipping blanks, duplications, or organizers.)
set last ___
set uidlist $userid()
if $(is_instructor)
   set uidlist $peo_members($(cnum))
end

#---For each user in the list...
set parity 1
for uid in $(uidlist)
   set access %access($(cnum) $(uid))
   if $and ($not_equal($(last) $(uid))   $not_empty (%per_lname($(uid))) \
            $less    ($(access) $priv(instructor)) \
            $greater ($(access) 0) )
      set parity $minus (1 $(parity))
      eval %rowtext (0 %lName($(uid)), %fName($(uid)) ($(uid)))

      set query \
         SELECT t.taskkey, s.userid, s.scorenumb, s.scoretext, t.weight \
           FROM grade_tasks t LEFT JOIN grade_scores s \
             ON (t.taskkey = s.taskkey  AND s.userid='$(uid)')  \
          WHERE t.cnum = $(cnum) \
          ORDER BY t.taskkey

      set dotprod 0
      set total   0
      set i         0
      set pcnt_all  0
      set pcnt_done 0
      set done      0
      set h $sql_query_open ($(query))
      while $sql_query_row  ($(h))
         eval %rowscore (0 $(taskkey) $(uid) $word(1 $(scorenumb) $(scoretext)))
         eval %rowtext  (0                   $word(1 $(scorenumb) $(scoretext)))

         set total   $plus ($(total) 0$(scorenumb))
         set dotprod $plus ($(dotprod) $mult(0$(scorenumb) 0$(weight)))

         set i        $plus ($(i) 1)
         set temp $mult ($word ($(i) $(wdivp)) 0$(scorenumb))
         set pcnt_all $plus ($(pcnt_all) $(temp))
         if $not_empty ($(scorenumb))
            set pcnt_done $plus ($(pcnt_done) $(temp))
            set done      $plus ($(done) $word ($(i) $(weights)))
         end
      end
      eval $sql_query_close ($(h))
      set  temp   $round ($mult (100 $quotient ($(pcnt_done) $(done) 4)) 1)\%
      eval %rowtext (0 $(temp))
      set  temp   $round ($mult (100 $quotient ($(pcnt_all)  $(wsum) 4)) 1)\%
      eval %rowtext (0 $(temp))
      "%rowflush (0)
   end
   set last $(uid)
end

eval %rowflush(0)
#------------------------------------------------------------------------
