#
#---countNewInCnum.i    Return count of # of items with new material 
#                       in conference $inc(1).
#
#:CR 03/02/07 New file.
#------------------------------------------------------------------

include $(main)/itemListOf.i $inc(1) $userid() new 0 0 0
"$divide($sizeof($(itemList)) 3)
