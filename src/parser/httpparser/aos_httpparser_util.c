#include "aos_httpparser_util.h"
#include <string.h>

static int initialized = 0;

void
parser_init()
{
	if (initialized == 0) {
		initialized = 1;
		
		aos_parser_init();
	}
}

void
http_req_init(http_req_t* req, char* in, uint32_t in_len)
{
	bzero(req, sizeof(http_req_t));
	
	req->parser_state = REQ_PARSER_INITIAL;
	req->req_p = in;
	req->total_len = in_len;
	req->parsed_len = 0; 
}

void
http_res_init(http_res_t* res, char* in, uint32_t in_len)
{
	bzero(res, sizeof(http_res_t));
	
	res->parser_state = RES_PARSER_INITIAL;
	res->res_p = in;
	res->total_len = in_len;
	res->parsed_len = 0; 
}

parser_result_t
http_req_parse(http_req_t* req)
{	
	char* temp_p;
	int temp_len;
	temp_len = 0;
	
	while (1) { /* parser method line */
		if (req->parsed_len > req->total_len)
			return HTTP_NEED_DATA;
			
		req->parser_state = req_parser_next(req->parser_state, req->req_p[req->parsed_len]);
	    switch (req->parser_state) {
			case GLOBAL_HTTP_PARSING_FAIL:
				
				return HTTP_PARSE_ERROR;
			
			case GLOBAL_HTTP_PARSING_DONE:
				
				return HTTP_PARSER_EOF;
				
			case REQ_PARSER_GET:
				req->http_method = HTTP_GET_METHOD;
				
				break;
	
	        case REQ_PARSER_POST:
				req->http_method = HTTP_POST_METHOD;
				
				break;

	        case REQ_PARSER_HEAD:
				req->http_method = HTTP_HEAD_METHOD;
				
				break;

	        case REQ_PARSER_PUT:
				req->http_method = HTTP_PUT_METHOD;
				
				break;

	        case REQ_PARSER_TRACE:
				req->http_method = HTTP_TRACE_METHOD;
				
				break;

	        case REQ_PARSER_DELETE:
				req->http_method = HTTP_DELETE_METHOD;
				
				break;

	        case REQ_PARSER_CONNECT:
				req->http_method = HTTP_CONNECT_METHOD;
				
				break;

	        case REQ_PARSER_OPTIONS:
				req->http_method = HTTP_OPTIONS_METHOD;
				
				break;

			case REQ_PARSER_UNSUPPORT_METHOD_END:
				req->http_method = HTTP_UNSUPPORTED_METHOD;
				
				break;
			
			case REQ_PARSER_URI_START:
				req->uri_p = req->req_p + req->parsed_len;
				req->uri_len = 1;
				
				break;
				
			case REQ_PARSER_URI:
				req->uri_len ++;
				
				break;
							
			case REQ_PARSER_VER_DIGITAL_START:
				/* start getting version */
				temp_p = req->req_p + req->parsed_len + 1;
				
				break;

			case REQ_PARSER_VER_DIGITAL:	
				
				temp_len ++;
				
				break;
				
			case REQ_PARSER_VER_END:
				/* get version number */
				if (memcmp("1.1", temp_p, temp_len) == 0) {        
					req->http_ver = HTTP_1_1;

				} else if (memcmp("1.0", temp_p, temp_len) == 0) {
					req->http_ver = HTTP_1_0;
				} else if (memcmp("0.9", temp_p, temp_len) == 0) {
					req->http_ver = HTTP_0_9;
				} else {
					req->http_ver = HTTP_UNSUPPORTED;
				}
				
				/* continue parsing */
				
				break;
			
			case REQ_PARSER_CONTENT_LENGTH:
				
				req->content_len = req->content_len*10 + (int)req->req_p[req->parsed_len] - '0';
				
				break;
					
			default:
				
				break;
		}
		
	req->parsed_len ++;
	}
	
	return HTTP_PARSE_ERROR;
}


parser_result_t
http_res_parse(http_res_t* res)
{	
	char* temp_p;
	int temp_len;
	temp_len = 0;        
	
	while (1) { /* parser method line */
		if (res->parsed_len > res->total_len)
			return HTTP_NEED_DATA;
			
		res->parser_state = res_parser_next(res->parser_state, res->res_p[res->parsed_len]);
	    switch (res->parser_state) {
			case GLOBAL_HTTP_PARSING_FAIL:
				
				return HTTP_PARSE_ERROR;
			
			case GLOBAL_HTTP_PARSING_DONE:

				return HTTP_PARSER_EOF;
				
			case RES_PARSER_STATUS_CODE:
				res->status_code = res->status_code*10 + (int)res->res_p[res->parsed_len] - '0';
				
				break;
			
			case RES_PARSER_VER_DIGITAL_START:
				/* start getting version */
				temp_p = res->res_p + res->parsed_len + 1;
				
				break;
				
			case RES_PARSER_VER_DIGITAL:	
				temp_len ++;
				
				break;
				
			case RES_PARSER_VER_END:
				/* get version number */
				if (memcmp("1.1", temp_p, temp_len) == 0) {
					res->http_ver = HTTP_1_1;

				} else if (memcmp("1.0", temp_p, temp_len) == 0) {
					res->http_ver = HTTP_1_0;
				} else if (memcmp("0.9", temp_p, temp_len) == 0) {
					res->http_ver = HTTP_0_9;
				} else {
					res->http_ver = HTTP_UNSUPPORTED;
				}
				
				/* continue parsing */
				
				break;
			
			case RES_PARSER_CONTENT_LENGTH:
				
				res->content_len = res->content_len*10 + (int)res->res_p[res->parsed_len] - '0';
				
				break;
					
			default:
				
				break;
		}
		
	res->parsed_len ++;
	}
	

	return HTTP_PARSE_ERROR;
}
