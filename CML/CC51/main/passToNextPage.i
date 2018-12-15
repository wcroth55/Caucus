#
#---passToNextPage.i    Show Pass button and target name of next page
#
#   If the target is not an item page, a conf page, or the center page,
#   force it to the relevant conf home page.
#
#   Input:
#      target_name   (from getNextPageTarget.i)
#      arg(2)-arg(8)
#
#:CR 04/22/2007 New file.
#----------------------------------------------------------------

if $equal ($arg(7) x)
   set href $page_return ($arg(2) # center.cml?$(nch)+0+x+x+x+x+x+x)
   if $and ($less ($str_index (viewitem $(href)) 0) \
            $less ($str_index (confhome $(href)) 0))
      set href $(mainurl)/confhome.cml?$unique()+$arg(2)+$arg(3)+$arg(4)+$arg(5)+\
                          $arg(6)+$arg(7)+$arg(8)
      set target_name %displayed_conf_name ($arg(3)) Home
   end
end
else
   set vi_index $plus ($arg(7) 3)
   set vi_temp  $rest ($(vi_index)  $($arg(8)) )
   set caller   $page_caller (1 $arg(2))

   #---If no more items, go to CML page "hidden" at end of triplet list,
   #   or if that's not there, to the original caller of viewitem.cml.
   if $empty ($word (2 $(vi_temp)))
     set target $if ($empty($(vi_temp)) $(caller) $(vi_temp))
     set href $(mainurl)/$page_return ($(target) # center.cml?$(nch)+0+x+x+x+x+x+x))
   end
   else
   
      #---On the other hand, if there is another item, go there!
      set vi_cnum $word( 1 $(vi_temp) )
      set vi_inum $word( 2 $(vi_temp) )
      set vi_rnum $word( 3 $(vi_temp) )
      
      set href $(mainurl)/viewitem.cml?$(nch)+$(caller)+\
                 $(vi_cnum)+$(vi_inum)+$(vi_rnum)+x+$(vi_index)+$arg(8)#here)
   end
end

"<table cellspacing=0 cellpadding=0 border=0>
"<tr>
"<td><a href="$(href)"><IMG BORDER=0 $(src_pass) HSPACE=0></a></td>
"<td>&nbsp;&nbsp;</td>
"<td>to $(target_name).</td>
"</table>

