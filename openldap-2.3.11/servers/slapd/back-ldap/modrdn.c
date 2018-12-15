/* modrdn.c - ldap backend modrdn function */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-ldap/modrdn.c,v 1.38.2.4 2005/08/09 21:04:02 kurt Exp $ */
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

#include <ac/socket.h>
#include <ac/string.h>

#include "slap.h"
#include "back-ldap.h"

int
ldap_back_modrdn(
		Operation	*op,
 		SlapReply	*rs )
{
	struct ldapconn *lc;
	ber_int_t	msgid;
	LDAPControl	**ctrls = NULL;
	int		do_retry = 1;
	int		rc = LDAP_SUCCESS;
	char		*newSup = NULL;

	lc = ldap_back_getconn( op, rs, LDAP_BACK_SENDERR );
	if ( !lc || !ldap_back_dobind( lc, op, rs, LDAP_BACK_SENDERR ) ) {
		return rs->sr_err;
	}

	if ( op->orr_newSup ) {
		int	version = LDAP_VERSION3;
		
		ldap_set_option( lc->lc_ld, LDAP_OPT_PROTOCOL_VERSION, &version );
		newSup = op->orr_newSup->bv_val;
	}

	ctrls = op->o_ctrls;
	rc = ldap_back_proxy_authz_ctrl( lc, op, rs, &ctrls );
	if ( rc != LDAP_SUCCESS ) {
		send_ldap_result( op, rs );
		rc = -1;
		goto cleanup;
	}

retry:
	rs->sr_err = ldap_rename( lc->lc_ld, op->o_req_ndn.bv_val,
			op->orr_newrdn.bv_val, newSup,
			op->orr_deleteoldrdn, ctrls, NULL, &msgid );
	rc = ldap_back_op_result( lc, op, rs, msgid, LDAP_BACK_SENDRESULT );
	if ( rs->sr_err == LDAP_SERVER_DOWN && do_retry ) {
		do_retry = 0;
		if ( ldap_back_retry( lc, op, rs, LDAP_BACK_SENDERR ) ) {
			goto retry;
		}
	}

cleanup:
	(void)ldap_back_proxy_authz_ctrl_free( op, &ctrls );

	if ( lc != NULL ) {
		ldap_back_release_conn( op, rs, lc );
	}

	return rc;
}

