////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_http_parser.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef _TCP_VS_HTTP_PARSER_H
#define _TCP_VS_HTTP_PARSER_H

typedef enum{
	PARSE_OK  = 0,
	PARSE_INCOMPLETE  =1, 
	PARSE_ERROR =2 , 
} parse_result_t;

#define CR	13
#define LF	10
#define SP	' '


//
// efgroup Methods List of Methods recognized by the server
// {

typedef enum{
	HTTP_M_UNKNOWN = 0,
	HTTP_M_GET,		//  RFC 2616: HTTP
	HTTP_M_HEAD,
	HTTP_M_PUT,
       HTTP_M_POST,
	HTTP_M_DELETE,
	HTTP_M_CONNECT,
	HTTP_M_OPTIONS,
	HTTP_M_TRACE,		// RFC 2616: HTTP
	HTTP_M_PATCH,		// no rfc(!)  ### remove this one?
	HTTP_M_PROPFIND,	// RFC 2518: WebDAV
	HTTP_M_PROPPATCH,	
	HTTP_M_MKCOL,		
	HTTP_M_COPY,		
	HTTP_M_MOVE,		
	HTTP_M_LOCK,		
	HTTP_M_UNLOCK,	 // RFC 2518: WebDAV
	HTTP_M_VERSION_CONTROL, 	// RFC 3253: WebDAV Versioning
	HTTP_M_CHECKOUT,
	HTTP_M_UNCHECKOUT,
	HTTP_M_CHECKIN,	
	HTTP_M_UPDATE,	
	HTTP_M_LABEL,		
	HTTP_M_REPORT,	
	HTTP_M_MKWORKSPACE,
	HTTP_M_MKACTIVITY,	
	HTTP_M_BASELINE_CONTROL,
	HTTP_M_MERGE,		
	HTTP_M_INVALID,	// RFC 3253: WebDAV Versioning
} http_methord_t;
#define HTTP_VERSION(major,minor)	(1000*(major)+(minor))

// HTTP MIME header
typedef struct http_mime_header_s {
	int content_length;
	int transfer_encoding;
	int connection_close;
	char *sep;			// THIS_STRING_SEPARATES
	ulong session_id;
} http_mime_header_t;

typedef struct http_request_s {
	const char *message;
	unsigned int message_len;
	unsigned int parsed_len;

	// request method
	int method;
	const char *method_str;
	unsigned int method_len;

	// request URI
	const char *uri_str;
	unsigned int uri_len;

	const char *host_str;
	unsigned int host_len;


	// http version
	int version;
	const char *version_str;
	unsigned int version_len;

	// MIME header
	http_mime_header_t mime;
} http_request_t;

// parser function prototypes
extern int parse_http_request_line(char *buffer, size_t len,
				   http_request_t * req);

extern void http_mime_parser_init(void);

extern int http_mime_parse(char *buffer, int len,
			   http_mime_header_t * mime);

extern char* search_sep(const char *s, int len, const char *sep);

extern long get_chunk_size(char *b);

extern int aos_http_header_parser(uint8 *buffer, int buffer_len, struct http_request_s *req);
	
extern int http_get_host(
	char *InBuff, 
	unsigned int InBuffLen, 
	char* OutBuff, 
	unsigned int OutBuffLen, 
	unsigned int* ReturnLength);

extern int http_get_port(char* InBuff, int InBuffLen, unsigned short *nPort);
#endif		// _TCP_VS_HTTP_PARSER_H

