/* $OpenLDAP: pkg/ldap/servers/slapd/overlays/pcache.c,v 1.41.2.9 2005/10/12 21:33:53 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2003-2005 The OpenLDAP Foundation.
 * Portions Copyright 2003 IBM Corporation.
 * Portions Copyright 2003 Symas Corporation.
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
 * This work was initially developed by Apurva Kumar for inclusion
 * in OpenLDAP Software and subsequently rewritten by Howard Chu.
 */

#include "portable.h"

#ifdef SLAPD_OVER_PROXYCACHE

#include <stdio.h>

#include <ac/string.h>
#include <ac/time.h>

#include "slap.h"
#include "lutil.h"
#include "ldap_rq.h"

#include "config.h"

/* query cache structs */
/* query */

typedef struct Query_s {
	Filter* 	filter; 	/* Search Filter */
	AttributeName* 	attrs;		/* Projected attributes */
	AttributeName*  save_attrs;	/* original attributes, saved for response */
	struct berval 	base; 		/* Search Base */
	int 		scope;		/* Search scope */
} Query;

/* struct representing a cached query */
typedef struct cached_query_s {
	Query 				query;		/* LDAP query */
	struct berval			q_uuid;		/* query identifier */
	int 				template_id;	/* template of the query */
	time_t 				expiry_time;	/* time till the query is considered valid */
	struct cached_query_s  		*next;  	/* next query in the template */
	struct cached_query_s  		*prev;  	/* previous query in the template */
	struct cached_query_s           *lru_up;	/* previous query in the LRU list */
	struct cached_query_s           *lru_down;	/* next query in the LRU list */
} CachedQuery;

/* struct representing a query template
 * e.g. template string = &(cn=)(mail=)
 */
typedef struct query_template_s {
	struct berval	querystr;	/* Filter string corresponding to the QT */
	int 		attr_set_index; /* determines the projected attributes */

	CachedQuery* 	query;	        /* most recent query cached for the template */
	CachedQuery* 	query_last;     /* oldest query cached for the template */

	int 		no_of_queries;  /* Total number of queries in the template */
	long 		ttl;		/* TTL for the queries of this template */
        ldap_pvt_thread_rdwr_t t_rwlock; /* Rd/wr lock for accessing queries in the template */
} QueryTemplate;

/*
 * Represents a set of projected attributes and any
 * supersets among all specified sets of attributes.
 */

struct attr_set {
	AttributeName*	attrs; 		/* specifies the set */
	int 		count;		/* number of attributes */
	int*		ID_array;	/* array of indices of supersets of 'attrs' */
};

struct query_manager_s;

/* prototypes for functions for 1) query containment
 * 2) query addition, 3) cache replacement
 */
typedef int 	(QCfunc)(struct query_manager_s*, Query*, int );
typedef void  	(AddQueryfunc)(struct query_manager_s*, Query*, int, struct berval*);
typedef void	(CRfunc)(struct query_manager_s*, struct berval * );

/* LDAP query cache */
typedef struct query_manager_s {
	struct attr_set* 	attr_sets;		/* possible sets of projected attributes */
	QueryTemplate*	  	templates;		/* cacheable templates */

	CachedQuery*		lru_top;		/* top and bottom of LRU list */
	CachedQuery*		lru_bottom;

	ldap_pvt_thread_mutex_t		lru_mutex;	/* mutex for accessing LRU list */

	/* Query cache methods */
	QCfunc			*qcfunc;			/* Query containment*/
	CRfunc 			*crfunc;			/* cache replacement */
	AddQueryfunc	*addfunc;			/* add query */
} query_manager;

/* LDAP query cache manager */
typedef struct cache_manager_s {
	BackendDB	db;	/* underlying database */
	unsigned long	num_cached_queries; 		/* total number of cached queries */
	unsigned long   max_queries;			/* upper bound on # of cached queries */
	int 	numattrsets;			/* number of attribute sets */
	int 	numtemplates;			/* number of cacheable templates */
	int 	cur_entries;			/* current number of entries cached */
	int 	max_entries;			/* max number of entries cached */
        int     num_entries_limit;		/* max # of entries in a cacheable query */

	char	response_cb;			/* install the response callback
						 * at the tail of the callback list */
#define PCACHE_RESPONSE_CB_HEAD	0
#define PCACHE_RESPONSE_CB_TAIL	1

	int     cc_period;		/* interval between successive consistency checks (sec) */
	int	cc_paused;
	void	*cc_arg;

	ldap_pvt_thread_mutex_t		cache_mutex;
	ldap_pvt_thread_mutex_t		remove_mutex;

	query_manager*   qm;	/* query cache managed by the cache manager */
} cache_manager;

static AttributeDescription *ad_queryid;
static char *queryid_schema = "( 1.3.6.1.4.1.4203.666.1.12 NAME 'queryid' "
			"DESC 'list of queries the entry belongs to' "
			"EQUALITY octetStringMatch "
			"SYNTAX 1.3.6.1.4.1.1466.115.121.1.40{64} "
			"NO-USER-MODIFICATION USAGE directoryOperation )";

/* Return 1 for an added entry, else 0 */
static int
merge_entry(
	Operation		*op,
	Entry			*e,
	struct berval*		query_uuid )
{
	int		rc;
	Modifications* modlist = NULL;
	const char* 	text = NULL;
	Attribute		*attr;
	char			textbuf[SLAP_TEXT_BUFLEN];
	size_t			textlen = sizeof(textbuf);

	SlapReply sreply = {REP_RESULT};

	slap_callback cb = { NULL, slap_null_cb, NULL, NULL };

	attr = e->e_attrs;
	e->e_attrs = NULL;

	/* add queryid attribute */
	attr_merge_one( e, ad_queryid, query_uuid, NULL );

	/* append the attribute list from the fetched entry */
	e->e_attrs->a_next = attr;

	op->o_tag = LDAP_REQ_ADD;
	op->o_protocol = LDAP_VERSION3;
	op->o_callback = &cb;
	op->o_time = slap_get_time();
	op->o_do_not_cache = 1;

	op->ora_e = e;
	op->o_req_dn = e->e_name;
	op->o_req_ndn = e->e_nname;
	rc = op->o_bd->be_add( op, &sreply );

	if ( rc != LDAP_SUCCESS ) {
		if ( rc == LDAP_ALREADY_EXISTS ) {
			slap_entry2mods( e, &modlist, &text, textbuf, textlen );
			modlist->sml_op = LDAP_MOD_ADD;
			op->o_tag = LDAP_REQ_MODIFY;
			op->orm_modlist = modlist;
			op->o_bd->be_modify( op, &sreply );
			slap_mods_free( modlist, 1 );
		} else if ( rc == LDAP_REFERRAL ||
					rc == LDAP_NO_SUCH_OBJECT ) {
			syncrepl_add_glue( op, e );
			e = NULL;
			rc = 1;
		}
		if ( e ) {
			entry_free( e );
			rc = 0;
		}
	} else {
		be_entry_release_w( op, e );
		rc = 1;
	}

	return rc;
}

/* compare base and scope of incoming and cached queries */
static int base_scope_compare(
	struct berval* ndn_stored,
	struct berval* ndn_incoming,
	int scope_stored,
	int scope_incoming	)
{
	struct berval pdn_incoming = BER_BVNULL;

	if (scope_stored < scope_incoming)
		return 0;

	if ( !dnIsSuffix(ndn_incoming, ndn_stored))
		return 0;

	switch(scope_stored) {
	case LDAP_SCOPE_BASE:
		return (ndn_incoming->bv_len == ndn_stored->bv_len);

	case LDAP_SCOPE_ONELEVEL:
		switch(scope_incoming){
		case LDAP_SCOPE_BASE:
			dnParent(ndn_incoming, &pdn_incoming);
			return (pdn_incoming.bv_len == ndn_stored->bv_len);

		case LDAP_SCOPE_ONELEVEL:
			return (ndn_incoming->bv_len == ndn_stored->bv_len);

		default:
			return 0;
		}
	case LDAP_SCOPE_SUBTREE:
		return 1;
		break;
	default:
		return 0;
		break;
    }
}

/* add query on top of LRU list */
static void
add_query_on_top (query_manager* qm, CachedQuery* qc)
{
	CachedQuery* top = qm->lru_top;
	Query* q = (Query*)qc;

	qm->lru_top = qc;

	if (top)
		top->lru_up = qc;
	else
		qm->lru_bottom = qc;

	qc->lru_down = top;
	qc->lru_up = NULL;
	Debug( LDAP_DEBUG_ANY, "Base of added query = %s\n",
			q->base.bv_val, 0, 0 );
}

/* remove_query from LRU list */

static void
remove_query (query_manager* qm, CachedQuery* qc)
{
	CachedQuery* up;
	CachedQuery* down;

	if (!qc)
		return;

	up = qc->lru_up;
	down = qc->lru_down;

	if (!up)
		qm->lru_top = down;

	if (!down)
		qm->lru_bottom = up;

	if (down)
		down->lru_up = up;

	if (up)
		up->lru_down = down;

	qc->lru_up = qc->lru_down = NULL;
}

static void
invert_string( struct berval *bv )
{
	int i;
	char c;

	for (i=0; i<bv->bv_len/2; i++) {
		c = bv->bv_val[i];
		bv->bv_val[i] = bv->bv_val[bv->bv_len-i-1];
		bv->bv_val[bv->bv_len-i-1] = c;
	}
}

/* find and remove string2 from string1
 * from start if position = 1,
 * from end if position = 3,
 * from anywhere if position = 2
 */

static int
find_and_remove(struct berval* ber1, struct berval* ber2, int position)
{
	char* temp;
	int len;
	int ret=0;

	char* arg1, *arg2;
	char* string1=ber1->bv_val;
	char* string2=ber2->bv_val;

	if (string2 == NULL)
		return 1;
	if (string1 == NULL)
		return 0;

	if (position == 3) {
		invert_string(ber1);
		invert_string(ber2);
	}

	arg1 = string1;
	arg2 = string2;

	temp = strstr(arg1, arg2);

	len = ber2->bv_len;

	if ( temp && (position == 2 || temp == arg1) ) {
		string1 = temp+len;
		strcpy( arg1, string1 );
		ber1->bv_len -= len;
		ret = 1;
	}
	if ( position == 3 ) {
		invert_string(ber1);
		invert_string(ber2);
	}
	return ret;
}


static struct berval*
merge_init_final(struct berval* init, struct berval* any, struct berval* final)
{
	struct berval* merged, *temp;
	int i, any_count, count;

	for (any_count=0; any && any[any_count].bv_val; any_count++)
		;

	count = any_count;

	if (init->bv_val)
		count++;
	if (final->bv_val)
		count++;

	merged = (struct berval*)(ch_malloc((count+1)*sizeof(struct berval)));
	temp = merged;

	if (init->bv_val) {
		*temp++ = *init;
	}

	for (i=0; i<any_count; i++) {
		*temp++ = *any++;
	}

	if (final->bv_val){
		*temp++ = *final;
	}
	temp->bv_val = NULL;
	temp->bv_len = 0;
	return merged;
}

static int
strings_containment(struct berval* stored, struct berval* incoming)
{
	struct berval* element;
	int k=0;
	int j, rc = 0;

	for ( element=stored; element->bv_val != NULL; element++ ) {
		for (j = k; incoming[j].bv_val != NULL; j++) {
			if (find_and_remove(&(incoming[j]), element, 2)) {
				k = j;
				rc = 1;
				break;
			}
			rc = 0;
		}
		if ( rc ) {
			continue;
		} else {
			return 0;
		}
	}
	return 1;
}

static int
substr_containment_substr(Filter* stored, Filter* incoming)
{
	int i;
	int rc = 0;
	int any_count = 0;

	struct berval init_incoming;
	struct berval final_incoming;
	struct berval *any_incoming = NULL;
	struct berval *remaining_incoming = NULL;

	if ((!(incoming->f_sub_initial.bv_val) && (stored->f_sub_initial.bv_val))
	   || (!(incoming->f_sub_final.bv_val) && (stored->f_sub_final.bv_val)))
		return 0;


	ber_dupbv(&init_incoming, &(incoming->f_sub_initial));
	ber_dupbv(&final_incoming, &(incoming->f_sub_final));

	if (incoming->f_sub_any) {
		for ( any_count=0; incoming->f_sub_any[any_count].bv_val != NULL;
				any_count++ )
			;

		any_incoming = (struct berval*)ch_malloc((any_count+1) *
						sizeof(struct berval));

		for (i=0; i<any_count; i++) {
			ber_dupbv(&(any_incoming[i]), &(incoming->f_sub_any[i]));
		}
		any_incoming[any_count].bv_val = NULL;
		any_incoming[any_count].bv_len = 0;
	}

	if (find_and_remove(&init_incoming,
			&(stored->f_sub_initial), 1) && find_and_remove(&final_incoming,
			&(stored->f_sub_final), 3))
	{
		if (stored->f_sub_any == NULL) {
			rc = 1;
			goto final;
		}
		remaining_incoming = merge_init_final(&init_incoming,
						any_incoming, &final_incoming);
		rc = strings_containment(stored->f_sub_any, remaining_incoming);
	}
final:
	free(init_incoming.bv_val);
	free(final_incoming.bv_val);
	if (any_incoming) ber_bvarray_free( any_incoming );
	free(remaining_incoming);

	return rc;
}

static int
substr_containment_equality(Filter* stored, Filter* incoming)
{
	struct berval incoming_val[2];
	int rc = 0;

	ber_dupbv(incoming_val, &(incoming->f_av_value));
	incoming_val[1].bv_val = NULL;
	incoming_val[1].bv_len = 0;

	if (find_and_remove(incoming_val,
			&(stored->f_sub_initial), 1) && find_and_remove(incoming_val,
			&(stored->f_sub_final), 3)) {
		if (stored->f_sub_any == NULL){
			rc = 1;
			goto final;
		}
		rc = strings_containment(stored->f_sub_any, incoming_val);
	}
final:
	free(incoming_val[0].bv_val);
	return rc;
}

/* check whether query is contained in any of
 * the cached queries in template template_index
 */
static int
query_containment(query_manager *qm,
		  Query *query,
		  int template_index)
{
	QueryTemplate* templa= qm->templates;
	CachedQuery* qc;
	Query* q;
	Filter* inputf = query->filter;
	struct berval* base = &(query->base);
	int scope = query->scope;
	int res=0;
	Filter* fs;
	Filter* fi;
	int ret, rc;
	const char* text;

	MatchingRule* mrule = NULL;
	if (inputf != NULL) {
		Debug( LDAP_DEBUG_ANY, "Lock QC index = %d\n",
				template_index, 0, 0 );
		ldap_pvt_thread_rdwr_rlock(&(templa[template_index].t_rwlock));
		for(qc=templa[template_index].query; qc != NULL; qc= qc->next) {
			q = (Query*)qc;
			if(base_scope_compare(&(q->base), base, q->scope, scope)) {
				fi = inputf;
				fs = q->filter;
				do {
					res=0;
					switch (fs->f_choice) {
					case LDAP_FILTER_EQUALITY:
						if (fi->f_choice == LDAP_FILTER_EQUALITY)
							mrule = fs->f_ava->aa_desc->ad_type->sat_equality;
						else
							ret = 1;
						break;
					case LDAP_FILTER_GE:
					case LDAP_FILTER_LE:
						mrule = fs->f_ava->aa_desc->ad_type->sat_ordering;
						break;
					default:
						mrule = NULL; 
					}
					if (mrule) {
						rc = value_match(&ret, fs->f_ava->aa_desc, mrule,
						 	SLAP_MR_VALUE_OF_ASSERTION_SYNTAX,
							&(fi->f_ava->aa_value),
							&(fs->f_ava->aa_value), &text);
						if (rc != LDAP_SUCCESS) {
							ldap_pvt_thread_rdwr_runlock(&(templa[template_index].t_rwlock));
							Debug( LDAP_DEBUG_ANY,
							"Unlock: Exiting QC index=%d\n",
							template_index, 0, 0 );
							return 0;
						}
					}
					switch (fs->f_choice) {
					case LDAP_FILTER_OR:
					case LDAP_FILTER_AND:
						fs = fs->f_and;
						fi = fi->f_and;
						res=1;
						break;
					case LDAP_FILTER_SUBSTRINGS:
						/* check if the equality query can be
						* answered with cached substring query */
						if ((fi->f_choice == LDAP_FILTER_EQUALITY)
							&& substr_containment_equality(
							fs, fi))
							res=1;
						/* check if the substring query can be
						* answered with cached substring query */
						if ((fi->f_choice ==LDAP_FILTER_SUBSTRINGS
							) && substr_containment_substr(
							fs, fi))
							res= 1;
						fs=fs->f_next;
						fi=fi->f_next;
						break;
					case LDAP_FILTER_PRESENT:
						res=1;
						fs=fs->f_next;
						fi=fi->f_next;
						break;
					case LDAP_FILTER_EQUALITY:
						if (ret == 0)
							res = 1;
						fs=fs->f_next;
						fi=fi->f_next;
						break;
					case LDAP_FILTER_GE:
						if (ret >= 0)
							res = 1;
						fs=fs->f_next;
						fi=fi->f_next;
						break;
					case LDAP_FILTER_LE:
						if (ret <= 0)
							res = 1;
						fs=fs->f_next;
						fi=fi->f_next;
						break;
					case LDAP_FILTER_NOT:
						res=0;
						break;
					default:
						break;
					}
				} while((res) && (fi != NULL) && (fs != NULL));

				if(res) {
					ldap_pvt_thread_mutex_lock(&qm->lru_mutex);
					if (qm->lru_top != qc) {
						remove_query(qm, qc);
						add_query_on_top(qm, qc);
					}
					ldap_pvt_thread_mutex_unlock(&qm->lru_mutex);
					return 1;
				}
			}
		}
		Debug( LDAP_DEBUG_ANY,
			"Not answerable: Unlock QC index=%d\n",
			template_index, 0, 0 );
		ldap_pvt_thread_rdwr_runlock(&(templa[template_index].t_rwlock));
	}
	return 0;
}

static void
free_query (CachedQuery* qc)
{
	Query* q = (Query*)qc;
	int i;

	free(qc->q_uuid.bv_val);
	filter_free(q->filter);
	free (q->base.bv_val);
	if ( q->attrs ) {
		for (i=0; q->attrs[i].an_name.bv_val; i++) {
			free(q->attrs[i].an_name.bv_val);
		}
		free(q->attrs);
	}
	free(qc);
}


/* Add query to query cache */
static void add_query(
	query_manager* qm,
	Query* query,
	int template_index,
	struct berval* uuid)
{
	CachedQuery* new_cached_query = (CachedQuery*) ch_malloc(sizeof(CachedQuery));
	QueryTemplate* templ = (qm->templates)+template_index;
	Query* new_query;
	new_cached_query->template_id = template_index;
	new_cached_query->q_uuid = *uuid;
	new_cached_query->lru_up = NULL;
	new_cached_query->lru_down = NULL;
	new_cached_query->expiry_time = slap_get_time() + templ->ttl;
	Debug( LDAP_DEBUG_ANY, "Added query expires at %ld\n",
			(long) new_cached_query->expiry_time, 0, 0 );
	new_query = (Query*)new_cached_query;

	ber_dupbv(&new_query->base, &query->base);
	new_query->scope = query->scope;
	new_query->filter = query->filter;
	new_query->attrs = query->attrs;

	/* Adding a query    */
	Debug( LDAP_DEBUG_ANY, "Lock AQ index = %d\n",
			template_index, 0, 0 );
	ldap_pvt_thread_rdwr_wlock(&templ->t_rwlock);
	if (templ->query == NULL)
		templ->query_last = new_cached_query;
	else
		templ->query->prev = new_cached_query;
	new_cached_query->next = templ->query;
	new_cached_query->prev = NULL;
	templ->query = new_cached_query;
	templ->no_of_queries++;
	Debug( LDAP_DEBUG_ANY, "TEMPLATE %d QUERIES++ %d\n",
			template_index, templ->no_of_queries, 0 );

	Debug( LDAP_DEBUG_ANY, "Unlock AQ index = %d \n",
			template_index, 0, 0 );
	ldap_pvt_thread_rdwr_wunlock(&templ->t_rwlock);

	/* Adding on top of LRU list  */
	ldap_pvt_thread_mutex_lock(&qm->lru_mutex);
	add_query_on_top(qm, new_cached_query);
	ldap_pvt_thread_mutex_unlock(&qm->lru_mutex);
}

static void
remove_from_template (CachedQuery* qc, QueryTemplate* template)
{
	if (!qc->prev && !qc->next) {
		template->query_last = template->query = NULL;
	} else if (qc->prev == NULL) {
		qc->next->prev = NULL;
		template->query = qc->next;
	} else if (qc->next == NULL) {
		qc->prev->next = NULL;
		template->query_last = qc->prev;
	} else {
		qc->next->prev = qc->prev;
		qc->prev->next = qc->next;
	}

	template->no_of_queries--;
}

/* remove bottom query of LRU list from the query cache */
static void cache_replacement(query_manager* qm, struct berval *result)
{
	CachedQuery* bottom;
	int temp_id;

	ldap_pvt_thread_mutex_lock(&qm->lru_mutex);
	bottom = qm->lru_bottom;

	result->bv_val = NULL;
	result->bv_len = 0;

	if (!bottom) {
		Debug ( LDAP_DEBUG_ANY,
			"Cache replacement invoked without "
			"any query in LRU list\n", 0, 0, 0 );
		return;
	}

	temp_id = bottom->template_id;
	remove_query(qm, bottom);
	ldap_pvt_thread_mutex_unlock(&qm->lru_mutex);

	*result = bottom->q_uuid;
	bottom->q_uuid.bv_val = NULL;

	Debug( LDAP_DEBUG_ANY, "Lock CR index = %d\n", temp_id, 0, 0 );
	ldap_pvt_thread_rdwr_wlock(&(qm->templates[temp_id].t_rwlock));
	remove_from_template(bottom, (qm->templates+temp_id));
	Debug( LDAP_DEBUG_ANY, "TEMPLATE %d QUERIES-- %d\n",
		temp_id, qm->templates[temp_id].no_of_queries, 0 );
	Debug( LDAP_DEBUG_ANY, "Unlock CR index = %d\n", temp_id, 0, 0 );
	ldap_pvt_thread_rdwr_wunlock(&(qm->templates[temp_id].t_rwlock));
	free_query(bottom);
}

struct query_info {
	struct query_info *next;
	struct berval xdn;
	int del;
};

static int
remove_func (
	Operation	*op,
	SlapReply	*rs
)
{
	Attribute *attr;
	struct query_info *qi;
	int count = 0;

	if ( rs->sr_type != REP_SEARCH ) return 0;

	for (attr = rs->sr_entry->e_attrs; attr!= NULL; attr = attr->a_next) {
		if (attr->a_desc == ad_queryid) {
			for (count=0; attr->a_vals[count].bv_val; count++)
				;
			break;
		}
	}
	if ( count == 0 ) return 0;
	qi = op->o_tmpalloc( sizeof( struct query_info ), op->o_tmpmemctx );
	qi->next = op->o_callback->sc_private;
	op->o_callback->sc_private = qi;
	ber_dupbv_x( &qi->xdn, &rs->sr_entry->e_nname, op->o_tmpmemctx );
	qi->del = ( count == 1 );

	return 0;
}

static int
remove_query_data (
	Operation	*op,
	SlapReply	*rs,
	struct berval* query_uuid)
{
	struct query_info	*qi, *qnext;
	char			filter_str[64];
#ifdef LDAP_COMP_MATCH
	AttributeAssertion	ava = { NULL, BER_BVNULL, NULL };
#else
	AttributeAssertion	ava = { NULL, BER_BVNULL };
#endif
	Filter			filter = {LDAP_FILTER_EQUALITY};
	SlapReply 		sreply = {REP_RESULT};
	slap_callback cb = { NULL, remove_func, NULL, NULL };
	int deleted = 0;

	sreply.sr_entry = NULL;
	sreply.sr_nentries = 0;
	op->ors_filterstr.bv_len = snprintf(filter_str, sizeof(filter_str),
		"(%s=%s)", ad_queryid->ad_cname.bv_val, query_uuid->bv_val);
	filter.f_ava = &ava;
	filter.f_av_desc = ad_queryid;
	filter.f_av_value = *query_uuid;

	op->o_tag = LDAP_REQ_SEARCH;
	op->o_protocol = LDAP_VERSION3;
	op->o_callback = &cb;
	op->o_time = slap_get_time();
	op->o_do_not_cache = 1;

	op->o_req_dn = op->o_bd->be_suffix[0];
	op->o_req_ndn = op->o_bd->be_nsuffix[0];
	op->ors_scope = LDAP_SCOPE_SUBTREE;
	op->ors_deref = LDAP_DEREF_NEVER;
	op->ors_slimit = SLAP_NO_LIMIT;
	op->ors_tlimit = SLAP_NO_LIMIT;
	op->ors_filter = &filter;
	op->ors_filterstr.bv_val = filter_str;
	op->ors_filterstr.bv_len = strlen(filter_str);
	op->ors_attrs = NULL;
	op->ors_attrsonly = 0;

	op->o_bd->be_search( op, &sreply );

	for ( qi=cb.sc_private; qi; qi=qnext ) {
		qnext = qi->next;

		op->o_req_dn = qi->xdn;
		op->o_req_ndn = qi->xdn;

		if ( qi->del) {
			Debug( LDAP_DEBUG_ANY, "DELETING ENTRY TEMPLATE=%s\n",
				query_uuid->bv_val, 0, 0 );

			op->o_tag = LDAP_REQ_DELETE;

			if (op->o_bd->be_delete(op, &sreply) == LDAP_SUCCESS) {
				deleted++;
			}
		} else {
			Modifications mod;
			struct berval vals[2];

			vals[0] = *query_uuid;
			vals[1].bv_val = NULL;
			vals[1].bv_len = 0;
			mod.sml_op = LDAP_MOD_DELETE;
			mod.sml_flags = 0;
			mod.sml_desc = ad_queryid;
			mod.sml_type = ad_queryid->ad_cname;
			mod.sml_values = vals;
			mod.sml_nvalues = NULL;
			mod.sml_next = NULL;
			Debug( LDAP_DEBUG_ANY,
				"REMOVING TEMP ATTR : TEMPLATE=%s\n",
				query_uuid->bv_val, 0, 0 );

			op->orm_modlist = &mod;

			op->o_bd->be_modify( op, &sreply );
		}
		op->o_tmpfree( qi->xdn.bv_val, op->o_tmpmemctx );
		op->o_tmpfree( qi, op->o_tmpmemctx );
	}
	return deleted;
}

static int
get_attr_set(
	AttributeName* attrs,
	query_manager* qm,
	int num
);

static int
attrscmp(
	AttributeName* attrs_in,
	AttributeName* attrs
);

static int
is_temp_answerable(
	int attr_set,
	struct berval* tempstr,
	query_manager* qm,
	int template_id )
{
	QueryTemplate *qt = qm->templates + template_id;

	if (attr_set != qt->attr_set_index) {
		int* id_array = qm->attr_sets[attr_set].ID_array;

		while (*id_array != -1) {
			if (*id_array == qt->attr_set_index)
				break;
			id_array++;
		}
		if (*id_array == -1)
			return 0;
	}
	return (qt->querystr.bv_len == tempstr->bv_len &&
		strcasecmp(qt->querystr.bv_val, tempstr->bv_val) == 0);
}

static int
filter2template(
	Filter			*f,
	struct			berval *fstr,
	AttributeName**		filter_attrs,
	int*			filter_cnt )
{
	AttributeDescription *ad;

	switch ( f->f_choice ) {
	case LDAP_FILTER_EQUALITY:
		ad = f->f_av_desc;
		sprintf( fstr->bv_val+fstr->bv_len, "(%s=)", ad->ad_cname.bv_val );
		fstr->bv_len += ad->ad_cname.bv_len + ( sizeof("(=)") - 1 );
		break;

	case LDAP_FILTER_GE:
		ad = f->f_av_desc;
		sprintf( fstr->bv_val+fstr->bv_len, "(%s>=)", ad->ad_cname.bv_val);
		fstr->bv_len += ad->ad_cname.bv_len + ( sizeof("(>=)") - 1 );
		break;

	case LDAP_FILTER_LE:
		ad = f->f_av_desc;
		sprintf( fstr->bv_val+fstr->bv_len, "(%s<=)", ad->ad_cname.bv_val);
		fstr->bv_len += ad->ad_cname.bv_len + ( sizeof("(<=)") - 1 );
		break;

	case LDAP_FILTER_APPROX:
		ad = f->f_av_desc;
		sprintf( fstr->bv_val+fstr->bv_len, "(%s~=)", ad->ad_cname.bv_val);
		fstr->bv_len += ad->ad_cname.bv_len + ( sizeof("(~=)") - 1 );
		break;

	case LDAP_FILTER_SUBSTRINGS:
		ad = f->f_sub_desc;
		sprintf( fstr->bv_val+fstr->bv_len, "(%s=)", ad->ad_cname.bv_val );
		fstr->bv_len += ad->ad_cname.bv_len + ( sizeof("(=)") - 1 );
		break;

	case LDAP_FILTER_PRESENT:
		ad = f->f_desc;
		sprintf( fstr->bv_val+fstr->bv_len, "(%s=*)", ad->ad_cname.bv_val );
		fstr->bv_len += ad->ad_cname.bv_len + ( sizeof("(=*)") - 1 );
		break;

	case LDAP_FILTER_AND:
	case LDAP_FILTER_OR:
	case LDAP_FILTER_NOT: {
		int rc = 0;
		sprintf( fstr->bv_val+fstr->bv_len, "(%c",
			f->f_choice == LDAP_FILTER_AND ? '&' :
			f->f_choice == LDAP_FILTER_OR ? '|' : '!' );
		fstr->bv_len += sizeof("(%") - 1;

		for ( f = f->f_list; f != NULL; f = f->f_next ) {
			rc = filter2template( f, fstr, filter_attrs, filter_cnt );
			if ( rc ) break;
		}
		sprintf( fstr->bv_val+fstr->bv_len, ")" );
		fstr->bv_len += sizeof(")") - 1;

		return rc;
		}

	default:
		strcpy( fstr->bv_val, "(?=?)" );
		fstr->bv_len += sizeof("(?=?)")-1;
		return -1;
	}

	*filter_attrs = (AttributeName *)ch_realloc(*filter_attrs,
				(*filter_cnt + 2)*sizeof(AttributeName));

	(*filter_attrs)[*filter_cnt].an_desc = ad;
	(*filter_attrs)[*filter_cnt].an_name = ad->ad_cname;
	(*filter_attrs)[*filter_cnt].an_oc = NULL;
	(*filter_attrs)[*filter_cnt].an_oc_exclude = 0;
	BER_BVZERO( &(*filter_attrs)[*filter_cnt+1].an_name );
	(*filter_cnt)++;
	return 0;
}

struct search_info {
	slap_overinst *on;
	Query query;
	int template_id;
	int max;
	int over;
	int count;
	Entry *head, *tail;
};

static int
cache_entries(
	Operation	*op,
	SlapReply	*rs,
	struct berval *query_uuid)
{
	struct search_info *si = op->o_callback->sc_private;
	slap_overinst *on = si->on;
	cache_manager *cm = on->on_bi.bi_private;
	query_manager*		qm = cm->qm;
	int		return_val = 0;
	Entry		*e;
	struct berval	crp_uuid;
	char		uuidbuf[ LDAP_LUTIL_UUIDSTR_BUFSIZE ];
	Operation op_tmp = *op;

	query_uuid->bv_len = lutil_uuidstr(uuidbuf, sizeof(uuidbuf));
	ber_str2bv(uuidbuf, query_uuid->bv_len, 1, query_uuid);

	op_tmp.o_bd = &cm->db;
	op_tmp.o_dn = cm->db.be_rootdn;
	op_tmp.o_ndn = cm->db.be_rootndn;

	Debug( LDAP_DEBUG_ANY, "UUID for query being added = %s\n",
			uuidbuf, 0, 0 );

	for ( e=si->head; e; e=si->head ) {
		si->head = e->e_private;
		e->e_private = NULL;
		Debug( LDAP_DEBUG_NONE, "LOCKING REMOVE MUTEX\n", 0, 0, 0 );
		ldap_pvt_thread_mutex_lock(&cm->remove_mutex);
		Debug( LDAP_DEBUG_NONE, "LOCKED REMOVE MUTEX\n", 0, 0, 0);
		while ( cm->cur_entries > (cm->max_entries) ) {
				qm->crfunc(qm, &crp_uuid);
				if (crp_uuid.bv_val) {
					Debug( LDAP_DEBUG_ANY,
						"Removing query UUID %s\n",
						crp_uuid.bv_val, 0, 0 );
					return_val = remove_query_data(&op_tmp, rs, &crp_uuid);
					Debug( LDAP_DEBUG_ANY,
						"QUERY REMOVED, SIZE=%d\n",
						return_val, 0, 0);
					ldap_pvt_thread_mutex_lock(
							&cm->cache_mutex );
					cm->cur_entries -= return_val;
					cm->num_cached_queries--;
					Debug( LDAP_DEBUG_ANY,
						"STORED QUERIES = %lu\n",
						cm->num_cached_queries, 0, 0 );
					ldap_pvt_thread_mutex_unlock(
							&cm->cache_mutex );
					Debug( LDAP_DEBUG_ANY,
						"QUERY REMOVED, CACHE ="
						"%d entries\n",
						cm->cur_entries, 0, 0 );
				}
		}

		return_val = merge_entry(&op_tmp, e, query_uuid);
		ldap_pvt_thread_mutex_unlock(&cm->remove_mutex);
		ldap_pvt_thread_mutex_lock(&cm->cache_mutex);
		cm->cur_entries += return_val;
		Debug( LDAP_DEBUG_ANY,
			"ENTRY ADDED/MERGED, CACHED ENTRIES=%d\n",
			cm->cur_entries, 0, 0 );
		return_val = 0;
		ldap_pvt_thread_mutex_unlock(&cm->cache_mutex);
	}
	ldap_pvt_thread_mutex_lock(&cm->cache_mutex);
	cm->num_cached_queries++;
	Debug( LDAP_DEBUG_ANY, "STORED QUERIES = %lu\n",
			cm->num_cached_queries, 0, 0 );
	ldap_pvt_thread_mutex_unlock(&cm->cache_mutex);

	return return_val;
}

static int
proxy_cache_response(
	Operation	*op,
	SlapReply	*rs )
{
	struct search_info *si = op->o_callback->sc_private;
	slap_overinst *on = si->on;
	cache_manager *cm = on->on_bi.bi_private;
	query_manager*		qm = cm->qm;
	struct berval uuid;

	if ( rs->sr_type == REP_SEARCH ) {
		Entry *e;
		/* If we haven't exceeded the limit for this query,
		 * build a chain of answers to store. If we hit the
		 * limit, empty the chain and ignore the rest.
		 */
		if ( !si->over ) {
			if ( si->count < si->max ) {
				si->count++;
				e = entry_dup( rs->sr_entry );
				if ( !si->head ) si->head = e;
				if ( si->tail ) si->tail->e_private = e;
				si->tail = e;
			} else {
				si->over = 1;
				si->count = 0;
				for (;si->head; si->head=e) {
					e = si->head->e_private;
					si->head->e_private = NULL;
					entry_free(si->head);
				}
				si->tail = NULL;
			}
		}

		if (rs->sr_attrs != op->ors_attrs ) {
			op->o_tmpfree( rs->sr_attrs, op->o_tmpmemctx );
		}
		rs->sr_attrs = si->query.save_attrs;
		op->o_tmpfree( op->ors_attrs, op->o_tmpmemctx );
		op->ors_attrs = si->query.save_attrs;
		si->query.save_attrs = NULL;

	} else if ( rs->sr_type == REP_RESULT ) {
		if ( si->count && cache_entries( op, rs, &uuid ) == 0 ) {
			qm->addfunc(qm, &si->query, si->template_id, &uuid);
			/* If the consistency checker suspended itself,
			 * wake it back up
			 */
			if ( cm->cc_paused ) {
				ldap_pvt_thread_mutex_lock( &slapd_rq.rq_mutex );
				if ( cm->cc_paused ) {
					cm->cc_paused = 0;
					ldap_pvt_runqueue_resched( &slapd_rq, cm->cc_arg, 0 );
				}
				ldap_pvt_thread_mutex_unlock( &slapd_rq.rq_mutex );
			}
		}

		/* free self */
		op->o_callback->sc_cleanup = slap_freeself_cb;
	}
	return SLAP_CB_CONTINUE;
}

static void
add_filter_attrs(
	Operation *op,
	AttributeName** new_attrs,
	AttributeName* attrs,
	AttributeName* filter_attrs )
{
	int alluser = 0;
	int allop = 0;
	int i;
	int count;

	/* duplicate attrs */
        if (attrs == NULL) {
		count = 1;
	} else {
		for (count=0; attrs[count].an_name.bv_val; count++)
			;
	}
	*new_attrs = (AttributeName*)(op->o_tmpalloc((count+1)*
		sizeof(AttributeName), op->o_tmpmemctx));
	if (attrs == NULL) {
		BER_BVSTR( &(*new_attrs)[0].an_name, "*" );
		(*new_attrs)[0].an_desc = NULL;
		(*new_attrs)[0].an_oc = NULL;
		(*new_attrs)[0].an_oc_exclude = 0;
		BER_BVZERO( &(*new_attrs)[1].an_name );
		alluser = 1;
		allop = 0;
	} else {
		for (i=0; i<count; i++) {
			(*new_attrs)[i].an_name = attrs[i].an_name;
			(*new_attrs)[i].an_desc = attrs[i].an_desc;
		}
		BER_BVZERO( &(*new_attrs)[count].an_name );
		alluser = an_find(*new_attrs, &AllUser);
		allop = an_find(*new_attrs, &AllOper);
	}

	for ( i=0; filter_attrs[i].an_name.bv_val; i++ ) {
		if ( an_find(*new_attrs, &filter_attrs[i].an_name ))
			continue;
		if ( is_at_operational(filter_attrs[i].an_desc->ad_type) ) {
			if (allop)
				continue;
		} else if (alluser)
			continue;
		*new_attrs = (AttributeName*)(op->o_tmprealloc(*new_attrs,
					(count+2)*sizeof(AttributeName), op->o_tmpmemctx));
		(*new_attrs)[count].an_name = filter_attrs[i].an_name;
		(*new_attrs)[count].an_desc = filter_attrs[i].an_desc;
		(*new_attrs)[count].an_oc = NULL;
		(*new_attrs)[count].an_oc_exclude = 0;
		count++;
		BER_BVZERO( &(*new_attrs)[count].an_name );
	}
}

/* NOTE: this is a quick workaround to let pcache minimally interact
 * with pagedResults.  A more articulated solutions would be to
 * perform the remote query without control and cache all results,
 * performing the pagedResults search only within the client
 * and the proxy.  This requires pcache to understand pagedResults. */
static int
proxy_cache_chk_controls(
	Operation	*op,
	SlapReply	*rs )
{
	const char	*non = "";
	const char	*stripped = "";

	switch( op->o_pagedresults ) {
	case SLAP_CONTROL_NONCRITICAL:
		non = "non-";
		stripped = "; stripped";
		/* fallthru */

	case SLAP_CONTROL_CRITICAL:
		Debug( LDAP_DEBUG_ANY, "%s: "
			"%scritical pagedResults control "
			"disabled with proxy cache%s.\n",
			op->o_log_prefix, non, stripped );
		
		slap_remove_control( op, rs, slap_cids.sc_pagedResults, NULL );
		break;

	default:
		rs->sr_err = SLAP_CB_CONTINUE;
		break;
	}

	return rs->sr_err;
}

static int
proxy_cache_search(
	Operation	*op,
	SlapReply	*rs )
{
	slap_overinst *on = (slap_overinst *)op->o_bd->bd_info;
	cache_manager *cm = on->on_bi.bi_private;
	query_manager*		qm = cm->qm;

	int count;

	int i = -1;

	AttributeName	*filter_attrs = NULL;

	Query		query;

	int 		attr_set = -1;
	int 		template_id = -1;
	int 		answerable = 0;
	int 		cacheable = 0;
	int		fattr_cnt=0;
	int		oc_attr_absent = 1;

	struct berval tempstr;

	tempstr.bv_val = op->o_tmpalloc( op->ors_filterstr.bv_len+1, op->o_tmpmemctx );
	tempstr.bv_len = 0;
	if (filter2template(op->ors_filter, &tempstr, &filter_attrs, &fattr_cnt)) {
		op->o_tmpfree( tempstr.bv_val, op->o_tmpmemctx );
		return SLAP_CB_CONTINUE;
	}

	Debug( LDAP_DEBUG_ANY, "query template of incoming query = %s\n",
					tempstr.bv_val, 0, 0 );

	/* FIXME: cannot cache/answer requests with pagedResults control */
	

	/* find attr set */
	attr_set = get_attr_set(op->ors_attrs, qm, cm->numattrsets);

	query.filter = op->ors_filter;
	query.attrs = op->ors_attrs;
	query.save_attrs = NULL;
	query.base = op->o_req_ndn;
	query.scope = op->ors_scope;

	/* check for query containment */
	if (attr_set > -1) {
		for (i=0; i<cm->numtemplates; i++) {
			/* find if template i can potentially answer tempstr */
			if (!is_temp_answerable(attr_set, &tempstr, qm, i))
				continue;
			if (attr_set == qm->templates[i].attr_set_index) {
				cacheable = 1;
				template_id = i;
			}
			Debug( LDAP_DEBUG_NONE, "Entering QC, querystr = %s\n",
			 		op->ors_filterstr.bv_val, 0, 0 );
			answerable = (*(qm->qcfunc))(qm, &query, i);

			if (answerable)
				break;
		}
	}
	op->o_tmpfree( tempstr.bv_val, op->o_tmpmemctx );

	query.save_attrs = op->ors_attrs;
	query.attrs = NULL;

	if (answerable) {
		/* Need to clear the callbacks of the original operation,
		 * in case there are other overlays */
		BackendDB	*save_bd = op->o_bd;
		slap_callback	*save_cb = op->o_callback;

		Debug( LDAP_DEBUG_ANY, "QUERY ANSWERABLE\n", 0, 0, 0 );
		free(filter_attrs);
		ldap_pvt_thread_rdwr_runlock(&qm->templates[i].t_rwlock);
		op->o_bd = &cm->db;
		op->o_callback = NULL;
		i = cm->db.bd_info->bi_op_search( op, rs );
		op->o_bd = save_bd;
		op->o_callback = save_cb;
		return i;
	}

	Debug( LDAP_DEBUG_ANY, "QUERY NOT ANSWERABLE\n", 0, 0, 0 );

	ldap_pvt_thread_mutex_lock(&cm->cache_mutex);
	if (cm->num_cached_queries >= cm->max_queries) {
		cacheable = 0;
	}
	ldap_pvt_thread_mutex_unlock(&cm->cache_mutex);

	if (cacheable) {
		slap_callback		*cb;
		struct search_info	*si;

		Debug( LDAP_DEBUG_ANY, "QUERY CACHEABLE\n", 0, 0, 0 );
		query.filter = str2filter(op->ors_filterstr.bv_val);
		if (op->ors_attrs) {
			for ( count = 0; !BER_BVISNULL( &op->ors_attrs[ count ].an_name ); count++ ) {
				if ( op->ors_attrs[count].an_desc == slap_schema.si_ad_objectClass ) {
					oc_attr_absent = 0;
				}
			}
			query.attrs = (AttributeName *)ch_malloc( ( count + 1 + oc_attr_absent )
									*sizeof(AttributeName) );
			for ( count = 0; !BER_BVISNULL( &op->ors_attrs[ count ].an_name ); count++ ) {
				ber_dupbv( &query.attrs[count].an_name, &op->ors_attrs[count].an_name );
				query.attrs[count].an_desc = op->ors_attrs[count].an_desc;
				query.attrs[count].an_oc = op->ors_attrs[count].an_oc;
				query.attrs[count].an_oc_exclude = op->ors_attrs[count].an_oc_exclude;
			}
			if ( oc_attr_absent ) {
				query.attrs[ count ].an_desc = slap_schema.si_ad_objectClass;
				ber_dupbv( &query.attrs[count].an_name,
					&slap_schema.si_ad_objectClass->ad_cname );
				query.attrs[ count ].an_oc = NULL;
				query.attrs[ count ].an_oc_exclude = 0;
				count++;
			}
			BER_BVZERO( &query.attrs[ count ].an_name );
		}
		add_filter_attrs(op, &op->ors_attrs, query.attrs, filter_attrs);

		cb = op->o_tmpalloc( sizeof(*cb) + sizeof(*si), op->o_tmpmemctx);
		cb->sc_response = proxy_cache_response;
		cb->sc_cleanup = NULL;
		cb->sc_private = (cb+1);
		si = cb->sc_private;
		si->on = on;
		si->query = query;
		si->template_id = template_id;
		si->max = cm->num_entries_limit ;
		si->over = 0;
		si->count = 0;
		si->head = NULL;
		si->tail = NULL;

		if ( cm->response_cb == PCACHE_RESPONSE_CB_HEAD ) {
			cb->sc_next = op->o_callback;
			op->o_callback = cb;

		} else {
			slap_callback		**pcb;

			/* need to move the callback at the end, in case other
			 * overlays are present, so that the final entry is
			 * actually cached */
			cb->sc_next = NULL;
			for ( pcb = &op->o_callback; *pcb; pcb = &(*pcb)->sc_next );
			*pcb = cb;
		}

	} else {
		Debug( LDAP_DEBUG_ANY, "QUERY NOT CACHEABLE\n",
					0, 0, 0);
	}

	free(filter_attrs);

	return SLAP_CB_CONTINUE;
}

static int
attrscmp(
	AttributeName* attrs_in,
	AttributeName* attrs)
{
	int i, count1, count2;
	if ( attrs_in == NULL ) {
		return (attrs ? 0 : 1);
	}
	if ( attrs == NULL )
		return 0;

	for ( count1=0;
	      attrs_in && attrs_in[count1].an_name.bv_val != NULL;
	      count1++ )
		;
	for ( count2=0;
	      attrs && attrs[count2].an_name.bv_val != NULL;
	      count2++)
		;
	if ( count1 != count2 )
		return 0;

	for ( i=0; i<count1; i++ ) {
		if ( !an_find(attrs, &attrs_in[i].an_name ))
			return 0;
	}
	return 1;
}

static int
get_attr_set(
	AttributeName* attrs,
	query_manager* qm,
	int num )
{
	int i;
	for (i=0; i<num; i++) {
		if (attrscmp(attrs, qm->attr_sets[i].attrs))
			return i;
	}
	return -1;
}

static void*
consistency_check(
	void *ctx,
	void *arg )
{
	struct re_s *rtask = arg;
	slap_overinst *on = rtask->arg;
	cache_manager *cm = on->on_bi.bi_private;
	query_manager *qm = cm->qm;
	Connection conn = {0};
	OperationBuffer opbuf;
	Operation *op;

	SlapReply rs = {REP_RESULT};
	CachedQuery* query, *query_prev;
	int i, return_val, pause = 1;
	QueryTemplate* templ;

	op = (Operation *) &opbuf;
	connection_fake_init( &conn, op, ctx );

	op->o_bd = &cm->db;
	op->o_dn = cm->db.be_rootdn;
	op->o_ndn = cm->db.be_rootndn;

      	cm->cc_arg = arg;

	for (i=0; qm->templates[i].querystr.bv_val; i++) {
		templ = qm->templates + i;
		query = templ->query_last;
		if ( query ) pause = 0;
		op->o_time = slap_get_time();
		ldap_pvt_thread_mutex_lock(&cm->remove_mutex);
		while (query && (query->expiry_time < op->o_time)) {
			ldap_pvt_thread_mutex_lock(&qm->lru_mutex);
			remove_query(qm, query);
			ldap_pvt_thread_mutex_unlock(&qm->lru_mutex);
			Debug( LDAP_DEBUG_ANY, "Lock CR index = %d\n",
					i, 0, 0 );
			ldap_pvt_thread_rdwr_wlock(&templ->t_rwlock);
			remove_from_template(query, templ);
			Debug( LDAP_DEBUG_ANY, "TEMPLATE %d QUERIES-- %d\n",
					i, templ->no_of_queries, 0 );
			Debug( LDAP_DEBUG_ANY, "Unlock CR index = %d\n",
					i, 0, 0 );
			ldap_pvt_thread_rdwr_wunlock(&templ->t_rwlock);
			return_val = remove_query_data(op, &rs, &query->q_uuid);
			Debug( LDAP_DEBUG_ANY, "STALE QUERY REMOVED, SIZE=%d\n",
						return_val, 0, 0 );
			ldap_pvt_thread_mutex_lock(&cm->cache_mutex);
			cm->cur_entries -= return_val;
			cm->num_cached_queries--;
			Debug( LDAP_DEBUG_ANY, "STORED QUERIES = %lu\n",
					cm->num_cached_queries, 0, 0 );
			ldap_pvt_thread_mutex_unlock(&cm->cache_mutex);
			Debug( LDAP_DEBUG_ANY,
				"STALE QUERY REMOVED, CACHE ="
				"%d entries\n",
				cm->cur_entries, 0, 0 );
			query_prev = query;
			query = query->prev;
			free_query(query_prev);
		}
		ldap_pvt_thread_mutex_unlock(&cm->remove_mutex);
	}
	ldap_pvt_thread_mutex_lock( &slapd_rq.rq_mutex );
	if ( ldap_pvt_runqueue_isrunning( &slapd_rq, rtask )) {
		ldap_pvt_runqueue_stoptask( &slapd_rq, rtask );
	}
	/* If there were no queries, defer processing for a while */
	cm->cc_paused = pause;
	ldap_pvt_runqueue_resched( &slapd_rq, rtask, pause );

	ldap_pvt_thread_mutex_unlock( &slapd_rq.rq_mutex );
	return NULL;
}


#define MAX_ATTR_SETS 500

/*
 * compares two sets of attributes (indices i and j)
 * returns 0: if neither set is contained in the other set
 *         1: if set i is contained in set j
 *         2: if set j is contained in set i
 *         3: the sets are equivalent
 */

static int
compare_sets(struct attr_set* set, int i, int j)
{
	int k,l,numI,numJ;
	int common=0;
	int result=0;

	if (( set[i].attrs == NULL ) && ( set[j].attrs == NULL ))
		return 3;

	if ( set[i].attrs == NULL )
		return 2;

	if ( set[j].attrs == NULL )
		return 1;

	numI = set[i].count;
	numJ = set[j].count;

	for ( l=0; l < numI; l++ ) {
		for ( k = 0; k < numJ; k++ ) {
			if ( strcmp( set[i].attrs[l].an_name.bv_val,
				     set[j].attrs[k].an_name.bv_val ) == 0 )
				common++;
		}
	}

	if ( common == numI )
		result = 1;

	if ( common == numJ )
		result += 2;

	return result;
}

static void
find_supersets ( struct attr_set* attr_sets, int numsets )
{
	int num[MAX_ATTR_SETS];
	int i, j, res;
	int* id_array;
	for ( i = 0; i < MAX_ATTR_SETS; i++ )
		num[i] = 0;

	for ( i = 0; i < numsets; i++ ) {
		attr_sets[i].ID_array = (int*) ch_malloc( sizeof( int ) );
		attr_sets[i].ID_array[0] = -1;
    	}

	for ( i = 0; i < numsets; i++ ) {
		for ( j=i+1; j < numsets; j++ ) {
			res = compare_sets( attr_sets, i, j );
			switch ( res ) {
			case 0:
				break;
			case 3:
			case 1:
				id_array = attr_sets[i].ID_array;
				attr_sets[i].ID_array = (int *) ch_realloc( id_array,
							( num[i] + 2 ) * sizeof( int ));
				attr_sets[i].ID_array[num[i]] = j;
				attr_sets[i].ID_array[num[i]+1] = -1;
				num[i]++;
				if (res == 1)
					break;
			case 2:
				id_array = attr_sets[j].ID_array;
				attr_sets[j].ID_array = (int *) ch_realloc( id_array,
						( num[j] + 2 ) * sizeof( int ));
				attr_sets[j].ID_array[num[j]] = i;
				attr_sets[j].ID_array[num[j]+1] = -1;
				num[j]++;
				break;
			}
		}
	}
}

enum {
	PC_MAIN = 1,
	PC_ATTR,
	PC_TEMP,
	PC_RESP
};

static ConfigDriver pc_cf_gen;
static ConfigLDAPadd pc_ldadd;
static ConfigCfAdd pc_cfadd;

static ConfigTable pccfg[] = {
	{ "proxycache", "backend> <max_entries> <numattrsets> <entry limit> "
				"<cycle_time",
		6, 6, 0, ARG_MAGIC|ARG_NO_DELETE|PC_MAIN, pc_cf_gen,
		"( OLcfgOvAt:2.1 NAME 'olcProxyCache' "
			"DESC 'ProxyCache basic parameters' "
			"SYNTAX OMsDirectoryString SINGLE-VALUE )", NULL, NULL },
	{ "proxyattrset", "index> <attributes...",
		2, 0, 0, ARG_MAGIC|PC_ATTR, pc_cf_gen,
		"( OLcfgOvAt:2.2 NAME 'olcProxyAttrset' "
			"DESC 'A set of attributes to cache' "
			"SYNTAX OMsDirectoryString )", NULL, NULL },
	{ "proxytemplate", "filter> <attrset-index> <TTL",
		4, 4, 0, ARG_MAGIC|PC_TEMP, pc_cf_gen,
		"( OLcfgOvAt:2.3 NAME 'olcProxyTemplate' "
			"DESC 'Filter template, attrset, and cache TTL' "
			"SYNTAX OMsDirectoryString )", NULL, NULL },
	{ "response-callback", "head|tail(default)",
		2, 2, 0, ARG_MAGIC|PC_RESP, pc_cf_gen,
		"( OLcfgOvAt:2.4 NAME 'olcProxyResponseCB' "
			"DESC 'Response callback position in overlay stack' "
			"SYNTAX OMsDirectoryString )", NULL, NULL },
	{ NULL, NULL, 0, 0, 0, ARG_IGNORED }
};

static ConfigOCs pcocs[] = {
	{ "( OLcfgOvOc:2.1 "
		"NAME 'olcPcacheConfig' "
		"DESC 'ProxyCache configuration' "
		"SUP olcOverlayConfig "
		"MUST ( olcProxyCache $ olcProxyAttrset $ olcProxyTemplate ) "
		"MAY olcProxyResponseCB )", Cft_Overlay, pccfg, NULL, pc_cfadd },
	{ "( OLcfgOvOc:2.2 "
		"NAME 'olcPcacheDatabase' "
		"DESC 'Cache database configuration' "
		"AUXILIARY )", Cft_Misc, pccfg, pc_ldadd },
	{ NULL, 0, NULL }
};

static int
pc_ldadd( CfEntryInfo *p, Entry *e, ConfigArgs *ca )
{
	slap_overinst *on;
	cache_manager *cm;

	if ( p->ce_type != Cft_Overlay || !p->ce_bi ||
		p->ce_bi->bi_cf_ocs != pcocs )
		return LDAP_CONSTRAINT_VIOLATION;

	on = (slap_overinst *)p->ce_bi;
	cm = on->on_bi.bi_private;
	ca->be = &cm->db;
	return LDAP_SUCCESS;
}

static int
pc_cfadd( Operation *op, SlapReply *rs, Entry *p, ConfigArgs *ca )
{
	CfEntryInfo *pe = p->e_private;
	slap_overinst *on = (slap_overinst *)pe->ce_bi;
	cache_manager *cm = on->on_bi.bi_private;
	struct berval bv;

	/* FIXME: should not hardcode "olcDatabase" here */
	bv.bv_len = sprintf( ca->msg, "olcDatabase=%s", cm->db.bd_info->bi_type );
	bv.bv_val = ca->msg;
	ca->be = &cm->db;

	/* We can only create this entry if the database is table-driven
	 */
	if ( cm->db.bd_info->bi_cf_ocs )
		config_build_entry( op, rs, pe, ca, &bv, cm->db.bd_info->bi_cf_ocs,
			&pcocs[1] );

	return 0;
}

static int
pc_cf_gen( ConfigArgs *c )
{
	slap_overinst *on = (slap_overinst *)c->bi;
	cache_manager* 	cm = on->on_bi.bi_private;
	query_manager*  qm = cm->qm;
	QueryTemplate* 	temp;
	AttributeName*  attr_name;
	AttributeName* 	attrarray;
	const char* 	text=NULL;
	int i, num, rc = 0;
	char *ptr;

	if ( c->op == SLAP_CONFIG_EMIT ) {
		struct berval bv;
		switch( c->type ) {
		case PC_MAIN:
			bv.bv_len = sprintf( c->msg, "%s %d %d %d %d",
				cm->db.bd_info->bi_type, cm->max_entries, cm->numattrsets,
				cm->num_entries_limit, cm->cc_period );
			bv.bv_val = c->msg;
			value_add_one( &c->rvalue_vals, &bv );
			break;
		case PC_ATTR:
			for (i=0; i<cm->numattrsets; i++) {
				if ( !qm->attr_sets[i].count ) continue;

				bv.bv_len = sprintf( c->msg, "%d", i );

				/* count the attr length */
				for ( attr_name = qm->attr_sets[i].attrs;
					attr_name->an_name.bv_val; attr_name++ )
					bv.bv_len += attr_name->an_name.bv_len + 1;

				bv.bv_val = ch_malloc( bv.bv_len+1 );
				ptr = lutil_strcopy( bv.bv_val, c->msg );
				for ( attr_name = qm->attr_sets[i].attrs;
					attr_name->an_name.bv_val; attr_name++ ) {
					*ptr++ = ' ';
					ptr = lutil_strcopy( ptr, attr_name->an_name.bv_val );
				}
				ber_bvarray_add( &c->rvalue_vals, &bv );
			}
			if ( !c->rvalue_vals )
				rc = 1;
			break;
		case PC_TEMP:
			for (i=0; i<cm->numtemplates; i++) {
				bv.bv_len = sprintf( c->msg, " %d %ld",
					qm->templates[i].attr_set_index,
					qm->templates[i].ttl );
				bv.bv_len += qm->templates[i].querystr.bv_len + 2;
				bv.bv_val = ch_malloc( bv.bv_len+1 );
				ptr = bv.bv_val;
				*ptr++ = '"';
				ptr = lutil_strcopy( ptr, qm->templates[i].querystr.bv_val );
				*ptr++ = '"';
				strcpy( ptr, c->msg );
				ber_bvarray_add( &c->rvalue_vals, &bv );
			}
			if ( !c->rvalue_vals )
				rc = 1;
			break;
		case PC_RESP:
			if ( cm->response_cb == PCACHE_RESPONSE_CB_HEAD ) {
				BER_BVSTR( &bv, "head" );
			} else {
				BER_BVSTR( &bv, "tail" );
			}
			value_add_one( &c->rvalue_vals, &bv );
			break;
		}
		return rc;
	} else if ( c->op == LDAP_MOD_DELETE ) {
		return 1;	/* FIXME */
#if 0
		switch( c->type ) {
		case PC_ATTR:
		case PC_TEMP:
		}
		return rc;
#endif
	}

	switch( c->type ) {
	case PC_MAIN:
		cm->numattrsets = atoi( c->argv[3] );
		if ( cm->numattrsets > MAX_ATTR_SETS ) {
			sprintf( c->msg, "numattrsets must be <= %d", MAX_ATTR_SETS );
			Debug( LDAP_DEBUG_ANY, "%s: %s\n", c->log, c->msg, 0 );
			return( 1 );
		}
		cm->db.bd_info = backend_info( c->argv[1] );
		if ( !cm->db.bd_info ) {
			sprintf( c->msg, "unknown backend type" );
			Debug( LDAP_DEBUG_ANY, "%s: %s\n", c->log, c->msg, 0 );
			return( 1 );
		}
		if ( cm->db.bd_info->bi_db_init( &cm->db ) ) {
			sprintf( c->msg, "backend %s init failed", c->argv[1] );
			Debug( LDAP_DEBUG_ANY, "%s: %s\n", c->log, c->msg, 0 );
			return( 1 );
		}

		/* This type is in use, needs to be opened */
		cm->db.bd_info->bi_nDB++;

		cm->max_entries = atoi( c->argv[2] );

		cm->num_entries_limit = atoi( c->argv[4] );
		cm->cc_period = atoi( c->argv[5] );
		Debug( LDAP_DEBUG_TRACE,
				"Total # of attribute sets to be cached = %d\n",
				cm->numattrsets, 0, 0 );
		qm->attr_sets = ( struct attr_set * )ch_malloc( cm->numattrsets *
			    			sizeof( struct attr_set ));
		for ( i = 0; i < cm->numattrsets; i++ ) {
			qm->attr_sets[i].attrs = NULL;
		}
		break;
	case PC_ATTR:
		num = atoi( c->argv[1] );
		if (num >= cm->numattrsets) {
			sprintf( c->msg, "attrset index out of bounds" );
			Debug( LDAP_DEBUG_ANY, "%s: %s\n", c->log, c->msg, 0 );
			return 1;
		}
		if ( c->argv[2] && strcmp( c->argv[2], "*" ) ) {
			qm->attr_sets[num].count = c->argc - 2;
			qm->attr_sets[num].attrs = (AttributeName*)ch_malloc(
						(c->argc-1) * sizeof( AttributeName ));
			attr_name = qm->attr_sets[num].attrs;
			for ( i = 2; i < c->argc; i++ ) {
				ber_str2bv( c->argv[i], 0, 1, &attr_name->an_name);
				attr_name->an_desc = NULL;
				if ( slap_bv2ad( &attr_name->an_name,
						&attr_name->an_desc, &text )) {
					strcpy( c->msg, text );
					Debug( LDAP_DEBUG_ANY, "%s: %s\n", c->log, c->msg, 0 );
					ch_free( qm->attr_sets[num].attrs );
					qm->attr_sets[num].attrs = NULL;
					qm->attr_sets[num].count = 0;
					return 1;
				}
				attr_name->an_oc = NULL;
				attr_name->an_oc_exclude = 0;
				attr_name++;
				BER_BVZERO( &attr_name->an_name );
			}
		}
		break;
	case PC_TEMP:
		if (( i = atoi( c->argv[2] )) >= cm->numattrsets ) {
			sprintf( c->msg, "template index invalid" );
			Debug( LDAP_DEBUG_ANY, "%s: %s\n", c->log, c->msg, 0 );
			return 1;
		}
		num = cm->numtemplates;
		if ( num == 0 )
			find_supersets( qm->attr_sets, cm->numattrsets );
		qm->templates = ( QueryTemplate* )ch_realloc( qm->templates,
				( num + 2 ) * sizeof( QueryTemplate ));
		temp = qm->templates + num;
		ldap_pvt_thread_rdwr_init( &temp->t_rwlock );
		temp->query = temp->query_last = NULL;
		temp->ttl = atoi( c->argv[3] );
		temp->no_of_queries = 0;

		ber_str2bv( c->argv[1], 0, 1, &temp->querystr );
		Debug( LDAP_DEBUG_TRACE, "Template:\n", 0, 0, 0 );
		Debug( LDAP_DEBUG_TRACE, "  query template: %s\n",
				temp->querystr.bv_val, 0, 0 );
		temp->attr_set_index = i;
		Debug( LDAP_DEBUG_TRACE, "  attributes: \n", 0, 0, 0 );
		if ( ( attrarray = qm->attr_sets[i].attrs ) != NULL ) {
			for ( i=0; attrarray[i].an_name.bv_val; i++ )
				Debug( LDAP_DEBUG_TRACE, "\t%s\n",
					attrarray[i].an_name.bv_val, 0, 0 );
		}
		temp++; 
		temp->querystr.bv_val = NULL;
		cm->numtemplates++;
		break;
	case PC_RESP:
		if ( strcasecmp( c->argv[1], "head" ) == 0 ) {
			cm->response_cb = PCACHE_RESPONSE_CB_HEAD;

		} else if ( strcasecmp( c->argv[1], "tail" ) == 0 ) {
			cm->response_cb = PCACHE_RESPONSE_CB_TAIL;

		} else {
			sprintf( c->msg, "unknown specifier" );
			Debug( LDAP_DEBUG_ANY, "%s: %s\n", c->log, c->msg, 0 );
			return 1;
		}
		break;
	}
	return rc;
}

static int
proxy_cache_config(
	BackendDB	*be,
	const char	*fname,
	int		lineno,
	int		argc,
	char		**argv
)
{
	slap_overinst	*on = (slap_overinst *)be->bd_info;
	cache_manager* 	cm = on->on_bi.bi_private;

	/* Something for the cache database? */
	if ( cm->db.bd_info && cm->db.bd_info->bi_db_config )
		return cm->db.bd_info->bi_db_config( &cm->db, fname, lineno,
			argc, argv );
	return SLAP_CONF_UNKNOWN;
}

static int
proxy_cache_init(
	BackendDB *be
)
{
	slap_overinst *on = (slap_overinst *)be->bd_info;
	cache_manager *cm;
	query_manager *qm;

	cm = (cache_manager *)ch_malloc(sizeof(cache_manager));
	on->on_bi.bi_private = cm;

	qm = (query_manager*)ch_malloc(sizeof(query_manager));

	cm->db = *be;
	SLAP_DBFLAGS(&cm->db) |= SLAP_DBFLAG_NO_SCHEMA_CHECK;
	cm->db.be_private = NULL;
	cm->db.be_pcl_mutexp = &cm->db.be_pcl_mutex;
	cm->qm = qm;
	cm->numattrsets = 0;
	cm->numtemplates = 0; 
	cm->num_entries_limit = 5;
	cm->num_cached_queries = 0;
	cm->max_entries = 0;
	cm->cur_entries = 0;
	cm->max_queries = 10000;
	cm->response_cb = PCACHE_RESPONSE_CB_TAIL;
	cm->cc_period = 1000;
	cm->cc_paused = 0;

	qm->attr_sets = NULL;
	qm->templates = NULL;
	qm->lru_top = NULL;
	qm->lru_bottom = NULL;

	qm->qcfunc = query_containment;
	qm->crfunc = cache_replacement;
	qm->addfunc = add_query;
	ldap_pvt_thread_mutex_init(&qm->lru_mutex);

	ldap_pvt_thread_mutex_init(&cm->cache_mutex);
	ldap_pvt_thread_mutex_init(&cm->remove_mutex);
	return 0;
}

static int
proxy_cache_open(
	BackendDB *be
)
{
	slap_overinst	*on = (slap_overinst *)be->bd_info;
	cache_manager	*cm = on->on_bi.bi_private;
	int		rc = 0;

	/* need to inherit something from the original database... */
	cm->db.be_def_limit = be->be_def_limit;
	cm->db.be_limits = be->be_limits;
	cm->db.be_acl = be->be_acl;
	cm->db.be_dfltaccess = be->be_dfltaccess;

	rc = backend_startup_one( &cm->db );

	/* There is no runqueue in TOOL mode */
	if ( slapMode & SLAP_SERVER_MODE ) {
		ldap_pvt_thread_mutex_lock( &slapd_rq.rq_mutex );
		ldap_pvt_runqueue_insert( &slapd_rq, cm->cc_period,
			consistency_check, on,
			"pcache_consistency", be->be_suffix[0].bv_val );
		ldap_pvt_thread_mutex_unlock( &slapd_rq.rq_mutex );

		/* Cached database must have the rootdn */
		if ( BER_BVISNULL( &cm->db.be_rootndn )
				|| BER_BVISEMPTY( &cm->db.be_rootndn ) )
		{
			Debug( LDAP_DEBUG_ANY, "proxy_cache_open(): "
				"underlying database of type \"%s\"\n"
				"    serving naming context \"%s\"\n"
				"    has no \"rootdn\", required by \"proxycache\".\n",
				on->on_info->oi_orig->bi_type,
				cm->db.be_suffix[0].bv_val, 0 );
			return 1;
		}
	}

	return rc;
}

static int
proxy_cache_close(
	BackendDB *be
)
{
	slap_overinst *on = (slap_overinst *)be->bd_info;
	cache_manager *cm = on->on_bi.bi_private;
	query_manager *qm = cm->qm;
	int i, j, rc = 0;

	/* cleanup stuff inherited from the original database... */
	cm->db.be_limits = NULL;
	cm->db.be_acl = NULL;

	if ( cm->db.bd_info->bi_db_close ) {
		rc = cm->db.bd_info->bi_db_close( &cm->db );
	}
	for ( i=0; i<cm->numtemplates; i++ ) {
		CachedQuery *qc, *qn;
		for ( qc = qm->templates[i].query; qc; qc = qn ) {
			qn = qc->next;
			free_query( qc );
		}
		free( qm->templates[i].querystr.bv_val );
		ldap_pvt_thread_rdwr_destroy( &qm->templates[i].t_rwlock );
	}
	free( qm->templates );
	qm->templates = NULL;

	for ( i=0; i<cm->numattrsets; i++ ) {
		free( qm->attr_sets[i].ID_array );
		for ( j=0; j<qm->attr_sets[i].count; j++ ) {
			free( qm->attr_sets[i].attrs[j].an_name.bv_val );
		}
		free( qm->attr_sets[i].attrs );
	}
	free( qm->attr_sets );
	qm->attr_sets = NULL;

	return rc;
}

static int
proxy_cache_destroy(
	BackendDB *be
)
{
	slap_overinst *on = (slap_overinst *)be->bd_info;
	cache_manager *cm = on->on_bi.bi_private;
	query_manager *qm = cm->qm;

	/* cleanup stuff inherited from the original database... */
	cm->db.be_suffix = NULL;
	cm->db.be_nsuffix = NULL;
	BER_BVZERO( &cm->db.be_rootdn );
	BER_BVZERO( &cm->db.be_rootndn );
	BER_BVZERO( &cm->db.be_rootpw );
	/* FIXME: there might be more... */

	backend_destroy_one( &cm->db, 0 );

	ldap_pvt_thread_mutex_destroy( &qm->lru_mutex );
	ldap_pvt_thread_mutex_destroy( &cm->cache_mutex );
	ldap_pvt_thread_mutex_destroy( &cm->remove_mutex );
	free( qm );
	free( cm );

	return 0;
}

static slap_overinst proxy_cache;

int pcache_init()
{
	LDAPAttributeType *at;
	int code;
	const char *err;

	at = ldap_str2attributetype( queryid_schema, &code, &err,
		LDAP_SCHEMA_ALLOW_ALL );
	if ( !at ) {
		Debug( LDAP_DEBUG_ANY,
			"pcache_init: ldap_str2attributetype failed %s %s\n",
			ldap_scherr2str(code), err, 0 );
		return code;
	}
	code = at_add( at, 0, NULL, &err );
	if ( !code ) {
		slap_str2ad( at->at_names[0], &ad_queryid, &err );
	}
	ldap_memfree( at );
	if ( code ) {
		Debug( LDAP_DEBUG_ANY,
			"pcache_init: at_add failed %s %s\n",
			scherr2str(code), err, 0 );
		return code;
	}

	proxy_cache.on_bi.bi_type = "pcache";
	proxy_cache.on_bi.bi_db_init = proxy_cache_init;
	proxy_cache.on_bi.bi_db_config = proxy_cache_config;
	proxy_cache.on_bi.bi_db_open = proxy_cache_open;
	proxy_cache.on_bi.bi_db_close = proxy_cache_close;
	proxy_cache.on_bi.bi_db_destroy = proxy_cache_destroy;
	proxy_cache.on_bi.bi_op_search = proxy_cache_search;

	proxy_cache.on_bi.bi_chk_controls = proxy_cache_chk_controls;

	proxy_cache.on_bi.bi_cf_ocs = pcocs;

	code = config_register_schema( pccfg, pcocs );
	if ( code ) return code;

	return overlay_register( &proxy_cache );
}

#if SLAPD_OVER_PROXYCACHE == SLAPD_MOD_DYNAMIC
int init_module(int argc, char *argv[]) {
	return pcache_init();
}
#endif

#endif	/* defined(SLAPD_OVER_PROXYCACHE) */
