#
#---anyconfhasnew.i   Implementation of %anyConfHasNew() macro.
#
#:CR 03/14/06 New file.
#:CR 09/05/07 Only look at confs that this user is a member of.
#--------------------------------------------------------------------------

for _name in $(_anyconf)
   set _c $cl_num($(_name))
   if $greater ($group_access ($userid() CONF $(_c)) $priv(minimum))
      include $(main)/itemListOf.i $(_c) $userid() new 0 0 0
      if $not_empty($(itemList))
         "<IMG $(src_isnew) BORDER=0>
         break
      end
   end
end
