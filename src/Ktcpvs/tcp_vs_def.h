////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_def.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Ktcpvs_tcp_vs_def_h
#define Ktcpvs_tcp_vs_def_h

#include "aosUtil/Rwlock.h"
#include "aosUtil/Types.h"
#include "aosUtil/Mutex.h"
#include "KernelSimu/list.h"
#include "KernelSimu/atomic.h"
#include "aosLogFunc.h"
//#include "AppProc/http_proc/AutoLogin.h"

#define KTCPVS_IDENTNAME_MAXLEN      33//17
#define KTCPVS_SCHEDNAME_MAXLEN      33//17
#define PMI_SYSNAME_MAXLEN 256
#define TIME_CONSUMED_MAX 10 // ms

#define CLIENT_USERTYPE_USB 0x100
#define CLIENT_USERTYPE_518 0x200

#define MAX_CONCURRENT_CONNECTIONS 2048
/*
 *      TCPVS connection object
 */
struct socket;
struct tcp_vs_dest;
struct tcp_vs_service;
struct tcp_vs_config;

struct tcp_vs_ident {
	char name[KTCPVS_IDENTNAME_MAXLEN];
};


struct tcp_vs_config {
	/* the IP address and/or port to which the server listens */
	__u32       addr;
	__u16       port;

	/* scheduler name */
	char sched_name[KTCPVS_SCHEDNAME_MAXLEN];
  
	unsigned    timeout;    // timeout in ticks
	int         startservers;
	int         maxSpareServers;
	int         minSpareServers;
  
	// the max number of servers running
	int         maxClients;
	int         keepAlive;
	int         maxKeepAliveRequests;
	int         keepAliveTimeout;

	// address/port to redirect
	__u32       redirect_addr;
	__u16       redirect_port;

	//
	// Chen Ding, 07/13/2005
	//
	__u32	   	front_ssl_flags;// Client to virtual service is SSL
	__u32	   	back_ssl_flags;	// Virtual service to bacnend server is SSL
	__u16       app_type;  		// Application type
	__u8        pmi;        	// Turn on or trun off PMI access control switch
	__u8        dynamic_dst;	// if dynamic_dst=1, dst is decided by the first page, 
                          		// if 0, dst is selected from static dst list
	// liqin 10/21/2005
	char pmi_sysname[PMI_SYSNAME_MAXLEN];

};

#define PROXY_USERNAME_LEN 	48
#define PROXY_PASSWORD_LEN 	48

#define TCP_VS_START 		0
#define TCP_VS_LOGIN 		1
#define TCP_VS_PASSWORD	 	2
#define TCP_VS_NORMAL 		3
#define TCP_VS_LOG 		4
#define TCP_VS_END 		5

#define SHA_HASH_LENGTH		20

struct aos_mutex;

struct AosSslContext;
struct AosRsaPrivateKey;
struct AosCertChain;
struct tcp_vs_conn 
{
	struct list_head 		n_list;			// d-linked list head
	__u32 					addr;			// client address
	unsigned 				flags;			// status flag

	struct socket 			*csock;			// socket connected to client
	struct socket 			*dsock;			// socket connected to server
	struct tcp_vs_dest 		*dest;			// destination server 
	struct tcp_vs_service 	*svc;			// service it belongs to 

	char					*server_buff;	// Buffer for the server
	size_t					sbuff_len;		// Server buffer length
	char					*client_buff;	// Buffer for the client
	size_t					cbuff_len;		// Client buffer length
	//int					svr_block_size;	// Server current block size
	//int					clt_block_size;	// Client current block size

	// application data received from client will be delay sent to server
	char					*delay_to_server;
	size_t					delay_to_server_len;

	// application protocol processing data (http use)
	void *					app_data;	
	
	struct AosSslContext 	*front_context;
	struct AosSslContext 	*backend_context;
	uint32_t 				front_ssl_flags;
	uint32_t 				backend_ssl_flags;
	struct _X509_CERT_INFO	*front_cert;		// front ssl certificate 
	struct _X509_CERT_INFO	*backend_cert;		// backend ssl certificate 
	struct AosRsaPrivateKey	*front_prikey;		// fornt ssl PrivateKey;
	struct AosRsaPrivateKey	*backend_prikey;	// backend ssl PrivateKey;
	struct AosCertChain     *front_authorities; // authorites trusted by server

	atomic_t				refcnt;				// connection reference count

	int						client_delay;		// rcv data from client, but server is not ready
	int						server_delay;		// rcv data from server, but client is not ready
	unsigned char			isSecureUser;

	__u8		tcp_vs_status;
	__u32 		proxy_logintime;			
	char			proxy_username[PROXY_USERNAME_LEN];
	char			proxy_password[PROXY_PASSWORD_LEN];
	//add by zql, 08/07/2006
	KFILE		*bin_file_ptr;			//write log data file point
	VCHAR		log_buffer;					//Backup log data when need, ....
	//char		tcp_vs_inputbuf[512];
	//int		tcp_vs_inputbuflen;
	__u64		session_id;				//conn ID
	struct socket *daemon_sock;			//conn connect daemon to send c-s (and s-c) data
	char			cs_msg_to_daemon;		//send client to server message to hac daemon if this value isn't equal 0
	char			sc_msg_to_daemon;		//send server to client message to hac daemon if this value isn't equal 0
	//add by lwang
	int			session_time_out_flags;
	struct timer_list *session_timer;
	long int 		session_time;
	VCHAR        	auth_fail_infor;
	VCHAR		error_info_prefix;
};

/*
 *	The information about the KTCPVS service
 */
struct tcp_vs_service 
{
	struct list_head 		list;			// Virtual service list
	struct tcp_vs_ident 	ident;			// Service ID
	struct tcp_vs_config 	conf;			// Service Configuration
	struct tcp_vs_scheduler *scheduler;		// bound scheduler object 
	void 					*sched_data;	// scheduler application data 
	struct list_head 		destinations;	// Dest list	
	struct list_head 		connections;	// Dest list	
	__u32 					num_dests;
	struct list_head 		rule_list;		// Rule list
	__u32 					num_rules;
	aos_rwlock_t 			lock;			// Lock for dest list and rule list
	int 					start;			// Service execution control	
	int 					stop;			// Service execution control
	struct socket 			*mainsock;		// The service socket
	atomic_t 				conns;			// connection counter 
	atomic_t 				childcount;		// child counter
	atomic_t 				running;		// running flag 

	// Chen Ding, 08/26/2005
	struct aos_deny_page 	*deny_page;

	// Chen Ding, 09/04/2005
//	struct AosHttp_AloginConf alogin_config;
};

extern int tcp_vs_add_service(
				struct tcp_vs_ident *ident, 
				struct tcp_vs_config *conf, 
				char *errmsg);
extern int tcp_vs_del_service(struct tcp_vs_service *svc);

extern int tcp_vs_edit_service(
				struct tcp_vs_service *svc, 
				struct tcp_vs_config *conf, 
				char *errmsg);
extern int proxy_send_string( char *str, int strlen, struct tcp_vs_conn *conn );


#endif

