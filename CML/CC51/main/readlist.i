#
#---READLIST.I      Tell the user about reading from a list of items.
#
#   Assumes that the list of items is not empty.
#
#   Input:
#     $inc(1)     name of variable containing triplet list of items
#     $inc(2)     name of var containing list of items w new material
#     label_some  Header label
#     nlabel_some header label (for new material list)
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#------------------------------------------------------------------

set c1 $word (1 $($inc(1)))
set i1 $word (2 $($inc(1)))
set r1 $word (3 $($inc(1)))
set or

#set read , read
set read read
if $not_empty ($(c1))
   "$(read)
   set read
   "<A HREF="viewitem.cml?$(nch)+$(nxt)+\
             $(c1)+$(i1)+$(r1)+x+1+$inc(1)#here" %help(h_readlist)>
   "<B>$(label_some)</A></B>
   set or or
end

#---If there's also a "new" list, offer that option also.
if $not_empty    (  $($inc(2)))
   "$(read)
   set nc1 $word (1 $($inc(2)))
   set ni1 $word (2 $($inc(2)))
   set nr1 $word (3 $($inc(2)))
   "$(or) <B><A HREF="viewitem.cml?$(nch)+$(nxt)+\
          $(nc1)+$(ni1)+$(nr1)+x+1+$inc(2)#here" %help(h_anew)>
   "$(nlabel_some)</A></B>,
end

if $not_empty ($(or))
   "$(or) click on item titles listed below to read individual items.
   "$if ($(is_explorer3) <BR>)
end
else
   ".
end
#----------------------------------------------------------------------
