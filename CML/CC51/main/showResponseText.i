#
#---showResponseText.i
#
#   Input
#      inc(1)      "props" value of response (type of text)
#      text        text of response
#      prohibited  list of prohibited tags
#
#: CR 12/02/10 Hack to allow <script> tag into macros.
#------------------------------------------------------------------------

#---CML response text
if $equal ($inc(1) 3)
   "$str_replace([caucusscript] script \
         $cleanhtml(prohibited \
         $protect($mac_expand($reval($cleanhtml(_empty $(text)))) )))
   "<BR CLEAR=ALL>
end
  
#---HTML response text
elif $equal ($inc(1) 2)
   "$str_replace([caucusscript] script \
         $cleanhtml(prohibited $protect($mac_expand($cleanhtml(_empty $(text))))))
   "<BR CLEAR=ALL>
end
  
#---"Literal" response text
elif $equal ($inc(1) 1)
   "<PRE>
   "$cleanhtml (prohibited $protect($mac_expand($t2html($(text)))))
   "</PRE>
   "<BR CLEAR=ALL>
end
  
#---Word-wrapped response text
else
   "$cleanhtml (prohibited \
                $protect($mac_expand($wrap2html($(text)))))
   "<BR CLEAR=ALL>
end
