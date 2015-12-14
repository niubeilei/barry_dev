////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_http_parser.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <linux/sched.h>
#include <linux/ctype.h>

#include "Ktcpvs/tcp_vs.h"
#include "Ktcpvs/tcp_vs_http_parser.h"

#define HTTP_VERSION_HEADER_LEN		5
#define HTTP_VERSION_NUMBER_LEN		3

const char *http_version_header = "HTTP/";

#define DEFAULT_MAX_COOKIE_AGE	1800
#define MAX_MIME_HEADER_STRING_LEN	64

typedef void (*HTTP_MIME_PARSER) (http_mime_header_t * mime, char *buffer);

typedef struct http_mime_parse_s {
	struct list_head plist;
	HTTP_MIME_PARSER parser;
	char mime_header_string[MAX_MIME_HEADER_STRING_LEN];
} http_mime_parse_t;

static http_mime_parse_t http_mime_parse_table[MAX_MIME_HEADER_STRING_LEN];


//
//	skip whitespace
//       LWS            = [CRLF] 1*( SP | HT )
//
static inline char *
skip_lws(const char *buffer)
{
	char *s = (char *) buffer;
	while ((*s == ' ') || (*s == '\t')) {
		s++;
	}
	return s;
}


//
//	search the seperator in a string
//
char *
search_sep(const char *s, int len, const char *sep)
{
	int l, ll;

	l = strlen(sep);
	if (!l)
		return (char *) s;

	ll = len;
	while (ll >= l) {
		ll--;
		if (!memcmp(s, sep, l))
			return (char *) s;
		s++;
	}
	return NULL;
}


//
//	extract the attribute-value pair
//	The input string has the form: A = "V", and it will also accept
//	A = V too.
//	return:
//		0  -- OK, and parse end.
//		\; -- OK, end with a ';'
//		\, -- OK, end with a ','
//		1  -- parse error.
//	Note:
//	The input buffer will be modified by this routine. be care!
//

int
extract_av(char **buffer, char **attribute, char **value)
{
	char *begin, *end, *pos, *a, *v;
	char c;
	int ret = 1;
	int flag = 0;

	a = v = end = NULL;

	/* get attribute */
	pos = a = begin = skip_lws(*buffer);
	for (;;) {
		c = *pos;
		switch (c) {
		case ' ':
		case '\t':
			end = pos;
			*end = 0;
			break;
		case '=':
			if (end == NULL) {
				end = pos;
				*end = 0;
			}
			goto get_value;
		case ';':
		case ',':
		case 0:
			if (end == NULL) {
				end = pos;
				*end = 0;
			}
			ret = c;
			goto exit;
		}
		pos++;
	}

      get_value:
	pos++;
	/* get value */
	pos = v = begin = skip_lws(pos);
	end = NULL;
	if (*pos == '"') {
		flag = 1;
		pos++;
		v++;
	}

	for (;;) {
		c = *pos;
		switch (c) {
		case ' ':
		case '\t':
			if ((flag == 0) && (end == NULL)) {
				end = pos;
				*end = 0;
			}
			break;
		case '"':
			if (flag == 1) {
				end = pos;
				*end = 0;
				flag = 0;
			}
			break;
		case ';':
		case ',':
			if (flag == 0) {
				if (end == NULL) {
					end = pos;
					*end = 0;
				}
				ret = c;
				goto exit;
			}
			break;
		case 0:
			if (end == NULL) {
				end = pos;
				*end = 0;
			}
			ret = c;
			goto exit;
		}
		pos++;
	}


      exit:
	if (*a == 0) {
		a = NULL;
	}

	if ((v != NULL) && (*v == 0)) {
		v = NULL;
	}

	if (ret > 1) {
		*buffer = (pos + 1);
	}
	*attribute = a;
	*value = v;
	return ret;
}


//
//	This doesn't accept 0x if the radix is 16. The overflow code assumes
//	a 2's complement architecture
//
#ifndef strtol
static long
strtol(char *string, char **endptr, int radix)
{
	char *s;
	long value;
	long new_value;
	int sign;
	int increment;

	value = 0;
	sign = 1;
	s = string;

	if ((radix == 1) || (radix > 36) || (radix < 0)) {
		goto done;
	}

	/* skip whitespace */
	while ((*s == ' ') || (*s == '\t') || (*s == '\n') || (*s == '\r')) {
		s++;
	}

	if (*s == '-') {
		sign = -1;
		s++;
	} else if (*s == '+') {
		s++;
	}

	if (radix == 0) {
		if (*s == '0') {
			s++;
			if ((*s == 'x') || (*s == 'X')) {
				s++;
				radix = 16;
			} else
				radix = 8;
		} else
			radix = 10;
	}

	/* read number */
	while (1) {
		if ((*s >= '0') && (*s <= '9'))
			increment = *s - '0';
		else if ((*s >= 'a') && (*s <= 'z'))
			increment = *s - 'a' + 10;
		else if ((*s >= 'A') && (*s <= 'Z'))
			increment = *s - 'A' + 10;
		else
			break;

		if (increment >= radix)
			break;

		new_value = value * radix + increment;
		// detect overflow 
		if ((new_value - increment) / radix != value) {
			s = string;
			value = -1 >> 1;
			if (sign < 0)
				value += 1;

			goto done;
		}

		value = new_value;
		s++;
	}

      done:
	if (endptr)
		*endptr = s;

	return value * sign;
}
#endif


//
//  Parse a chunk extension, detect overflow.
//  There are two error cases:
//  1) If the conversion would require too many bits, a -1 is returned.
//  2) If the conversion used the correct number of bits, but an overflow
//     caused only the sign bit to flip, then that negative number is
//     returned.
//  In general, any negative number can be considered an overflow error.
//
long
get_chunk_size(char *b)
{
	long chunksize = 0;
	size_t chunkbits = sizeof(long) * 8;

	/* skip whitespace */
	while ((*b == ' ') || (*b == '\t') || (*b == '\n') || (*b == '\r')) {
		b++;
	}

	/* Skip leading zeros */
	while (*b == '0') {
		++b;
	}

	while (isxdigit(*b) && (chunkbits > 0)) {
		int xvalue = 0;

		if (*b >= '0' && *b <= '9') {
			xvalue = *b - '0';
		} else if (*b >= 'A' && *b <= 'F') {
			xvalue = *b - 'A' + 0xa;
		} else if (*b >= 'a' && *b <= 'f') {
			xvalue = *b - 'a' + 0xa;
		}

		chunksize = (chunksize << 4) | xvalue;
		chunkbits -= 4;
		++b;
	}
	if (isxdigit(*b) && (chunkbits <= 0)) {
		/* overflow */
		return -1;
	}

	return chunksize;
}

//
//
//  This routine is borrowed from apache server
//
static int
lookup_builtin_method(const char *method, int len)
{
	// Note: the following code was generated by the "shilka" tool from
	//   the "cocom" parsing/compilation toolkit. It is an optimized lookup
	//   based on analysis of the input keywords. Postprocessing was done
	//   on the shilka output, but the basic structure and analysis is
	//   from there. Should new HTTP methods be added, then manual insertion
	//   into this code is fine, or simply re-running the shilka tool on
	//   the appropriate input.

	switch (len) {
	case 3:
		switch (method[0]) {
		case 'P':
			return (method[1] == 'U'
				&& method[2] == 'T'
				? HTTP_M_PUT : HTTP_M_UNKNOWN);
		case 'G':
			return (method[1] == 'E'
				&& method[2] == 'T'
				? HTTP_M_GET : HTTP_M_UNKNOWN);
		default:
			return HTTP_M_UNKNOWN;
		}

	case 4:
		switch (method[0]) {
		case 'H':
			return (method[1] == 'E'
				&& method[2] == 'A'
				&& method[3] == 'D'
				? HTTP_M_GET : HTTP_M_UNKNOWN);
		case 'P':
			return (method[1] == 'O'
				&& method[2] == 'S'
				&& method[3] == 'T'
				? HTTP_M_POST : HTTP_M_UNKNOWN);
		case 'M':
			return (method[1] == 'O'
				&& method[2] == 'V'
				&& method[3] == 'E'
				? HTTP_M_MOVE : HTTP_M_UNKNOWN);
		case 'L':
			return (method[1] == 'O'
				&& method[2] == 'C'
				&& method[3] == 'K'
				? HTTP_M_LOCK : HTTP_M_UNKNOWN);
		case 'C':
			return (method[1] == 'O'
				&& method[2] == 'P'
				&& method[3] == 'Y'
				? HTTP_M_COPY : HTTP_M_UNKNOWN);
		default:
			return HTTP_M_UNKNOWN;
		}

	case 5:
		switch (method[2]) {
		case 'T':
			return (memcmp(method, "PATCH", 5) == 0
				? HTTP_M_PATCH : HTTP_M_UNKNOWN);
		case 'R':
			return (memcmp(method, "MERGE", 5) == 0
				? HTTP_M_MERGE : HTTP_M_UNKNOWN);
		case 'C':
			return (memcmp(method, "MKCOL", 5) == 0
				? HTTP_M_MKCOL : HTTP_M_UNKNOWN);
		case 'B':
			return (memcmp(method, "LABEL", 5) == 0
				? HTTP_M_LABEL : HTTP_M_UNKNOWN);
		case 'A':
			return (memcmp(method, "TRACE", 5) == 0
				? HTTP_M_TRACE : HTTP_M_UNKNOWN);
		default:
			return HTTP_M_UNKNOWN;
		}

	case 6:
		switch (method[0]) {
		case 'U':
			switch (method[5]) {
			case 'K':
				return (memcmp(method, "UNLOCK", 6) == 0
					? HTTP_M_UNLOCK : HTTP_M_UNKNOWN);
			case 'E':
				return (memcmp(method, "UPDATE", 6) == 0
					? HTTP_M_UPDATE : HTTP_M_UNKNOWN);
			default:
				return HTTP_M_UNKNOWN;
			}
		case 'R':
			return (memcmp(method, "REPORT", 6) == 0
				? HTTP_M_REPORT : HTTP_M_UNKNOWN);
		case 'D':
			return (memcmp(method, "DELETE", 6) == 0
				? HTTP_M_DELETE : HTTP_M_UNKNOWN);
		default:
			return HTTP_M_UNKNOWN;
		}

	case 7:
		switch (method[1]) {
		case 'P':
			return (memcmp(method, "OPTIONS", 7) == 0
				? HTTP_M_OPTIONS : HTTP_M_UNKNOWN);
		case 'O':
			return (memcmp(method, "CONNECT", 7) == 0
				? HTTP_M_CONNECT : HTTP_M_UNKNOWN);
		case 'H':
			return (memcmp(method, "CHECKIN", 7) == 0
				? HTTP_M_CHECKIN : HTTP_M_UNKNOWN);
		default:
			return HTTP_M_UNKNOWN;
		}

	case 8:
		switch (method[0]) {
		case 'P':
			return (memcmp(method, "PROPFIND", 8) == 0
				? HTTP_M_PROPFIND : HTTP_M_UNKNOWN);
		case 'C':
			return (memcmp(method, "CHECKOUT", 8) == 0
				? HTTP_M_CHECKOUT : HTTP_M_UNKNOWN);
		default:
			return HTTP_M_UNKNOWN;
		}

	case 9:
		return (memcmp(method, "PROPPATCH", 9) == 0
			? HTTP_M_PROPPATCH : HTTP_M_UNKNOWN);

	case 10:
		switch (method[0]) {
		case 'U':
			return (memcmp(method, "UNCHECKOUT", 10) == 0
				? HTTP_M_UNCHECKOUT : HTTP_M_UNKNOWN);
		case 'M':
			return (memcmp(method, "MKACTIVITY", 10) == 0
				? HTTP_M_MKACTIVITY : HTTP_M_UNKNOWN);
		default:
			return HTTP_M_UNKNOWN;
		}

	case 11:
		return (memcmp(method, "MKWORKSPACE", 11) == 0
			? HTTP_M_MKWORKSPACE : HTTP_M_UNKNOWN);

	case 15:
		return (memcmp(method, "VERSION-CONTROL", 15) == 0
			? HTTP_M_VERSION_CONTROL : HTTP_M_UNKNOWN);

	case 16:
		return (memcmp(method, "BASELINE-CONTROL", 16) == 0
			? HTTP_M_BASELINE_CONTROL : HTTP_M_UNKNOWN);

	default:
		return HTTP_M_UNKNOWN;
	}

	// NOTREACHED
}


int
parse_http_host_line(char *buffer, size_t size, http_request_t * req)
{
	int ret = PARSE_ERROR;
	char c;

	aos_trace( "parsing host line:" );
	//aos_trace( "--------------------" );

	// terminate string
	c = buffer[size];
	buffer[size] = 0;

	//aos_trace( "%s", buffer );
	//aos_trace( "--------------------" );
	

	req->host_str = strstr(buffer,"Host: ");
	req->host_len=0;
	if(req->host_str)
	{
		req->host_str+=strlen("Host: ");
		char * tmp=strchr(req->host_str,'\r');
		req->host_len=tmp-req->host_str;
		ret = PARSE_OK;
	}

	buffer[size] = c;	// restore string
	return ret;
}

//
//   Parse http request line. (request line is terminated by CRLF)
//
//   RFC 2616, 19.3
//   Clients SHOULD be tolerant in parsing the Status-Line and servers
//   tolerant when parsing the Request-Line. In particular, they SHOULD
//   accept any amount of SP or HT characters between fields, even though
//   only a single SP is required.
//

int
parse_http_request_line(char *buffer, size_t size, http_request_t * req)
{
	char *pos, *method, *ver;
	char c;
	int ret = PARSE_ERROR;
	int len, major, minor;

	EnterFunction(5);

	aos_trace( "parsing request line: size=%d",size );
	// terminate string
	c = buffer[size];
	buffer[size] = 0;

	aos_trace( "%s", buffer );

	// try to get method
	method = skip_lws(buffer);
	if ((pos = strchr((char *) method, SP)) == NULL) {
		aos_alarm( eAosMD_TcpProxy, eAosAlarm_ProgErr, "not enough message for request methord parser");
		goto exit;
	}
	len = pos - method;

	req->method = lookup_builtin_method(method, len);
	if (req->method == HTTP_M_UNKNOWN) {
		aos_alarm( eAosMD_TcpProxy, eAosAlarm_ProgErr, "Unknow http method." );
		goto exit;
	}

	// get URI string
	req->uri_str = skip_lws(pos + 1);
	if ((pos = strchr((char *) req->uri_str, SP)) == NULL) {
		aos_alarm( eAosMD_TcpProxy, eAosAlarm_ProgErr, "not enough message for request URI parser");
		goto exit;
	}
	req->uri_len = pos - req->uri_str;

	// get http version
	req->version_str = skip_lws(pos + 1);

	if (strnicmp(req->version_str,
		     http_version_header, HTTP_VERSION_HEADER_LEN) != 0) {
		aos_alarm( eAosMD_TcpProxy, eAosAlarm_ProgErr, "version error");
		goto exit;
	}

	ver = (char *) req->version_str + HTTP_VERSION_HEADER_LEN;
	len = strlen(ver);

	// Avoid sscanf in the common case
	if (len == HTTP_VERSION_NUMBER_LEN
	    && isdigit(ver[0]) && ver[1] == '.' && isdigit(ver[2])) {
		req->version = HTTP_VERSION(ver[0] - '0', ver[2] - '0');
	} else if (2 == sscanf(ver, "%u.%u", &major, &minor)
		   && (minor < HTTP_VERSION(1, 0)))	// don't allow HTTP/0.1000
		req->version = HTTP_VERSION(major, minor);
	else
		req->version = HTTP_VERSION(1, 0);
	
	
	pos = strchr( req->version_str, '\n' );
	if ( pos == NULL )
	{
		aos_alarm( eAosMD_TcpProxy, eAosAlarm_ProgErr, "request line error %s", buffer );
		goto exit;
	}

	req->parsed_len = pos - req->message + 1;
	
	ret = PARSE_OK;
exit:
	buffer[size] = c;	// restore string
	return ret;
}


/****************************************************************************
*
* register_mime_parser - register a http mime header parser
*
*/
static void
register_mime_parser(HTTP_MIME_PARSER parser, char *mime_str)
{
	int index;
	http_mime_parse_t *parse_entry, *new_entry;

	assert(parser != NULL);
	assert(mime_str != NULL);

	index = strlen(mime_str);
	if (index >= MAX_MIME_HEADER_STRING_LEN)
		return;

	parse_entry = &http_mime_parse_table[index];
	if (parse_entry->parser == NULL) {
		INIT_LIST_HEAD(&parse_entry->plist);
	} else {
		if (strnicmp
		    (mime_str, parse_entry->mime_header_string,
		     index) == 0) {
			return;
		}
		new_entry = kmalloc(sizeof(http_mime_parse_t), GFP_KERNEL);
		if (new_entry == NULL) {
			TCP_VS_ERR("Out of memory!\n");
			return;
		}
		list_add_tail(&new_entry->plist, &parse_entry->plist);
		parse_entry = new_entry;
	}

	strcpy(parse_entry->mime_header_string, mime_str);
	parse_entry->parser = parser;
}

/****************************************************************************
*
* transfer_encoding_parser - http mime header parser for "Transfer-Encoding"
*
*/
static void
transfer_encoding_parser(http_mime_header_t * mime, char *buffer)
{
	EnterFunction(6);

	if (strnicmp(buffer, "chunked", 7) == 0) {
		mime->transfer_encoding = 1;
		TCP_VS_DBG(6, "Transfer-Encoding: chunked\n");
	}

	LeaveFunction(6);
	return;
}


/****************************************************************************
*
* content_length_parser - http mime header parser for "Content-Length"
*
*/
static void
content_length_parser(http_mime_header_t * mime, char *buffer)
{
	EnterFunction(6);

	mime->content_length = strtol(buffer, NULL, 10);
	TCP_VS_DBG(6, "Content-Length: %d\n", mime->content_length);

	LeaveFunction(6);
	return;
}


/****************************************************************************
*
* connection_parser - http mime header parser for "Connection"
*
*/
static void
connection_parser(http_mime_header_t * mime, char *buffer)
{
	EnterFunction(6);

	if (strnicmp(buffer, "close", 5) == 0) {
		mime->connection_close = 1;
		TCP_VS_DBG(5, "Connection: close\n");
	}

	LeaveFunction(6);
	return;
}

/****************************************************************************
*
* content_type_parser - http mime header parser for "Content-type"
*
* Note: buffer should be a NULL terminated string.
*/
static void
content_type_parser(http_mime_header_t * mime, char *buffer)
{
	int sep_len;
	char *pos;

	EnterFunction(6);

	if (strnicmp(buffer, "multipart/byteranges", 20) == 0) {
		TCP_VS_DBG(6, "multipart/byteranges\n");
		pos = buffer + 20 + 1;	/* skip ';' */
		pos = skip_lws(pos + 1);
		if (strnicmp(pos, "boundary=", 9) != 0) {
			goto exit;
		}

		/* the rest of this line is THIS_STRING_SEPARATES */
		pos += 9;
		sep_len = strlen(pos);
		if ((mime->sep = kmalloc(sep_len + 1, GFP_KERNEL)) == NULL) {
			goto exit;
		}

		/* RFC 2046 [40] permits the boundary string to be quoted */
		if (pos[0] == '"' || pos[0] == '\'') {
			pos++;
			sep_len--;
		}
		strncpy(mime->sep, pos, sep_len);
		mime->sep[sep_len] = 0;
		TCP_VS_DBG(5, "THIS_STRING_SEPARATES : %s\n", mime->sep);
	}

      exit:
	LeaveFunction(6);
	return;
}

void
http_mime_parser_init(void)
{
	memset(http_mime_parse_table, 0, sizeof(http_mime_parse_table));
	register_mime_parser(transfer_encoding_parser, "Transfer-Encoding");
	register_mime_parser(content_length_parser, "Content-Length");
	register_mime_parser(connection_parser, "Connection");
	register_mime_parser(content_type_parser, "Content-type");
}

/******************************************************************************
* http_mime_parse - parse MIME line in a buffer
*
* This routine parses the MIME line in a buffer.
*
* NOTE: Some MIME headers (host, Referer) need be considered again, tbd.
*
*/
int
http_mime_parse(char *buffer, int len, http_mime_header_t * mime)
{
	char *pos, c;
	int l, ret = PARSE_OK;
	http_mime_parse_t *parse_entry, *pe;

	assert(buffer != NULL);

	/* terminate string */
	c = buffer[len];
	buffer[len] = 0;

	TCP_VS_DBG(5, "MIME Header: %s\n", buffer);

	buffer = skip_lws(buffer);
	if ((pos = strchr(buffer, ':')) == NULL) {
		ret = PARSE_ERROR;
		goto exit;
	}

	l = pos - buffer;
	if (l >= MAX_MIME_HEADER_STRING_LEN)
		goto exit;

	parse_entry = &http_mime_parse_table[l];

	if (parse_entry->parser == NULL) {	/* an unregistered mime header */
		goto exit;
	}

	assert(parse_entry->mime_header_string != NULL);

	if (strnicmp(parse_entry->mime_header_string, buffer, l) != 0) {
		struct list_head *list;
		int found = 0;
		list_for_each(list, &parse_entry->plist) {
			pe = list_entry(list, http_mime_parse_t, plist);
			if (strnicmp(pe->mime_header_string, buffer, l) ==
			    0) {
				parse_entry = pe;
				found = 1;
				break;
			}
		}
		if (!found)	/* an unregistered mime header */
			goto exit;
	}

	pos = skip_lws(pos + 1);
	parse_entry->parser(mime, pos);

      exit:
	buffer[len] = c;	/* restore string */
	return ret;
}

//
// Judge the Message Type. "GET" or "POST"
// 
int http_check_action(char* InBuff, int InBuffLen)
{
	char* p;
	p = InBuff;
	while ((*p!=0x20)&&(p < (InBuff + InBuffLen))) p++; //move p to first black
	if (p - InBuff > 4 )
	{
		return 0; // action name length is too long
	}
	
	if ((memcmp(InBuff, "GET", p - InBuff)!=0)&&(memcmp(InBuff, "POST", p - InBuff)!=0))	//if action name is not GET or POST
	{
		return 0;
	}
	
	return 1;
}

//
//	This function can pick up HostName from a http head buffer
//
int http_get_host(char *InBuff, unsigned int InBuffLen, char* OutBuff, unsigned int OutBuffLen, unsigned int* ReturnLength)
{
	char  *p =0;
	char *q =0;
	char *m = 0;
	
	if ( http_check_action(InBuff, InBuffLen) == 0 )
	{
		return 0;
	}
	p = InBuff;
	while (p <= (InBuff + InBuffLen))
	{
		if ((memcmp(p, "Host: ", 6) == 0) || (memcmp(p, "host: ", 6)==0))
			break;
		p++;
	}
	if (p > (InBuff + InBuffLen))
		return 0;
	
	p += 6;
	q = p;
	while (q <= (InBuff + InBuffLen))
	{
		if (memcmp(q, "\r\n", 2)==0)
			break;
		q++;
	}
	
	if (q >= (InBuff + InBuffLen))
		return 0;
	
	m = q;
	while (m != p)
	{
		if (*m == ':')
			break;
		m--;
	}
	
	if (m == p)
	{
		*ReturnLength = (int)(q - p);
		if (*ReturnLength > OutBuffLen)
			return 0;
		memcpy(OutBuff, p, *ReturnLength);
		return 1;
	}else
	{
		*ReturnLength = (int)(m - p);
		if (*ReturnLength > OutBuffLen)
			return 0;
		memcpy(OutBuff, p, *ReturnLength);
		return 1;
	}
	
	
}

//
//	This function can pick up "port" from a http head buffer if it exists.
//
int http_get_port(char* InBuff, int InBuffLen, unsigned short *nPort)
{
	unsigned int port = 0;
	char  *p,*q, *m;
	char PortStr[6];
	memset(PortStr, 0x00, 6);
	
	*nPort = 80;	
	if ( http_check_action(InBuff, InBuffLen) == 0 )
	{
		return 0;
	}
	p = InBuff;
	
	while (p <= (InBuff + InBuffLen))
	{
		if (memcmp(p, "Host:", 5) == 0)
			break;
		p++;
	}
	
	if (p == (InBuff + InBuffLen))
		return 0;
	
	p += 5;
	
	q = p;
	while (q <= (InBuff + InBuffLen))
	{
		if (memcmp(q, "\r\n", 2) == 0)
			break;
		q++;
	}
	
	if (q == (InBuff + InBuffLen))
		return 0;
	
	m = p;
	while (m != q)
	{
		if (*m == ':')
			break;
		m++;
	}
	
	if (m == q)
	{
		return 1;	
	}
	else
	{	
		
		m++;
		memcpy(PortStr, m, (unsigned int)(q - m));
		sscanf(PortStr, "%u", &port);
		*nPort = (unsigned short)port;
		return 1;
	}
	
}

int aos_http_header_parser(uint8 *buffer, int buffer_len, struct http_request_s *req)
{
	req->message=buffer;
	aos_trace("will parse header, size=%d, str=%sEND",
			buffer_len,
			buffer
			);
	
	if ( PARSE_OK != parse_http_request_line( buffer, buffer_len, req ) )
	{
		aos_alarm( eAosMD_TcpProxy, eAosAlarm_ProgErr, "http request parse failed" );
		return -1;
	}	

	aos_trace("parse heaer %d ok , will parse host line, size=%d, str=%sEND",
			req->parsed_len,
			buffer_len-req->parsed_len,
			(char*)(req->message+req->parsed_len)
			);
	if ( PARSE_OK != parse_http_host_line( (char*)(req->message+req->parsed_len), buffer_len-req->parsed_len, req) )
	{
		aos_alarm( eAosMD_TcpProxy, eAosAlarm_ProgErr, "http host parse failed" );
		return -1;
	}

	/*
	if ( PARSE_OK != http_mime_parse( (char*)(req->message+req->parsed_len), buffer_len-req->parsed_len, &req->mime) )
	{
		aos_alarm( eAosMD_TcpProxy, eAosAlarm_ProgErr, "http mime parse failed" );
		return -1;
	}
	*/

	return 0;
}

