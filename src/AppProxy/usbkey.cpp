////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: usbkey.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include "aos/aosKernelApi.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include "AppProxy/usbkey.h"

static char *usbkeys[USBKEYMAX];

int	is_the_same_lan(struct socket *socket)
{
	int	len;
	int	ret;
	struct sockaddr_in 	my_addr;
	struct sockaddr_in 	peer_addr;
	struct net_device	*dev;
	struct in_device	*in_dev;


	socket->ops->getname(socket,(struct sockaddr*)&my_addr,&len,0);
	socket->ops->getname(socket,(struct sockaddr*)&peer_addr,&len,1);
	ret=0;
	aos_trace("is_the_same_lan(): my_addr is %d.%d.%d.%d, peer_addr is %d.%d.%d.%d",
		NIPQUAD(my_addr.sin_addr.s_addr),
		NIPQUAD(peer_addr.sin_addr.s_addr));
	
	dev=dev_get_by_name("eth0");
	if(!dev)
	{
		aos_trace("is_the_same_lan(): get dev failed");
		goto out;
	}
	in_dev=in_dev_get(dev);
	if(!in_dev)
	{
		aos_trace("is_the_same_lan(): get in_dev failed");
		goto dev_release;
	}
	aos_trace("is_the_same_lan(): get in_dev ok");
	for_ifa(in_dev)
	{
		if(inet_ifa_match(peer_addr.sin_addr.s_addr,ifa))
		{
			aos_trace("is_the_same_lan(): match");
			ret= 1;
			goto in_dev_release;
		}
		aos_trace("is_the_same_lan(): not match");
	}
	endfor_ifa(indev);
in_dev_release:
	in_dev_put(in_dev);
dev_release:
	dev_put(dev);
out:
	return ret;
}


int	in_usbkey_list(char * id)
{
	int i;


	for(i=0;i<USBKEYMAX;i++)
	{
		if(usbkeys[i] && 0==strcmp(usbkeys[i],id))
			return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////// 
// Description
//	It adds a deny page to the array 'sg_deny_pages[]'. If there is 
//  no empty slot, it means the maximum allowed is reached. Return 
//  an error. Otherwise, it allocates the memory, copies the contents
//  of the page, and stores the pointer to an empty slot. 
// 
// Parameters:
//	page:	IN
//		The page to be created. 
//
// Lock Management:
//	It should lock the array before operating on the array.
//
// Errors:
//	eAosRc_NullPointer
//	If 'page' is null. An alarm is raised.
//
//	eAosRc_MemErr
//	If it fails to allocate memory. An alarm is raised.
//
//	eAosRc_MaxDenyPageExceed
//	If it exceeds the maximum number of deny pages. An alarm is raised.
//
//	eAosRc_DenyPageTooLong
//	If the page length exceeds eAosDenyPageMaxLength.
//
/////////////////////////////////////////////////////////////////////// 
int aos_usbkey_add(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen)
{
	char 	*key,*tmp;
	int	i,found;

	
	*length=0;
	key=parms->mStrings[0];
	aos_trace("usbkey:%s\n",key);

	if ((key == 0)||(*key == '\0'))
	{
		snprintf(errmsg, errlen, "usbkey name error\n");
		return -1;
	}

	if( USBKEYLEN != strlen(key) )
	{
		snprintf(errmsg,errlen,"length of the usbkey name is not %d\n",USBKEYLEN);
		return -1;
	}
	found=-1;
	for(i=USBKEYMAX-1;i>=0;i--)
	{
		if(usbkeys[i]==0)
		{
			found=i;
			continue;
		}
		if(0==strcmp(usbkeys[i],key))
		{
			snprintf(errmsg,errlen,"duplicate usbkey\n");
			return -1;
		}
	}

	if(-1==found)
	{
		snprintf(errmsg,errlen,"too many usbkeys\n");
		aos_trace("too many usbkeys");
		return -1;
	}

	tmp=(char*)aos_malloc(USBKEYLEN+1);
	if(!tmp)
	{
		aos_alarm(eAosMD_Platform, 1,"out of memory while allocate a usbkey");
		return -1; 
	}
	strcpy(tmp,key);
	usbkeys[found]=tmp;
	aos_trace("usbkey add at %d",found);
	return 0;

}


/////////////////////////////////////////////////////////////////////// 
// Description:
//	It removes a deny page pointed to by 'page'. If the page is not 
//	found in the array, it returns an error. Otherwise, it removes
//	the page from the array and frees the memory. 
//
// IMPORTANT TO CALLERS:
//	One should make sure no one is using the page before calling this
//  function. After this function, the memory pointed to by 'page' 
//	is freed. Any reference to this memory will results in disarsters. 
//
// Parameters:
//	page:	IN
//		The page to be removed.
//
// Lock Management:
//	It should lock the array before operating on the array.
//
// Errors:
//	eAosRc_NullPointer
//	If 'page' is null. This is a program error. An alarm is raised.
//
//  eAosRc_PageNotFound
//	If 'page' does not point to a page stored in the array. This is
//  a program error (should never happen). An alarm is raised.
/////////////////////////////////////////////////////////////////////// 
int aos_usbkey_remove(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen)
{
	char 	*key;
	int	i=0;

	*length=0;

	
	key=parms->mStrings[0];
	aos_trace("usbkey:%s\n",key);

	if ((key == 0)||(*key == '\0'))
	{
		snprintf(errmsg, errlen, "usbkey name error\n");
		return -1;
	}

	if( USBKEYLEN != strlen(key) )
	{
		snprintf(errmsg,errlen,"length of the usbkey name is not %d\n",USBKEYLEN);
		return -1;
	}
	for(i=0;i<USBKEYMAX;i++)
	{
		if(usbkeys[i]==0)
		{
			continue;
		}
		if(0==strcmp(usbkeys[i],key))
		{
			aos_free(usbkeys[i]);
			usbkeys[i]=0;
			return 0;
		}
		aos_trace("usbkeys[%d]=%s,key=%s",i,usbkeys[i],key);
	}

	snprintf(errmsg,errlen,"no such usbkey\n");
	return -1;
}


/////////////////////////////////////////////////////////////////////// 
// Description:
//
// IMPORTANT TO CALLERS:
//
// Parameters:
//
// Lock Management:
//
// Errors:
/////////////////////////////////////////////////////////////////////// 
int aos_usbkey_show(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int optlen = *length;
	unsigned int rsltIndex = 0;
	char tmpbuf[256];
	int i;
	
	*length=0;
	if (parms->mNumStrings > 0 || parms->mNumIntegers > 0)
	{
		sprintf(errmsg, "The command should not have parameters: %d, %d", 
			parms->mNumStrings, parms->mNumIntegers);
		return -1;
	}

	sprintf(tmpbuf, "USB Key");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
	sprintf(tmpbuf, "\n--------------------------------");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));

	for(i=0;i<USBKEYMAX;i++)
	{
		if(!usbkeys[i])
			continue;
		snprintf(tmpbuf,256,"\n%s",usbkeys[i]);
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
	}

	sprintf(tmpbuf, "\n--------------------------------\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
		
	*length =rsltIndex;
	return 0;
}


int aos_usbkey_save_config(char *data, 
						   unsigned int *length, 
						   struct aosKernelApiParms *parms,
						   char *errmsg, 
						   const int errlen)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int optlen = *length;
	unsigned int rsltIndex = 0;
	char tmpbuf[256];
	int i;

	for(i =0; i< USBKEYMAX; i++)
	{
		if (!usbkeys[i]) continue;
		sprintf(tmpbuf, "<Cmd>usbkey add %s</Cmd>\n", usbkeys[i]);
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
	}
	*length = rsltIndex;
	return 0;
}

int aos_usbkey_clear_config(char *data, 
							   unsigned int *length, 
							   struct aosKernelApiParms *parms,
							   char *errmsg, 
							   const int errlen)
{
	int i;

	*length = 0;

	for(i =0; i< USBKEYMAX; i++)
	{
		if (!usbkeys[i]) 
			continue;
		aos_free(usbkeys[i]);
		usbkeys[i]=0;
	}
	return 0;
}


