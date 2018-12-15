#
#---MKCONF.I     Make conference home page
#
#   Purpose:
#
#   How it works
#
#   Inputs:
#      target     where to write static HTML files
#      inc(1)     conference number
#      inc(2)     number of first existing item in conference
#      inc(3)     name of variable containing list of all item #s
#      inc(4)     name of variable containing list of last page #s
#
#   Notes:
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 12/04/02 Add -p option, do not build/display personal info.
#==========================================================================


#---Get pretty form of conference name.
set vi_cnum $inc(1)
set cname %displayed_conf_name ($(vi_cnum))

set fcnum $mac_expand (%digit4($(vi_cnum)))

#---Create conference home page
eval $output ($(target)/temp)

"<!-- quirksmode -->
"<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
"<HTML>
"<HEAD>
"<TITLE>$(cname) $(home)</TITLE>
include $(dir)/popup.i
include $(dir)/css.i
"</HEAD>

set bgcolor $word (1 $site_data ($inc(1) - conf_colors) BGCOLOR="#FFFFFF")
"<BODY $(bgcolor)>
"<div>

set org $co_org($(vi_cnum))
set pagename <FONT SIZE=+2><B>$(cname) $(home)</B></FONT><BR>
if $empty ($user_info($(org) lname))
   set pagename $(pagename)<FONT SIZE=-1>($(org)), Organizer</FONT>
end
elif $(do_people)
   set pagename $(pagename)<FONT SIZE=-1><A HREF="../PEOPLE/$(org).htm">\
       %per_name($(org))$if ($(show_id) &nbsp;($(org)))</A>, Organizer</FONT>
end
else
   set pagename $(pagename)<FONT SIZE=-1>\
           %per_name($(org))$if ($(show_id) &nbsp;($(org))), Organizer</FONT>
end

include $(dir)/headtool.i $(fcnum)
 
"<TABLE CELLSPACING=0 CELLPADDING=0 WIDTH=100% BORDER=0>
   "<TR>
   "<TD VALIGN=CENTER><IMG $(src_compass) ALIGN=LEFT>
   "<BR><FONT SIZE=-1><NOBR>
   "<A HREF="../C0000/index.htm">$(center_name)</A>
   "</NOBR>
   
   
   #---Below a conf home page, make conf home page clickable.
   "$(compass_arrow)
   "<A HREF="../C$(fcnum)/index.htm">$(cname)</A>
   
     
   "</FONT></TD>
   "<TD>&nbsp;</TD>
   "<TD ALIGN=right VALIGN=CENTER>
      include $(dir)/youare.i $userid()
   "</TD>
"</TABLE>



#-------------------------------------------------------------
#---Display conference greeting, if any, applying appropriate filtering.
"<BLOCKQUOTE>

set prohibited $(tag_list)
if $not_equal(x$site_data($(vi_cnum) - tag_list_org) x)
   set prohibited $site_data($(vi_cnum) - tag_list_org)
end

set tp $site_data ($(vi_cnum) - greet_tp)

if $empty ($site_data($(vi_cnum) - greet1))
   "This is the central page for the <B>$(cname)</B> conference.
end
elif   $equal ($(tp) cml)
   "$cleanhtml(prohibited \
         $protect($mac_expand($reval($site_data($(vi_cnum) - greet1)))))
   "<BR CLEAR=ALL>
end

elif $equal ($(tp) html)
   "$cleanhtml(prohibited $protect($mac_expand($site_data($(vi_cnum) - greet1))))
   "<BR CLEAR=ALL>
end

elif $equal ($(tp) literal)
   "<PRE>
   "$cleanhtml (prohibited \
       $protect($mac_expand($t2html($site_data($(vi_cnum) - greet1)))))
   "</PRE>
end

elif $equal ($(tp) format)
   "$cleanhtml (prohibited \
        $protect($mac_expand($wrap2html($site_data($(vi_cnum) - greet1)))))
   "<BR CLEAR=ALL>
end

else
   "This is the central page for the <B>$(cname)</B> conference.
end
"</BLOCKQUOTE>
"<P>

set finum1 $inc(2)
"You may read 
"<A HREF="../C$(fcnum)/$(finum1)0001.htm">all the items</A>,
"or click on item titles listed below to read
"individual items.
"<P>

"<table class="itemlist_table">

"<tr class="itemlist_header">
"<TD class="itemlist_row_num"><B>#</b>&nbsp;</td>
"<TD class="itemlist_row_text"><B>Title</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
"<TD class="itemlist_row_text"><b>Author</b></td>
"<TD class="itemlist_row_num"><b>Last</b></td>

   
   #---Row for each item in '$inc(1)' list.
   set inums $sizeof ($($inc(3)))
   set parity 1
   count dex 1 $(inums)
      set parity $minus (1 $(parity))
      set inum  $word ($(dex) $($inc(3)))
      set ipage $word ($(dex) $($inc(4)))
      "<TR class="itemlist_row$(parity)" valign=top>

      set q SELECT i.title, i.lastresp, i.userid, u.fname, u.lname \
              FROM items i \
              LEFT JOIN user_info u \
                ON (u.userid = i.userid) \
             WHERE i.id=$(inum)

      set h $sql_query_open ($(q))
      eval  $sql_query_row  ($(h))
      eval  $sql_query_close($(h))

      #---Item number.
      set finum $item_label ($inc(1) $(inum))
      "<TD class="itemlist_row_num"
      "><A HREF="../C$(fcnum)/$(inum)0001.htm">$(finum)</A>&nbsp;</TD>
  
      #---Item title.
      "<TD class="itemlist_row_text"
      "><A HREF="../C$(fcnum)/$(inum)0001.htm">$(title)</a>

#     if $it_visib ($(vi_cnum) $(inum))
#        "   $t2hbr($re_title($(vi_cnum) $(inum) 0))</A>
#     end
#     else
#        "<I>$t2hbr($re_title($(vi_cnum) $(inum) 0))</I></A>
#     end
      "</TD>

      #---Author.
      "<TD class="itemlist_row_text"
      if $empty($(fname)$(lname))
         ">($(userid))</td>
      end
      elif $(do_people)
         "><A HREF="../PEOPLE/$(userid).htm"
         "><NOBR>$(fname) $(lname)</NOBR></A></TD>
      end
      else
         "><NOBR>$(fname) $(lname)</NOBR></TD>
      end
  
      #---"Last" response (# of responses)
      set fpnum $mac_expand (%digit4($(ipage)))
      "<TD class="itemlist_row_num"
      "><A HREF="../C$(fcnum)/$(inum)$(fpnum).htm#last"
      ">$(lastresp)</A></TD>
   end
"</TABLE>

"<P>

"</div>
"</BODY>
"</HTML>
eval $output()
eval $copyfile ($(target)/temp $(target)/C$(fcnum)/index.htm 755)
#------------------------------------------------------------
