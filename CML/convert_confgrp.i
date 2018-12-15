#
#---convert_confgrp.i    Convert a Caucus 4 "course conference" group.
#
#   inc(1)  full group name
#   cnum    conference number
#   access  current access level
#
#:CR 08/31/05
#-----------------------------------------------------------------

   set f2 $open ($caucus_path()/GROUPS/$inc(1) r)
   while $readln ($(f2) gline)
      set wg $word    (  1 $(gline))
      set cg $str_sub (0 1 $(wg))
      if $or ($empty($(wg)) $equal ($(cg) #))
         continue
      end

      if $equal ($(cg) *)
         include $(main)/man_grplist.i $(cnum) x x
         for uid in $(list)
            set grkey $sql_sequence(grouprules)
            set q1 INSERT INTO grouprules \
                  (grkey, owner, name, userid, access) VALUES \
                  ($(grkey), 'CONF', '$(cnum)', '$(uid)', $(access))
            set h $sql_query_open ($(q1))
            eval  $sql_query_close($(h))
         end
      end

      set grkey $sql_sequence(grouprules)
      if $equal ($(cg) <)
         set wg $word (1 $str_sub(1 1000 $(gline)))
         set q1 SELECT DISTINCT owner FROM grouprules \
                 WHERE owner IN ('Caucus4', 'Filesafe') \
                   AND name = '$(wg)'
         set gtype $word (1 $sql_query_select ($(q1)) Caucus4)
         set q1 INSERT INTO grouprules  \
                  (grkey, owner, name, subowner, subname, access) \
                  VALUES ($(grkey), 'CONF', '$(cnum)', '$(gtype)',  \
                          '$(wg)', $(access))
      end
      else
         set optional $if ($equal (* $(wg)) 1 0)
         set wg $replace(* % $(wg))
         set wildcard $if ($gt_equal ($str_index (% $(wg)) 0) 1 0)
         set q1 INSERT INTO grouprules \
                (grkey, owner, name, userid, access, wildcard, optional) \
                VALUES ($(grkey), 'CONF', '$(cnum)', '$(wg)', $(access), \
                        $(wildcard), $(optional))
      end
      set h $sql_query_open ($(q1))
      if $less ($(h) 0)
         "h=$(h), query=$(q1)
      end
      eval  $sql_query_close($(h))

   end
   eval $close($(f2))
