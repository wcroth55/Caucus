/* $OpenLDAP: pkg/ldap/servers/slapd/back-meta/modrdn.c,v 1.19.2.8 2005/08/09 21:04:04 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1999-2005 The OpenLDAP Foundation.
 * Portions Copyright 2001-2003 Pierangelo Masarati.
 * Portions Copyright 1999-2003 Howard Chu.
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

#include <ac/socket.h>
#include <ac/string.h>

#include "slap.h"
#include "../back-ldap/back-ldap.h"
#include "back-meta.h"

int
meta_back_modrdn( Operation *op, SlapReply *rs )
{
	metainfo_t	*mi = ( metainfo_t * )op->o_bd->be_private;
	metaconn_t	*mc;
	int		candidate = -1;
	struct berval	mdn = BER_BVNULL,
			mnewSuperior = BER_BVNULL;
	dncookie	dc;
	int		msgid;
	int		do_retry = 1;

	mc = meta_back_getconn( op, rs, &candidate, LDAP_BACK_SENDERR );
	if ( !mc || !meta_back_dobind( op, rs, mc, LDAP_BACK_SENDERR ) ) {
		return rs->sr_err;
	}

	assert( mc->mc_conns[ candidate ].msc_ld != NULL );

	dc.conn = op->o_conn;
	dc.rs = rs;

	if ( op->orr_newSup ) {
		int	version = LDAP_VERSION3;

		/*
		 * NOTE: the newParent, if defined, must be on the 
		 * same target as the entry to be renamed.  This check
		 * has been anticipated in meta_back_getconn()
		 */
		/*
		 * FIXME: one possibility is to delete the entry
		 * from one target and add it to the other;
		 * unfortunately we'd need write access to both,
		 * which is nearly impossible; for administration
		 * needs, the rootdn of the metadirectory could
		 * be mapped to an administrative account on each
		 * target (the binddn?); we'll see.
		 */
		/*
		 * NOTE: we need to port the identity assertion
		 * feature from back-ldap
		 */

		/* newSuperior needs LDAPv3; if we got here, we can safely
		 * enforce it */
		ldap_set_option( mc->mc_conns[ candidate ].msc_ld,
				LDAP_OPT_PROTOCOL_VERSION, &version );

		/*
		 * Rewrite the new superior, if defined and required
	 	 */
		dc.target = &mi->mi_targets[ candidate ];
		dc.ctx = "newSuperiorDN";
		if ( ldap_back_dn_massage( &dc, op->orr_newSup, &mnewSuperior ) ) {
			rs->sr_err = LDAP_OTHER;
			goto cleanup;
		}
	}

	/*
	 * Rewrite the modrdn dn, if required
	 */
	dc.target = &mi->mi_targets[ candidate ];
	dc.ctx = "modrDN";
	if ( ldap_back_dn_massage( &dc, &op->o_req_dn, &mdn ) ) {
		rs->sr_err = LDAP_OTHER;
		goto cleanup;
	}

retry:;
	rs->sr_err = ldap_rename( mc->mc_conns[ candidate ].msc_ld,
			mdn.bv_val, op->orr_newrdn.bv_val,
			mnewSuperior.bv_val, op->orr_deleteoldrdn,
			op->o_ctrls, NULL, &msgid );
	if ( rs->sr_err == LDAP_UNAVAILABLE && do_retry ) {
		do_retry = 0;
		if ( meta_back_retry( op, rs, mc, candidate, LDAP_BACK_SENDERR ) ) {
			goto retry;
		}

	} else if ( rs->sr_err == LDAP_SUCCESS ) {
		struct timeval	tv, *tvp = NULL;
		LDAPMessage	*res = NULL;
		int		rc;

		if ( mi->mi_targets[ candidate ].mt_timeout[ META_OP_MODRDN ] != 0 ) {
			tv.tv_sec = mi->mi_targets[ candidate ].mt_timeout[ META_OP_MODRDN ];
			tv.tv_usec = 0;
			tvp = &tv;
		}

		rs->sr_err = LDAP_OTHER;
		rc = ldap_result( mc->mc_conns[ candidate ].msc_ld,
			msgid, LDAP_MSG_ONE, tvp, &res );
		switch ( rc ) {
		case -1:
			break;

		case 0:
			ldap_abandon_ext( mc->mc_conns[ candidate ].msc_ld,
				msgid, NULL, NULL );
			rs->sr_err = op->o_protocol >= LDAP_VERSION3 ?
				LDAP_ADMINLIMIT_EXCEEDED : LDAP_OPERATIONS_ERROR;
			break;

		case LDAP_RES_RENAME:
			rc = ldap_parse_result( mc->mc_conns[ candidate ].msc_ld,
				res, &rs->sr_err, NULL, NULL, NULL, NULL, 1 );
			if ( rc != LDAP_SUCCESS ) {
				rs->sr_err = rc;
			}
			break;

		default:
			ldap_msgfree( res );
			break;
		}
	}

cleanup:;
	if ( mdn.bv_val != op->o_req_dn.bv_val ) {
		free( mdn.bv_val );
		BER_BVZERO( &mdn );
	}
	
	if ( !BER_BVISNULL( &mnewSuperior )
			&& mnewSuperior.bv_val != op->orr_newSup->bv_val )
	{
		free( mnewSuperior.bv_val );
		BER_BVZERO( &mnewSuperior );
	}

	if ( rs->sr_err == LDAP_SUCCESS ) {
		meta_back_op_result( mc, op, rs, candidate );
	} else {
		send_ldap_result( op, rs );
	}

	meta_back_release_conn( op, mc );

	return rs->sr_err;
}

