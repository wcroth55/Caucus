#
#---chatloguid.i    Get userid from chat log line, put in variable 'uid'
#
#   inc(1)  line of text from chatlog
#-------------------------------------------------------------------------

#---Make sure there's a ">".
set uid _
set gt $str_index (> $inc(1))
if $less ($(gt) 0)
   return
end

#---Find the ">", remove it and everything afterward.
set uid $str_sub (0 $(gt) $inc(1))

#---Get the last word of what remains, remove ( and ).  What's left
#   is the userid.
set uid $word ($sizeof($(uid)) $(uid))
set uid $replace (40 32 $(uid))
set uid $replace (41 32 $(uid))
set uid $word (1 $(uid))
#-------------------------------------------------------------------------
