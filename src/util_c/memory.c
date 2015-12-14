////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Memory.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "util_c/memory.h"

#include "alarm_c/alarm.h"
#include "aos/aosKernelApi.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/AosPlatform.h"
#include "porting_c/mutex.h"
#include "util_c/hashtab.h"
#include "util_c/rc.h"
#include "util_c/list.h"

#include <stdio.h>

typedef struct AosMemShowEntry
{
	char	file[eAosMemory_FileNameLen];
	int		line;
	int		count[eAosMemory_NumCounts];
} AosMemShowEntry_t;

static aos_hashtab_t *sgHash;
static aos_lock_t 		 sgLock;
static int sgMemoryInitFlag = 0;
static int AosMemory_Status = eAosMemoryStatus_NotSet;
static int sgMaxMemEntries = 50;
static AosMemShowEntry_t *sgMemEntries = 0;


typedef struct AosMemEntry
{
	char	file[eAosMemory_FileNameLen+1];
	int		line;
} AosMemEntry_t;


typedef struct AosMemCliEntry
{
	char	file[eAosMemory_FileNameLen+1];
	int		line;
	int		count;
} AosMemCliEntry_t;


// 
// Description
// It returns 1 if all bytes pointed to by 'mem' equals to 'value'.
// Otherwise, it returns 0. On errors, it returns -1.
//
int aos_memcmp_equal(void *mem, const char value, const int size)
{
	aos_assert_r(mem, -1);
	aos_assert_r(size > 0, -1);
	int i;
	char *mm = (char *)mem;
	for (i=0; i<size; i++)
	{
		if (mm[i] != value) return 0;
	}

	return 1;
}


static u32 memory_hash_func(aos_hashtab_t *h, void *key)
{
	unsigned long value = (unsigned long)key;
	return value + (value >> 16);
}

static int memory_keycmp_func(aos_hashtab_t *h, void *k1, void *k2)
{
	if (((unsigned long)k1) < ((unsigned long)k2)) return -1;
	if (((unsigned long)k1) > ((unsigned long)k2)) return 1;
	return 0;
}


int AosMemory_init(char *errmsg)
{
	int ret = 0;
	if (sgMemoryInitFlag) return 0;

	aos_init_lock(&sgLock);
	sgHash = aos_hashtab_create(memory_hash_func, memory_keycmp_func, 4096);

	sgMemoryInitFlag = 1;

	if (ret)
	{
		sprintf(errmsg, "Failed to initialize the hash table: %d", ret);
		aos_alarm(errmsg); 
		AosMemory_Status = eAosMemoryStatus_InitFailed;
		return ret;
	}

	sgMemEntries = (AosMemShowEntry_t *)
		aos_malloc_raw(sizeof(AosMemShowEntry_t) * sgMaxMemEntries,GFP_KERNEL);
	if (!sgMemEntries)
	{
		AosMemory_Status = eAosMemoryStatus_InitFailed;
		return -eAosRc_MemoryCatcherErr;
	}

	memset(sgMemEntries, 0, sizeof(AosMemShowEntry_t) * sgMaxMemEntries);

	return 0;
}


void * AosMemory_malloc(char *file, int line, u32 size, int gfp)
{
	// 
	// 1. Allocate the memory
	// 2. Create an entry and add it to the hash table
	//
	int ret;
	u32 name_len;
	AosMemEntry_t *entry;
	char *ptr;
	void *memory;

	if (sgMemoryInitFlag == 0 || AosMemory_Status == eAosMemoryStatus_InitFailed) 
	{
		ptr = (char *)aos_malloc_raw(size,gfp);
		return ptr;
	}

	memory = (AosMemEntry_t *)
		aos_malloc_raw(size + sizeof(AosMemEntry_t),gfp);
	ptr = (char *)memory;
	entry = (AosMemEntry_t *)(ptr + size);

//aos_printk("%s:%d: Allocated: %x\n", __FILE__, __LINE__, (unsigned int)ptr);
	if (!memory) return 0;

	name_len = strlen(file);
	if (name_len >= eAosMemory_FileNameLen)
	{
		int delta = name_len - eAosMemory_FileNameLen + 1;
		file += delta;
		// strcpy(entry->file, &file[name_len-eAosMemory_FileNameLen-1]);
	}
//	else
//	{
//		strcpy(entry->file, file);
//	}
	strcpy(entry->file, file);
	entry->file[strlen(file)] = 0;

	entry->line = line;
	// entry->memory = ptr + sizeof(AosMemEntry_t);
	
	aos_lock(&sgLock);
	ret = aos_hashtab_insert(sgHash, memory, entry);
	if (ret)
	{
		aos_printk("********** %s:%d: Failed to add: %d\n", __FILE__, __LINE__, ret);
	}

	aos_unlock(&sgLock);
	return memory;
}


int AosMemory_free(void *ptr)
{
	int ret = 0;
	void *entry = 0;

	if (ptr == 0) return 0;

	if (sgMemoryInitFlag == 0 || AosMemory_Status == eAosMemoryStatus_InitFailed) 
	{
		aos_free_raw(ptr);
		return 0;
	}

	aos_lock(&sgLock);

	aos_hashtab_remove(sgHash, ptr, 0, 0);
	if (ret)
	{
		aos_free_raw(ptr);
		aos_unlock(&sgLock);
		return 0;
	}

	if (!entry)
	{
		aos_printk("Retrieved is null\n");
	}

	aos_free_raw(ptr);
	aos_unlock(&sgLock);
	
	return 0;
}


int AosMemory_initCli(char *data,
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg,
                      const int errlen)
{
	int ret = AosMemory_init(errmsg);	
	*length = 0;

	return ret;
}


int AosMemory_showCli(char *data,
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg,
                      const int errlen)
{
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
	char local[200];

	if (sgMemoryInitFlag)
	{
		sprintf(local, "Memory Check Initialized\n");
	}
	else
	{
		sprintf(local, "Memory Check not initialized\n");
	}
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	switch (AosMemory_Status)
	{
	case eAosMemoryStatus_NotSet:
		 sprintf(local, "Status: Not Turned On\n");
		 break;

	case eAosMemoryStatus_Set:
		 sprintf(local, "Status: Turned On\n");
		 break;

	case eAosMemoryStatus_InitFailed:
		 sprintf(local, "Status: Initialization Failed\n");
		 break;

	default:
		 sprintf(local, "Status: Unrecognized status: %d", AosMemory_Status);
		 break;
	}
	
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	
	return 0;
}


int AosMemory_setStatusCli(char *data,
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg,
                      const int errlen)
{
	// 
	// memory set status <on|off>
	//
	char *status = parms->mStrings[0];

	*length = 0;

	if (!status)
	{
		sprintf(errmsg, "Missing parameter <onoff>");
		return -eAosRc_MemoryCliErr;
	}

	if (strcmp(status, "on") == 0)
	{
		AosMemory_Status = eAosMemoryStatus_Set;
		return 0;
	}

	if (strcmp(status, "off") == 0)
	{
		AosMemory_Status = eAosMemoryStatus_NotSet;
		return 0;
	}

	sprintf(errmsg, "Invalid status paramter: %s", status);
	return -eAosRc_MemoryCliErr;
}


int AosMemory_getRsltCli(char *data,
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg,
                      const int errlen)
{
	// IMPORTANT
	// temporarily commented out by Chen Ding, 01/18/2008
	/*
	struct aos_list_head *ee;
	AosMemEntry_t *entry;
	int i, j;
	char local[200];
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
	int found;

	if (sgMemoryInitFlag == 0)
	{
		sprintf(errmsg, "Memory check not initialized yet");
		return eAosRc_MemoryCliErr;
	}

	if (AosMemory_Status == eAosMemoryStatus_InitFailed)
	{
		sprintf(errmsg, "Memory initialization failed");
		return eAosRc_MemoryCliErr;
	}

	// 
	// Shift the counters
	//
	for (i=0; i<sgMaxMemEntries; i++)
	{
		for (j=0; j<eAosMemory_NumCounts-1; j++)
		{
			sgMemEntries[i].count[j] = sgMemEntries[i].count[j+1];
		}

		sgMemEntries[i].count[eAosMemory_NumCounts-1] = 0;
	}

	aos_list_for_each(ee, &sgHash->list)
	{
		entry = (AosMemEntry_t *)(((struct AosHashBinRaw_entry*)ee)->value);
		found = 0;
		for (i=0; i<sgMaxMemEntries; i++)
		{
			if (strcmp(entry->file, sgMemEntries[i].file) == 0 &&
				entry->line == sgMemEntries[i].line)
			{
				sgMemEntries[i].count[eAosMemory_NumCounts-1]++;
				found = 1;
				break;
			}
		}

		if (!found)
		{
			// 
			// Did not find. Add it.
			//
			// 
			// Find an idle slot
			//
			for (i=0; i<sgMaxMemEntries; i++)
			{
				if (sgMemEntries[i].file[0] == 0)
				{
					strcpy(sgMemEntries[i].file, entry->file);
					sgMemEntries[i].file[strlen(entry->file)] = 0;
					sgMemEntries[i].line = entry->line;
					sgMemEntries[i].count[eAosMemory_NumCounts-1] = 1;
					break;
				}
			}

			if (i >= sgMaxMemEntries)
			{
				aos_printk("Too many entries\n");
			}
		}
	}

	//
	// Remove the ones whose counts are 0.
	//
	for (i=0; i<sgMaxMemEntries; i++)
	{
		if (sgMemEntries[i].count[eAosMemory_NumCounts-1] == 0)
		{
			sgMemEntries[i].file[0] = 0;
			for (j=0; j<eAosMemory_NumCounts; j++)
			{
				sgMemEntries[i].count[j] = 0;
			}
		}
	}

	// 
	// Sort it
	//
	for (i=0; i<sgMaxMemEntries; i++)
	{
		int diff = sgMemEntries[i].count[eAosMemory_NumCounts-1] - 
			sgMemEntries[i].count[eAosMemory_NumCounts-2];
		for (j=i+1; j<sgMaxMemEntries-1; j++)
		{
			if (sgMemEntries[j].count[eAosMemory_NumCounts-1] - 
				sgMemEntries[j].count[eAosMemory_NumCounts-2] > diff)
			{
				AosMemShowEntry_t entry;
				memcpy(&entry, &sgMemEntries[i], sizeof(AosMemShowEntry_t));
				memcpy(&sgMemEntries[i], &sgMemEntries[j], sizeof(AosMemShowEntry_t));
				memcpy(&sgMemEntries[j], &entry, sizeof(AosMemShowEntry_t));
			}
		}
	}

	for (i=0; i<sgMaxMemEntries; i++)
	{
		if (strlen(sgMemEntries[i].file) > 0)
		{
			sprintf(local, "%-20s %-10d %-5d %-5d %-5d\n", 
				sgMemEntries[i].file, 
				sgMemEntries[i].line, 
				sgMemEntries[i].count[eAosMemory_NumCounts-3], 
				sgMemEntries[i].count[eAosMemory_NumCounts-2], 
				sgMemEntries[i].count[eAosMemory_NumCounts-1]); 
        	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		}
	}

	*length = rsltIndex;
	return 0;
	*/
	return -1;
}


int AosMemory_setStatus()
{
	AosMemory_Status = eAosMemoryStatus_Set;
	return 0;
}


/*
 * Not sure whether we need this function. Chen Ding, 01/18/2008
 *
int AosMemory_checkMemory(char *ptr)
{
	int ret;
	void *entry;

	aos_lock(&sgLock);
	ret = AosHashBinRaw_get(sgHash, (char *)&ptr, 4, &entry, 0);
	aos_unlock(sgLock);
	return ret;
}
*/


int AosMemory_saveConfCli(char *data,
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg,
                      const int errlen)
{
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
	char local[200];

	if (AosMemory_Status == eAosMemoryStatus_Set)
	{
		 sprintf(local, "<Cmd>memory set status off</Cmd>");
	}
	else
	{
		 sprintf(local, "<Cmd>memory set status on</Cmd>");
	}
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	
	return 0;
}


