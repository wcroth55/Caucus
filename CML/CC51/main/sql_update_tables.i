#
#---sql_update_tables.i    Automatically update any tables that require
#      changes since the previous software revision.
#
#:CR 10/16/06 New file.
#-------------------------------------------------------------------------

#---Add 'floating' column (does quiz problem text float, or is it tabular?)
set query SELECT floating FROM quizproblems LIMIT 1
set h $sql_query_open($(query))
eval  $sql_query_close($(h))
if $less ($(h) 0)
   set query ALTER TABLE quizproblems ADD COLUMN floating int default 0
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
end

#---Add seeAnswers column to quiz table.
set query SELECT seeAnswers FROM quiz LIMIT 1
set h $sql_query_open ($(query))
eval  $sql_query_close($(h))
if $less ($(h) 0)
   set query ALTER TABLE quiz ADD COLUMN seeAnswers int default 0
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
end

#---Site-wide user field 'registeredon'.
set query SELECT registeredon FROM user_info LIMIT 1
set h $sql_query_open ($(query))
eval  $sql_query_close($(h))
if $less ($(h) 0)
   set query ALTER TABLE user_info ADD COLUMN registeredon date
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
   set query  INSERT INTO user_columns VALUES \
                     ('registeredon','date','Registered',1)
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
end

#---Site-wide user field 'regface'.
set query SELECT regface FROM user_info LIMIT 1
set h $sql_query_open ($(query))
eval  $sql_query_close($(h))
if $less ($(h) 0)
   set query ALTER TABLE user_info ADD COLUMN regface varchar(255)
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
   set query  INSERT INTO user_columns VALUES \
                     ('regface','string','Reg Interface',1)
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
end

#---Add 'copy_cnum' column to 'resps'.
set query SELECT copy_cnum FROM resps LIMIT 1
set h $sql_query_open($(query))
eval  $sql_query_close($(h))
if $less ($(h) 0)
   set query ALTER TABLE resps ADD COLUMN copy_cnum int default 0
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
end
