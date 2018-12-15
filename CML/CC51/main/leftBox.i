#
#---leftBox.i    Implementation of fixed left bar.
#
#   Notes:
#      We can majorly improve performance by caching IFS items.
#
#:CR 03/27/2007 New file.
#----------------------------------------------------------

if 0$(leftBox)
   "<div id="leftBox">

   set contentCode $str_sub(0 1 $(leftBoxContent))
   if $equal ($(contentCode) t)
      "$mac_expand($reval ($(leftBoxText)))
   end

   elif $equal ($(contentCode) i)

      eval        $cl_list (interface_storage_$(iface))
      set ifsnum  $cl_num  (interface_storage_$(iface))
      set boxItem $item_id($(ifsnum) $str_sub (1 1000 $(leftBoxContent)))
      set boxResp 0
      
      eval %mpush ($(ifsnum) $(boxItem) 0)
      
      set qIFS SELECT r.text boxText, r.prop boxProp FROM resps r \
                WHERE r.items_id = $(boxItem) AND r.rnum=$(boxResp) AND r.deleted=0 AND r.version= \
                  (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum)
      set hi $sql_query_open ($(qIFS))
      eval   $sql_query_row  ($(hi))
      eval   $sql_query_close($(hi))
      
      if $not_empty($(boxProp))
         #---CML response text
         if $equal ($(boxProp) 3)
            "$cleanhtml(prohibited \
                  $protect($mac_expand($reval($cleanhtml(_empty $(boxText)))) ))
            "<BR CLEAR=ALL>
         end
         
         #---HTML response text
         elif $equal ($(boxProp) 2)
            "$cleanhtml(prohibited \
                  $protect($mac_expand($cleanhtml(_empty $(boxText)))))
            "<BR CLEAR=ALL>
         end
         
         #---"Literal" response text
         elif $equal ($(boxProp) 1)
            "<PRE>
            "$cleanhtml (prohibited $protect($mac_expand($t2html($(boxText)))))
            "</PRE>
         end
         
         #---Word-wrapped response text
         else
            "$cleanhtml (prohibited $protect($mac_expand($wrap2html($(boxText)))))
         end
      end

      eval %mpop()

   end
   "</div>
end

