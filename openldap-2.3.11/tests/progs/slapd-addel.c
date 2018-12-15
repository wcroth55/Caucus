/* $OpenLDAP: pkg/ldap/tests/progs/slapd-addel.c,v 1.27.2.4 2005/08/19 18:09:06 kurt Exp $ */
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
 * This work was initially developed by Kurt Spanier for inclusion
 * in OpenLDAP Software.
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

static char *
get_add_entry( char *filename, LDAPMod ***mods );

static void
do_addel( char *uri, char *host, int port, char *manager, char *passwd,
	char *dn, LDAPMod **attrs, int maxloop, int maxretries, int delay );

static void
usage( char *name )
{
        fprintf( stderr,
		"usage: %s "
		"-H <uri> | ([-h <host>] -p <port>) "
		"-D <manager> "
		"-w <passwd> "
		"-f <addfile> "
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
	char		*host = "localhost";
	char		*uri = NULL;
	int		port = -1;
	char		*manager = NULL;
	char		*passwd = NULL;
	char		*filename = NULL;
	char		*entry = NULL;
	int		loops = LOOPS;
	int		retries = RETRIES;
	int		delay = 0;
	LDAPMod		**attrs = NULL;

	while ( (i = getopt( argc, argv, "H:h:p:D:w:f:l:r:t:" )) != EOF ) {
		switch( i ) {
		case 'H':		/* the server's URI */
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

		case 'f':		/* file with entry search request */
			filename = strdup( optarg );
			break;

		case 'l':		/* the number of loops */
			loops = atoi( optarg );
			break;

		case 'r':		/* number of retries */
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

	if (( filename == NULL ) || ( port == -1 && uri == NULL ) ||
				( manager == NULL ) || ( passwd == NULL ))
		usage( argv[0] );

	entry = get_add_entry( filename, &attrs );
	if (( entry == NULL ) || ( *entry == '\0' )) {

		fprintf( stderr, "%s: invalid entry DN in file \"%s\".\n",
				argv[0], filename );
		exit( EXIT_FAILURE );

	}

	if (( attrs == NULL ) || ( *attrs == '\0' )) {

		fprintf( stderr, "%s: invalid attrs in file \"%s\".\n",
				argv[0], filename );
		exit( EXIT_FAILURE );

	}

	do_addel( uri, host, port, manager, passwd, entry, attrs,
			loops, retries, delay );

	exit( EXIT_SUCCESS );
}


static void
addmodifyop( LDAPMod ***pmodsp, int modop, char *attr, char *value, int vlen )
{
    LDAPMod		**pmods;
    int			i, j;
    struct berval	*bvp;

    pmods = *pmodsp;
    modop |= LDAP_MOD_BVALUES;

    i = 0;
    if ( pmods != NULL ) {
		for ( ; pmods[ i ] != NULL; ++i ) {
	    	if ( strcasecmp( pmods[ i ]->mod_type, attr ) == 0 &&
		    	pmods[ i ]->mod_op == modop ) {
				break;
	    	}
		}
    }

    if ( pmods == NULL || pmods[ i ] == NULL ) {
		if (( pmods = (LDAPMod **)realloc( pmods, (i + 2) *
			sizeof( LDAPMod * ))) == NULL ) {
	    		perror( "realloc" );
	    		exit( EXIT_FAILURE );
		}
		*pmodsp = pmods;
		pmods[ i + 1 ] = NULL;
		if (( pmods[ i ] = (LDAPMod *)calloc( 1, sizeof( LDAPMod )))
			== NULL ) {
	    		perror( "calloc" );
	    		exit( EXIT_FAILURE );
		}
		pmods[ i ]->mod_op = modop;
		if (( pmods[ i ]->mod_type = strdup( attr )) == NULL ) {
	    	perror( "strdup" );
	    	exit( EXIT_FAILURE );
		}
    }

    if ( value != NULL ) {
		j = 0;
		if ( pmods[ i ]->mod_bvalues != NULL ) {
	    	for ( ; pmods[ i ]->mod_bvalues[ j ] != NULL; ++j ) {
				;
	    	}
		}
		if (( pmods[ i ]->mod_bvalues =
			(struct berval **)ber_memrealloc( pmods[ i ]->mod_bvalues,
			(j + 2) * sizeof( struct berval * ))) == NULL ) {
	    		perror( "ber_realloc" );
	    		exit( EXIT_FAILURE );
		}
		pmods[ i ]->mod_bvalues[ j + 1 ] = NULL;
		if (( bvp = (struct berval *)ber_memalloc( sizeof( struct berval )))
			== NULL ) {
	    		perror( "malloc" );
	    		exit( EXIT_FAILURE );
		}
		pmods[ i ]->mod_bvalues[ j ] = bvp;

	    bvp->bv_len = vlen;
	    if (( bvp->bv_val = (char *)malloc( vlen + 1 )) == NULL ) {
			perror( "malloc" );
			exit( EXIT_FAILURE );
	    }
	    AC_MEMCPY( bvp->bv_val, value, vlen );
	    bvp->bv_val[ vlen ] = '\0';
    }
}


static char *
get_add_entry( char *filename, LDAPMod ***mods )
{
	FILE    *fp;
	char    *entry = NULL;

	if ( (fp = fopen( filename, "r" )) != NULL ) {
		char  line[BUFSIZ];

		if ( fgets( line, BUFSIZ, fp )) {
			char *nl;

			if (( nl = strchr( line, '\r' )) || ( nl = strchr( line, '\n' )))
				*nl = '\0';
			entry = strdup( line );

		}

		while ( fgets( line, BUFSIZ, fp )) {
			char	*nl;
			char	*value;

			if (( nl = strchr( line, '\r' )) || ( nl = strchr( line, '\n' )))
				*nl = '\0';

			if ( *line == '\0' ) break;
			if ( !( value = strchr( line, ':' ))) break;

			*value++ = '\0'; 
			while ( *value && isspace( (unsigned char) *value ))
				value++;

			addmodifyop( mods, LDAP_MOD_ADD, line, value, strlen( value ));

		}
		fclose( fp );
	}

	return( entry );
}


static void
do_addel(
	char *uri,
	char *host,
	int port,
	char *manager,
	char *passwd,
	char *entry,
	LDAPMod **attrs,
	int maxloop,
	int maxretries,
	int delay
)
{
	LDAP	*ld = NULL;
	int  	i = 0, do_retry = maxretries;
	pid_t	pid = getpid();
	int	rc = LDAP_SUCCESS;

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
		fprintf( stderr, "PID=%ld - Add/Delete(%d): entry=\"%s\".\n",
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
				if ( delay != 0 ) {
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

		/* add the entry */
		rc = ldap_add_s( ld, entry, attrs );
		if ( rc != LDAP_SUCCESS ) {
			ldap_perror( ld, "ldap_add" );
			if ( rc == LDAP_BUSY && do_retry > 0 ) {
				do_retry--;
				goto retry;
			}
			break;

		}

#if 0
		/* wait a second for the add to really complete */
		/* This masks some race conditions though. */
		sleep( 1 );
#endif

		/* now delete the entry again */
		rc = ldap_delete_s( ld, entry );
		if ( rc != LDAP_SUCCESS ) {
			ldap_perror( ld, "ldap_delete" );
			if ( rc == LDAP_BUSY && do_retry > 0 ) {
				do_retry--;
				goto retry;
			}
			break;
		}
	}

	fprintf( stderr, " PID=%ld - Add/Delete done (%d).\n", (long) pid, rc );

	ldap_unbind( ld );
}


