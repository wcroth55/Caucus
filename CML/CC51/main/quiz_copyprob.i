#
#---quiz_copyprob.i   Copy a single problem to a different quiz.
#
#   Input:
#      inc(1)   quiz key (copy to this quiz)
#      inc(2)   problem key (copy from this problem)
#
#   Plus these globals, presumably already SELECTed from the
#   problem to be copied.
#      category 
#      title
#      text
#      texttype
#      totalpoints
#      reference
#
#   Plus these:
#      order    new sort order for new problem
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 02/01/06 New file.
#: CR 02/13/06 Copy attached files, too.
#: CR 01/23/07 Replace %sql macro calls.
#: CR 11/24/07 Also copy 'reference' field.
#: CR 10/06/09 Remove $sql_sequence(quizproblems,quizanswers)
#-----------------------------------------------------------------------

set q3 \
    INSERT INTO quizproblems (pkey, owner, category, title, text,  \
                   texttype, totalpoints, reference) \
    VALUES (NULL, '$userid()', '$escsingle($(category))', \
            '$escsingle($(title))', '$escsingle($(text))', '$(texttype)',\
             0$(totalpoints), '$(reference)')
set h3 $sql_query_open ($(q3))
eval   $sql_query_close($(h3))
set newkey $sql_last_insert()
#"h3=$(h3), q3=$(q3)

#---Make new map (relation table) entries for the problems.
set order $plus(1 $(order))
set q4 INSERT INTO quizmap (qkey, pkey, sortorder) \
                    VALUES ($inc(1), $(newkey), $(order))
set h4 $sql_query_open ($(q4))
eval   $sql_query_close($(h4))
#"h4=$(h4), q4=$(q4)

#---Copy all of the answer rows for each problem.
set q5 SELECT a.text1, a.type, a.text2, a.answer, a.sortorder, a.points \
         FROM quizanswers a WHERE a.pkey = $inc(2)
set h5 $sql_query_open ($(q5))
while  $sql_query_row  ($(h5))
   set q6 INSERT INTO quizanswers (akey, pkey, text1, type, text2, \
                          answer, sortorder, points) \
          VALUES (NULL, $(newkey), '$escsingle($(text1))', '$(type)', \
                 '$escsingle($(text2))', '$escsingle($(answer))', \
                  0$(sortorder), 0$(points))
   set h6 $sql_query_open ($(q6))
   eval   $sql_query_close($(h6))
   set akey $sql_last_insert()
#  "h6=$(h6), q6=$(q6)
end
eval $sql_query_close ($(h5))

#---Copy any attached files.
for f in $dirlist (0 $caucus_path()/public_html/LIB/QUIZ/$inc(2))
   eval $copy2lib (  $caucus_path()/public_html/LIB/QUIZ/$inc(2)/$(f) \
                      QUIZ/$(newkey)/$(f))
end

#-----------------------------------------------------------------------
