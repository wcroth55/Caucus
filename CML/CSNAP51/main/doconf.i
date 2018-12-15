#
#---DOCONF.I   Process conference CNUM.
#
#   Input:
#     cnum       conference number
#
#   Output:
#     allusers   list of all userids (see mkpeople.i)
#
#   Purpose:
#     "Do" everything about a particular conference.  Most of the
#     real work is done by other, more specific, include files.
#
#   How it works
#
#   Notes:
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 12/04/02 Add -p option, do not build/display personal info.
#: CR 06/07/06 lowercase dname before copying LIB directories.
#==========================================================================

   set m_cnum $(cnum)
   set home Home
   if $not_empty     (  $site_data($(cnum) - home_page))
      set home $rest (2 $site_data($(cnum) - home_page))
   end

   include CC51/main/itemListOf.i $(cnum) $userid() all 0 0 0
   set items
   for ic ii ir in $(itemList)
      set items $(items) $(ii)
   end

   set ifirst $word (1 $(items))
   set ilast  $word ($sizeof($(items)) $(items))

   set lastpages
   count index 1 $sizeof($(items))
      set inum  $word (       $(index)    $(items))
      set inext $word ($plus ($(index) 1) $(items))
      "   Item $item_label ($(cnum) $(inum))
      include $(dir)/mkitem.i $(cnum) $(inum) lastpages $(inext)
#     "          (done)
      if $(localerrs)
         "$errors()
      end
   end

   include $(dir)/mkconf.i   $(cnum) $(ifirst) items lastpages

   if $(do_people)
      include $(dir)/mkpeople.i $(cnum)
   end

#  set dname $lower($cl_name($(cnum)))
#  for udir in $dirlist (0 $caucus_lib()/$(dname))
#     include $(dir)/copydir.i $(udir) $caucus_lib()/$(dname) \
#                               $(target)/LIB/C$(cnum)
#  end
