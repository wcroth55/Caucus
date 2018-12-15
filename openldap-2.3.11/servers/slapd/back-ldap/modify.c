/* modify.c - ldap backend modify function */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-ldap/modify.c,v 1.58.2.6 2005/09/28 00:30:32 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1999-2005 The OpenLDAP Foundation.
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
ldap_back_modify(
		Operation	*op,
		SlapReply	*rs )
{
	struct ldapconn	*lc;
	LDAPMod		**modv = NULL,
			*mods = NULL;
	Modifications	*ml;
	int		i, j, rc;
	ber_int_t	msgid;
	int		isupdate;
	int		do_retry = 1;
	LDAPControl	**ctrls = NULL;

	lc = ldap_back_getconn( op, rs, LDAP_BACK_SENDERR );
	if ( !lc || !ldap_back_dobind( lc, op, rs, LDAP_BACK_SENDERR ) ) {
		return rs->sr_err;
	}

	for ( i = 0, ml = op->oq_modify.rs_modlist; ml; i++, ml = ml->sml_next )
		/* just count mods */ ;

	modv = (LDAPMod **)ch_malloc( ( i + 1 )*sizeof( LDAPMod * )
			+ i*sizeof( LDAPMod ) );
	if ( modv == NULL ) {
		rc = LDAP_NO_MEMORY;
		goto cleanup;
	}
	mods = (LDAPMod *)&modv[ i + 1 ];

	isupdate = be_shadow_update( op );
	for ( i = 0, ml = op->oq_modify.rs_modlist; ml; ml = ml->sml_next ) {
		if ( !isupdate && !get_manageDIT( op ) && ml->sml_desc->ad_type->sat_no_user_mod  )
		{
			continue;
		}

		modv[ i ] = &mods[ i ];
		mods[ i ].mod_op = ( ml->sml_op | LDAP_MOD_BVALUES );
		mods[ i ].mod_type = ml->sml_desc->ad_cname.bv_val;

		if ( ml->sml_values != NULL ) {
			if ( ml->sml_values == NULL ) {	
				continue;
			}

			for ( j = 0; !BER_BVISNULL( &ml->sml_values[ j ] ); j++ )
				/* just count mods */ ;
			mods[ i ].mod_bvalues =
				(struct berval **)ch_malloc( ( j + 1 )*sizeof( struct berval * ) );
			for ( j = 0; !BER_BVISNULL( &ml->sml_values[ j ] ); j++ )
			{
				mods[ i ].mod_bvalues[ j ] = &ml->sml_values[ j ];
			}
			mods[ i ].mod_bvalues[ j ] = NULL;

		} else {
			mods[ i ].mod_bvalues = NULL;
		}

		i++;
	}
	modv[ i ] = 0;

	ctrls = op->o_ctrls;
	rc = ldap_back_proxy_authz_ctrl( lc, op, rs, &ctrls );
	if ( rc != LDAP_SUCCESS ) {
		send_ldap_result( op, rs );
		rc = -1;
		goto cleanup;
	}

retry:
	rs->sr_err = ldap_modify_ext( lc->lc_ld, op->o_req_ndn.bv_val, modv,
			ctrls, NULL, &msgid );
	rc = ldap_back_op_result( lc, op, rs, msgid, LDAP_BACK_SENDRESULT );
	if ( rs->sr_err == LDAP_UNAVAILABLE && do_retry ) {
		do_retry = 0;
		if ( ldap_back_retry( lc, op, rs, LDAP_BACK_SENDERR ) ) {
			goto retry;
		}
	}

cleanup:;
	(void)ldap_back_proxy_authz_ctrl_free( op, &ctrls );

	for ( i = 0; modv[ i ]; i++ ) {
		ch_free( modv[ i ]->mod_bvalues );
	}
	ch_free( modv );

	if ( lc != NULL ) {
		ldap_back_release_conn( op, rs, lc );
	}

	return rc;
}

