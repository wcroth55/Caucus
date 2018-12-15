/* compare.c - ldap backend compare function */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-ldap/compare.c,v 1.52.2.3 2005/07/11 05:55:31 kurt Exp $ */
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
ldap_back_compare(
		Operation	*op,
		SlapReply	*rs )
{
	struct ldapconn	*lc;
	ber_int_t	msgid;
	int		do_retry = 1;
	LDAPControl	**ctrls = NULL;
	int		rc = LDAP_SUCCESS;

	lc = ldap_back_getconn( op, rs, LDAP_BACK_SENDERR );
	if ( !lc || !ldap_back_dobind( lc, op, rs, LDAP_BACK_SENDERR ) ) {
		lc = NULL;
		goto cleanup;
	}

	ctrls = op->o_ctrls;
	rc = ldap_back_proxy_authz_ctrl( lc, op, rs, &ctrls );
	if ( rc != LDAP_SUCCESS ) {
		send_ldap_result( op, rs );
		goto cleanup;
	}

retry:
	rs->sr_err = ldap_compare_ext( lc->lc_ld, op->o_req_ndn.bv_val,
			op->orc_ava->aa_desc->ad_cname.bv_val,
			&op->orc_ava->aa_value, 
			ctrls, NULL, &msgid );
	rc = ldap_back_op_result( lc, op, rs, msgid, LDAP_BACK_SENDRESULT );
	if ( rc == LDAP_UNAVAILABLE && do_retry ) {
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

	return rs->sr_err;
}
