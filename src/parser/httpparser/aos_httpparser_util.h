#ifndef Parser_Httpparser_Aos_Httpparser_Util_H 
#define Parser_Httpparser_Aos_Httpparser_Util_H

#include "aos_parser.h"
#include <stdint.h>

/* structure */
typedef enum {
	HTTP_GET_METHOD = 100,		    	/* GET  */
	HTTP_POST_METHOD,			     			/* POST */
	HTTP_PUT_METHOD,		     				/* PUT */
	HTTP_DELETE_METHOD,							/* DELETE */
	HTTP_TRACE_METHOD,							/* TRACE */
	HTTP_CONNECT_METHOD,						/* CONNECT */
	HTTP_OPTIONS_METHOD,						/* OPTIONS */
	HTTP_HEAD_METHOD,								/* HEAD */
	HTTP_UNSUPPORTED_METHOD  				/* unsupported method */
} http_method_t;


typedef enum {
	HTTP_0_9 = 1000,					/* HTTP VER 0.9 */
	HTTP_1_0,					/* HTTP VER 1.0 */
	HTTP_1_1,					/* HTTP VER 1.1 */
	HTTP_UNSUPPORTED				/* Unsupported */
} http_ver_t;

typedef enum{
	HTTP_PARSER_EOF = 0,
	HTTP_NEED_DATA, 
	HTTP_PARSE_ERROR, 
} parser_result_t;

typedef struct http_req
{
	char* req_p;
	uint32_t total_len;
	uint32_t parsed_len;
	/* header total length */
	uint32_t header_len;
	uint16_t parser_state;
	
	http_method_t http_method;
	http_ver_t http_ver;
	
	/* uri */	 
	char* uri_p;
	int uri_len;
	
	/* host */
	char* host_p;
	int host_len;
	
	/* content_len */
	uint32_t content_len;
} http_req_t;

typedef struct http_res
{
	char* res_p;
	uint32_t total_len;
	uint32_t parsed_len;
	/* header total length */
	uint32_t header_len;
	uint16_t parser_state;
	
    /*http_state_t http_state;*/
	http_ver_t http_ver;
	
	/*connect state code*/
	uint32_t status_code;

	/* host */
	char* host_p;
	int host_len;
	
	/* content_len */
	uint32_t content_len;
} http_res_t;


extern void parser_init();

void http_req_init(http_req_t* req, char* in, uint32_t in_len);
parser_result_t http_req_parse(http_req_t* req);

void http_res_init(http_res_t* res, char* in, uint32_t in_len);
parser_result_t http_res_parse(http_res_t* res);

#endif

