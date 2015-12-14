////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ForwardTable.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppProc/http_proc/ForwardTable.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Mutex.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include "AppProc/ReturnCode.h"
#include "aosUtil/StringUtil.h"

static struct aos_http_fwdtbl sgForwardTable;
static AOS_DECLARE_MUTEX(sgLock);
static int aos_http_fwdtbl_init_flag = 0;
static struct aos_http_fwdtbl_entry *sgEntryList[eAosHttpFwdtbl_MaxEntries];
static u32 sgEntryListIndex = 0;


int aos_http_fwdtbl_init(void)
{
	int i;

	if (aos_http_fwdtbl_init_flag)
	{
		// 
		// Already initialized.
		//
		aos_trace("forward table already init");
		return 0;
	}

	AOS_INIT_MUTEX(sgLock);
	sgForwardTable.entry_tree = (struct aosCharPtree *)aosCharPtree_create();
	aos_assert1(sgForwardTable.entry_tree);
	sgForwardTable.num_entries = 0;

	for (i=0; i<eAosHttpFwdtbl_MaxEntries; i++)
	{
		sgEntryList[i] = 0;
	}

	aos_http_fwdtbl_init_flag = 1;
	aos_trace("forward table init");
	return 0;
}


static int aos_fwdtbl_check_names(char *vsname, 
					char *url, 
					char *errmsg)
{
	u32 vslen;

	if (!vsname)
	{
		strcpy(errmsg, "Missing virtual service name");
		return -eAosRc_MissingParms;
	}

	if (!url)
	{
		strcpy(errmsg, "Missing url");
		return -eAosRc_MissingParms;
	}

	vslen = strlen(vsname);
	if (vslen >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Virtual service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}

	if (strlen(url) >= eAosHttpFwdtbl_MaxUrlLen)
	{
		sprintf(errmsg, "URL too long: %d. Maximum allowed: %d",
			strlen(url), eAosHttpFwdtbl_MaxUrlLen);
		return -eAosRc_UrlTooLong;
	}

	return 0;
}


static int aos_fwdtbl_entry_constructor(struct aos_http_fwdtbl_entry **entry)
{
	*entry = (struct aos_http_fwdtbl_entry *)
		aos_malloc(sizeof(struct aos_http_fwdtbl_entry));
	if (!(*entry))
	{
		return -eAosRc_MemErr;
	}

	memset(*entry, 0, sizeof(struct aos_http_fwdtbl_entry));
	return 0;
}


static int aos_fwdtbl_entry_destructor(struct aos_http_fwdtbl_entry *entry)
{
	memset(entry, 0, sizeof(struct aos_http_fwdtbl_entry));
	aos_free(entry);
	return 0;
}


int aos_fwdtbl_entry_add_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	//
	// forward table entry add <vsname> <url> <addr> <port> <flag>
	//
	char *vsname = parms->mStrings[0];
	char *url = parms->mStrings[1];
	char *flag = parms->mStrings[2];
	u32 addr = parms->mIntegers[0];
	u16 port = parms->mIntegers[1];
	int isSsl;
	struct aos_http_fwdtbl_entry *entry = 0;
	int foundIndex;
	int ret;
	u32 vslen;
	void *ptr = 0;
	char *id = NULL;

	*length = 0;
	aos_debug_log(eAosMD_AppProc, "To add: %s, %s, %s", vsname, url, flag);
	
	id = (char *)aos_malloc(eAosHttpFwdtbl_MaxIdLen);
	if (!id)
	{
		//aos_alarm(eAosMD_AppProc, eAosAlarm_ProgErr, 
		//	("can not alloc id, size is %d", eAosHttpFwdtbl_MaxIdLen));
		aos_alarm(eAosMD_AppProc, eAosAlarm_ProgErr, 
			"can not alloc id, size is %d", eAosHttpFwdtbl_MaxIdLen);
		return -eAosRc_InternalError;
	}

	if (!aos_http_fwdtbl_init_flag)
	{
		ret = aos_http_fwdtbl_init();
	}
	
	if (!vsname || !url || !flag)
	{
		strcpy(errmsg, "Missing parameters");
		aos_free(id);
		return -eAosRc_MissingParms;
	}

	if (strlen(vsname) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Virtual service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		aos_free(id);
		return -eAosRc_NameTooLong;
	}

	if (strlen(url) >= eAosHttpFwdtbl_MaxUrlLen)
	{
		sprintf(errmsg, "URL too long: %d. Maximum allowed: %d",
			strlen(url), eAosHttpFwdtbl_MaxUrlLen);
		aos_free(id);
		return -eAosRc_UrlTooLong;
	}

	if (strcmp(flag, "ssl") == 0)
	{
		isSsl = eAosConnType_SSL;
	}
	else if (strcmp(flag, "plain") == 0)
	{
		isSsl = eAosConnType_Plain;
	}
	else
	{
		strcpy(errmsg, "Invalid flag");
		aos_free(id);
		return -eAosRc_InvalidFlag;
	}

	if (addr == 0)
	{
		strcpy(errmsg, "Missing IP address");
		aos_free(id);
		return eAosRc_MissingAddr;
	}

	// Check the maximum number of entries
	if (sgForwardTable.num_entries >= eAosHttpFwdtbl_MaxEntries)
	{
		strcpy(errmsg, "Too many entries");
		aos_free(id);
		return eAosRc_TooManyEntries;
	}

	strcpy(id, vsname);
	vslen = strlen(vsname);
	id[vslen] = 'z';
    strncpy(&id[vslen+1], url, eAosHttpFwdtbl_MaxIdLen-vslen-1);

	// Check whether the entry is already in the table
	aos_lock(sgLock);
	aos_assert1(sgForwardTable.entry_tree);
	ret = aosCharPtree_get(sgForwardTable.entry_tree, 
			id, &foundIndex, &ptr);
	if (ptr)
	{
		// The entry is already in the system. Modify it.
		entry = (struct aos_http_fwdtbl_entry *)ptr;
		strcpy(entry->vsname, vsname);
		entry->addr = addr;
		entry->port = port;
		entry->ssl_flag = isSsl;
		aos_unlock(sgLock);
		aos_free(id);
		return 0;
	}

	// It is not in the system yet
	aos_fwdtbl_entry_constructor(&entry);
	if (!entry)
	{
		strcpy(errmsg, "Failed to allocate memory");
		aos_unlock(sgLock);
		aos_free(id);
		return -eAosRc_MemErr;
	}

	strcpy(entry->vsname, vsname);
	strcpy(entry->url, url);
	entry->addr = addr;
	entry->port = port;
	entry->ssl_flag = isSsl;
	
	aos_assert1(sgForwardTable.entry_tree);
	if ((ret = aosCharPtree_insert(sgForwardTable.entry_tree, id, strlen(id), entry, 0)))
	{
		aos_unlock(sgLock);
		sprintf(errmsg, "Failed to add: %d", ret);
		aos_fwdtbl_entry_destructor(entry);
		aos_free(id);
		return -eAosRc_FailedToInsert;
	}

	// Inserted successfully
	sgForwardTable.num_entries++;
	if (sgEntryListIndex >= eAosHttpFwdtbl_MaxEntries ||
		sgEntryList[sgEntryListIndex])
	{
		aos_progerr(eAosMD_AppProc, eAosAlarm_ProgErr, 
			"Index out of bound: %d or Entry not null: %x", 
			sgEntryListIndex, (u32)sgEntryList[sgEntryListIndex]);
		aos_unlock(sgLock);
		strcpy(errmsg, "Internal error");
		aos_free(id);
		return -eAosRc_InternalError;
	}

	sgEntryList[sgEntryListIndex++] = entry;
	aos_unlock(sgLock);
	aos_free(id);
	return 0;
}


int aos_fwdtbl_entry_del_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	//
	// forward table entry remove <vsname> <url> 
	//
	char *vsname = parms->mStrings[0];
	char *url = parms->mStrings[1];
	// char id[eAosHttpFwdtbl_MaxIdLen];
	char *id;
	u32 vslen, i, j;
	int ret;
	void *ptr = 0;
	struct aos_http_fwdtbl_entry *entry;

	*length = 0;

	if (!aos_http_fwdtbl_init_flag)
	{
		ret = aos_http_fwdtbl_init();
	}

	if (!vsname || !url)
	{
		strcpy(errmsg, "Missing parameters");
		return -eAosRc_MissingParms;
	}

	vslen = strlen(vsname);
	if (vslen >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Virtual service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}

	if (strlen(url) >= eAosHttpFwdtbl_MaxUrlLen)
	{
		sprintf(errmsg, "URL too long: %d. Maximum allowed: %d",
			strlen(url), eAosHttpFwdtbl_MaxUrlLen);
		return -eAosRc_UrlTooLong;
	}

	id = (char *)aos_malloc(eAosHttpFwdtbl_MaxIdLen);
	aos_assert1(id);

	strcpy(id, vsname);
	vslen = strlen(vsname);
	id[vslen] = 'z';
	strcpy(&id[vslen+1], url);

	aos_lock(sgLock);
	aos_assert1(sgForwardTable.entry_tree);
	ret = aosCharPtree_remove(sgForwardTable.entry_tree, id, &ptr);
	if (!ptr)
	{
		aos_unlock(sgLock);
		strcpy(errmsg, "Entry not found");
		aos_free(id);
		return -eAosRc_EntryNotFound;
	}
	
	entry = (struct aos_http_fwdtbl_entry *)ptr;
	sgForwardTable.num_entries--;

	for (i=0; i<sgEntryListIndex; i++)
	{
		if (sgEntryList[i] == entry)
		{
			for (j=0; j<sgEntryListIndex-1; j++)
			{
				sgEntryList[j] = sgEntryList[j+1];
			}

			sgEntryListIndex--;
			if (sgEntryListIndex < 0)
			{
			//	aos_alarm(eAosMD_AppProc, eAosAlarm_ProgErr, 
			//		("Index is negative: %d", sgEntryListIndex));
				aos_alarm(eAosMD_AppProc, eAosAlarm_ProgErr, 
					"Index is negative: %d", sgEntryListIndex);
				sgEntryListIndex = 0;
			}

			sgEntryList[sgEntryListIndex] = 0;
			break;
		}
	}

	aos_fwdtbl_entry_destructor(entry);
	aos_unlock(sgLock);
	aos_free(id);
	return 0;
}


int aos_fwdtbl_list(char *vsname, 
					char *data, 
					unsigned int *length, 
					char *errmsg, 
					const int errlen)
{
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
	char *local = (char *)aos_malloc(eAosHttpFwdtbl_MaxIdLen+20);
	unsigned int i;
	char name[KTCPVS_IDENTNAME_MAXLEN+1];
	int found = 0;

	aos_assert1(local);
	if (vsname) strcpy(name, vsname);

	sprintf(local, "----------------------------------------------------------------\n");
   	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	aos_lock(sgLock);

	if (vsname)
	{
		for (i=0; i<sgEntryListIndex; i++)
		{
			if (sgEntryList[i] && strcmp(sgEntryList[i]->vsname, name) == 0)
			{
				found++;
				sprintf(local, "%-18s %-s %-s %d %-s\n", 
					sgEntryList[i]->vsname, 
					sgEntryList[i]->url,
					aosAddrToStr(sgEntryList[i]->addr),
					sgEntryList[i]->port,
					sgEntryList[i]->ssl_flag==eAosConnType_SSL?"ssl":"plain");
    			aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
			}
		}
	}
	else
	{
		for (i=0; i<sgEntryListIndex; i++)
		{
			if (sgEntryList[i])
			{
				found++;
				sprintf(local, "%-18s %-s %-s %d %-s\n", 
					sgEntryList[i]->vsname, 
					sgEntryList[i]->url,
					aosAddrToStr(sgEntryList[i]->addr),
					sgEntryList[i]->port,
					sgEntryList[i]->ssl_flag==eAosConnType_SSL?"ssl":"plain");
    			aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
			}
		}
	}
		
	aos_unlock(sgLock);

	if (found == 0)
	{
		sprintf(local, "No forward table entries found\n");
   		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}

	sprintf(local, "----------------------------------------------------------------\n");
   	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	aos_free(local);
	return 0;
}


int aos_fwdtbl_show_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	//
	// forward table show [<vsname> <url>]
	//
	char *vsname = parms->mStrings[0];
	char *url = parms->mStrings[1];
	char *local = NULL;
	void *ptr;
	struct aos_http_fwdtbl_entry *entry;
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
	char *id = NULL;
	int ret;
	u32 vslen;
	int foundIndex;

	local = (char *)aos_malloc(eAosHttpFwdtbl_MaxIdLen);
	if (!local)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, 
			"alloc buffer for local failed, size if %d", 
			eAosHttpFwdtbl_MaxIdLen);
		*length = 0;
		aos_free(local);
		return -eAosRc_InternalError;
	}

	id  = (char *)aos_malloc(eAosHttpFwdtbl_MaxIdLen);
	if (!id)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, 
			"alloc buffer for id failed, size if %d", 
				eAosHttpFwdtbl_MaxIdLen);
		aos_free(local);
		*length = 0;
		return -eAosRc_InternalError;
	}

	if (!aos_http_fwdtbl_init_flag)
	{
		aos_http_fwdtbl_init();
	}

	if (!vsname || !url)
	{
		return aos_fwdtbl_list(vsname, data, length, errmsg, errlen);
	}

	ret = aos_fwdtbl_check_names(vsname, url, errmsg);
	if (ret)
	{
		aos_free(local);
		aos_free(id);
		*length = 0;
		return ret;
	}

	strcpy(id, vsname);
	vslen = strlen(vsname);
	id[vslen] = 'z';
    strncpy(&id[vslen+1], url, eAosHttpFwdtbl_MaxIdLen-vslen-1);

	aos_lock(sgLock);
	aos_assert1(sgForwardTable.entry_tree);
	ret = aosCharPtree_get(sgForwardTable.entry_tree, 
			id, &foundIndex, &ptr);
	aos_unlock(sgLock);
	entry = (struct aos_http_fwdtbl_entry *)ptr;

	if (!entry)
	{
		strcpy(errmsg, "Entry not found");
		aos_free(local);aos_free(id);
		*length = 0;
		return -eAosRc_EntryNotFound;
	}

	sprintf(local, "-------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Service Name:   %s\n", entry->vsname);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "URL:            %s\n", entry->url);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Address:        %s\n", aosAddrToStr(entry->addr));
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Port:           %d\n", entry->port);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));


	if (entry->ssl_flag == eAosConnType_SSL)
	{
		sprintf(local, "Conn Type:      SSL\n");
	}
	else
	{
		sprintf(local, "Conn Type:      Plain\n");
	}
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "-------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	aos_free(local);
	aos_free(id);
	return 0;
}


int aos_fwdtbl_clearall_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// forward table clear config
	//
	unsigned int i;
	int ret = 0;
	char *id = (char *)aos_malloc(eAosHttpFwdtbl_MaxIdLen);
	u32 vslen;
	void *ptr;

	aos_assert1(id);

	*length = 0;

	if (!aos_http_fwdtbl_init_flag)
	{
		aos_http_fwdtbl_init();
		aos_free(id);
		return 0;
	}

	aos_lock(sgLock);
	for (i=0; i<sgEntryListIndex; i++)
	{
		if (sgEntryList[i])
		{
			strcpy(id, sgEntryList[i]->vsname);
			vslen = strlen(sgEntryList[i]->vsname);
			id[vslen] = 'z';
			strcpy(&id[vslen+1], sgEntryList[i]->url);

			aos_assert1(sgForwardTable.entry_tree);
			ret = aosCharPtree_remove(sgForwardTable.entry_tree, id, &ptr);
			if (!ptr)
			{
				ret = -eAosRc_EntryNotFound;
			}

			aos_fwdtbl_entry_destructor(sgEntryList[i]);
		}

		sgEntryList[i] = 0;
	}

	// 
	// Chen Ding, 09/24/2005, Bug #10. 
	// Need to set the number to 0.
	//
	sgForwardTable.num_entries = 0;
	sgEntryListIndex = 0;
	aos_unlock(sgLock);
		
	aos_free(id);
	return 0;
}


int aos_fwdtbl_saveconf_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,      
                       const int errlen)
{
	// 
	// forward table save config
	//
	unsigned int i;
	int ret = 0;
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
	char *local = (char *)aos_malloc(eAosHttpFwdtbl_MaxIdLen+200);
	char flag[10];

	aos_assert1(local);
	*length = 0;

	aos_lock(sgLock);
	for (i=0; i<sgEntryListIndex; i++)
	{
		if (sgEntryList[i])
		{
			if (sgEntryList[i]->ssl_flag)
			{
				sprintf(flag, "ssl");
			}
			else
			{
				sprintf(flag, "plain");
			}

			sprintf(local, "<Cmd>forward table entry add %s %s %s %d %s</Cmd>",
				sgEntryList[i]->vsname, 
				sgEntryList[i]->url,
				aosAddrToStr(sgEntryList[i]->addr),
				sgEntryList[i]->port,
				flag);

    		ret |= aosCheckAndCopy(rsltBuff, &rsltIndex, 
				optlen, local, strlen(local));
		}
	}

	aos_unlock(sgLock);

	*length = rsltIndex;
	aos_free(local);
	return ret;
}


int aos_http_fwdtbl_lookup(char *vsname, 
					   char *url,
					   u32 *addr,
					   u16 *port,
					   int *connType)
{
	u32 vslen, urllen;
	// char id[eAosHttpFwdtbl_MaxIdLen];
	char *id = 0;
	void *ptr;
	int foundIndex;
	struct aos_http_fwdtbl_entry *entry;
	int ret;

	if (!vsname || !url)
	{
		return aos_progerr(eAosMD_AppProc, eAosAlarm_NullPointer, 
			"Null pointer: %x, %x", (u32)vsname, (u32)url);
	}

	vslen = strlen(vsname);
	urllen = strlen(url);

	if (vslen >= KTCPVS_IDENTNAME_MAXLEN)
	{
		return aos_alarm(eAosMD_AppProc, eAosRc_NameTooLong,
			"Service name too long: %d", vslen);
	}

	if (urllen >= eAosHttpFwdtbl_MaxUrlLen)
	{
		return aos_alarm(eAosMD_AppProc, eAosRc_UrlTooLong,
			"URL too long: %d", urllen);
	}

	id = (char *)aos_malloc(eAosHttpFwdtbl_MaxIdLen);
	aos_assert1(id);

	strcpy(id, vsname);
	id[vslen] = 'z';
	strncpy(&id[vslen+1], url, eAosHttpFwdtbl_MaxIdLen-vslen-1);

	aos_lock(sgLock);
	aos_assert1(sgForwardTable.entry_tree);
	ret = aosCharPtree_get(sgForwardTable.entry_tree, 
			id, &foundIndex, &ptr);
	if (!ptr)
	{
		aos_unlock(sgLock);
		aos_free(id);
		return eAosRc_ServiceNotFound;
	}

	entry = (struct aos_http_fwdtbl_entry *)ptr;
	
	*addr = entry->addr;
	*port = entry->port;
	*connType = entry->ssl_flag;
	
	aos_unlock(sgLock);
	aos_free(id);
	return 0;
}

