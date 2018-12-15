#
#---ilistTitleSorter.i   Prepare temporary table 'sorter' to be used
#     in ilist.i for sorting by item title.
#
#   Purpose:
#      Sorting a list of items (with up to 5 levels of breakouts)
#      is a non-trivial task.  This script sorts each level of 
#      breakouts, using 2 temporary tables, so that breakouts still
#      appear with their parent item, but sorted by Title *within*
#      each level.
#
#   Input:
#      inc(1)  cnum
#
#   Output:
#      sorterNNNN  MySQL table containing the sorted list of items.
#      sorter      this variable contains "sorterNNNN", i.e. the name of
#                  the table that was created.  It is unique to each user/process.
#                  It MUST be DROP'd by the caller!
#
# 04/10/09 CR.  First version.
#-------------------------------------------------------------------

set pidnum $word (1 $replace(/ 32 $pid()))
set sorter sorter$(pidnum)
set holder holder$(pidnum)

set h $sql_query_open  (DROP   TABLE IF EXISTS $(sorter))
eval  $sql_query_close ($(h))

set h $sql_query_open  (\
   CREATE TABLE $(sorter) ( \
      sort_id int NOT NULL auto_increment, PRIMARY KEY(sort_id), \
      item_id int, \
      i0      int, \
      i1      int, \
      i2      int, \
      i3      int, \
      i4      int, \
      i5      int, \
      s0      int, \
      s1      int, \
      s2      int, \
      s3      int, \
      s4      int, \
      s5      int))
eval  $sql_query_close ($(h))

set h $sql_query_open  (DROP   TABLE IF EXISTS $(holder))
eval  $sql_query_close ($(h))

set h $sql_query_open  ( \
   CREATE TABLE $(holder) ( \
      sort_id    int, \
      sort_value int   ) )
eval  $sql_query_close ($(h))

#---Level 0 items.
set h $sql_query_open  ( \
   INSERT INTO $(sorter) (sort_id, item_id, i0, i1, i2, i3, i4, i5, s0, s1, s2, s3, s4, s5) \
      (SELECT NULL, has.items_id, has.item0, 0, 0, 0, 0, 0,  \
                                         -1, 0, 0, 0, 0, 0   \
         FROM conf_has_items has, items \
        WHERE has.cnum=$inc(1) \
          AND has.items_id = items.id \
          AND has.item1=0 AND has.item2=0 AND has.item3=0 AND has.item4=0 AND has.item5=0 \
        ORDER BY items.title) )
eval  $sql_query_close ($(h))

set h $sql_query_open  (UPDATE $(sorter) SET s0 = sort_id)
eval  $sql_query_close ($(h))

#---Level 1 items.
set h $sql_query_open  ( \
   INSERT INTO $(sorter) (sort_id, item_id, i0, i1, i2, i3, i4, i5, s0, s1, s2, s3, s4, s5) \
      (SELECT NULL, has.items_id, has.item0, has.item1, 0, 0, 0, 0,  \
                                       sr.sort_id,  -1, 0, 0, 0, 0 \
         FROM conf_has_items has, items, $(sorter) sr \
        WHERE has.cnum=$inc(1) \
          AND has.items_id = items.id  \
          AND sr.i0 = has.item0 AND sr.i1 = 0 \
          AND has.item1>0 AND has.item2=0 AND has.item3=0 AND has.item4=0 AND has.item5=0 \
        ORDER BY items.title) )
eval  $sql_query_close ($(h))
   
set h $sql_query_open  (UPDATE $(sorter) SET s1 = sort_id WHERE s1 = -1)
eval  $sql_query_close ($(h))
   
   
#---Level 2 items
set h $sql_query_open  ( \
   INSERT INTO $(sorter) (sort_id, item_id, i0, i1, i2, i3, i4, i5, s0, s1, s2, s3, s4, s5) \
      (SELECT NULL, has.items_id, has.item0, has.item1, has.item2, 0, 0, 0,  \
                                         -1,        -2,        -3, 0, 0, 0 \
         FROM conf_has_items has, items \
        WHERE has.cnum=$inc(1) \
          AND has.items_id = items.id  \
          AND has.item1>0 AND has.item2>0 AND has.item3=0 AND has.item4=0 AND has.item5=0 \
        ORDER BY items.title) )
eval  $sql_query_close ($(h))
   
#---Level 2, i2
set h $sql_query_open  (UPDATE $(sorter) SET s2 = sort_id WHERE s2 = -3)
eval  $sql_query_close ($(h))
   
#---Level 3 items.
set h $sql_query_open  ( \
   INSERT INTO $(sorter) (sort_id, item_id, i0, i1, i2, i3, i4, i5, s0, s1, s2, s3, s4, s5) \
      (SELECT NULL, has.items_id, has.item0, has.item1, has.item2, has.item3, 0, 0,  \
                                         -1,        -2,        -3,        -4, 0, 0 \
         FROM conf_has_items has, items \
        WHERE has.cnum=$inc(1) \
          AND has.items_id = items.id  \
          AND has.item1>0 AND has.item2>0 AND has.item3>0 AND has.item4=0 AND has.item5=0 \
        ORDER BY items.title) )
   
#---Level 3, i3
set h $sql_query_open  (UPDATE $(sorter) SET s3 = sort_id WHERE s3 = -4)
eval  $sql_query_close ($(h))
   
#---Level 4 items.
set h $sql_query_open  ( \
   INSERT INTO $(sorter) (sort_id, item_id, i0, i1, i2, i3, i4, i5, s0, s1, s2, s3, s4, s5) \
      (SELECT NULL, has.items_id, has.item0, has.item1, has.item2, has.item3, has.item4, 0,  \
                                         -1,        -2,        -3,        -4,        -5, 0 \
         FROM conf_has_items has, items \
        WHERE has.cnum=$inc(1) \
          AND has.items_id = items.id  \
          AND has.item1>0 AND has.item2>0 AND has.item3>0 AND has.item4>0 AND has.item5=0 \
        ORDER BY items.title) )
eval  $sql_query_close ($(h))
   
#---Level 4, i4
set h $sql_query_open  (UPDATE $(sorter) SET s4 = sort_id WHERE s4 = -5)
eval  $sql_query_close ($(h))
   
#---Level 5 items.
set h $sql_query_open  ( \
   INSERT INTO $(sorter) (sort_id, item_id, i0, i1, i2, i3, i4, i5, s0, s1, s2, s3, s4, s5) \
      (SELECT NULL, has.items_id, has.item0, has.item1, has.item2, has.item3, has.item4, has.item5,  \
                                         -1,        -2,        -3,        -4,        -5,        -6 \
         FROM conf_has_items has, items \
        WHERE has.cnum=$inc(1) \
          AND has.items_id = items.id  \
          AND has.item1>0 AND has.item2>0 AND has.item3>0 AND has.item4>0 AND has.item5>0 \
        ORDER BY items.title) )
   
#---Level 5, i5
set h $sql_query_open  (UPDATE $(sorter) SET s5 = sort_id WHERE s5 = -6)
eval  $sql_query_close ($(h))
   
#---Level *, i0
set h $sql_query_open  ( \
   INSERT INTO $(holder) (sort_id, sort_value) \
     (SELECT sort_id,  \
           (SELECT sort2.s0 FROM $(sorter) sort2 WHERE sort2.i0 = sort1.i0 AND sort2.i1 = 0) new \
        FROM  $(sorter) sort1 \
       WHERE sort1.s0 = -1) )
eval  $sql_query_close ($(h))
   
set h $sql_query_open  ( \
   UPDATE $(sorter) sr, $(holder) hr   SET sr.s0 = hr.sort_value \
    WHERE sr.sort_id = hr.sort_id)
eval  $sql_query_close ($(h))
   
#---Level *, i1
set h $sql_query_open  (DELETE FROM $(holder))
eval  $sql_query_close ($(h))
set h $sql_query_open  ( \
   INSERT INTO $(holder) (sort_id, sort_value) \
     (SELECT sort_id,  \
           (SELECT sort2.s1 FROM $(sorter) sort2  \
             WHERE sort2.i0 = sort1.i0 AND sort2.i1 = sort1.i1 AND sort2.i2 = 0) new \
        FROM  $(sorter) sort1 \
       WHERE sort1.s1 = -2) )
eval  $sql_query_close ($(h))
   
set h $sql_query_open  ( \
   UPDATE $(sorter) sr, $(holder) hr   SET sr.s1 = hr.sort_value \
    WHERE sr.sort_id = hr.sort_id)
eval  $sql_query_close ($(h))
   
   
#---Level *, i2
set h $sql_query_open  (DELETE FROM $(holder))
eval  $sql_query_close ($(h))
set h $sql_query_open  ( \
   INSERT INTO $(holder) (sort_id, sort_value) \
     (SELECT sort_id,  \
           (SELECT sort2.s2 FROM $(sorter) sort2  \
             WHERE sort2.i0 = sort1.i0 AND sort2.i1 = sort1.i1  \
               AND sort2.i2 = sort1.i2 AND sort2.i3 = 0) new \
        FROM  $(sorter) sort1 \
       WHERE sort1.s2 = -3) )
eval  $sql_query_close ($(h))
   
set h $sql_query_open  ( \
   UPDATE $(sorter) sr, $(holder) hr   SET sr.s2 = hr.sort_value \
    WHERE sr.sort_id = hr.sort_id)
eval  $sql_query_close ($(h))
   
#---Level *, i3
set h $sql_query_open  (DELETE FROM $(holder))
set h $sql_query_open  ( \
   INSERT INTO $(holder) (sort_id, sort_value) \
     (SELECT sort_id,  \
           (SELECT sort2.s3 FROM $(sorter) sort2  \
             WHERE sort2.i0 = sort1.i0 AND sort2.i1 = sort1.i1  \
               AND sort2.i2 = sort1.i2 AND sort2.i3 = sort1.i3 AND sort2.i4 = 0) new \
        FROM  $(sorter) sort1 \
       WHERE sort1.s3 = -4) )
eval  $sql_query_close ($(h))
   
set h $sql_query_open  ( \
   UPDATE $(sorter) sr, $(holder) hr   SET sr.s3 = hr.sort_value \
    WHERE sr.sort_id = hr.sort_id)
eval  $sql_query_close ($(h))
   
   
#---Level *, i4
set h $sql_query_open  (DELETE FROM $(holder))
set h $sql_query_open  ( \
   INSERT INTO $(holder) (sort_id, sort_value) \
     (SELECT sort_id,  \
           (SELECT sort2.s4 FROM $(sorter) sort2  \
             WHERE sort2.i0 = sort1.i0 AND sort2.i1 = sort1.i1  \
               AND sort2.i2 = sort1.i2 AND sort2.i3 = sort1.i3  \
               AND sort2.i4 = sort1.i4 AND sort2.i5 = 0) new \
        FROM  $(sorter) sort1 \
       WHERE sort1.s4 = -5) )
eval  $sql_query_close ($(h))
   
set h $sql_query_open  ( \
   UPDATE $(sorter) sr, $(holder) hr   SET sr.s4 = hr.sort_value \
    WHERE sr.sort_id = hr.sort_id)
eval  $sql_query_close ($(h))
   
#---Cleanup.  Caller must DROP $(sorter) !
set h $sql_query_open  (DROP TABLE $(holder))
eval  $sql_query_close ($(h))

