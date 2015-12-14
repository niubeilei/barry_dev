////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cert_verify.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/kmod.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/delay.h>

#include "aos/aosKernelApi.h"
#include "aosUtil/Memory.h"
#include "CertVerify/cert_verify.h"
#include "CertVerify/jns_ocsp.h"
#include "PKCS/Base64.h"

static X509_VERIFY_SET	x509_verify_set, *x509_set = &x509_verify_set;

static struct task_struct * crl_thread = NULL;
static struct task_struct * request_thread = NULL;
static struct task_struct * response_thread = NULL;

/* function protype */
static int x509_ocsp_callback(struct aos_tcpapi_data *sock, char *data, int len, void *userdata, int rc);

/***********************************************
 *           config routine for CLI
 **********************************************/
static int _set_status(struct aosKernelApiParms *parms, UINT32 flag) {
	int on = -1;

	if (parms->mNumStrings < 1)
		return -1;

	if (strcmp(parms->mStrings[0], "on") == 0)
		on = 1;
	if (strcmp(parms->mStrings[0], "off") == 0)
		on = 0;

	switch (on) {
	case 1:
		set_flag(x509_set->flags, flag);
		break;
	case 0:
		clear_flag(x509_set->flags, flag);
		break;
	default:
		return -1;
	}

	return 0;
}

/* open or close ocsp certificate check */
int aos_ocsp_status(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {
	*length = 0;
	return _set_status(parms, X509_OCSP_OPEN);
}



/* open or close ocsp crl check */
int aos_crl_status(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {

	*length = 0;
	return _set_status(parms, X509_CRL_OPEN);
}

/* set ocsp server ip address and port */
int aos_ocsp_set_addr(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {
	UINT32 addr;
	WORD   port;

	if (parms->mNumIntegers != 2)
		return -1;

	addr = parms->mIntegers[0];
	port = (WORD)parms->mIntegers[1];

	x509_set->ocsp_server_addr = addr;
	x509_set->ocsp_server_port = port;

	set_flag(x509_set->flags, X509_OCSP_ADDR_SET);

	*length = 0;
	return 0;
}


int aos_crl_set_addr(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {
	UINT32 addr;
	WORD   port;

	if (parms->mNumIntegers != 2)
		return -1;

	addr = parms->mIntegers[0];
	port = (WORD)parms->mIntegers[1];

	x509_set->crl_server_addr = addr;
	x509_set->crl_server_port = port;

	set_flag(x509_set->flags, X509_CRL_ADDR_SET);

	*length = 0;
	return 0;
}

int aos_crl_url_set(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {
	int len;
	char * ptr;

	if (parms->mNumStrings < 1 || parms->mStrings[0] == NULL)
		return -1;

	len = strlen(parms->mStrings[0]);
	if (len < 1)
		return -1;
	ptr = aos_zmalloc(len + 1);
	if (!ptr)
		return -1;
	strcpy(ptr, parms->mStrings[0]);

	if (x509_set->crl_url)
		aos_free(x509_set->crl_url);
	x509_set->crl_url = ptr;

	set_flag(x509_set->flags, X509_CRL_URL_SET);

	*length = 0;
	return 0;
}

int aos_crl_timeout_set(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {
	UINT32 cur_time;

	if (parms->mNumIntegers != 1)
		return -1;
		
	cur_time = get_seconds();
	x509_set->crl_timeout = parms->mIntegers[0] * 60;

	if (x509_set->crl_sock == 0 && 
		x509_set->crl_next_update > (cur_time + x509_set->crl_timeout))
		x509_set->crl_next_update = cur_time + x509_set->crl_timeout;
		
	*length = 0;
	return 0;
}

int aos_crl_update(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {
	UINT32 cur_time;
	char *buf = aosKernelApi_getBuff(data);
	char *ptr;
	int len;

	len = 0; ptr = buf;
	cur_time = get_seconds();

	if (x509_set->crl_sock == 0)  {
		x509_set->crl_next_update = get_seconds();
		len = sprintf(ptr + len, "CRL will update...\n");
	} else 
		len = sprintf(ptr + len, "CRL is doing update, please wait a while\n");
		
	*length = 0;
	return 0;
}

int aos_crl_show_list(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen)
{
	char *buf = aosKernelApi_getBuff(data);
	char *ptr;
	int len;

	len = 0; ptr = buf;
	len += sprintf(ptr + len, "Crl list:\n");

	if (is_flag_set(x509_set->flags, X509_CRL_IMPORTED)) {
		int i;
		
		read_lock_bh(&x509_set->crl_lock);
		for (i = 0; i < CRL_HASH_BUCKETS; i++) {
			X509_CRL_ENTRY *entry = x509_set->crl_hash_tbl[i];
	
			while (entry) {
				asn_data_t *sn = &entry->serial_number;
	
				len += sprintf(ptr + len, "\t%s\n", sn->data);
				entry = entry->next;
			}
		}
		read_unlock_bh(&x509_set->crl_lock);
	} else 
		len += sprintf(ptr + len, "\t list not imported\n");
		
	*length = len;
	return 0;
}


static void crl_flush(void) {
	X509_CRL_ENTRY *entry;
	int i;

	for (i = 0; i < CRL_HASH_BUCKETS; i++) {
		entry = x509_set->crl_hash_tbl[i];
		while (entry) {
			X509_CRL_ENTRY *t_entry;

			t_entry = entry;
			entry = entry->next;
			aos_free(t_entry);
		}
		x509_set->crl_hash_tbl[i] = NULL;
	}
}

/* flush the crl hash table */
int aos_crl_flush(char *data,
		unsigned int *length,
		struct aosKernelApiParms *parms,
		char *errmsg,
		const int errlen) {

	if (!is_flag_set(x509_set->flags, X509_CRL_IMPORTED))
		return 0;

	/* flush the hash table */
	write_lock_bh(&x509_set->crl_lock);

	crl_flush();
	clear_flag(x509_set->flags, X509_CRL_IMPORTED);
	x509_set->crl_next_update = 0;

	write_unlock_bh(&x509_set->crl_lock);

	*length = 0;
	return 0;
}

int aos_ocsp_save_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen)
{
	char *buf = aosKernelApi_getBuff(data);
	char *ptr;
	int len;

	len = 0;
	ptr = buf;

	/* ocsp */
	if (is_flag_set(x509_set->flags, X509_OCSP_OPEN))
		len += sprintf(ptr + len, "<Cmd>ocsp status on</Cmd>\n");
	if (is_flag_set(x509_set->flags, X509_OCSP_ADDR_SET)) {
		unsigned int addr = x509_set->ocsp_server_addr;
		int port = x509_set->ocsp_server_port;

		len += sprintf(ptr + len, "<Cmd>ocsp set addr %u.%u.%u.%u %d</Cmd>\n",
			(addr) & 0xff, (addr >> 8) & 0xff, (addr >> 16) &0xff,
			(addr >> 24) & 0xff, port);
	}

	/* crl */
	if (is_flag_set(x509_set->flags, X509_CRL_OPEN))
		len += sprintf(ptr + len, "<Cmd>crl status on</Cmd>\n");
	if (is_flag_set(x509_set->flags, X509_CRL_ADDR_SET)) {
		unsigned int addr = x509_set->crl_server_addr;
		int port = x509_set->crl_server_port;

		len += sprintf(ptr + len, "<Cmd>crl set addr %u.%u.%u.%u %d</Cmd>\n",
			(addr) & 0xff, (addr >> 8) & 0xff, (addr >> 16) &0xff,
			(addr >> 24) & 0xff, port);
	}
	if (is_flag_set(x509_set->flags, X509_CRL_URL_SET))
		len += sprintf(ptr + len, "<Cmd>crl set url %s</Cmd>\n", x509_set->crl_url);

	len += sprintf(ptr + len, "<Cmd>crl set timeout %lu</Cmd>\n", x509_set->crl_timeout/60);

	*length = len;
	return 0;
}

int aos_ocsp_clear_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen)
{
	clear_flag(x509_set->flags, X509_CRL_OPEN);
	clear_flag(x509_set->flags, X509_OCSP_OPEN);

	write_lock_bh(&x509_set->crl_lock);
	if (is_flag_set(x509_set->flags, X509_CRL_URL_SET))
	{
		aos_free(x509_set->crl_url);
		x509_set->crl_url = NULL;
	}
	x509_set->crl_server_addr = 0;
	x509_set->crl_server_port = 0;
	x509_set->crl_timeout = 60;
	x509_set->crl_next_update = 0;
	crl_flush();
	write_unlock_bh(&x509_set->crl_lock);

	x509_set->flags = 0;
	*length = 0;
	return 0;
}

int aos_show_ocsp_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen)
{
	char *buf = aosKernelApi_getBuff(data);
	char *ptr;
	int len;

	len = 0; ptr = buf;
	len += sprintf(ptr + len, "OCSP information\n\tstatus :%s\n",
			(x509_set->flags & X509_OCSP_OPEN) ? "on" : "off");
	if (is_flag_set(x509_set->flags, X509_OCSP_ADDR_SET)) {
		unsigned int addr = x509_set->ocsp_server_addr;
		int port = x509_set->ocsp_server_port;

		len += sprintf(ptr + len, "\tserver : %u.%u.%u.%u:%d\n",
			(addr) & 0xff, (addr >> 8) & 0xff, (addr >> 16) &0xff, (addr >> 24) & 0xff,
			port);
	} else
		len += sprintf(ptr + len, "\tserver: unset\n");

	if (len > *length) {
		aos_free(buf);
		return -1;
	}

	*length = len;
	return 0;
}

int aos_show_crl_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen)
{
	char *buf = aosKernelApi_getBuff(data);
	char *ptr;
	int len;

	len = 0; ptr = buf;
	len += sprintf(ptr + len, "CRL information\n\tstatus: %s\n",
			(x509_set->flags & X509_CRL_OPEN) ? "on" : "off");
	if (is_flag_set(x509_set->flags, X509_CRL_ADDR_SET)) {
		unsigned int addr = x509_set->crl_server_addr;
		int port = x509_set->crl_server_port;

		len += sprintf(ptr + len, "\tserver : %u.%u.%u.%u:%d\n",
			(addr) & 0xff, (addr >> 8) & 0xff, (addr >> 16) &0xff, (addr >> 24) & 0xff,
			port);
	} else
		len += sprintf(ptr + len, "\tserver: unset\n");
	if (is_flag_set(x509_set->flags, X509_CRL_URL_SET))
		len += sprintf(ptr + len, "\turl:%s\n", x509_set->crl_url);
	else
		len += sprintf(ptr + len, "\turl: unset\n");

	len += sprintf(ptr + len, "\ttimeout: %lu minutes", x509_set->crl_timeout/60);
	
	len += sprintf(ptr + len, "\tCRL list imported: %s\n",
			is_flag_set(x509_set->flags, X509_CRL_IMPORTED) ? "yes" : "no");
	
	len += sprintf(ptr + len, "\tnext get crl from server left %lu seconds\n",
			x509_set->crl_next_update - get_seconds());
	
	if (len > *length) {
		aos_free(buf);
		return -1;
	}

	*length = len;
	return 0;
}

/* crl hash function */
static inline int crl_hash(asn_data_t *bin) {
	int i, hash = 0;

	for (i = 0; i < bin->len; i++)
		hash += bin->data[i];

	return hash % CRL_HASH_BUCKETS;
}

static inline void _crl_add_entry(X509_CRL_ENTRY *entry) {
	int hash;

	hash = crl_hash(&entry->serial_number);
	entry->next = x509_set->crl_hash_tbl[hash];
	x509_set->crl_hash_tbl[hash] = entry;

	return;
}

static int crl_import(X509_CRL_INFO *crlinfo) {
	X509_CRL_ENTRY *list = crlinfo->list;

	if (!crlinfo)
		return -1;

	/* if already imported before, flush it clr list */
	if (x509_set->flags & X509_CRL_IMPORTED) {
		clear_flag(x509_set->flags, X509_CRL_IMPORTED);
		crl_flush();
	}

	while(list) {
		X509_CRL_ENTRY *entry = list;

		list = list->next;
		entry->next = NULL;

		_crl_add_entry(entry);
	}

	crlinfo->list = NULL;
	crlinfo->entry_num = 0;

	/* set crlinfo's imported flag */
	set_flag(x509_set->flags, X509_CRL_IMPORTED);

	return 0;
}


//when error or get url data, do this function
static inline void clear_crl_state(X509_VERIFY_SET *set) {
	UINT32 cur_time;

	cur_time = get_seconds();

	if (set->crl_sock) {
		aos_tcp_close(set->crl_sock);
		set->crl_sock = NULL;
	}

	set->crl_flags = X509_CRL_SOCK_CLOSED;

	if (set->crl_data)
		aos_free(set->crl_data);
	set->crl_data = NULL;
	set->crl_data_len = 0;

	if (set->crl_next_update <= cur_time)
		set->crl_next_update = cur_time + set->crl_timeout;
	return;
}

static int parse_url(char *url, char **dn, char **surl) {
	char *start, *end, *p;

	if (!url)
		return -1;

	p = strstr(url, "http://");
	if (!p)
		return -1;
	else
		start = p + 7;

	end = strchr(start, '/');
	if (!end)
		return -1;

	*end = 0;
	*surl = end + 1;
	*dn = start;

	return 0;
}

static char *strdup(char *s) {
	char *rv = aos_zmalloc(strlen(s) + 1);
	if (rv)
		strcpy(rv, s);
	return rv;
}

static int crl_send_http_request(X509_VERIFY_SET *set) {
	char * http_get_of_crl = "GET /%s / HTTP/1.1\r\nUser-Agent: Wget/1.9\r\n"
		"Host: %s\r\nAccept: */*\r\nConnection: Keep-Alive\r\n\r\n";
	char buffer[256], *url = NULL, *surl, *dn;
	int length, len;

	url = strdup(set->crl_url);
	if (!url)
		return -1;

	if (parse_url(url, &dn, &surl) < 0)
		goto error;

	length = sprintf(buffer, http_get_of_crl, surl, dn);
	len = aos_tcp_send_data(set->crl_sock, buffer, length);
	if (len != length)
		goto error;

	aos_free(url);
	return 0;

error:
	if (url)	aos_free(url);
	return -1;
}

static int parse_http_header(char *header, int length, char **content) {
	char *p;
	char *http_ok = "200 OK";
	char *content_len = "Content-Length:";
	char *inter_content = "\r\n\r\n";
	int len;

	if (!header || length <=0)
		return -1;

	p = header;
	p = strstr(p, http_ok);
	if (!p)
		return -1;

	p = strstr(p, content_len);
	if (!p)
		return -1;

	p += strlen(content_len);
	while(is_space(*p)) p++;

	len = 0;
	while (is_digit(*p)) {
		len = len * 10 + *p - '0';
		p++;
	}

	p = strstr(p, inter_content);
	if (!p)
		return -1;

	*p = 0;
	p += strlen(inter_content);
	*content = p;

	return len;
}

static int crl_tcp_callback(struct aos_tcpapi_data *sock, char *data, int data_len,
			void *userdata, int rc) {
	X509_VERIFY_SET *set = (X509_VERIFY_SET *)userdata;
	BYTE * buf, *p;

	switch (rc) {
	case eAosRc_ConnSuccess:
#if 0
		if (set->crl_sock) {
			printk(KERN_ERR"crl_sock not NULL when eAosRc_ConnSuccess\n");
			aos_tcp_close(set->crl_sock);
			set->crl_sock = NULL;
		}
#endif
		if (!is_flag_set(set->crl_flags, X509_CRL_WAITTING_SOCK)) {
			set_flag(set->crl_flags, X509_CRL_SOCK_ERROR);
			break;
		}

		set->crl_sock = sock;
		clear_flag(set->crl_flags, X509_CRL_WAITTING_SOCK);
		set_flag(set->crl_flags, X509_CRL_SOCK_CREATED);
		break;
	case eAosRc_DataRead:
		if (!is_flag_set(set->crl_flags, X509_CRL_WAITTING_DATA)) {
			set_flag(set->crl_flags, X509_CRL_SOCK_ERROR);
			break;
		}

		buf = (BYTE *)aos_zmalloc(set->crl_data_len + data_len + 1);
		if (!buf) {
			clear_flag(set->crl_flags, X509_CRL_WAITTING_DATA);
			set_flag(set->crl_flags, X509_CRL_SOCK_ERROR);
			break;
		}
		if (set->crl_data) {
			memcpy(buf, set->crl_data, set->crl_data_len);
			memcpy(buf + set->crl_data_len, data, data_len);
		} else
			memcpy(buf, data, data_len);

		p = set->crl_data;
		set->crl_data = buf;
		set->crl_data_len += data_len;
		aos_free(p);

		set_flag(set->crl_flags, X509_CRL_DATA_RECV);

		break;
	default:	//received an error, always a socket failed
		set_flag(set->crl_flags, X509_CRL_SOCK_ERROR);
		break;
	}

	return 0;
}

static int do_crl_thread(void *p) {
	X509_VERIFY_SET *set = (X509_VERIFY_SET *)p;
	int length, ret;
	UINT32 cur_time;

	__set_current_state(TASK_RUNNING);

	do {
		// do connect to crl server
		cur_time = get_seconds();

		if (is_flag_set(set->flags, X509_CRL_ON) &&
		    is_flag_set(set->crl_flags, X509_CRL_SOCK_CLOSED) &&
		    set->crl_next_update <= cur_time) {
			ret = aos_tcp_client_create(set->crl_server_addr, set->crl_server_port,
				&set->crl_sock, set, 10, crl_tcp_callback);
			if (ret != 0) {
				printk(KERN_ALERT"[crl_thread]: aos_tcp_client_create failed\n");
				goto crl_sleep;
			}

			clear_flag(set->crl_flags,X509_CRL_SOCK_CLOSED);
			set_flag(set->crl_flags, X509_CRL_WAITTING_SOCK);
			goto crl_sleep;
		} else {
			if (set->crl_next_update <= cur_time)
				set->crl_next_update = cur_time + set->crl_timeout;
		}

		//send http request to crl server
		if (is_flag_set(set->crl_flags, X509_CRL_SOCK_CREATED)) {
			ret = crl_send_http_request(set);
			if (ret < 0) {
				printk(KERN_ALERT"[crl_thread]: crl_send_http_request failed\n");
				clear_crl_state(set);
				goto crl_sleep;
			}
			clear_flag(set->crl_flags, X509_CRL_SOCK_CREATED);
			set_flag(set->crl_flags, X509_CRL_WAITTING_DATA);
			goto crl_sleep;
		}

		//data received
		if (is_flag_set(set->crl_flags, X509_CRL_DATA_RECV)) {
			X509_CRL_INFO *crlinfo = NULL;
			DATA_OBJECT object;
			char *data;
			int len;

			len = parse_http_header(set->crl_data, set->crl_data_len, &data);
			if (len < 0) {
				printk(KERN_ALERT"[crl_thread]: parse_http_header failed\n");
				clear_crl_state(set);
				goto crl_sleep;
			}

			length = set->crl_data_len - (data - (char *)set->crl_data);
			asn_make_object(&object, data, length);
			//case of no enough data
			if (length < 4 ||
			    length < asn_peek_object_length(&object)) {
				clear_flag(set->crl_flags, X509_CRL_DATA_RECV);
				goto crl_sleep;
			}
			
			crlinfo = (X509_CRL_INFO *)aos_zmalloc(sizeof(X509_CRL_INFO));
			if (!crlinfo) 
				goto crl_sleep;

			ret = x509_decode_crlinfo(&object, crlinfo);
			if (ret == 0) {
				/* import the new crl list and set flag */
				write_lock(&set->crl_lock);
				crl_import(crlinfo);
				write_unlock(&set->crl_lock);

				//modify next update time
				if (ret == 0 && crlinfo->next_update != 0)
					set->crl_next_update = crlinfo->next_update;

				crlinfo->object = NULL;
				x509_free_crlinfo(crlinfo);
				printk(KERN_INFO"[crl_thread]: crl received and imported!\n");
			} else	{
				aos_free(crlinfo);
				printk(KERN_ALERT"[crl_thread]: Decode crlinfo failed!\n");
			}

			clear_crl_state(set);
			goto crl_sleep;
		}

		//when sock error
		if (is_flag_set(set->crl_flags, X509_CRL_SOCK_ERROR)) {
			printk(KERN_ALERT"[crl_thread]: socket get X509_CRL_SOCK_ERROR and failed!\n");
			clear_crl_state(set);
			goto crl_sleep;
		}

crl_sleep:
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
		__set_current_state(TASK_RUNNING);

		continue;
	} while (!kthread_should_stop());

	return 0;
}


static int
crl_check_valid(X509_CERT_INFO * cert) {
	asn_data_t *sn = &cert->serial_number;
	X509_CRL_ENTRY *entry;
	int hash;

	hash = crl_hash(sn);

	read_lock(&x509_set->crl_lock);

	entry = x509_set->crl_hash_tbl[hash];
	while (entry) {
		if (sn->len == entry->serial_number.len &&
		    !memcmp(sn->data, entry->serial_number.data, sn->len)) {
		    read_unlock(&x509_set->crl_lock);
		    return 0; //revoked certificate
		}
		entry = entry->next;
	}

	read_unlock(&x509_set->crl_lock);

	/* can not find in hash table, we think it valid */
	return 1;
}


/* accept SSL certificate request and return as soon as possible
 * called by lijin----------:)
 * return:
 *      -1: internal error;
 *       0: accepted, and will return by callback
 */
int aos_verify_cert(void *certinfo, void * context, SSL_REQUEST_CALLBACK callback)
{
	X509_VERIFY_REQUEST	*request = NULL;

	if (!certinfo)
		return eAosRc_CertificateError;

	/* is ocsp or crl already config ok? */
	if ( !is_flag_set(x509_set->flags, X509_OCSP_CAN_CHECK) &&
	     !is_flag_set(x509_set->flags, X509_CRL_CAN_CHECK))
	     return eAosRc_CertificateError;

	request = aos_zmalloc(sizeof(X509_VERIFY_REQUEST));
	if (request == NULL)
		return eAosRc_CertificateError;

	/* copy the parameter */
	memset(request, 0, sizeof(X509_VERIFY_REQUEST));

	request->certinfo	= (X509_CERT_INFO *)certinfo;
	request->result		= eAosRc_Error;
	request->context	= context;
	request->callback	= callback;

	/* request list is FIFO, so add the request to the tail */
	write_lock(&x509_set->request_lock);
	list_add_tail(&request->link, &x509_set->request_list);
	write_unlock(&x509_set->request_lock);

	wake_up_process(request_thread);
	return eAosRc_AsyncResult;
}

static void aos_free_request(X509_VERIFY_REQUEST *request) {
	if (!request)
		return;

	//if (request->certinfo)
	//	aos_free(request->certinfo);
printk("will call request->callback\n");
	//give the requestor an answer */
	request->callback(request->certinfo, request->context, request->result);

	aos_free(request);

	return;
}

static void aos_free_conn(X509_OCSP_CONN *conn) {
	X509_VERIFY_REQUEST *request;

	if (!conn)
		return;
printk("do aos_tcp_close\n");
	aos_tcp_close(conn->sock);

	request = conn->request;
	aos_free_request(request);

	if (conn->data && conn->len > 0) {
printk("do free conn->data:%d\n", conn->len);
		aos_free(conn->data);
}
	/* unlink from list */
	write_lock(&x509_set->response_lock);
	list_del(&conn->link);
	write_unlock(&x509_set->response_lock);

	aos_free(conn);
}

/* get random nonce */
static int get_nonce(BYTE *buf, int len) {
	static unsigned long nonce_data[32];
	static int nonce_inited = 0;
	HASH_STATE md;
	BYTE hash[SHA1_HASH_SIZE];
	int i;

	if (!buf || len <= 0)
		return -1;

	/* initial seed */
	if (!nonce_inited) {

		nonce_data[0] = (jiffies + CURRENT_TIME.tv_sec) ^ 0x47fad957;
		for (i = 1; i < 32; i++)
			nonce_data[i] = nonce_data[0] + (0x1 << i) - 1;
		nonce_inited = 1;
	}

	while (len > 0) {
		unsigned long *plong;

		sha1_init(&md);
		sha1_update(&md, (BYTE *)nonce_data, 32 * sizeof(unsigned long));
		sha1_final(&md,hash);

		if (len < SHA1_HASH_SIZE) {
			memcpy(buf, hash, len);
			buf += len;
			len = 0;
		} else {
			memcpy(buf,hash, SHA1_HASH_SIZE);
			buf += SHA1_HASH_SIZE;
			len -= SHA1_HASH_SIZE;
		}

		/* refresh hash seed */
		for (i = 0; i < SHA1_HASH_SIZE/sizeof(unsigned long); i++)
			nonce_data[i] = nonce_data[i + SHA1_HASH_SIZE];

		plong = (unsigned long *)hash;
		for (i = 32 - SHA1_HASH_SIZE/sizeof(unsigned long); i < 32; i++)
			nonce_data[i] = *plong++;
	}

	return 0;
}

/* connect to ocsp server by set of config,
 * and move conn to ocsp request list from request list */
static int do_ocsp_create_conn(X509_VERIFY_REQUEST *request) {
	X509_OCSP_CONN 		*conn = NULL;
	int ret;

	if (!request || !is_flag_set(x509_set->flags, X509_OCSP_ON))
		return -1;

	conn = (X509_OCSP_CONN *)aos_zmalloc(sizeof(X509_OCSP_CONN));
	if (!conn)
		return -1;

	conn->request = request;
	conn->addr = x509_set->ocsp_server_addr;
	conn->port = x509_set->ocsp_server_port;
	conn->data = NULL;
	conn->len  = 0;
	conn->sock = NULL;
	conn->flags = OCSP_CONN_WAITTING_SOCK;

	ret = aos_tcp_client_create(x509_set->ocsp_server_addr, x509_set->ocsp_server_port,
				 &conn->sock, conn, 60, x509_ocsp_callback);
	if (ret != 0) {
		aos_free(conn);
		return -1;
	}
printk("ocsp_create_conn: aos_tcp_client_create return ok\n");

	/* add ocsp connection to response list */
	write_lock(&x509_set->response_lock);
	list_add_tail(&conn->link, &x509_set->response_list);
	write_unlock(&x509_set->response_lock);

	return 0;
}

/* when socket built, send ocsp request to server */
static int do_ocsp_send_request(X509_OCSP_CONN *conn) {
	X509_VERIFY_REQUEST 	*request;
	JNS_OCSP_REQUEST	ocsp_request;
	DATA_OBJECT 		*ocsp_request_object = NULL; //ocsp request data
	char *buf = NULL;
	int len, len_octets, ret = -1, i;
	
printk("do_ocsp_send_request.....\n");
	if (!conn)	return -1;

	request = conn->request;

	ocsp_request.sn = &request->certinfo->serial_number;
	get_nonce(ocsp_request.nonce, JNS_NONCE_LEN);
	ocsp_request.nonce_len = JNS_NONCE_LEN;
	ocsp_request.time = get_seconds();

	ocsp_request_object = jns_encode_ocsp_request(&ocsp_request);
	if (!ocsp_request_object) {
		printk("jns_encode_ocsp_request failed\n");
		return -1;
	}

	len = asn_object_datalen(ocsp_request_object);
printk("ocsp request data len:%d\n", len);
	len_octets = (len <= 0xFF ) ? 1 : \
			(len <= 0xFFFFL) ? 2 : \
			(len <= 0xFFFFFFL) ? 3 : 4;
	buf = aos_zmalloc(len + len_octets + 1);
	if (!buf)
		goto error;
	buf[0] = len_octets;
	for (i = 0; i < len_octets; i++)
		buf[i + 1] = (len >> (i * 8)) & 0xff;
	memcpy(buf + len_octets + 1, ocsp_request_object->data, len);
	len = len + len_octets + 1;
	ret = aos_tcp_send_data(conn->sock, buf, len);
printk("send ocsp request:%d, ret=%d\n", len, ret);
	if (ret != len)
		ret = -1;
error:
	if (ocsp_request_object)	asn_free_object(ocsp_request_object);
	if (buf)			aos_free(buf);
	return ret;
}


static int do_request_thread(void * p) {
	//DECLARE_WAIT_QUEUE_HEAD(cvt_wq);
	X509_VERIFY_SET 	*set = (X509_VERIFY_SET *)p;
	X509_VERIFY_REQUEST 	*request;
	int ret;


	do {
		int result = eAosRc_Error; /* set default status */

		/* z Z £Ú...*/
		if (list_empty(&set->request_list)) {
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(HZ);
			__set_current_state(TASK_RUNNING);

			//interruptible_sleep_on_timeout(&cvt_wq, HZ);
			continue;
		}

		/* get SSL_Certificate_Verify reqeust */
		write_lock(&set->request_lock);
		request = list_entry(set->request_list.next, X509_VERIFY_REQUEST, link);
		list_del(&request->link);
		write_unlock(&set->request_lock);


		/* 1. OCSP certificate check */
	     	if (is_flag_set(set->flags, X509_OCSP_ON)) {

			/* 1.2 request to ocsp server */
			ret = do_ocsp_create_conn(request);
printk("do_ocsp_create_conn:%d\n", ret);
			if (ret < 0)
				goto out; 	/* failed */

			/* we can not return response at here, and will process it in callback */
			continue;
		}

		/* 2.CRL certificate check */
		if (is_flag_set(set->flags, X509_CRL_CAN_CHECK)) {
			printk("crl status on, can check");
			ret = crl_check_valid(request->certinfo);

			switch(ret) {
				case  1:	/* not found in crl */
					result = eAosRc_CertificateValid;
					break;
				case  0:
				default:	/* may not happen, just as -1 */
					result = eAosRc_CertificateInvalid;
					break;
			}
		}
		else if (is_flag_set(set->flags,X509_CRL_OPEN))
		{
			printk("crl status on, can not check");
			result = eAosRc_CertificateInvalid;
		}
		else
		{
			printk("crl status off");
		}

out:
		request->result = result;
		aos_free_request(request);
	} while (!kthread_should_stop());

	return 0;
}

/* only receive TCP socket data or error information */
static int x509_ocsp_callback(struct aos_tcpapi_data *sock,
				char *data, int len, void *userdata, int rc) {
	X509_OCSP_CONN * conn = (X509_OCSP_CONN *)userdata;
	BYTE * pdata;
	int data_len;

	switch(rc) {
	case eAosRc_ConnSuccess:
		printk("ocsp socket created\n");
		//conn->sock = sock;
		clear_flag(conn->flags, OCSP_CONN_WAITTING_SOCK);
		set_flag(conn->flags, OCSP_CONN_SOCK_SUCCESS);
		break;
	case eAosRc_DataRead:
		printk("ocsp data received:%d\n", len);
		if (!data || len <= 0)	//no data, just skip
			break;
		data_len = conn->len + len;
		pdata = (BYTE *)aos_zmalloc(data_len);
		if (!pdata) {
			printk(KERN_ERR"ocsp_callback:aos_zmalloc failed (eAosRc_DataRead)\n");
			set_flag(conn->flags, OCSP_CONN_ERROR);
			break;
		}
		if (conn->data) {
			memcpy(pdata, conn->data, conn->len);
			memcpy(pdata + conn->len, data, len);

			aos_free(conn->data);
		} else
			memcpy(pdata, data, len);

		conn->data = pdata;
		conn->len = data_len;
		set_flag(conn->flags, OCSP_CONN_DATA_RECV);
		break;
	default:	//received an error
		set_flag(conn->flags, OCSP_CONN_ERROR);
		break;
	}

printk("ocsp_tcp_callback:to wakeup response_thread\n");
	wake_up_process(response_thread);

	return 0;
}

static int do_response_thread(void* p) {
	X509_VERIFY_SET * set = (X509_VERIFY_SET *)p;
	//DECLARE_WAIT_QUEUE_HEAD(cst_wq);

	do {
		struct list_head *p;
		X509_OCSP_CONN * conn = NULL;
		int ret = eAosRc_Error, rtn;
		//X509_VERIFY_REQUEST *request;

		/* if list is empty, sleep a while */
		if (list_empty(&set->response_list)) {
resp_sleep:
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(HZ);
			__set_current_state(TASK_RUNNING);

			//interruptible_sleep_on_timeout(&cst_wq, HZ);
			continue;
		}

		/* search in the list for a socket answer */
		read_lock(&x509_set->response_lock);
		list_for_each(p, &set->response_list) {
			conn = list_entry(p, X509_OCSP_CONN, link);
			if ((conn->flags & OCSP_CONN_RESPONSE) == 0)
				conn = NULL;
			else
				break;
		}
		read_unlock(&x509_set->response_lock);

		/* if no socket's answer found, have a rest again */
		if (conn == NULL)
			goto resp_sleep;

		/* case 1: an error occured, abort it */
		if (is_flag_set(conn->flags, OCSP_CONN_ERROR)) {
			clear_flag(conn->flags, OCSP_CONN_ERROR);
			goto error;
		}

		/* case 2: connection created */
		if (is_flag_set(conn->flags, OCSP_CONN_SOCK_SUCCESS)) {
			clear_flag(conn->flags, OCSP_CONN_SOCK_SUCCESS);
			rtn = do_ocsp_send_request(conn);
			if (rtn < 0)
				goto error;
printk("do_ocsp_send_request ok\n");
			continue;
		}

		/* case 3: new data arrived */
		if (conn->flags & OCSP_CONN_DATA_RECV) {
			DATA_OBJECT object;
			int i, len_octets, len;
printk("ocsp data received:%d\n", conn->len);
			clear_flag(conn->flags, OCSP_CONN_DATA_RECV);
			if (conn->len <= 0)   //no data, may be error?  do waitting for new data
				continue;
			len_octets = conn->data[0];
			if (conn->len < len_octets + 1) {
printk("ocsp data not enough-1\n");
				continue;
}

			for (len = 0, i = 0; i < len_octets; i++)
				len |= conn->data[i + 1] << (i * 8);
			/* data is not enough ? */
			if (conn->len < len + len_octets + 1) {
printk("ocsp data not enough\n");
				continue;
}
			asn_make_object(&object, conn->data + len_octets + 1, len);

			rtn = jns_decode_ocsp_response(&object);
			if (rtn == JNS_OCSP_CERTSTATUS_GOOD)
				ret = eAosRc_CertificateValid;
			else
				ret = eAosRc_CertificateInvalid;
		}

error:
		/* send the caller the result (ret), and clear connection */
		conn->request->result = ret;
		aos_free_conn(conn);
	} while (1);//!kthread_should_stop());
	return 0;
}

/* initial data */
static void x509_verify_init(X509_VERIFY_SET *set) {
	int i;

	set->flags		= 0;

	set->ocsp_server_addr	= 0;
	set->ocsp_server_port	= 0;

	set->crl_url		= NULL;
	set->crl_server_addr	= 0;
	set->crl_server_port	= 0;
	set->crl_timeout	= 60;
	set->crl_next_update	= 0;
	set->crl_flags		= X509_CRL_SOCK_CLOSED;
	set->crl_data		= NULL;
	set->crl_data_len	= 0;
	set->crl_sock		= NULL;
	set->crl_lock		= RW_LOCK_UNLOCKED;

	/* initial list and hash table */
	for (i = 0; i < CRL_HASH_BUCKETS; i++)
		set->crl_hash_tbl[i] = NULL;

	set->request_lock = RW_LOCK_UNLOCKED;
	INIT_LIST_HEAD(&set->request_list);

	set->response_lock = RW_LOCK_UNLOCKED;
	INIT_LIST_HEAD(&set->response_list);

	/* set ocsp off as default */
	set->flags = 0;
};


/* module initial function */
static int __init aos_cert_verify_init(void) {

	x509_verify_init(x509_set);

	printk(KERN_ERR"Do aos_cert_verify_init\n");

	crl_thread = kthread_run(do_crl_thread, x509_set, "crl_thread");
	if (IS_ERR(crl_thread)) {
		printk(KERN_ERR"aos_cert_verify_init:creation kernel thread for do_crl_thread------[failed]\n");
		return -1;
	}
	printk("aos_cert_verify_init:creation kernel thread for do_crl_thread------[OK]\n");

	request_thread = kthread_run(do_request_thread, x509_set, "request_thread");
	if (IS_ERR(request_thread)) {
		printk(KERN_ERR"aos_cert_verify_init:creation kernel thread for do_request_thread------[failed]\n");
		goto error;
	}
	printk("aos_cert_verify_init:creation kernel thread for do_request_thread------[OK]\n");

	response_thread = kthread_run(do_response_thread, x509_set, "response_thread");
	if (IS_ERR(response_thread)) {
		printk(KERN_ERR"aos_cert_verify_init:creation kernel thread for do_response_thread------[failed]\n");
		goto error;
	}
	printk("aos_cert_verify_init:creation kernel thread for do_response_thread------[OK]\n");

	return 0;
error:
	if (crl_thread)		kthread_stop(crl_thread);
	if (request_thread)	kthread_stop(request_thread);

	return -1;
}


static void __exit aos_cert_verify_exit(void) {
	X509_VERIFY_REQUEST *request, *nr;
	X509_OCSP_CONN *conn, *nc;

	kthread_stop(crl_thread);
	kthread_stop(request_thread);
	kthread_stop(response_thread);

	write_lock(&x509_set->crl_lock);
	crl_flush();
	write_unlock(&x509_set->crl_lock);

	write_lock(&x509_set->request_lock);
	list_for_each_entry_safe(request, nr, &x509_set->request_list, link) {
		list_del(&request->link);
		aos_free(request);
	}
	write_unlock(&x509_set->request_lock);

	write_lock(&x509_set->response_lock);
	list_for_each_entry_safe(conn, nc, &x509_set->response_list, link) {
		list_del(&conn->link);
		aos_free(conn);
	}
	write_unlock(&x509_set->response_lock);

	return;
}
#ifdef MODULE
module_init(aos_cert_verify_init)
module_exit(aos_cert_verify_exit)
MODULE_LICENSE("GPL");
#else
__initcall(aos_cert_verify_init);
#endif
