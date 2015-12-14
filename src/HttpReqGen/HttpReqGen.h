#include "RandomUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define	MAX_METHOD_LEN	10
#define MAX_URI_LEN		1024
#define MAX_VERSION_LEN	16

static char* GenerateMethod();
static char* GeneratePath();
static char* GenerateQuery();
static char* GenerateURI();
static char* GenerateVer();
static char* gen_host_header();
static char* gen_accept_header();
static char* gen_accept_charset_header();
static char* gen_accept_encoding_header();
static char* gen_accept_language_header();
static char* gen_authorization_header();
static char* gen_expect_header();
static char* gen_form_header();
static char* gen_if_match_header();
static char* gen_if_modified_since_header();
static char* gen_if_none_match_header();
static char* gen_if_range_header();
static char* gen_if_unmodified_since_header();
static char* gen_max_forwards_header();
static char* gen_proxy_authorization_header();
static char* gen_range_header();
static char* gen_referer_header();
static char* gen_te_header();
static char* gen_user_agent_header();
char * gen_header_element();
int AosRandHttpRequest_CreateReqHeader(char *header, uint32_t buflen);
int AosRandHttpRequest_CreateReqLine(char *str, uint32_t buflen);
int AosRandHttpRequest_CreateBody(char *str, uint32_t buflen);
int AosRandHttpRequest(char *request, uint32_t buflen);

int gen_version(char *version);
int gen_method(char *method);
