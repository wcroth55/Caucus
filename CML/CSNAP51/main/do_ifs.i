#
#---DO_IFS.I     Make IFS pages.
#
#   Purpose:
#
#   How it works
#
#   Inputs:
#      target     where to write static HTML files
#      inc(1)     conference number
#
#   Notes:
#: CR  8/28/02 new file, for 4.4
#: CR 07/25/10 Rewrite for MySQL!!!
#==========================================================================

include $(main)/itemListOf.i $inc(1) $userid() all 1 1 1
for cval ival rval in $(itemList)
   set title $unquote($sql_query_select (SELECT title FROM items WHERE id=$(ival)))
   set ilabel $item_label ($inc(1) $(ival))
   set query SELECT r.rnum, r.text, r.prop FROM resps r \
              WHERE r.items_id=$(ival) \
                AND r.deleted=0 AND r.version =  \
                   (SELECT MAX(q.version) FROM resps q \
                     WHERE q.items_id=$(ival) AND q.rnum = r.rnum)
   set h $sql_query_open ($(query))
   while $sql_query_row  ($(h))
      eval $output ($(target)/temp)
      
      "<HTML>
      "<HEAD>
      
      "<title>$(title)</title>
      
      include $(dir)/popup.i
      include $(dir)/css.i
      "</HEAD>
      
      set bgcolor $word (1 $site_data ($inc(1) - conf_colors) BGCOLOR="#FFFFFF")
      "<BODY $(bgcolor)>
      
      set pagename <h2>$(title)</h2>
      include $(dir)/headtool.i 0000
      
      "<TABLE CELLSPACING=0 CELLPADDING=0 WIDTH=100% BORDER=0>
         "<TR>
         "<TD VALIGN=CENTER><IMG $(src_compass) ALIGN=LEFT>
         "<BR><FONT SIZE=-1><NOBR>
         "<A HREF="../C0000/index.htm">$(center_name)</A>
         "</NOBR>
         "$(compass_arrow)
         "<A HREF="#">$(title)</A>
           
         "</FONT></TD>
         "<TD>&nbsp;</TD>
         "<TD ALIGN=right VALIGN=CENTER>
            include $(dir)/youare.i $userid()
         "</TD>
      "</TABLE>
      
      "<P>
      
      #---Make sure %liburl2(), %libimg2() etc know where to find this item.
      set  m_cnum $inc(1)
      set  m_inum $(ival)
      set  m_rnum $(rnum)
      
      #---CML response text
      if $equal ($(prop) 3)
         "$cleanhtml(prohibited \
               $protect($mac_expand($reval($cleanhtml(_empty $(text)))) ))
         "<BR CLEAR=ALL>
      end
      
      #---HTML response text
      elif $equal ($(prop) 2)
         "$cleanhtml(prohibited \
               $protect($mac_expand($cleanhtml(_empty $(text)))))
         "<BR CLEAR=ALL>
      end
      
      #---"Literal" response text
      elif $equal ($(prop) 1)
         "<PRE>
         "$cleanhtml (prohibited $protect($mac_expand($t2html($(text)))))
         "</PRE>
      end
      
      #---Word-wrapped response text
      else
         "$cleanhtml (prohibited $protect($mac_expand($wrap2html($(text)))))
      end
      
      "<p/>
      "<HR>
      
      "</BODY>
      "</HTML>
      eval $output()
      
      eval $copyfile ($(target)/temp $(target)/IFS/$(ilabel)-$(rnum).htm 755)
   end
   eval  $sql_query_close($(h))
end
