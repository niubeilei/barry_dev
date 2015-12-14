#include "RandomUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#define	SERV_PORT	80

#define DOMAINS_PATH	"/root/AOS/src/HttpReqGen/Turtor/blacklists/ads/domains"
#define	URLS_PATH	"/root/AOS/src/HttpReqGen/Turtor/blacklists/ads/urls"
#define	MAX_THREAD		1	

struct thread_struct {
	char* host;
	char* message;
} example;

static void* sendmessage(struct stread_struct *example);
int is_ipaddr(char *buf);
int gen_request_urls();
int gen_request_domains();
