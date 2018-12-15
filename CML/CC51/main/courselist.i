#---courselist.i   Display list of course conferences available to this user.
#
#   Implementation of %courselist() macro.
#
#   Arguments
#      header={yes|no|both}          (defaults to yes.  both=member/nonmember)
#      show={all|member|nonmember)   (defaults to member)
#      desc=n                        (# of chars of descrip to show. 0=>none)
#      nobr={y|n}                    (defaults "n" [no] for conf & person names)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/20/04  New file.
#: CR 05/04/04  Reorder columns, improve speed, add CSS classes.
#: CR 05/25/04  Add desc, nobr options, header=both
#: CR 06/07/04  Remove filesafe columns; get 1st instructor.
#: CR 07/12/04  Adjust cell padding, table layout.
#: CR 11/21/05  Rewrite for C5.
#: CR 11/28/05  Include non-member courses, new form of readnew.cml.
#----------------------------------------------------------------------------

#---Parse options
set _course_header y
set _course_show   m
set _course_desc   10000
set _course_nobr   y
for option in $inc(1) $inc(2) $inc(3) $inc(4) $inc(5) $inc(6) $inc(7) $inc(8)
   for opt value in $replace (= 32 $(option))
      if $numeric($(value))
         set _course_$lower($(opt)) $(value)
      end
      else
         set _course_$lower($(opt)) $str_sub (0 1 $lower($(value)))
      end
   end
end
set nobr1 <nobr>
set nobr2 </nobr>
if $equal ($(_course_nobr) n)
   set nobr1
   set nobr2
end

#"course_header =$(_course_header)<br>
#"course_show   =$(_course_show)<br>
#"course_desc   =$(_course_desc)<br>
#"course_nobr   =$(_course_nobr)<br>
#"<p/>

#---Count # of assignments in each course
set assign_cnums
set assign_counts
set query SELECT   cnum, count(*) ccount FROM assignments  GROUP BY cnum
set hcl $sql_query_open ($(query))
while $sql_query_row  ($(hcl))
   set assign_cnums  $(assign_cnums)  $(cnum)
   set assign_counts $(assign_counts) $(ccount)
end
eval  $sql_query_close($(hcl))

"<table class="courses"  border=0>
set parity 1
set show_none 0

#---Table (column) headers
if $tablefind ($(_course_header) y b)
   "<tr valign=top class="courses_hdr">
   "<td colspan=2><table cellspacing=0 cellpadding=0 width="100%" border=0>
   "    <tr valign=top class="courses_hdr"
   "       ><td>&nbsp;<b>My&nbsp;Courses</b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
   "        <td><b><a href="conflistedit.cml?$(nch)+$(nxt)+x+x+x+x+x+m"
   "           >(Edit Course List)</a></b></td>
   "        <td align=right><b>Assignments</b>&nbsp;&nbsp;</td>
   "    </table></td>

   "<td>&nbsp;<b><a href="$(mainurl)/readnew.cml?$(nch)+$(nxt)+1+x+x+x+x+x" 
   "  onMouseOver="window.status='Read all new Items & Responses'; return true;"
   "  onMouseOut="window.status=''; return true;"
   " >New</a></b>&nbsp;&nbsp;</td>

   set colspan $if ($greater ($(_course_desc) 0) 2 1)
   "<td colspan=$(colspan)><table cellspacing=0 cellpadding=0 width="100%">
   "   <tr class="courses_hdr">
          if $greater ($(_course_desc) 0)
             "<td><b>Description</b>&nbsp;&nbsp;&nbsp;</td>
          end
   "      <td align=right><b>Members</b></td>
   "   </table></td>
   "<td>&nbsp;&nbsp;&nbsp;<b>Instructor</b>&nbsp;</td>
end

set ismgr %mgr_in_group ($userid() conferenceChange conferenceAllPrivileges \
                         interfaceManage)

#---Courses (all conferences) I am a member of.
if $tablefind ($(_course_show) m a)
   set show_none $tablefind ($(_course_header) y b)

#   set query  \
#      SELECT a.name, a.cnum, a.access, o.alpha FROM \
#        ((SELECT c.name, c.cnum, g.access FROM (confs c, groups g) \
#            LEFT JOIN confhidden h \
#              ON (h.userid = '$userid()' AND  g.name = h.cnum) \
#           WHERE  g.userid = '$userid()' AND  g.active > 0  AND g.access >= $priv(minimum) \
#             AND  g.owner = 'CONF'  AND  c.cnum = g.name \
#             AND  h.hidden IS NULL) a, site_data s) \
#        LEFT JOIN conforder o \
#          ON (o.userid = '$userid()'  AND o.cnum = a.cnum) \
#       WHERE a.cnum = s.cnum AND s.name='ConfType' AND s.value='Course' \
#       ORDER BY ifnull(o.alpha, a.name)

   set query  \
      SELECT a.name, a.cnum, a.access, o.alpha FROM \
        (site_data s, \
         (SELECT c.name, c.cnum, g.access FROM (confs c, groups g) \
            LEFT JOIN confhidden h \
              ON (h.userid = '$userid()' AND  g.name = h.cnum) \
           WHERE  g.userid = '$userid()' AND  g.active > 0  AND g.access >= $priv(minimum) \
             AND  g.owner = 'CONF'  AND  c.cnum = g.name \
             AND  c.deleted = 0 \
             AND  h.hidden IS NULL) a) \
        LEFT JOIN conforder o \
          ON (o.userid = '$userid()'  AND o.cnum = a.cnum) \
       WHERE a.cnum = s.cnum AND s.name='ConfType' AND s.value='Course' \
       ORDER BY ifnull(o.alpha, a.name)
#  "<tr><td colspan=10>$(query)</td>

   set hcl $sql_query_open ($(query))
#  "<tr><td colspan=10>hcl=$(hcl)</td>
   while $sql_query_row  ($(hcl))
 
      #---Skip interface_storage confs, unless appropriate manager or organizer.
      if $or ($equal ($str_index (interface_storage $lower($(name))) 0) \
              $equal (system_announcements          $lower($(name))))
         if $and ($not($(ismgr))  $less ($(access) $priv(instructor)))
            continue
         end
      end

      set dname $replace (_ 32 $(name))

      #---Make sure user's participation record exists.
      include $(main)/confpartic.i $(cnum)

      set show_none 0
      set parity $minus (1 $(parity))
      "<tr valign=top class="courses_row$(parity)">
      "<td class="indented"
      "  >$(nobr1)&nbsp;<a href="$(mainurl)/confhome.cml?$(nch)+$(nxt)+$(cnum)+\
          x+x+x+x+x">$(dname)</a>&nbsp;&nbsp;$(nobr2)</td>

      set pos $tablefind ($(cnum) $(assign_cnums))
      if $greater ($(pos) 0)
         "<td align=right>&nbsp;&nbsp;<a 
         "    href="$(mainurl)/assign.cml?$(nch)+$(nxt)+$(cnum)+\
              x+x+d+x+x">$word($(pos) $(assign_counts))</a>&nbsp;&nbsp;</td>
      end
      else
         "<td></td>
      end

#     set inew $plus($it_inew($(cnum)) $it_iunseen($(cnum)) $it_wnew($(cnum)))
      set inew %countNewInCnum($(cnum))
      if $greater ($(inew) 0)
         "<td align=right><a href="$(mainurl)/anew.cml?$(nch)+$(nxt)+$(cnum)+\
              x+x+x+x+x">$(inew)</a>&nbsp;&nbsp;</td>
      end
      else
         "<td></td>
      end
      if $greater ($(_course_desc) 0)
         set desctext $site_data ($(cnum) - description)
         if $greater ($strlen($(desctext)) $(_course_desc))
            set desctext $str_sub (0 $(_course_desc) $(desctext))...
         end
         "<td>$(desctext)&nbsp;</td>
      end

      set members $sql_query_select ( \
                    SELECT count(*) FROM groups  \
                     WHERE owner='CONF' AND name='$(cnum)' AND access >= $priv(minimum))
      "<td align=right
      "    >&nbsp;<a href="$(mainurl)/members.cml?$(nch)+$(nxt)+$(cnum)+\
           x+x+x+x+x">$(members)</a>&nbsp;</td>

      set instructor $sql_query_select ( \
          SELECT userid FROM groups WHERE owner='CONF' \
             AND name='$(cnum)' AND access >= $priv(instructor) ORDER BY access DESC LIMIT 1)
      "$if ($empty($(instructor)) <td></td> \
          <td class="indented" \
          >&nbsp;&nbsp;&nbsp;$(nobr1)%personpop($(instructor))$(nobr2)\
           &nbsp;&nbsp;&nbsp;</td>)
   end
   eval $sql_query_close($(hcl))
   if $(show_none)
      "<tr><td><i>(none)</i></td>
   end
end

#"</table>
#----------------------------------------------------------------------------
#---Courses I am not a member of (but could join).
if $tablefind ($(_course_show) n a)

   set query  \
      SELECT a.name, a.cnum, a.access FROM \
         (SELECT c.name, c.cnum, g.access FROM (confs c, groups g) \
            LEFT JOIN confhidden h \
              ON (h.userid = '$userid()' AND  g.name = h.cnum) \
           WHERE  g.userid = '$userid()' AND  g.active > 0 \
             AND  g.access > 0  AND  g.access < $priv(minimum) \
             AND  c.deleted = 0 \
             AND  g.owner = 'CONF'  AND  c.cnum = g.name \
             AND  h.hidden IS NULL) a, \
         site_data s \
       WHERE a.cnum = s.cnum AND s.name='ConfType' AND s.value='Course' \
       ORDER BY a.name

   set noheader2 1
   set h $sql_query_open ($(query))
#  "<tr><td colspan=10>query=$(query)</td>
#  "<tr><td colspan=10>h=$(h)</td>
   while $sql_query_row  ($(h))

      #---Show non-member header (once) if desired and needed.
      if $and ($(noheader2)  $equal($(_course_header) b))
         set noheader2 0
         "<tr><td>&nbsp;</td>
         "<tr valign=top class="courses_hdr">
         "<td colspan=3><b>Other Available Courses</b></td>
         if $greater ($(_course_desc) 0)
            "<td><b>Description</b>&nbsp;</td>
         end
         "<td>&nbsp;&nbsp;</td>
         "<td>&nbsp;&nbsp;&nbsp;<b>Instructor</b></td>
      end

      set parity $minus (1 $(parity))
      "<tr valign=top class="courses_non$(parity)">
      "<td class="indented"
      "  >$(nobr1)<a href="$(mainurl)/confhome.cml?$(nch)+$(nxt)+$(cnum)+\
         x+x+x+x+x">$replace(_ 32 $(name))</a>&nbsp;&nbsp;$(nobr2)</td>
      "<td></td>
      "<td></td>
      if $greater ($(_course_desc) 0)
         set desctext $site_data ($(cnum) - description)
         if $greater ($strlen($(desctext)) $(_course_desc))
            set desctext $str_sub (0 $(_course_desc) $(desctext))...
         end
         "<td>$(desctext)&nbsp;</td>
      end
      "<td></td>
      set instructor $sql_query_select ( \
          SELECT userid FROM groups WHERE owner='CONF' \
             AND name='$(cnum)' AND access >= $priv(instructor) ORDER BY access LIMIT 1)
      "$if ($empty($(instructor)) <td></td> \
          <td class="indented"\
          >&nbsp;&nbsp;&nbsp;$(nobr1)%personpop($(instructor))$(nobr2)&nbsp;&nbsp;&nbsp;</td>)
   end
   eval $sql_query_close($(h))
end

"</table>
#----------------------------------------------------------------------------
