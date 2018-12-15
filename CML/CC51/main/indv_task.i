#
#---indv_task.i   Handle results of TASK from indvresp.cml
#                Then close current (pop-up) window.
#
#   $form(task)        tid of task selected
#   $form(checklist)   checklist of responses
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 03/09/05 New file.
#:CR 10/06/09 Remove $sql_sequence(tasks)
#-----------------------------------------------------------------------

#---If "Create new task", create a mostly empty one.
set tid $form(task)
if $equal ($(tid) 0)
   set tid  $sql_sequence(tid)
   set query INSERT INTO tasks (tkey, tid, lead, assigner, category, \
                                current, deleted, title) \
         VALUES (NULL, $(tid), '$userid()', '$userid()', 0, 1, 0, '(new)')
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
   set tkey $sql_last_insert()
end

#---Link the item/resp(s) to the task.
for r in $form(checklist)
   set query INSERT INTO task_links (tid, cnum, inum, rnum) \
                  VALUES ($(tid), $(icnum), $(iinum), $(r))
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
end

#---And either go to the edit-the-task page (if new task), or
#   else just clear the checkmarks and close the pop-up.
"Content-type: text/html
"
"<HTML>
"<HEAD>
"<TITLE> ...Working... </TITLE>
"<script type="text/javascript" language="javascript">
    if $equal ($form(task) 0)
    "  window.opener.location.href = \
          'taske.cml?$unique()+$(nxt)+$(arglist)+$(tkey)';
    end
    else
       include $(main)/clearchk.js
    "  window.opener.pop_gone('indv');
    "  window.close();
    end
"</SCRIPT>
"</HEAD>

"<BODY $(body_bg) >
"<CENTER><H2>...Working...</H2></CENTER>
"</div>
"</body>
"</HTML>
#-----------------------------------------------------------
