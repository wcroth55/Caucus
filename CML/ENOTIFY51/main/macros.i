#
#---macros.i     Macros used for enotify daemon.
#
#: CR 03/27/00 20:56 New file.  Same bad_email_address as CS41/Local/macros.i
#: CR 12/30/03 Expand bad_email_address to include "<>()".
#              Add %displayed_conf_name().
#: CR 11/10/05 Add %sql().
#----------------------------------------------------------------------------

#---%bad_email_address (x)   
#   Bad if not empty and (no @, no ., has any of ";|&,<>()", or > 1 words).
#   Could be made smarter...
eval $mac_define (bad_email_address \
        \$and (\$not_empty(@@) \
            \$or (\$less    (\$str_index (@ @@)  0) \
                  \$less    (\$str_index (. @@)  0) \
                  \$greater (\$str_index (; @@) -1) \
                  \$greater (\$str_index (| @@) -1) \
                  \$greater (\$str_index (& @@) -1) \
                  \$greater (\$str_index (, @@) -1) \
                  \$greater (\$str_index (< @@) -1) \
                  \$greater (\$str_index (> @@) -1) \
                  \$greater (\$str_index ($char(40) @@) -1) \
                  \$greater (\$str_index ($char(41) @@) -1) \
                  \$greater (\$sizeof(@@) 1))))

#---%displayed_conf_name(cnum)
#   Calculate display form of conference name for conf 'cnum'.
eval $mac_define (displayed_conf_name \$replace(_ 32 \$cl_name(@1)))

#---%sql(query)
#   Simple execution of SQL.  For non-SELECTs only.  We keep _h in case
#   we ever need to debug this.
eval $mac_define (sql \$set (_h \$sql_query_open(@@))\
                      \$eval(\$sql_query_close(\$(_h))))

#---%item_lastresp(item_id)
eval $mac_define (item_lastresp  \
         \$sql_query_select(SELECT lastresp FROM items WHERE id=@1))
