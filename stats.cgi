#!/bin/sh
#
#---stats.cgi   Flow-through for Caucus statistics wizard, 
#               from sqlreport.cml output.
#
#:CR 04/28/05 New file.
#----------------------------------------------------------------------

#---Fetch the type of data (html vs xls) and the temp output file name
#   from the URL query string.
echo $QUERY_STRING | tr "\&" " " | (\
   read type tmpout

   #---Sleep until the output file is ready.
   while test ! -f /tmp/$tmpout; do
      sleep 1
   done
    
   #---And then send its contents along to the user's browser.
   if test "x$type" = "xhtml"; then
      echo "Content-type: text/html"
   else
      echo "Content-type: application/vnd.ms-excel"
   fi
   echo ""
   cat   /tmp/$tmpout
   rm -f /tmp/$tmpout
)
