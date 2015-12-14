////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: WebWall.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "WebWall/WebWall.h"
#include <linux/module.h>
#include <linux/string.h>

//#include "aosUtil/Types.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/UrlPtree.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "aosUtil/ReturnCode.h"

#include "WebWall/WebWallCli.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("liqin");

extern void AosHttp_registerWebWallApi( struct webwall_api_s *webwall_api );
extern void AosHttp_unregisterWebWallApi( void );

extern int gwebwall_enable;

struct aosUrlPtree *gblack_list = NULL;
struct aosUrlPtree *gblack_list_dn = NULL;
struct webwall_access_time gaccess_time[MAX_ACCESS_TIME_ENTRIES];
char *gwebwall_denypage = NULL;
char gwebwall_denypage_path[MAX_DENYPAGE_PATH_LENGTH];

char gwebwall_denypage_body[] ={
	"<HTML><HEAD><META HTTP-EQUIV='Content-Type' CONTENT='text/html; charset=iso-8859-1'>\r\n"
	"<TITLE>ERROR: The requested URL could not be retrieved</TITLE>\r\n"
"<STYLE type='text/css'><!--BODY{background-color:#ffffff;font-family:verdana,sans-serif}PRE{font-family:sans-serif}--></STYLE>\r\n"
	"</HEAD><BODY>\r\n"
	"<H1>ERROR</H1>\r\n"
	"<H2>The requested URL could not be retrieved</H2>\r\n"
	"<HR noshade size='1px'>\r\n"
	"<P>\r\n"
	"While trying to retrieve the URL:\r\n"
	"<P>\r\n"
	"The following error was encountered:\r\n"
	"<UL>\r\n"
	"<LI>\r\n"
	"<STRONG>\r\n"
	"Access Denied.\r\n"
	"</STRONG>\r\n"
	"<P>\r\n"
	"Access control configuration prevents your request from "
	"being allowed at this time.  Please contact your service provider if "
	"you feel this is incorrect.\r\n"
	"</UL>\r\n"
	"\r\n"
	"\r\n"
	"<BR clear='all'>\r\n"
	"<HR noshade size='1px'>\r\n"
	"</ADDRESS>\r\n"
	"</BODY></HTML>\r\n"
};

//
// it will check the input ip string format
// the format should be like 192.168.0.1
//
// return 0: failed
// 
unsigned long AosAddrStrToLong( char* ip_input )
{
	unsigned long ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;
	char *pos;
	int format_right = 0, state = 0;
	
	enum{
		IP_ADDR_STATUS_IP1 = 0 ,
		IP_ADDR_STATUS_IP1_DOT,
		IP_ADDR_STATUS_IP2,
		IP_ADDR_STATUS_IP2_DOT,
		IP_ADDR_STATUS_IP3,
		IP_ADDR_STATUS_IP3_DOT,
		IP_ADDR_STATUS_IP4,
		IP_ADDR_STATUS_END,
	};

	pos = ip_input;
	aos_trace("ip input %s", ip_input);
	while( *pos )
	{
		if( *pos >= '0' && *pos <= '9' )
		{
			switch( state )
			{
				case IP_ADDR_STATUS_IP1:
					ip1 = ip1*10 + *pos - '0';
					break;
				case IP_ADDR_STATUS_IP1_DOT:
					ip2 = ip2*10 + *pos - '0';
					state++;
					break;
				case IP_ADDR_STATUS_IP2:
					ip2 = ip2*10 + *pos - '0';
					break;
				case IP_ADDR_STATUS_IP2_DOT:
					ip3 = ip3*10 + *pos - '0';
					state++;
					break;
				case IP_ADDR_STATUS_IP3:
					ip3 = ip3*10 + *pos - '0';
					break;
				case IP_ADDR_STATUS_IP3_DOT:
					ip4 = ip4*10 + *pos - '0';
					format_right = 1;
					state++;
					break;
				case IP_ADDR_STATUS_IP4:
					ip4 = ip4*10 + *pos - '0';
					format_right = 1;
					break;
				default:
	aos_trace("liqin");
					return 0;
			}
		}// if( *pos < '0' && *pos < '9' )
		else if( *pos == '.' )
		{
			switch( state )
			{
				case IP_ADDR_STATUS_IP1:
				case IP_ADDR_STATUS_IP2:
				case IP_ADDR_STATUS_IP3:
					state++;
					break;
				default:
	aos_trace("liqin");
					return 0;
			}
				
		}
		else
		{
			aos_trace("liqin");
			return 0;
		}
		pos++;
	}// while( *pos )

	
	if ( !format_right ) 
	{
			aos_trace("liqin");
		return 0;
	}

	if ( (long)ip1 > 255 || (long)ip2 > 255 || (long)ip3 > 255 || (long)ip4 > 255 ) 
	{
		aos_trace("liqin");
		return 0;
	}

	return ( (ip4<<24)|(ip3<<16)|(ip2<<8)|ip1 );
}

//
// convert ip address to binary string which is 32 bytes long
// for example 192.168.0.1 will be
// 1 1 0 0 0 0 0 0  1 0 0 1 1 0 0 0  0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 1
// 
int AosLongToBinStr( unsigned long ip_addr, char* ip_keyword_out, int out_len )
{
	unsigned long ip;
	int i;
	if ( out_len < 33 ) return -1;

	ip = (ip_addr&0xff)<<24;
	ip |= (ip_addr&0xff00)<<8;
	ip |= (ip_addr&0xff0000)>>8;
	ip |= (ip_addr&0xff000000)>>24;
	for( i = 0; i < 32; i++ )
	{
		ip_keyword_out[i] = (ip & ( 0x1 << (31 - i ) )) ? '1':'0';
	}
	ip_keyword_out[i] = 0;

	return 0;
}
int AosWebWall_isEnabled(void)
{
	return gwebwall_enable;
}

int AosWebWall_urlFilter( uint8 *uri, int uri_len, uint8 *host, int host_len )
{
	struct black_list_tab *entry;
	char c, *dn, *url ;
	unsigned long ip_addr;
	int ret;
	aos_assert1( gblack_list );
	char * tmp;

	c = uri[uri_len];
	uri[uri_len] = 0;
	//aos_trace("uri %s", uri);

	// uri black list search
	if ( strnicmp( uri, "http://", 7 ) == 0 )
	{
		if ( uri_len <= 7 ) return -1;
		url = uri + 7;
	}
	else if ( strnicmp( uri, "https://", 8 ) == 0 )
	{
		if ( uri_len <= 8 ) return -1;
		url = uri + 8;
	}
	else
	{
		url =  uri;
	}
	
	tmp=aos_malloc(uri_len+host_len+1);
	int i;
	for(i=0;i<host_len;i++)
		tmp[i]=host[i];
	strcpy(&tmp[host_len],url);
	/*
	for(i=0;i<uri_len-(url-uri);i++)
		tmp[i+host_len]=url[i];
	tmp[host_len+uri_len-(url-uri)]=0;
	*/
	aos_trace( "final url is %sEND", tmp );
	ret = aosUrlPtree_get( gblack_list, tmp, (void**)(&entry) );

	if( ret == 0 )
	{
		aos_trace("block this url");
		ret = eWebWall_Ctl_Block;
		goto URLFILTER_EXIT;
	}
	else if( ret < 0 )
	{
		aos_alarm( eAosMD_WebWall, eAosAlarm_ProgErr, "web wall failed to lookup ptree");
		goto URLFILTER_EXIT;
	}
	
	//
	// domain name and ip address black list search:w
	// first get domain name from url
	//
	/*
	if ( (pos  = strchr( url, '/' )) == NULL )
	{
		dn = url;
		cdn = url[0];
	}
	else
	{
		dn_len = pos - url;
		cdn = url[dn_len];
		url[dn_len] = 0;
		dn = url;
	}
	*/
	dn=tmp;
	dn[host_len]=0;
	
	aos_trace( "domain name is %sEND", dn );
	if ( (ip_addr = AosAddrStrToLong(dn)) != 0 )
	{
		char ip_keyword[34];
		ret = AosLongToBinStr( ip_addr, ip_keyword, 34 );
		if ( ret )
		{
			aos_trace("block this uri due to its doman name or ip address");
			ret = eWebWall_Ctl_Block;
			goto URLFILTER_EXIT;
		}
		aos_trace("domain name is ip address");
		ret = aosUrlPtree_get( gblack_list_dn, ip_keyword, (void**)(&entry) );
	}
	else
	{
		ret = aosUrlPtree_get( gblack_list_dn, dn, (void**)(&entry) );
	}

	//if ( dn_len ) url[dn_len] = cdn;
//	url[dn_len] = cdn;
	
	if( ret == 0 )
	{
		aos_trace("block this uri due to its doman name or ip address");
		ret = eWebWall_Ctl_Block;
		goto URLFILTER_EXIT;
	}
	else if( ret < 0 )
	{
		aos_alarm( eAosMD_WebWall, eAosAlarm_ProgErr, "web wall failed to lookup ptree");
		goto URLFILTER_EXIT;
	}

	ret = eWebWall_Ctl_Forward;
URLFILTER_EXIT:
	aos_free(tmp);
	uri[uri_len] = c;
	return ret;
}

static int AosWebWall_createHttpResponseHeader(char *header, int header_len,int body_len)
{
	
	snprintf(header, header_len, "HTTP/1.1 403 Forbidden\r\n"
								"Accept-Ranges: bytes\r\n"
								"Content-Length: %d\r\n"
								"Connection: close\r\n"
								"Content-Type: text/html; charset=iso-8859-1\r\n"
								"\r\n", body_len);

	return strlen(header);
}

int AosWebWall_createDenyPage( char* body, int body_len )
{
	int header_len;
	int buff_size = 256 + body_len + 1;
	
	if ( gwebwall_denypage ) aos_free( gwebwall_denypage );
	gwebwall_denypage = (char*) aos_malloc( buff_size );
	if ( !gwebwall_denypage )
	{
		aos_alarm(eAosMD_WebWall, eAosAlarm_MemErr, "malloc memory for denypage buffer failed");
		return -1;
	}
	memset( gwebwall_denypage, 0x00, buff_size);

	header_len = AosWebWall_createHttpResponseHeader( gwebwall_denypage, buff_size, body_len);
	if ( header_len <= 0 )
	{
		aos_alarm( eAosMD_WebWall, eAosAlarm_MemErr, "deny page header created error" );
		aos_free( gwebwall_denypage );
		gwebwall_denypage = NULL;
		return -1;
	}
	
	strncpy( gwebwall_denypage+header_len, body, buff_size-header_len );
	
	return eAosRc_Success;
}

char* AosWebWall_getDenyPage(void)
{
	return gwebwall_denypage;
}

int AosWebWall_timeControl(void)
{
	unsigned long second_cur = get_seconds();
	uint8 sec, min, hour;
	int i;
	struct timeval ttt;

	do_gettimeofday( &ttt );
	second_cur = ttt.tv_sec;

	
	sec = second_cur%60;
	min = ((second_cur-sec)/60)%60;
	hour = (((second_cur-sec)/60-min)/60)%24;
	// current time stripped of date;
	second_cur = (hour*60+min)*60+sec;

	for ( i = 0; i < MAX_ACCESS_TIME_ENTRIES; i++ )
	{
		if ( second_cur >= gaccess_time[i].start_time && 
			 second_cur <= gaccess_time[i].end_time )
		{
			aos_trace("the access is blocked due to the block time span");
			return eWebWall_Ctl_Block;
		}
	}
	aos_trace("the access is NOT blocked by time span. current: %d:%d:%d",hour,min,sec);
	return eWebWall_Ctl_Forward;
}

static int AosWebWall_initBlackList( void )
{
	// init uri black list
	gblack_list = aosUrlPtree_create();
	aos_assert1( gblack_list );
	// init domain name and ip address black list 
	gblack_list_dn = aosUrlPtree_create();
	aos_assert1( gblack_list_dn );
	
	return 0;
}

static struct webwall_api_s webwall_api = {
	AosWebWall_isEnabled,
	AosWebWall_urlFilter,
	AosWebWall_timeControl,
	AosWebWall_getDenyPage,
};

static __init int AosWebWall_moduleInit(void)
{
	int ret;
	printk(KERN_INFO "web wall module loading" );
	ret = AosWebWall_registerCli();
	ret = AosWebWall_initBlackList();
	memset( gaccess_time, 0, MAX_ACCESS_TIME_ENTRIES*sizeof(struct webwall_access_time) );
	AosHttp_registerWebWallApi( &webwall_api );

	// create denypage
	AosWebWall_createDenyPage( gwebwall_denypage_body, strlen(gwebwall_denypage_body) );
	
	return ret;
}

static __exit void AosWebWall_moduleExit(void)
{
	printk(KERN_INFO "web wall module unloading");
	AosWebWall_unregisterCli();
	AosHttp_unregisterWebWallApi();
	if ( gwebwall_denypage ) aos_free( gwebwall_denypage );
}


module_init( AosWebWall_moduleInit );
module_exit( AosWebWall_moduleExit );


