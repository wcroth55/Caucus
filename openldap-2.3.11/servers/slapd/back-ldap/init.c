/* init.c - initialize ldap backend */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-ldap/init.c,v 1.79.2.8 2005/08/09 21:04:02 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2003-2005 The OpenLDAP Foundation.
 * Portions Copyright 1999-2003 Howard Chu.
 * Portions Copyright 2000-2003 Pierangelo Masarati.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */
/* ACKNOWLEDGEMENTS:
 * This work was initially developed by the Howard Chu for inclusion
 * in OpenLDAP Software and subsequently enhanced by Pierangelo
 * Masarati.
 */

#include "portable.h"

#include <stdio.h>

#include <ac/string.h>
#include <ac/socket.h>

#include "slap.h"
#include "back-ldap.h"

int
ldap_back_open( BackendInfo	*bi )
{
	bi->bi_controls = slap_known_controls;
	return 0;
}

int
ldap_back_initialize( BackendInfo *bi )
{
	bi->bi_open = ldap_back_open;
	bi->bi_config = 0;
	bi->bi_close = 0;
	bi->bi_destroy = 0;

	bi->bi_db_init = ldap_back_db_init;
	bi->bi_db_config = config_generic_wrapper;
	bi->bi_db_open = ldap_back_db_open;
	bi->bi_db_close = 0;
	bi->bi_db_destroy = ldap_back_db_destroy;

	bi->bi_op_bind = ldap_back_bind;
	bi->bi_op_unbind = 0;
	bi->bi_op_search = ldap_back_search;
	bi->bi_op_compare = ldap_back_compare;
	bi->bi_op_modify = ldap_back_modify;
	bi->bi_op_modrdn = ldap_back_modrdn;
	bi->bi_op_add = ldap_back_add;
	bi->bi_op_delete = ldap_back_delete;
	bi->bi_op_abandon = 0;

	bi->bi_extended = ldap_back_extended;

	bi->bi_chk_referrals = 0;
	bi->bi_entry_get_rw = ldap_back_entry_get;

	bi->bi_connection_init = 0;
	bi->bi_connection_destroy = ldap_back_conn_destroy;

	if ( chain_init() ) {
		return -1;
	}

	return ldap_back_init_cf( bi );
}

int
ldap_back_db_init( Backend *be )
{
	struct ldapinfo	*li;

	li = (struct ldapinfo *)ch_calloc( 1, sizeof( struct ldapinfo ) );
	if ( li == NULL ) {
 		return -1;
 	}

	BER_BVZERO( &li->acl_authcID );
	BER_BVZERO( &li->acl_authcDN );
	BER_BVZERO( &li->acl_passwd );

	li->acl_authmethod = LDAP_AUTH_NONE;
	BER_BVZERO( &li->acl_sasl_mech );
	li->acl_sb.sb_tls = SB_TLS_DEFAULT;

	li->idassert_mode = LDAP_BACK_IDASSERT_LEGACY;

	BER_BVZERO( &li->idassert_authcID );
	BER_BVZERO( &li->idassert_authcDN );
	BER_BVZERO( &li->idassert_passwd );

	BER_BVZERO( &li->idassert_authzID );

	li->idassert_authmethod = LDAP_AUTH_NONE;
	BER_BVZERO( &li->idassert_sasl_mech );
	li->idassert_sb.sb_tls = SB_TLS_DEFAULT;

	/* by default, use proxyAuthz control on each operation */
	li->idassert_flags = LDAP_BACK_AUTH_PRESCRIPTIVE;

	li->idassert_authz = NULL;

	/* initialize flags */
	li->flags = LDAP_BACK_F_CHASE_REFERRALS;

	/* initialize version */
	li->version = LDAP_VERSION3;

	ldap_pvt_thread_mutex_init( &li->conn_mutex );

	be->be_private = li;
	SLAP_DBFLAGS( be ) |= SLAP_DBFLAG_NOLASTMOD;

	be->be_cf_ocs = be->bd_info->bi_cf_ocs;

	return 0;
}

int
ldap_back_db_open( BackendDB *be )
{
	struct ldapinfo	*li = (struct ldapinfo *)be->be_private;

	Debug( LDAP_DEBUG_TRACE,
		"ldap_back_db_open: URI=%s\n",
		li->url != NULL ? li->url : "", 0, 0 );

	/* by default, use proxyAuthz control on each operation */
	switch ( li->idassert_mode ) {
	case LDAP_BACK_IDASSERT_LEGACY:
	case LDAP_BACK_IDASSERT_SELF:
		/* however, since admin connections are pooled and shared,
		 * only static authzIDs can be native */
		li->idassert_flags &= ~LDAP_BACK_AUTH_NATIVE_AUTHZ;
		break;

	default:
		break;
	}

#if 0 && defined(SLAPD_MONITOR)
	{
		/* FIXME: disabled because namingContexts doesn't have
		 * a matching rule, and using an MRA filter doesn't work
		 * because the normalized assertion is compared to the 
		 * non-normalized value, which in general differs from
		 * the normalized one.  See ITS#3406 */
		struct berval	filter,
				base = BER_BVC( "cn=Databases," SLAPD_MONITOR );
		struct berval	vals[ 2 ];
		Attribute	a = { 0 };

		filter.bv_len = STRLENOF( "(&(namingContexts:distinguishedNameMatch:=)(monitoredInfo=ldap))" )
			+ be->be_nsuffix[ 0 ].bv_len;
		filter.bv_val = ch_malloc( filter.bv_len + 1 );
		snprintf( filter.bv_val, filter.bv_len + 1,
				"(&(namingContexts:distinguishedNameMatch:=%s)(monitoredInfo=ldap))",
				be->be_nsuffix[ 0 ].bv_val );

		a.a_desc = slap_schema.si_ad_labeledURI;
		ber_str2bv( li->url, 0, 0, &vals[ 0 ] );
		BER_BVZERO( &vals[ 1 ] );
		a.a_vals = vals;
		a.a_nvals = vals;
		if ( monitor_back_register_entry_attrs( NULL, &a, NULL, &base, LDAP_SCOPE_SUBTREE, &filter ) ) {
			/* error */
		}

		ch_free( filter.bv_val );
	}
#endif /* SLAPD_MONITOR */

	if ( li->flags & LDAP_BACK_F_SUPPORT_T_F_DISCOVER ) {
		int		rc;

		li->flags &= ~LDAP_BACK_F_SUPPORT_T_F_DISCOVER;

		rc = slap_discover_feature( li->url, li->version,
				slap_schema.si_ad_supportedFeatures->ad_cname.bv_val,
				LDAP_FEATURE_ABSOLUTE_FILTERS );
		if ( rc == LDAP_COMPARE_TRUE ) {
			li->flags |= LDAP_BACK_F_SUPPORT_T_F;
		}
	}

	return 0;
}

void
ldap_back_conn_free( void *v_lc )
{
	struct ldapconn	*lc = v_lc;

	if ( lc->lc_ld != NULL ) {	
		ldap_unbind_ext( lc->lc_ld, NULL, NULL );
	}
	if ( !BER_BVISNULL( &lc->lc_bound_ndn ) ) {
		ch_free( lc->lc_bound_ndn.bv_val );
	}
	if ( !BER_BVISNULL( &lc->lc_cred ) ) {
		memset( lc->lc_cred.bv_val, 0, lc->lc_cred.bv_len );
		ch_free( lc->lc_cred.bv_val );
	}
	if ( !BER_BVISNULL( &lc->lc_local_ndn ) ) {
		ch_free( lc->lc_local_ndn.bv_val );
	}
	ch_free( lc );
}

int
ldap_back_db_destroy(
    Backend	*be
)
{
	struct ldapinfo	*li;

	if ( be->be_private ) {
		li = ( struct ldapinfo * )be->be_private;

		ldap_pvt_thread_mutex_lock( &li->conn_mutex );

		if ( li->url != NULL ) {
			ch_free( li->url );
			li->url = NULL;
		}
		if ( li->lud ) {
			ldap_free_urldesc( li->lud );
			li->lud = NULL;
		}
		if ( !BER_BVISNULL( &li->acl_authcID ) ) {
			ch_free( li->acl_authcID.bv_val );
			BER_BVZERO( &li->acl_authcID );
		}
		if ( !BER_BVISNULL( &li->acl_authcDN ) ) {
			ch_free( li->acl_authcDN.bv_val );
			BER_BVZERO( &li->acl_authcDN );
		}
		if ( !BER_BVISNULL( &li->acl_passwd ) ) {
			ch_free( li->acl_passwd.bv_val );
			BER_BVZERO( &li->acl_passwd );
		}
		if ( !BER_BVISNULL( &li->acl_sasl_mech ) ) {
			ch_free( li->acl_sasl_mech.bv_val );
			BER_BVZERO( &li->acl_sasl_mech );
		}
		if ( !BER_BVISNULL( &li->acl_sasl_realm ) ) {
			ch_free( li->acl_sasl_realm.bv_val );
			BER_BVZERO( &li->acl_sasl_realm );
		}
		if ( !BER_BVISNULL( &li->idassert_authcID ) ) {
			ch_free( li->idassert_authcID.bv_val );
			BER_BVZERO( &li->idassert_authcID );
		}
		if ( !BER_BVISNULL( &li->idassert_authcDN ) ) {
			ch_free( li->idassert_authcDN.bv_val );
			BER_BVZERO( &li->idassert_authcDN );
		}
		if ( !BER_BVISNULL( &li->idassert_passwd ) ) {
			ch_free( li->idassert_passwd.bv_val );
			BER_BVZERO( &li->idassert_passwd );
		}
		if ( !BER_BVISNULL( &li->idassert_authzID ) ) {
			ch_free( li->idassert_authzID.bv_val );
			BER_BVZERO( &li->idassert_authzID );
		}
		if ( !BER_BVISNULL( &li->idassert_sasl_mech ) ) {
			ch_free( li->idassert_sasl_mech.bv_val );
			BER_BVZERO( &li->idassert_sasl_mech );
		}
		if ( !BER_BVISNULL( &li->idassert_sasl_realm ) ) {
			ch_free( li->idassert_sasl_realm.bv_val );
			BER_BVZERO( &li->idassert_sasl_realm );
		}
		if ( li->idassert_authz != NULL ) {
			ber_bvarray_free( li->idassert_authz );
			li->idassert_authz = NULL;
		}
                if ( li->conntree ) {
			avl_free( li->conntree, ldap_back_conn_free );
		}

		ldap_pvt_thread_mutex_unlock( &li->conn_mutex );
		ldap_pvt_thread_mutex_destroy( &li->conn_mutex );
	}

	ch_free( be->be_private );

	return 0;
}

#if SLAPD_LDAP == SLAPD_MOD_DYNAMIC

/* conditionally define the init_module() function */
SLAP_BACKEND_INIT_MODULE( ldap )

#endif /* SLAPD_LDAP == SLAPD_MOD_DYNAMIC */

