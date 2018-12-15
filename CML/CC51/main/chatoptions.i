#
#---chatoptions.i   Parse chat options
#
#   inc(1)  options string
#: CR 08/14/03 New file.
#: CR 08/22/03 Many new options.
#-------------------------------------------------------------

#---Define default values of options.
set chat_type       float
set chat_whiteboard 1
set chat_width      600
set chat_height     480
set chat_pageuser   1
set chat_im         1
set chat_help       1
set chat_ignore     1
set chat_fullnames  0
set chat_backcolor
set chat_buttoncolor

#---Parse options that were specifically set.
for optvalue in $replace(, 32 $inc(1))
   for opt value in $replace (= 32 $(optvalue))
      set chat_$(opt) $(value)
   end
end
