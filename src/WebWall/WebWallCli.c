////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: webwall_cli.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "WebWall/WebWallCli.h"

#include <linux/string.h>

#include "aos/aosKernelApi.h"
#include "aosUtil/Types.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/Memory.h"
#include "aosUtil/UrlPtree.h"

#include "WebWall/WebWall.h"

char *gblack_list_type_name[BLACK_LIST_TYPE_MAX] = { "url", "dn", "ip" };
int gwebwall_enable = 0;

struct black_list_tab gblack_list_tab = { NULL, 0, NULL };


//
// convert time format to seconds, time string format is like 
// hh:mm:ss, for example 02:01:11 is 2*3600+1*60+11 = 7271 seconds
// 
// return: 
//   <0  failed
//   >=0 seconds
// 		
static long time_str2_long( char *time_str )
{
	uint8 hh = 0, mm = 0, ss = 0;
	int state, len, i;
	
	enum{
		TIME_STATE_HH1,
		TIME_STATE_HH2,
		TIME_STATE_MM,
		TIME_STATE_MM1,
		TIME_STATE_MM2,
		TIME_STATE_SS,
		TIME_STATE_SS1,
		TIME_STATE_SS2,
		TIME_STATE_MAX,
	};
	
	len = strlen( time_str );
	if ( len < 5 || len > 8 ) return -1;

	state = TIME_STATE_HH1;
	for( i = 0; i < len; i++ )
	{
		if ( time_str[i] >= '0' && time_str[i] <= '9' )
		{
			switch( state )
			{
				case TIME_STATE_HH1:
					hh = time_str[i] - '0';
					state++;
					break;
				case TIME_STATE_HH2:
					hh = hh*10 + time_str[i] - '0';
					state++;
					break;
				case TIME_STATE_MM:
					return -1;
				case TIME_STATE_MM1:
					mm = time_str[i] - '0';
					state++;
					break;
				case TIME_STATE_MM2:
					mm = mm*10 + time_str[i] - '0';
					state++;
					break;
				case TIME_STATE_SS:
					return -1;
				case TIME_STATE_SS1:
					ss = time_str[i] - '0';
					state++;
					break;
				case TIME_STATE_SS2:
					ss = ss*10 + time_str[i] - '0';
					state++;
					break;
				default:
					return -1;
			}// end of switch
		}
		else
		{
			switch( state )
			{
				case TIME_STATE_HH2:
					if( time_str[i] != ':' ) return -1;
					state += 2;
					break;
				case TIME_STATE_MM:
					if( time_str[i] != ':' ) return -1;
					state++;
					break;
				case TIME_STATE_MM2:
					if( time_str[i] != ':' ) return -1;
					state += 2;
					break;
				case TIME_STATE_SS:
					if( time_str[i] != ':' ) return -1;
					state++;
					break;
				default:
					return -1;
			}
		} // end of if
	}// end of for
	if ( hh >= 24 || mm >= 60 || ss >= 60 ) return -1;
	return 	(hh*3600 + mm*60 + ss);
}					

static char gstime_str[10][10];
static const char* time_long2_str( unsigned long time )
{
	static int index = 0;
	char *str;
	uint8 hour, min, sec;
	hour = time/3600;
	min = (time-hour*3600)/60;
	sec = (time-hour*3600-min*60);

	snprintf( gstime_str[index], 10, "%02d:%02d:%02d", hour, min, sec );
	str = gstime_str[index];
	index = (index+1)%10;
	return str;
}

//
// check ip keyword input format, the format should be like
// 192.168.100.112/24 or 192.168.100.112
int AosWebWall_ipKeywordCheck( char* ip_input, char* ip_keyword_out, int out_len )
{

	uint8 single_ip = 0;
	char* mask_str = NULL;
	char *pos;
	int len, mask;
	unsigned long ip_addr;

	// check string length
	len = strlen( ip_input );
	if ( len > 18 ) return -1;

	// check if it is a single ip
	if ( (pos = strchr( ip_input, '/' )) == NULL )
	{
		aos_trace( "it is a single ip %s", ip_input );
		single_ip = 1;
	}
	else
	{
		ip_input[ pos-ip_input ] = 0;
		mask_str = pos+1;
		if ( mask_str-ip_input >= len ) return -1;
	}

	ip_addr = AosAddrStrToLong( ip_input );

	if ( !single_ip )
	{
		ip_input[mask_str-ip_input-1] = '/';
	}
	
	if ( !ip_addr ) 
	{
		aos_trace("ip address string format error");
		return -1;
	}

	AosLongToBinStr( ip_addr, ip_keyword_out, out_len );

	if ( !single_ip )
	{
		
		mask = (int)AosStrToLong( mask_str, NULL, 10 ) ;
		if ( mask > 24 )
		{
			aos_trace("ip mask error");
			return -1;
		}
		ip_keyword_out[mask+1] = '*';
		ip_keyword_out[mask+2] = 0;
	}
	aos_trace( "ip binary string %s", ip_keyword_out );
	return eAosRc_Success;
}

int AosWebWall_onCli(
	char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	*length = 0;

	gwebwall_enable = 1;
	aos_min_log( eAosMD_WebWall, "web wall is enabled" );
	
	return 0;
}

int AosWebWall_offCli(
	char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	*length = 0;

	gwebwall_enable = 0;
	aos_min_log( eAosMD_WebWall, "web wall is disabled" );
	
	return 0;
}

int AosWebWall_addBlackList(
	char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	char *type = parms->mStrings[0];
	char *key_word = parms->mStrings[1];
	char *ip_key_word = NULL;
	struct black_list_tab *entry, *entry_new;
	int i, ret;
			
	*length = 0;
	if ( !strlen(key_word) )
	{
		sprintf( errmsg, "black list entry string required" );
		return -eAosRc_InvalidParm;
	}	

	// new a black list entry
	entry_new = aos_malloc( sizeof( struct black_list_tab ) );
	aos_assert1( entry_new );
	memset( entry_new, 0, sizeof(struct black_list_tab) );
	entry_new->key_word = aos_malloc( strlen(key_word)+1 );
	aos_assert1( entry_new->key_word );
	strncpy( entry_new->key_word, key_word, strlen( key_word )+1 );

	for ( i = 0; i < BLACK_LIST_TYPE_MAX; i ++ )
	{
	 	if( strcmp( type, gblack_list_type_name[i] ) == 0 )
	 	{
	 		entry_new->type = i;
			break;
	 	}
	}

	// Insert new entry
	if ( entry_new->type == BLACK_LIST_TYPE_IP )
	{
		ip_key_word = (char*) aos_malloc( 34 );
		aos_assert1( ip_key_word );
		ret = AosWebWall_ipKeywordCheck( entry_new->key_word, ip_key_word, 34 );
		if ( ret != 0 )
		{
			sprintf( errmsg, "ip address format error" );
			aos_free( ip_key_word );
			aos_free( entry_new );
			return -1;
		}
		aos_trace("ip keyword entry_new->key_word %s", entry_new->key_word );
	}

	if ( entry_new->type == BLACK_LIST_TYPE_URL ) 
	{
		aos_trace("black list keyword =%s=", entry_new->key_word);
		if ( !aosUrlPtree_get( gblack_list, entry_new->key_word, (void*)(&entry) ) )
		{
			sprintf( errmsg, " failed to add %s, this key word already in blacklist", entry_new->key_word);
			aos_free( entry_new );
			return -1;
		}
		ret = aosUrlPtree_insert( gblack_list, entry_new->key_word, entry_new, 0 );
		if ( ret != 0 )
		{
			sprintf( errmsg, "failed to add %s to black list", key_word );
			aos_free( entry_new );
			return -1;
		}
	}
	else if ( entry_new->type == BLACK_LIST_TYPE_IP )
	{
		if ( !aosUrlPtree_get( gblack_list_dn, ip_key_word, (void*)(&entry) ) )
		{
			sprintf( errmsg, " failed to add %s, this key word already in blacklist", key_word);
			aos_free( ip_key_word );
			aos_free( entry_new );
			return -1;
		}
		ret = aosUrlPtree_insert( gblack_list_dn, ip_key_word, entry_new, 0 );
		if ( ret != 0 )
		{
			sprintf( errmsg, "failed to add %s to black list for dn", key_word );
			aos_free( entry_new );
			return -1;
		}
	}
	else
	{
		if ( !aosUrlPtree_get( gblack_list_dn, entry_new->key_word, (void*)(&entry) ) )
		{
			sprintf( errmsg, " failed to add %s, this key word already in blacklist", entry_new->key_word);
			aos_free( entry_new );
			return -1;
		}

		ret = aosUrlPtree_insert( gblack_list_dn, entry_new->key_word, entry_new, 0 );
		if ( ret != 0 )
		{
			sprintf( errmsg, "failed to add %s to black list for dn", key_word );
			aos_free( entry_new );
			return -1;
		}
	}
	
	//aosUrlPtree_print( gblack_list );
	//aosUrlPtree_print( gblack_list_dn );
	
	if ( ip_key_word ) aos_free( ip_key_word );
	return 0;
}


int AosWebWall_removeBlackList(
	char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	char *type_str = parms->mStrings[0];
	char *key_word = parms->mStrings[1];
	struct black_list_tab *entry = NULL;
	char *ip_key_word = NULL;
	int type = 0, i, ret;
		
	*length = 0;

	if ( !strlen(key_word) )
	{
		sprintf( errmsg, "black list entry string required" );
		return -eAosRc_InvalidParm;
	}	
	
	for ( i = 0; i < BLACK_LIST_TYPE_MAX; i ++ )
	{
	 	if( strcmp( type_str, gblack_list_type_name[i] ) == 0 )
	 	{
	 		type = i;
			break;
	 	}
	}

	if ( type == BLACK_LIST_TYPE_IP )
	{
		ip_key_word = (char*) aos_malloc( 34 );
		aos_assert1( ip_key_word );
		ret = AosWebWall_ipKeywordCheck( key_word, ip_key_word, 34 );
		if ( ret != 0 )
		{
			sprintf( errmsg, "ip address format error" );
			aos_free( ip_key_word );
			return -1;
		}
	}

	// remove entry in Ptree
	if ( type == BLACK_LIST_TYPE_URL )
	{
		aosUrlPtree_remove( gblack_list, key_word, (void*)(&entry) );
		if ( !entry )
		{
			sprintf( errmsg, "Keyword %s is not found in black list", key_word );
			return -1;
		}
	}
	else if ( type == BLACK_LIST_TYPE_IP )
	{
		aosUrlPtree_remove( gblack_list_dn, ip_key_word, (void*)(&entry) );
		if ( !entry )
		{
			sprintf( errmsg, "Keyword %s is not found in black list", key_word );
			aos_free( ip_key_word );
			return -1;
		}
	}
	else
	{
		aosUrlPtree_remove( gblack_list_dn, key_word, (void*)(&entry) );
		if ( !entry )
		{
			sprintf( errmsg, "Keyword %s is not found in black list", key_word );
			return -1;
		}
	}
	
	if ( entry )
	{
		if ( entry->key_word ) aos_free( entry->key_word );
		aos_free( entry );
	}

	if ( ip_key_word ) aos_free( ip_key_word );

	return 0;
}

static int AosWebWall_blacklistPtreeShow( 
	struct aosUrlPtree* tree, 
	char* rsltBuff, 
	unsigned int *rsltIndex,
	int length,
	char *errmsg,
	const int errlen)
{
	unsigned int optlen = length;
	char *local = (char *)aos_malloc(256);
	struct black_list_tab *entry;
	struct aosCharPtreeNode * node;
	int ret;
	
	aos_assert1(local);
	
	node = aosCharPtree_listFirst( tree->tree1, tree->tree1->mRoot );

	while(node)
	{
	#define LEN 200
		char key[LEN];

		ret = aosCharPtree_fullKey(tree->tree1, node, key, LEN);
		if(ret<=0)
		{
			aos_trace( "the length of key too small" );
			key[0]=0;
		}
		key[LEN-1]=0;

		entry = (struct black_list_tab*) node->mData;
		sprintf( local, "webwall blacklist %s %s\n", gblack_list_type_name[entry->type], entry->key_word );
		aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));		
		
		node = aosCharPtree_listNext( tree->tree1, node, 0);
	}


	node = aosCharPtree_listFirst( tree->tree2, tree->tree2->mRoot );

	while(node)
	{
	#define LEN 200
		char key[LEN];

		ret = aosCharPtree_fullKey(tree->tree1, node, key, LEN);
		if(ret<=0)
		{
			aos_trace( "the length of key too small" );
			key[0]=0;
		}
		key[LEN-1]=0;
		
		entry = (struct black_list_tab*) node->mData;
		sprintf( local, "webwall blacklist %s %s\n", gblack_list_type_name[entry->type], entry->key_word );
		aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));		

		node = aosCharPtree_listNext( tree->tree1, node, 0);
	}

	aos_free(local);

	return 0;

}

static int AosWebWall_blacklistShow(
	char* rsltBuff,
	unsigned int *rsltIndex,
	int length,
	char *errmsg,
	const int errlen)
{
	//unsigned int optlen = length;
	char *local = (char *)aos_malloc(256);
	//struct black_list_tab *entry;
	
	aos_assert1(local);

//	sprintf(local, "\nBlack List Configuration:\n");
//	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

//	sprintf(local, "------------------------------------------\n");
//	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
//	sprintf(local, "TYPE \tKEYWORD \n");
//	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	AosWebWall_blacklistPtreeShow( gblack_list, rsltBuff, rsltIndex, length, errmsg, errlen );
	AosWebWall_blacklistPtreeShow( gblack_list_dn, rsltBuff, rsltIndex, length, errmsg, errlen );
	
//	sprintf(local, "------------------------------------------\n");
//	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	aos_free(local);

	return eAosRc_Success;
}

int AosWebWall_timeControlAdd(
	char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int  index = parms->mIntegers[0] - 1;
	char *start_time_str = parms->mStrings[0];
	char *end_time_str = parms->mStrings[1];
	long start_time, end_time;
	
	*length = 0;
	
	if ( index < 0 || index >=MAX_ACCESS_TIME_ENTRIES )
	{
		sprintf( errmsg, "index should between 1 and %d", MAX_ACCESS_TIME_ENTRIES );
		return -eAosRc_InvalidParm;
	}
		
	if ( (start_time = time_str2_long( start_time_str )) < 0 )
	{
		sprintf( errmsg, "start time format error" );
		return -eAosRc_InvalidParm;
	}	
	
	if ( (end_time = time_str2_long( end_time_str )) < 0 )
	{
		sprintf( errmsg, "end time format error" );
		return -eAosRc_InvalidParm;
	}

	if ( end_time < start_time )
	{
		sprintf( errmsg, "end time should be larger than start time" );
		return -eAosRc_InvalidParm;
	}
	
	// check time span range 
	gaccess_time[index].start_time = start_time;
	
	gaccess_time[index].end_time = end_time;
		
	return eAosRc_Success;
}

int AosWebWall_timeControlRemove(
	char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int  index = parms->mIntegers[0] - 1;
	
	*length = 0;
	
	if ( index < 0 || index >=MAX_ACCESS_TIME_ENTRIES )
	{
		sprintf( errmsg, "index should between 1 and %d", MAX_ACCESS_TIME_ENTRIES );
		return -eAosRc_InvalidParm;
	}
	gaccess_time[index].start_time = 0;
	gaccess_time[index].end_time = 0;

	return eAosRc_Success;
}
static int AosWebWall_timeControlShow(
	char* rsltBuff,
	unsigned int *rsltIndex,
	int length,
	char *errmsg,
	const int errlen)
{
	unsigned int optlen = length;
	char *local = (char *)aos_malloc(256);
	int i;
	
	aos_assert1(local);

//	sprintf(local, "\nBlock Time Control Configuration:\n");
//	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

//	sprintf(local, "------------------------------------------\n");
//	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
//	sprintf(local, "Index \tStart Time \t End Time\n");
//	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	for( i = 0; i < MAX_ACCESS_TIME_ENTRIES; i++ )
	{
		if ( gaccess_time[i].start_time != 0 && gaccess_time[i].end_time != 0 )
		{
			sprintf( local, "webwall timectrl %d %s %s\n", i+1, 
					time_long2_str(gaccess_time[i].start_time), 
					time_long2_str(gaccess_time[i].end_time) );
			aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));		
		}
	}

//	sprintf(local, "------------------------------------------\n");
//	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	aos_free(local);

	return eAosRc_Success;
}

static int AosWebWall_denyPageAdd(
	char* data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms, 
	char* errmsg, 
	const int errlen)
{
	char *deny_page_contents;
	char *deny_page_file_path;
	unsigned int contents_length = 0;
	int ret;

	deny_page_file_path = parms->mStrings[0];
	deny_page_contents = parms->mStrings[1];
	
	aos_trace("deny_page_file_name:%s\n", deny_page_file_path);
	
	*length = 0;

	if ((deny_page_file_path == 0)||(*deny_page_file_path == '\0'))
	{
		snprintf(errmsg, errlen, "deny page file name error\n");
		return -1;
	}

	if ((deny_page_contents == 0)||(*deny_page_contents == '\0'))
	{
		snprintf(errmsg, errlen, "no deny page contents in file\n");
		return -1;
	}

	if ( strlen(deny_page_file_path) > MAX_DENYPAGE_PATH_LENGTH )
	{
		snprintf(errmsg, errlen, "file name too long\n");
		return -1;
	}

	if ( strlen(deny_page_contents) > MAX_DENYPAGE_LENGTH )
	{
		aos_alarm(eAosMD_WebWall, eAosAlarm_ProgErr, 
			"Contents too long: %d", strlen(deny_page_contents));
		sprintf(errmsg, "Contents too long: %d", strlen(deny_page_contents));
		return -1;
	}
	else
	{
		contents_length = strlen(deny_page_contents)+1;
	} 

	ret = AosWebWall_createDenyPage( deny_page_contents, contents_length );
	if ( ret )
	{
		aos_alarm( eAosMD_WebWall, eAosAlarm_ProgErr, 
			"Memory error: %d", contents_length);
		sprintf(errmsg, "Memory error: %d", contents_length);
		return -1;
	}
	
	strncpy( gwebwall_denypage_path, deny_page_file_path, MAX_DENYPAGE_PATH_LENGTH );
	
	aos_trace( "DenyPage Path %s", gwebwall_denypage_path );
	aos_trace( "DenyPage Contents:\n%s", gwebwall_denypage ); 
	
	return eAosRc_Success;
}

int AosWebWall_denyPageShow(
	char* data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms, 
	char* errmsg, 
	const int errlen)
{
	// 
	// deny page show
	//
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int optlen = *length;
	unsigned int rsltIndex = 0;
	char *tmpbuf;
	
	if (parms->mNumStrings > 0 || parms->mNumIntegers > 0)
	{
		sprintf(errmsg, "The command should not have parameters: %d, %d", 
			parms->mNumStrings, parms->mNumIntegers);
		return -1;
	}

	if ( gwebwall_denypage )
		tmpbuf = (char*)aos_malloc( strlen(gwebwall_denypage)>256 ? strlen(gwebwall_denypage):256 );
	else
		tmpbuf = (char*)aos_malloc( 256 );
	
	if ( gwebwall_denypage_path )
	{
		sprintf( tmpbuf, "Deny Page File Name: %s\n", gwebwall_denypage_path );
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
	}

	if ( gwebwall_denypage )
	{
		sprintf(tmpbuf, "Page Contents:\n");
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
		sprintf(tmpbuf, "--------------------------------\n");
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));

		snprintf(tmpbuf, strlen(gwebwall_denypage), "%s", gwebwall_denypage);
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
		
		sprintf(tmpbuf, "--------------------------------\n");
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
	}
	else
	{
		sprintf(tmpbuf, "No deny pages found\n");
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
	}

	aos_free( tmpbuf );		
	*length =rsltIndex;
	return 0;
}

int AosWebWall_showConfigCli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char *rsltBuff = aosKernelApi_getBuff(data);
	char local[200];
	int ret = 0;

	*length = 0;

	sprintf(local, "\nwebwall %s\n", gwebwall_enable?"on":"off" );
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	ret = AosWebWall_blacklistShow(rsltBuff, &rsltIndex, optlen, errmsg, errlen);
	if (ret != 0)
	{
		return ret;
	}
	if ( 0 != AosWebWall_timeControlShow(rsltBuff, &rsltIndex, optlen, errmsg, errlen) )
	{
		return ret;
	}	
	
	if ( gwebwall_denypage_path && strlen(gwebwall_denypage_path) )
	{
		sprintf( local, "webwall denypage import %s\n", gwebwall_denypage_path );
		aosCheckAndCopy( rsltBuff, &rsltIndex, optlen, local, strlen(local) );
	}

	*length = rsltIndex;
	return ret;
}

static int AosWebWall_clearConfigCli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	int ret = 0;

	*length = 0;

	gwebwall_enable=0;

	/*
	AosWebWall_blacklistClear();
	AosWebWall_timeControlClear(); 
	AosWebWall_denypageClear(); 
	*/

	return ret;
}

int AosWebWall_registerCli(void)
{
	int ret;
	printk(KERN_INFO  "Aos web wall cli register" );

	ret  = OmnKernelApi_addCliCmd( "WebWallOn",	AosWebWall_onCli );
	ret |= OmnKernelApi_addCliCmd( "WebWallOff",	AosWebWall_offCli );
	ret |= OmnKernelApi_addCliCmd( "WebWallBlackListAdd", AosWebWall_addBlackList );
	ret |= OmnKernelApi_addCliCmd( "WebWallBlackListRemove", AosWebWall_removeBlackList );
	ret |= OmnKernelApi_addCliCmd( "WebWallTimeControlAdd", AosWebWall_timeControlAdd );
	ret |= OmnKernelApi_addCliCmd( "WebWallTimeControlRemove", AosWebWall_timeControlRemove );
	ret |= OmnKernelApi_addCliCmd( "WebWallDenyPageAdd", AosWebWall_denyPageAdd );
	ret |= OmnKernelApi_addCliCmd( "WebWallDenyPageShow", AosWebWall_denyPageShow );
	ret |= OmnKernelApi_addCliCmd( "WebWallShowConfig", AosWebWall_showConfigCli );
	ret |= OmnKernelApi_addCliCmd( "WebWallClearConfig", AosWebWall_clearConfigCli );
	if(ret==0)
	{
		printk(KERN_INFO  "Aos web wall cli register success" );
	}
	return ret;
}	

int AosWebWall_unregisterCli(void)
{
	int ret;
	aos_min_log( eAosMD_WebWall, "Aos web wall cli unregister" );

	ret  = OmnKernelApi_delCliCmd( "WebWallOn" );
	ret |= OmnKernelApi_delCliCmd( "WebWallOff" );
	ret |= OmnKernelApi_delCliCmd( "WebWallBlackListAdd" );
	ret |= OmnKernelApi_delCliCmd( "WebWallBlackListRemove");
	ret |= OmnKernelApi_delCliCmd( "WebWallTimeControlAdd");
	ret |= OmnKernelApi_delCliCmd( "WebWallTimeControlRemove");
	ret |= OmnKernelApi_delCliCmd( "WebWallDenyPageAdd" );
	ret |= OmnKernelApi_delCliCmd( "WebWallDenyPageShow" );
	ret |= OmnKernelApi_delCliCmd( "WebWallShowConfig" );
	ret |= OmnKernelApi_delCliCmd( "WebWallClearConfig" );

	return ret;
}


