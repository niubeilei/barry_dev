#include "aos_httpparser_util.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define request

#define TEST_FILE_NAME   "/tmp/test"

int main()
{
	struct stat st;
	FILE* fp;
	http_req_t req;
//	http_res_t res;
	char* buf;
//	int tem;

	if (stat(TEST_FILE_NAME, &st) < 0) {
		printf("Can't get the status of file %s\n", TEST_FILE_NAME);
		return -1;
	}
	
	buf = (char*)malloc(st.st_size + 1);
	if (!buf) {
		printf("Can't allocate memory\n");
		return -1;
	}
	
	fp = fopen(TEST_FILE_NAME, "r");
	if (!fp) {
		printf("File %s can't be opened.\n", TEST_FILE_NAME);
		return -1;
	}
	
	fread(buf, 1, st.st_size, fp);
	fclose(fp);
	
	/* start parser */
	parser_init();


#ifdef request
	http_req_init(&req, buf, st.st_size);
    tem = http_req_parse(&req);
	
	/* print result */
	printf("Parser result:\n");
	printf("	Total statistics:\n");
	printf("		total len: %d, parsed len: %d\n\n", req.total_len, req.parsed_len);
	
	printf("	Parsed Result:\n");
	if (req.http_method != 0) {
		printf("		Method = %d\n", req.http_method);
	}
	
	if (req.content_len != 0) {
		printf("		Content_Len = %d\n", req.content_len);
	}
	
	if (req.uri_len != 0) {
		char temp[256];
		strncpy(temp, req.uri_p, req.uri_len);
		temp[req.uri_len] = 0;
		printf("		uri = %s, uri_len = %d\n", temp, req.uri_len);
	}
	
	if (req.http_ver != 0) {
		printf("		version = %d\n", req.http_ver);
	}
	
//	printf ("actual ret = %d\n",tem);

#else
	http_res_init(&res, buf, st.st_size);
	tem = http_res_parse(&res);
	
	/* print result */
	printf("Parser result:\n");
	printf("	Total statistics:\n");
	printf("		total len: %d, parsed len: %d\n\n", res.total_len, res.parsed_len);
	
	printf("	Parsed Result:\n");
	if (res.status_code != 0) {
		printf("		Status_Code = %d\n", res.status_code);
	}
	
	if (res.content_len != 0) {
		printf("		Content_Len = %d\n", res.content_len);
	}
	
	if (res.http_ver != 0) {
		printf("		version = %d\n", res.http_ver);
	}
	
//	printf ("actual ret = %d\n",tem);
#endif
	
	free(buf);
	
	return 0;
}




