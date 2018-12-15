#
#---createItem.i   Implementation of %createItem() macro.
#
# 11/03/10 CR
#----------------------------------------------------------

set ciAccess  %access($(m_cnum))
set ciCanAdd $or ( $gt_equal($(ciAccess) $priv(instructor)) \
                  $and( $gt_equal($(ciAccess) $priv(include)) \
                        $plus (0$site_data($(m_cnum) - co_add) \
                               0$site_data($(m_cnum) - addWiki) \
                               0$site_data($(m_cnum) - addFreeBlog) \
                               0$site_data($(m_cnum) - addModBlog))))

if $(ciCanAdd)
   "<a href="additem.cml?$(nch)+$(nxt)+$(m_cnum)+x+x+x+x+x"
   ">$inc(1)</a>
end
