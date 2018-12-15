/* $OpenLDAP: pkg/ldap/libraries/libldap/turn.c,v 1.1.2.1 2005/05/06 16:10:22 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2005 The OpenLDAP Foundation.
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
 * This program was orignally developed by Kurt D. Zeilenga for inclusion in
 * OpenLDAP Software.
 */

/*
 * LDAPv3 Turn Operation Request
 */

#include "portable.h"

#include <stdio.h>
#include <ac/stdlib.h>

#include <ac/socket.h>
#include <ac/string.h>
#include <ac/time.h>

#include "ldap-int.h"
#include "ldap_log.h"

int
ldap_turn(
	LDAP *ld,
	int mutual,
	LDAP_CONST char* identifier,
	LDAPControl **sctrls,
	LDAPControl **cctrls,
	int *msgidp )
{
	BerElement *turnvalber = NULL;
	struct berval *turnvalp = NULL;
	int rc;

	turnvalber = ber_alloc_t( LBER_USE_DER );
	if( mutual ) {
		ber_printf( turnvalber, "{bs}", mutual, identifier );
	} else {
		ber_printf( turnvalber, "{s}", identifier );
	}
	ber_flatten( turnvalber, &turnvalp );

	rc = ldap_extended_operation( ld, LDAP_EXOP_X_TURN,
			turnvalp, sctrls, cctrls, msgidp );
	ber_free( turnvalber, 1 );
	return rc;
}

int
ldap_turn_s(
	LDAP *ld,
	int mutual,
	LDAP_CONST char* identifier,
	LDAPControl **sctrls,
	LDAPControl **cctrls )
{
	BerElement *turnvalber = NULL;
	struct berval *turnvalp = NULL;
	int rc;

	turnvalber = ber_alloc_t( LBER_USE_DER );
	if( mutual ) {
		ber_printf( turnvalber, "{bs}", 0xFF, identifier );
	} else {
		ber_printf( turnvalber, "{s}", identifier );
	}
	ber_flatten( turnvalber, &turnvalp );

	rc = ldap_extended_operation_s( ld, LDAP_EXOP_X_TURN,
			turnvalp, sctrls, cctrls, NULL, NULL );
	ber_free( turnvalber, 1 );
	return rc;
}

