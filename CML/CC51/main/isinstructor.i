#
#---isinstructor.i   Is current user a manager, or organizer or instructor
#                    of any conf?
#
#   Implementation of %is_instructor() macro.
#
#: CR 04/20/04  New file.
#: CR 12/08/05  Rewrite for C5.
#----------------------------------------------------------------------------

if $empty ($(_is_instructor))
   set _is_instructor 0

   set query SELECT userid FROM groups \
              WHERE userid='$userid()' AND \
                    ( (owner='MGR'  AND access >  0)  OR \
                      (owner='CONF' AND access >= $priv(instructor)) )  LIMIT 1
   if $not_empty ($sql_query_select ($(query)))
      set _is_instructor 1
   end
end

"$(_is_instructor)

#----------------------------------------------------------------------------
