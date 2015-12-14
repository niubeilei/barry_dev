////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: app_ftp.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "proxy_service.h"
#include "proxy_app.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"

#include <linux/random.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zzh");

static int com2n( __u32 * ip, __u16 *port, unsigned char * a)
{
	*ip = ntohl( a[3] + (a[2]<<8) + (a[1]<<16) + (a[0]<<24) );
	*port = ( (a[4]<<8) + a[5] );

	return 0;
}


static int n2com( __u32 ip, __u16 port, unsigned char * a)
{
	
	ip = ntohl(ip);
	port = port;

	a[0] = ( ip & 0xFF000000) >> 24;
	a[1] = ( ip & 0x00FF0000) >> 16;
	a[2] = ( ip & 0x0000FF00) >> 8;
	a[3] = ( ip & 0x000000FF);
	a[4] = ( port & 0xFF00) >> 8;
	a[5] = ( port & 0x00FF);

	return 0;
}


static int extract_address( char * arg, __u32 * ip, __u16 * port )
{
	char * tmp;
	unsigned char n[6];
	int	i;

	for(i=0;i<6;i++)
	{
		n[i]=simple_strtol(arg,&tmp,0);
		arg=tmp+1;
	}
	
	com2n(ip,port,n);

	return 0;
}

static __u16 find_unused_port( __u32 ip )
{
	//this way is not efficient, we need to change it later. use tcp_v4_get_port()
	struct socket	*sock;
	struct sockaddr_in	addr;
	__u16 port;
	__u32 rand;
	int	i;
	int	error;

	addr.sin_addr.s_addr = ip;
	addr.sin_family = AF_INET;

	error=sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&sock);
	if(error<0)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error during creation of socket");
		return -1;
	}

	for(i=0;i<50000;i++)
	{
		get_random_bytes(&rand,sizeof(__u32));
		port=10000+(i+rand)%50000;
		addr.sin_port=htons(port);
		error = sock->ops->bind(sock,(struct sockaddr*)&addr,sizeof(addr));
		if(!error)
			break;
		if(error != -EADDRINUSE)
		{
			port=0;
			break;
		}
		
	}
	sock_release(sock);
	return port;
}



static int AosFtp_parseABOR( ProxyObject_t * po, char * buffer, int *len )
{

	return 0;
}

static inline int get_appname( char * appname )
{
	int i;
	__u32	r;

	appname[PROXY_SERVICE_NAME_LEN]=0;
	do
	{
		for( i=0; i<PROXY_SERVICE_NAME_LEN; i++ )
		{
			get_random_bytes( &r, sizeof(__u32) );
			appname[i]= '0'+ r%10 ;
		}
	}
	while( AosProxy_getServiceByName(appname) );

	return 0;
}

static int AosFtp_parsePORT( ProxyObject_t * po, char * buffer, int *len )
{
	unsigned char	a[6];
	char	appname[PROXY_SERVICE_NAME_LEN+1];
	__u32	ip,ip2;
	__u16	port,port2;
	int		ret;
	struct sockaddr_in addr;


	aos_debug("parsing PORT\n");
	
	get_appname( appname );

	//po->backSock->ops->getname( po->backSock, (struct sockaddr*)&addr, &ret, 0 );
	// addr is network order
	ip = addr.sin_addr.s_addr; // network order
	port = find_unused_port( ip ); // port is host order 
	aos_debug("AosFtp_parsePort: proxy will listen on %u.%u.%u.%u:%d\n",NIPQUAD(ip),port);
	ret = AosProxy_serviceAdd(appname, "ftpdata", ip, port, po->svc->backWt, po->svc->frontWt,po->svc->scheduler, 0);
	if(ret)
	{
		aos_trace( KERN_EMERG "ftpdata service add failed");
		return -1;
	}
	else
	{

	}

	extract_address( &buffer[5], &ip2, &port2 );
	// now ip is the network order
	// port is the host order
	aos_debug("AosFtp_parsePort: the ftpdata server is %u.%u.%u.%u:%d\n",NIPQUAD(ip),port);
	ret = AosProxy_rsAdd(appname, ip2, port2, 0, 0);

	AosProxy_serviceStart( appname, 0 );

	n2com( ip, port, a );
	sprintf(buffer,"PORT %d,%d,%d,%d,%d,%d\r\n",a[0],a[1],a[2],a[3],a[4],a[5] );
	*len=strlen(buffer);
	aos_debug("parse PORT: rewrite PORT '%s'",buffer);

	return 0;
}


static int AosFtp_parsePASV( ProxyObject_t * po, char * buffer, int *len )
{

	return 0;
}


static int	strcasencmp(char * s1, char * s2, int n)
{
	int	i;
	for(i=0;i<n;i++)
	{
		if( (s1[i]-s2[i]) % ('a'-'A') )
			return 1;
	}
	return 0;
}


static int AosFtp_clientToServer( ProxyObject_t * po, char * buffer, int len )
{
	char	cmd[5];
	int	i;

	struct 
	{
		char	*cmd;
		int		( *func ) ( ProxyObject_t * po,char * buffer , int *len );
	}
	cmd_list[]=
	{
		{"ABOR", AosFtp_parseABOR},
		{"ACCT", 0},
		{"APPE", 0},
		{"ALLO", 0},
		{"AUTH", 0},
		{"CWD", 0},
		{"CDUP", 0},
		{"DELE", 0},
		{"EPRT", 0},
		{"EPSV", 0},
		{"FEAT", 0},
		{"HELP", 0},
		{"LIST", 0},
		{"MDTM", 0},
		{"MKD", 0},
		{"MODE", 0},
		{"MLFL", 0},
		{"MAIL", 0},
		{"MSND", 0},
		{"MSOM", 0},
		{"MSAM", 0},
		{"MRSQ", 0},
		{"MRCP", 0},
		{"NLST", 0},
		{"NOOP", 0},
		{"OPTS", 0},
		{"PASV", AosFtp_parsePASV},
		{"PORT", AosFtp_parsePORT},	
		{"PWD", 0},
		{"PASS", 0},
		{"QUIT", 0},
		{"REIN", 0},
		{"RETR", 0},
		{"RMD", 0},
		{"REST", 0},
		{"RNFR", 0},
		{"RNTO", 0},
		{"SMNT", 0},
		{"STRU", 0},
		{"SITE", 0},
		{"SYST", 0},
		{"STAT", 0},
		{"STOR", 0},
		{"SIZE", 0},
		{"STOU", 0},
		{"TYPE", 0},
		{"USER", 0},
		{"XCUP", 0},
		{"XCWD", 0},
		{"XMKD", 0},
		{"XPWD", 0},
		{"XRMD", 0},
		{0, 0}

	};
	
	/*
	if(*len>=eAosSSL_RecordMaxLen-500)
		*len=eAosSSL_RecordMaxLen-500-1;
	*/

	//buffer[*len]=0;

	if(len<4)
		return -1;
		
	for(i=0;i<4;i++)
		cmd[i]=buffer[i];
	cmd[4]=0;
	if(cmd[3]==10 || cmd[3]==13)
		cmd[3]=0;

	aos_eng_log(eAosMD_TcpProxy, "client_to_server: '%s'",cmd);
	
	i=0;
	do
	{
		if( strcasencmp(cmd_list[i].cmd,cmd,4)==0 )
		{
			if(cmd_list[i].func)
			{
				cmd_list[i].func( po, buffer, &len );
			}
			len = po->backWo->wt->sendData( po->backWo, buffer, len );
			return len;
		}
		i++;
	}
	while(cmd_list[i].cmd);

	aos_eng_log(eAosMD_TcpProxy,"client sending rubbish");

	return -1;

}

static int AosFtpdata_poReleased( ProxyService_t * svc )
{
	aos_debug("poReleased, release ftpdata svc\n");
	__AosProxy_serviceStop(svc);
	AosProxy_serviceRemove(svc);

	return 0;
}

// if the return value of this function is less than 0, 
// the lower layer should close this connection
static int
AosFtpdata_dataReceived( ProxyObject_t * po, struct sock * sk, char * buffer, int len )
{
	AosSockWrapperObject_t *wo;

	aos_debug( KERN_INFO "app ftpdata dataReceived(): receive %d, call the other end wo->wt->sendData() \n", len );	

	wo = ( sk==po->frontSk ) ? po->backWo : po->frontWo ;
	len = wo->wt->sendData( wo, buffer, len );
	aos_debug( KERN_INFO "app ftpdata dataReceived(): sent %d \n", len );	
	return len;
}


// if the return value of this function is less than 0, 
// the lower layer should close this connection
static int
AosFtp_dataReceived( ProxyObject_t * po, struct sock * sk, char * buffer, int len )
{
	AosSockWrapperObject_t *wo;

	aos_debug( KERN_INFO "app Ftp dataReceived(): receive %d, call the other end wo->wt->sendData() \n", len );	

	if( sk == po->frontSk )
	{
		
		len = AosFtp_clientToServer( po, buffer, len );

		return len;
	}

	wo = ( sk==po->frontSk ) ? po->backWo : po->frontWo ;
	len = wo->wt->sendData( wo, buffer, len );
	aos_debug( KERN_INFO "app Ftp dataReceived(): sent %d \n", len );	
	return len;
}

static int
AosFtp_readyAcceptData( ProxyObject_t * po, AosSockWrapperObject_t *wo )
{
	wo = ( wo == po->frontWo ) ? po->backWo : po->frontWo ; 
	return wo->wt->isWritable( wo ); 
}

static int
AosFtp_finReceived( ProxyObject_t * po, struct sock * sk )
{
	AosSockWrapperObject_t *wo;

	// received a fin from one side(sock)
	// we have no data to send to the other side
	// send fin to the other side
	aos_debug( KERN_INFO "app ftp finReceived()\n" );	
	wo = ( sk==po->frontSk ) ? po->backWo : po->frontWo ;
	wo->wt->sendFin( wo );

	return 0;
}


static int
AosFtp_passiveConnected( ProxyObject_t * po )
{
	int		err=1;
	//err = po->backWo->wt->connect( po->backWo, po->svc->backIp, po->svc->backPort);
	if( err )
	{
		return err;
	}
	po->backSk = po->backWo->sk;
	return 0;
}


static int
AosFtp_initFtpdata( AppProxy_t * app )
{

	memset( app, 0, sizeof(*app) );
	PROXY_SETNAME( app, "ftpdata" );
	app->owner = THIS_MODULE;	

	app->readyAcceptData= AosFtp_readyAcceptData;
	app->dataReceived = AosFtpdata_dataReceived;
	app->passiveConnected = AosFtp_passiveConnected;
	app->poReleased = AosFtpdata_poReleased;
	

	return 0;
}

static int
AosFtp_initFtp( AppProxy_t * app )
{

	memset( app, 0, sizeof(*app) );
	PROXY_SETNAME( app, "ftp" );
	app->owner = THIS_MODULE;	

	app->readyAcceptData= AosFtp_readyAcceptData;
	app->dataReceived = AosFtp_dataReceived;
	app->finReceived = AosFtp_finReceived;
	app->passiveConnected = AosFtp_passiveConnected;
	app->poReleased = 0;

	return 0;
}

static __init int 
AosFtp_initModule( void )
{
	AppProxy_t	* app;

	printk( KERN_INFO "loading Ftp proxy ...\n" );
	
	app = aos_malloc( sizeof(*app) );
	if ( !app )
	{
		printk( KERN_ERR "kmalloc ftp error\n" );
		return -ENOMEM;
	}

	AosFtp_initFtp( app );
	AosProxy_registerAppProxy( app );


	app = aos_malloc( sizeof(*app) );
	if ( !app )
	{
		printk( KERN_ERR "kmalloc ftpdata error\n" );
		return -ENOMEM;
	}

	AosFtp_initFtpdata( app );
	AosProxy_registerAppProxy( app );

	return 0;
}

static __exit void 
AosFtp_exitModule( void )
{
	AppProxy_t	* app;
	printk(KERN_INFO "unloading Ftp proxy ...\n");
	app = AosProxy_unregisterAppProxy("ftp");
	if ( app )
	{
		kfree( app );
	}
	app = AosProxy_unregisterAppProxy("ftpdata");
	if ( app )
	{
		kfree( app );
	}

}

module_init(AosFtp_initModule);
module_exit(AosFtp_exitModule);


