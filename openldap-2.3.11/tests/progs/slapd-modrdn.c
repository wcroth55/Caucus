/* $OpenLDAP: pkg/ldap/tests/progs/slapd-modrdn.c,v 1.7.2.4 2005/08/19 18:09:06 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1999-2005 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */
/* ACKNOWLEDGEMENTS:
 * This work was initially developed by Howard Chu, based in part
 * on other OpenLDAP test tools, for inclusion in OpenLDAP Software.
 */

#include "portable.h"

#include <stdio.h>

#include <ac/stdlib.h>

#include <ac/ctype.h>
#include <ac/param.h>
#include <ac/socket.h>
#include <ac/string.h>
#include <ac/unistd.h>
#include <ac/wait.h>

#define LDAP_DEPRECATED 1
#include <ldap.h>

#define LOOPS	100
#define RETRIES	0

static void
do_modrdn( char *uri, char *host, int port, char *manager, char *passwd,
		char *entry, int maxloop, int maxretries, int delay );

static void
usage( char *name )
{
        fprintf( stderr,
		"usage: %s "
		"-H <uri> | ([-h <host>] -p <port>) "
		"-D <manager> "
		"-w <passwd> "
		"-e <entry> "
		"[-l <loops>] "
		"[-r <maxretries>] "
		"[-t <delay>]\n",
			name );
	exit( EXIT_FAILURE );
}

int
main( int argc, char **argv )
{
	int		i;
	char		*uri = NULL;
	char		*host = "localhost";
	int		port = -1;
	char		*manager = NULL;
	char		*passwd = NULL;
	char		*entry = NULL;
	int		loops = LOOPS;
	int		retries = RETRIES;
	int		delay = 0;

	while ( (i = getopt( argc, argv, "H:h:p:D:w:e:l:r:t:" )) != EOF ) {
		switch( i ) {
		case 'H':		/* the server uri */
			uri = strdup( optarg );
			break;

		case 'h':		/* the servers host */
			host = strdup( optarg );
			break;

		case 'p':		/* the servers port */
			port = atoi( optarg );
			break;

		case 'D':		/* the servers manager */
			manager = strdup( optarg );
			break;

		case 'w':		/* the server managers password */
			passwd = strdup( optarg );
			break;

		case 'e':		/* entry to rename */
			entry = strdup( optarg );
			break;

		case 'l':		/* the number of loops */
			loops = atoi( optarg );
			break;

		case 'r':		/* the number of retries */
			retries = atoi( optarg );
			break;

		case 't':		/* delay in seconds */
			delay = atoi( optarg );
			break;

		default:
			usage( argv[0] );
			break;
		}
	}

	if (( entry == NULL ) || ( port == -1 && uri == NULL ))
		usage( argv[0] );

	if ( *entry == '\0' ) {

		fprintf( stderr, "%s: invalid EMPTY entry DN.\n",
				argv[0] );
		exit( EXIT_FAILURE );

	}

	do_modrdn( uri, host, port, manager, passwd, entry, loops, retries, delay );
	exit( EXIT_SUCCESS );
}


static void
do_modrdn( char *uri, char *host, int port, char *manager,
	char *passwd, char *entry, int maxloop, int maxretries, int delay )
{
	LDAP	*ld = NULL;
	int  	i = 0, do_retry = maxretries;
	pid_t	pid;
	char *DNs[2];
	char *rdns[2];
	int         rc = LDAP_SUCCESS;


	pid = getpid();
	DNs[0] = entry;
	DNs[1] = strdup( entry );

	/* reverse the RDN, make new DN */
	{
		char *p1, *p2;

		p1 = strchr( entry, '=' ) + 1;
		p2 = strchr( p1, ',' );

		*p2 = '\0';
		rdns[1] = strdup( entry );
		*p2-- = ',';

		for (i = p1 - entry;p2 >= p1;)
			DNs[1][i++] = *p2--;
		
		DNs[1][i] = '\0';
		rdns[0] = strdup( DNs[1] );
		DNs[1][i] = ',';
	}

retry:;
	if ( uri ) {
		ldap_initialize( &ld, uri );
	} else {
		ld = ldap_init( host, port );
	}
	if ( ld == NULL ) {
		perror( "ldap_init" );
		exit( EXIT_FAILURE );
	}

	{
		int version = LDAP_VERSION3;
		(void) ldap_set_option( ld, LDAP_OPT_PROTOCOL_VERSION,
			&version ); 
	}

	if ( do_retry == maxretries ) {
		fprintf( stderr, "PID=%ld - Modrdn(%d): entry=\"%s\".\n",
			(long) pid, maxloop, entry );
	}

	rc = ldap_bind_s( ld, manager, passwd, LDAP_AUTH_SIMPLE );
	if ( rc != LDAP_SUCCESS ) {
		ldap_perror( ld, "ldap_bind" );
		switch ( rc ) {
		case LDAP_BUSY:
		case LDAP_UNAVAILABLE:
			if ( do_retry > 0 ) {
				do_retry--;
				if ( delay > 0) {
				    sleep( delay );
				}
				goto retry;
			}
		/* fallthru */
		default:
			break;
		}
		exit( EXIT_FAILURE );
	}

	for ( ; i < maxloop; i++ ) {
		rc = ldap_modrdn2_s( ld, DNs[0], rdns[0], 0 );
		if ( rc != LDAP_SUCCESS ) {
			ldap_perror( ld, "ldap_modrdn" );
			if ( rc == LDAP_BUSY && do_retry > 0 ) {
				do_retry--;
				goto retry;
			}
			if ( rc != LDAP_NO_SUCH_OBJECT ) break;
			continue;
		}
		rc = ldap_modrdn2_s( ld, DNs[1], rdns[1], 1 );
		if ( rc != LDAP_SUCCESS ) {
			ldap_perror( ld, "ldap_modrdn" );
			if ( rc == LDAP_BUSY && do_retry > 0 ) {
				do_retry--;
				goto retry;
			}
			if ( rc != LDAP_NO_SUCH_OBJECT ) break;
			continue;
		}
	}

	fprintf( stderr, " PID=%ld - Modrdn done (%d).\n", (long) pid, rc );

	ldap_unbind( ld );
}


