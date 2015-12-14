#include "HttpReqGen.h"

static struct rand_method_t {
	char* method_name;
	int weight;
} all_methods[] = {{"GET", 40}, {"POST", 30}, {"HEAD", 10}, {"PUT", 15}, {"TRACE", 5}};

static char* GenerateMethod()
{
	static char method[MAX_METHOD_LEN];
	int num = sizeof(all_methods)/sizeof(all_methods[0]);
	int i, k = 0;

	int rand_num = AosRandInt(0,100);
	for (i = 0; i < num; i++) {
		k += all_methods[i].weight;
		if (rand_num <= k) {
			strcpy(method, all_methods[i].method_name);
			break;
		}
	}
	
	if (i == num)
		return NULL;

	return method;
}

static char* GeneratePath()
{
	static char path[256];
	int i, num_slash, len;
	
	len = AosRandWord(path, 256, 1, 255);
    if (len < 0) 
		return NULL;

	int rand_num = AosRandInt(0, 100);
	if (rand_num < 30)
		num_slash = 1;
	else if (rand_num < 60)
		num_slash = 2;
	else if (rand_num < 80) 
		num_slash = 3;
	else
		num_slash = 4;
	
	path[0] = '/';
	for (i = 0; i < num_slash - 1; i++) { 
		//int pos = AosRandInt(2, len - 1);
		int pos = AosRandInt(2, 20);//modify by xyb 2006/11/24 just want the pos number better
		if (path[pos] == '/' || path[pos - 1] == '/' || (pos < len - 1 && path[pos + 1] == '/')) {
			i--;
			continue;
		}
		path[pos] = '/';
	}
	return path;
}

static char* GenerateQuery()
{
	static char query[256];
	int i, num_and, len;
/*	
	len = AosRandWord(query, MAX_URI_LEN, 0, MAX_URI_LEN);
    if (len < 0) 
		return NULL;*/

	strcpy(query, "?");
	
	int rand_num = AosRandInt(0, 100);
	if (rand_num < 50)
		num_and = 1;
	else if (rand_num < 95)
		num_and = 2;
	else if (rand_num < 98) 
		num_and = 3;
	else
		num_and = 4;

	/* the length has no way to exceed over MAX_URI_LEN */
	for (i = 0; i < num_and; i++) { 
		int len, pos;
		char word[32];

		len = AosRandWord(word, 32, 3, 32);
		if (len < 0)
			return NULL;

		pos = AosRandInt(1, len - 2);
		word[pos] = '=';

		if (i == 0)
			strcat(query, word);
		else {
			strcat(query, "&");
			strcat(query, word);
		}
	}
	
	return query;
}

static char* GenerateURI()
{
	static char uri[MAX_URI_LEN];
	char *path, *query;
	
	path = GeneratePath();
	if (path == NULL)
		return NULL;
	
	strcpy(uri, path);
	if (AosRandBool2(70)) 
		return uri;	
	else {
		query = GenerateQuery();
		if (strlen(uri) + strlen(query) > MAX_URI_LEN)
			return uri;

		return strcat(uri, query);
	}
}

static char* GenerateVer()
{
	static char ver[MAX_VERSION_LEN];

	strcpy(ver, "HTTP/");
	
	if (AosRandBool2(70))
		strcat(ver, "1.1");
	else
		strcat(ver, "1.0");

	return ver;
		
}

static char* gen_host_header()
{
	static char host[144];
	char host_value[128];	
	
	if (AosRandBool2(70)) {
		AosRandDomainName(host_value, 128);
	} else {
		struct in_addr ip;
		int ipaddr = random();
		ip.s_addr = (uint32_t)ipaddr;
		strcpy(host_value, inet_ntoa(ip));
	}

	sprintf(host, "HOST: %s\r\n", host_value); 
	return host;
}

static char* gen_accept_header()
{
	static char accept[256];
	char word[10];

	AosRandWord(word, 10, 1, 10);
	strcpy(accept, "Accept: ");
	strcat(accept, word);
	strcat(accept, "/");
	if (AosRandBool()) {
		AosRandWord(word, 10, 1, 10);
		strcat(accept, word); 
	} else { 
		strcat(accept, "*");
	}

	if (AosRandBool()) {
		strcat(accept, ";level=");
		int i = AosRandInt(1, 4);
		char c = i + '0'; 
		char test[2];
		test[0] = c;
		test[1] = '\0';
		strcat(accept, test);
	}
	strcat(accept, "\r\n");
	return accept;
}

static char* gen_accept_charset_header()
{
	static char accept_charset[64] = {"Accept-Charset: iso-8859-5, unicode-1-1;q=0.8\r\n"};
	return accept_charset; 
}

static char* gen_accept_encoding_header()
{
	static char accept_encoding[64] = {"Accept-Encoding: compress, gzip\r\n"};
	return accept_encoding;
}

static char* gen_accept_language_header()
{
	static char accept_language[64] = {"Accept-Language: da, en-gb;q=0.8, en;q=0.7\r\n"};
	return accept_language;
}

static char* gen_authorization_header()
{
	static char authorization[64] = {"Authorization : credentials\r\n"};		
	return authorization;
}

static char* gen_expect_header()
{
	static char expect[64]  = {"Expect : 1#expectation\r\n"};
	return expect;
}
static char* gen_form_header()
{
	static char form[64];
	char word[20];

	strcpy(form, "From: ");
	AosRandWord(word, sizeof(word), 3, 20);
	strcat(form, word);
	strcat(form, "\r\n");
	return form;
}

static char* gen_if_match_header()
{
	static char if_match[64];
	char word[10]; 

	strcpy(if_match, "IF-Match : ");
	AosRandWord(word, sizeof(word), 3, 10);
	strcat(if_match, word);
	strcat(if_match, "\r\n");

	return if_match;
}

static char *time_area[] = {"UTC","GMT","EDT","EST","CDT","CST","MDT","MST","PDT","PST"};
static char* gen_if_modified_since_header()
{ 
	static char if_modified_since[128];
	time_t time_int = random();

	char * rand_time = ctime(&time_int);
	rand_time[strlen(rand_time) - 1] = ' ';

	strcpy(if_modified_since, "If-Modified-Since: ");
	strcat(if_modified_since, rand_time);

	int len = sizeof(time_area)/sizeof(time_area[0]);
	int rand_num = AosRandInt(0, len - 1);

	strcat(if_modified_since, time_area[rand_num]);
	strcat(if_modified_since, "\r\n"); 

	return if_modified_since;
}

static char* gen_if_none_match_header()
{
	static char if_none_match[64];
	char word[10]; 

	strcpy(if_none_match, "IF-None-Match : ");
	AosRandWord(word, sizeof(word), 3, 10);
	strcat(if_none_match, word);
	strcat(if_none_match, "\r\n");

	return if_none_match;
}

static char* gen_if_range_header()
{
	static char if_range[100];
	long int time_int;

	time_int = time(NULL);
	char * rand_time = ctime(&time_int);
	rand_time[strlen(rand_time) - 1] = ' ';

	strcpy(if_range, "If-Range: ");
	strcat(if_range, rand_time);

	int len = sizeof(time_area)/sizeof(time_area[0]);
	int rand_num = AosRandInt(0, len - 1);

	strcat(if_range, time_area[rand_num]);
	strcat(if_range, "\r\n");

	return if_range;
}

static char* gen_if_unmodified_since_header()
{
	static char if_unmodified_since[100];
	long int time_int;

	time_int =  time(NULL);
	char * rand_time = ctime(&time_int);
	rand_time[strlen(rand_time) - 1] = ' ';

	strcpy(if_unmodified_since, "If-Unmodified-Since:");
	strcat(if_unmodified_since, rand_time);

	int len = sizeof(time_area)/sizeof(time_area[0]);
	int rand_num = AosRandInt(0, len - 1);

	strcat(if_unmodified_since, time_area[rand_num]);
	strcat(if_unmodified_since, "\r\n");

	return if_unmodified_since;
}

static char* gen_max_forwards_header()
{
	static char max_forwards[64] = {"Max-Forwards : 1*DIGIT\r\n"};
	return max_forwards;
}
static char* gen_proxy_authorization_header()
{
	static char proxy_authorization[64] = {"Proxy-Authorization : credentials\r\n"};	
	return proxy_authorization;
}
static char* gen_range_header()
{
	static char range[64] = {"Range : ranges-specifier\r\n"};
	return range;
}
static char* gen_referer_header()
{
	static char referer[128] = {"Referer: http://www.w3.org/hypertext/DataSources/Overview.html\r\n"};
	return referer;
}
static char* gen_te_header()
{
	static char te[64] = {"TE : #( t-codings\r\n"};
	return te;
}
static char* gen_user_agent_header()
{
	static char user_agent[64] = {"User-Agent: CERN-LineMode/2.15 libwww/2.17b3\r\n"};
	return user_agent;
}

static struct header_gen_t {
	char* (*gen_header)();
} all_headers[] = {
	gen_accept_header,
	gen_accept_charset_header,
	gen_accept_encoding_header,
	gen_accept_language_header,
	gen_authorization_header,
	gen_expect_header,
	gen_form_header,
	gen_if_match_header,
	gen_if_modified_since_header,
	gen_if_none_match_header,
	gen_if_range_header,
	gen_if_unmodified_since_header,
	gen_max_forwards_header,
	gen_proxy_authorization_header,
	gen_range_header,
	gen_referer_header,
	gen_te_header,
	gen_user_agent_header
};

char * gen_header_element()
{
	char header_element[1024];
	char element[128];
	int total_headers = sizeof(all_headers)/sizeof(all_headers[0]);
	//int rand_num = AosRandInt(0, total_headers);
	int rand_num = AosRandInt(0, 10);//modify by xyb 2006/11/24 ,just want to the rand_num be better

	int i, h, tmp[rand_num];
	for (i=0; i < rand_num; i++) {
		int j =	AosRandInt(0, total_headers - 1);
		for (h = 0; h < i; h++) {
			if (tmp[h] == j) 
				break;
		}
		if (h != i) {
			i --;
			continue;
		}
		tmp[i] = j;
		sprintf(element, "%s\r\n", all_headers[j].gen_header());
		if (strlen(header_element) + strlen(element) > 1024)
			break;

		strcat(header_element, element);
	}
	return header_element;
}
int AosRandHttpRequest_CreateReqHeader(char *header, uint32_t buflen)
{
	char * header_host = gen_host_header();
	strncpy(header, header_host, buflen - 1);
	char * header_element = gen_header_element();
	strcat(header, header_element);
	return strlen(header);
}

int AosRandHttpRequest_CreateReqLine(char *str, uint32_t buflen)
{
	snprintf(str, buflen - 1, "%s %s %s\r\n", GenerateMethod(), GenerateURI(), GenerateVer());
	str[buflen - 1] = 0;

	str[buflen - 2] = '\n';
	str[buflen - 3] = '\r';
	return strlen(str);
}

int AosRandHttpRequest_CreateBody(char *str, uint32_t buflen)
{
	if (buflen == 0) {
		str[0] = '\0';
		return 0;
	}
	AosRandStr(str, buflen, 0, buflen);
	return strlen(str);
}
int AosRandHttpRequest(char *request, uint32_t buflen)
{
	char reqline[1024];
	char header[1024];
	char body[3];

	AosRandInitialize(0);
	AosRandHttpRequest_CreateReqLine(reqline, 1024);
	AosRandHttpRequest_CreateReqHeader(header, 1024);
	AosRandHttpRequest_CreateBody(body, 0);

	sprintf(request, "%s%s\r\n%s", reqline,header,body);
}

int gen_version(char *version)
{
	strcpy(version, GenerateVer());
	return 0;
}
int gen_method(char *method)
{
	strcpy(method, GenerateMethod());
	return 0;
}
