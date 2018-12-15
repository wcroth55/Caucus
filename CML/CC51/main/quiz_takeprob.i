#
#---quiz_takeprob.i.   CML section that displays problem(s) to take.
#
#   Input:
#      inc(1)  quiz key
#      inc(2)  problem key
#      inc(3)  problem number (label)
#      inc(4)  userid
#      count   global supplied from caller, used to create form field names.
#                 (should start at -1)
#
#   Output:
#      noAnswerGiven  1=there is a problem, but user has not entered an answer.
#
#   Caller must wrap this file in a <form> that will handle
#   saving the results.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 02/01/06 New file, extracted from old quiz_takeprob.cml
#: CR 10/16/06 Add 'floating' field.
#: CR 10/25/06 Handle "still taking" vs reviewing results cases.
#: CR 01/23/07 Move stillTaking to calling code.
#: CR 09/25/07 Add problem reference file.
#: CR 11/20/07 Handle viewing other people's scores.
#-----------------------------------------------------------------------

#---Get fields from current problem.
set title
set q3 SELECT owner, category, title, text, texttype, totalpoints, floating, reference \
         FROM quizproblems WHERE pkey=$inc(2) AND deleted=0
set h3 $sql_query_open ($(q3))
eval   $sql_query_row  ($(h3))
eval   $sql_query_close($(h3))

"<tr valign=top><td align=right>$ifthen ($not_empty($inc(3)) $inc(3).&nbsp;)</td>
"<td colspan=5>

#---CML response text
if $equal ($(texttype) cml)
   "$cleanhtml(prohibited \
         $protect($mac_expand($reval($cleanhtml(_empty $(text)))) ))
#  "<BR CLEAR=ALL>
end
  
#---HTML response text
elif $equal ($(texttype) html)
   set output $cleanhtml(prohibited \
         $protect($mac_expand($cleanhtml(_empty $(text)))))
   if $equal (<p> $lower ($str_sub (0 3 $(output))))
      set output $str_sub (3 1000000 $(output))
   end
   set outlen $minus ($strlen($(output)) 4)
   if $equal (</p> $lower ($str_sub ($(outlen) 4 $(output))))
      set output $str_sub (0 $(outlen) $(output))
   end
#  "$(output)<BR CLEAR=ALL>
   "$(output)
end
  
#---"Literal" response text
elif $equal ($(texttype) literal)
   "<PRE>
   "$cleanhtml (prohibited $protect($mac_expand($t2html($(text)))))
   "</PRE>
#  "<BR CLEAR=ALL>
end
  
#---Word-wrapped response text
else
   "$cleanhtml (prohibited \
                $protect($mac_expand($wrap2html($(text)))))
#  "<BR CLEAR=ALL>
end
"</td>

set q4 \
    SELECT a.akey, a.text1, a.type, a.text2, a.answer, s.answer sanswer, \
           a.points, a.newrow, a.answer \
      FROM quizanswers a \
      LEFT JOIN quizscore s \
        ON (a.akey = s.akey AND s.userid='$inc(4)') \
     WHERE a.pkey=$inc(2) ORDER BY sortorder
set h4 $sql_query_open ($(q4))

set td0 $ifthen ($not($(floating)) <td>)
set td1 $ifthen ($not($(floating)) </td>)

set headerRow
if $and ($(seeAnswers) $not($(stillTaking)))
   set headerRow <tr><td><td><td></td> $(td0)$(td1)$(td0)$(td1)  \
                     <td class="quizAnswer"><B>Correct answer:</B></td>
end
set firstrow      1
set noAnswerGiven 1
set allQuestionsAnswered 1
set parity 1
while $sql_query_row  ($(h4))
   if $not_empty($(sanswer))
      set noAnswerGiven 0
   end
   if $empty($(sanswer))
      set allQuestionsAnswered 0
   end
   

   "$(headerRow)
   set headerRow
   set count $plus($(count) 1)
   "<input type="hidden" name="pkey_$(count)" value="$inc(2)">
   "<input type="hidden" name="akey_$(count)" value="$(akey)">
   set tdClose
   if $or ($(firstrow) 0$(newrow))
      set parity $minus(1 $(parity))
      "<tr valign=top class="itemlist_row$(parity)"><td></td><td></td>
      "<td class="quizProbRow">
      set tdClose </td>
      set firstrow 0
   end
   elif $not($(floating))
      "<td class="quizProbRow">
      set tdClose </td>
   end
   
   "&nbsp;$(text1)$(td1)
   if $equal ($(type) number)
      "$(td0)<input type="text" size="5" name="ans_$(count)" 
      "          value="$escquote($(sanswer))">&nbsp;$(td1)
   end
   elif $equal ($(type) radio)
      "$(td0)<input type="radio" name="radio_$inc(2)" value="$(akey)"
      "       $ifthen(0$(sanswer) checked)>&nbsp;
   end
   elif $equal ($(type) tf)
      set true  $equal (x$(sanswer) xt)
      set false $equal (x$(sanswer) xf)
      "$(td0)<nobr><input type="radio" name="ans_$(count)" value="t"
      "    $ifthen($(true)  checked)
      "   >&nbsp;True&nbsp;&nbsp;&nbsp;<input 
      "    $ifthen($(false) checked)
      "      type="radio" name="ans_$(count)" value="f"
      "   >&nbsp;False&nbsp;&nbsp;&nbsp;</nobr>$(td1)
   end
   elif $equal ($(type) check)
      "$(td0)<input type="checkbox" name="ans_$(count)" value="1"
      "       $ifthen(0$(sanswer) checked)>$(td1)
   end
   elif $equal ($(type) text)
      "$(td0)<input type="text" size="40" name="ans_$(count)" 
      "          value="$escquote($(sanswer))">$(td1)
   end
   elif $equal ($(type) box)
      "$(td0)<textarea cols=40 rows=4 name="ans_$(count)"
      "   >$escquote($(sanswer))</textarea>$(td1)
   end
#  else
#     "$(td0)type: $(type)</td>
#  end
   "$(td0)$(text2)&nbsp;
   "$(tdClose)

   #---Display the correct answer.
   if $and ($(seeAnswers) $not($(stillTaking)))
      if $equal ($strlen($(answer)) 1)
         set answer $upper($(answer))
      end
      if $tablefind ($(type) check radio)
         "<td class="quizAnswer">
         if $greater (0$(points) 0)
            "$(points) points
         end
         "</td>
      end
      else
         "<td class="quizAnswer">$(answer)</td>
      end
   end
end

if $(firstrow)
   set noAnswerGiven 0
end

eval  $sql_query_close($(h4))

if $and ($(seeAnswers) $not($(stillTaking)) $not_empty($(reference)))
   "<tr><td><td> $(td0)$(td1)$(td0)$(td1) 
   "    <td class="quizAnswer"><B>Reference:</B>
   "    <a href="/~$caucus_id()/LIB/QUIZ/$inc(2)/$(reference)"
   "       target="_blank">$(reference)</a></td>
end
