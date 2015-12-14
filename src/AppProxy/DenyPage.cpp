////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DenyPage.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppProxy/DenyPage.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "AppProxy/ReturnCode.h"
#include "KernelSimu/kernel.h"
#include "aosUtil/Memory.h"
#include "Ktcpvs/tcp_vs.h"
#include "Ktcpvs/tcp_vs_def.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"

extern int tcp_vs_denypage_remove(struct aos_deny_page *page);
extern int tcp_vs_denypage_assigned_show(char* buff, unsigned int *rsltIndex, int bufflen, struct aos_deny_page *page);
extern int tcp_vs_denypage_save_config(char* buff, unsigned int *rsltIndex, int bufflen, struct aos_deny_page *page);
extern struct tcp_vs_service *tcp_vs_lookup_byident(const struct tcp_vs_ident *id);

static struct aos_deny_page *sg_deny_pages[eAosMaxDenyPages];
static int sg_deny_page_init_flag = 0;

int aos_deny_page_init(void)
{
	int i;

	if (sg_deny_page_init_flag)
	{
		// 
		// It has already been initialized.
		//
		return aos_warn(eAosMD_DenyPage, eAosRc_DenyPageReinit, 
			("Deny Page module being initiated multiple times"));
	}
	aos_min_log(eAosMD_DenyPage, "aos deny page module init");	
	for (i=0; i<eAosMaxDenyPages; i++)
	{
		sg_deny_pages[i] = 0;
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
int aos_deny_page_add(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen)
{
	char *deny_page_name;
	char *deny_page_contents;
	char *deny_page_file_path;
	unsigned int contents_length = 0;
	struct aos_deny_page *page  = 0;
	int i;
	deny_page_name = parms->mStrings[0];
	deny_page_file_path = parms->mStrings[1];
	deny_page_contents = parms->mStrings[2];
	
	aos_trace("deny_page_name:%s\n", deny_page_name);
	aos_trace("deny_page_file_name:%s\n", deny_page_file_path);
	
	*length = 0;

	if ((deny_page_name == 0)||(*deny_page_name == '\0'))
	{
		snprintf(errmsg, errlen, "deny page name error\n");
		return -eAosRc_DenyPageCliErr;
	}

	if ((deny_page_file_path == 0)||(*deny_page_file_path == '\0'))
	{
		snprintf(errmsg, errlen, "deny page file name error\n");
		return -eAosRc_DenyPageCliErr;
	}

	if ((deny_page_contents == 0)||(*deny_page_contents == '\0'))
	{
		snprintf(errmsg, errlen, "no deny page contents in file\n");
		return -eAosRc_DenyPageCliErr;
	}

	if (strlen(deny_page_name)>eAosDenyPageMaxName)
	{
		snprintf(errmsg, errlen, "deny page name too long\n");
		return -eAosRc_DenyPageCliErr;
	}

	if (strlen(deny_page_file_path)>eAosDenyPageMaxPathLen)
	{
		snprintf(errmsg, errlen, "file name too long\n");
		return -eAosRc_DenyPageCliErr;
	}

	if (strlen(deny_page_contents)> eAosDenyPageMaxLength)
	{
		aos_alarm(eAosMD_DenyPage, eAosAlarm_DenyPageLengthExceed, 
			"Contents too long: %d", strlen(deny_page_contents));
		sprintf(errmsg, "Contents too long: %d", strlen(deny_page_contents));
		return -eAosRc_DenyPageCliErr;
	}
	else
	{
		contents_length = strlen(deny_page_contents)+1;
	} 

	page = (struct aos_deny_page*)aos_malloc(sizeof(struct aos_deny_page));
	if (!page)
	{
		aos_alarm(eAosMD_DenyPage, eAosAlarm_DenyPagePoolMem, 
			"Memory error: %d", sizeof(struct aos_deny_page));
		sprintf(errmsg, "Memory error: %d", sizeof(struct aos_deny_page));
		return -eAosRc_DenyPageCliErr;
	}
	
	page->page_contents = (char*)aos_malloc(contents_length + 1 );
	if (!page->page_contents)
	{
		aos_alarm(eAosMD_DenyPage, eAosAlarm_DenyPagePoolMem, 
			"Memory error: %d", contents_length);
		sprintf(errmsg, "Memory error: %d", contents_length);
		aos_free(page);
		return -eAosRc_DenyPageCliErr;
	}

	memset(page->page_name, 0, sizeof(page->page_name));
	strncpy(page->page_name, deny_page_name, sizeof(page->page_name));
	memset(page->page_contents, 0, contents_length +1 );
	strncpy(page->page_contents, deny_page_contents, contents_length +1);
	strncpy(page->page_path, deny_page_file_path, eAosDenyPageMaxPathLen);
	
	for (i=0; i< eAosMaxDenyPages; i++)
	{
		if (sg_deny_pages[i] == 0)
		{
			sg_deny_pages[i] = page;
			return eAosRc_Success;	// successfull create a new deny page
		}
	}
	
	aos_alarm(eAosMD_DenyPage, eAosAlarm_DenyPageMaxExceed, 
		"Too many pages: %d", eAosMaxDenyPages);
	sprintf(errmsg, "Too many pages. Maximum allowed: %d", eAosMaxDenyPages);
	aos_free(page->page_contents);
	aos_free(page);
	return -eAosRc_DenyPageCliErr;
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
int aos_deny_page_remove(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen)
{
	struct aos_deny_page *page;
	char* deny_page_name;
	unsigned int deny_page_name_length;
	char* p;
	int i;
	deny_page_name = parms->mStrings[0];
	*length = 0;

	if ((deny_page_name == 0)||(*deny_page_name == '\0'))
	{
		snprintf(errmsg, errlen, "deny page name error\n");
		return -eAosRc_DenyPageCliErr;
	}

	p = deny_page_name;
	while (*p!='\0')  p++;
	if ((p - deny_page_name)>=eAosDenyPageMaxName)
	{
		snprintf(errmsg, errlen, "deny page name too long\n");
		return -eAosRc_DenyPageCliErr;
	}
	deny_page_name_length = p - deny_page_name + 1;

	for(i =0; i< eAosMaxDenyPages; i++)
	{
		page = sg_deny_pages[i];
		if (page && strcmp(deny_page_name, page->page_name) == 0)
			break;
	}
	if (i >= eAosMaxDenyPages)
	{
		snprintf(errmsg, errlen, "deny page \"%s\" not found\n", deny_page_name);
		return -eAosRc_DenyPageCliErr;
	}
	
	tcp_vs_denypage_remove(page);

	aos_free(page->page_contents);
	aos_free(page);
	sg_deny_pages[i] = NULL;

	return 0;
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
int aos_deny_page_show(char* data, 
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
	struct aos_deny_page *page;
	char tmpbuf[256];
	int i;
	int found = 0;
	
	if (parms->mNumStrings > 0 || parms->mNumIntegers > 0)
	{
		sprintf(errmsg, "The command should not have parameters: %d, %d", 
			parms->mNumStrings, parms->mNumIntegers);
		return -eAosRc_DenyPageCliErr;
	}

	sprintf(tmpbuf, "Page Name          File Name");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
	sprintf(tmpbuf, "\n--------------------------------");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));

	for (i = 0; i< eAosMaxDenyPages; i++)
	{
		page = sg_deny_pages[i];
		if (!page) continue;

		found++;
		snprintf(tmpbuf, 256,
			"\n%-16s   %s", page->page_name, page->page_path);
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
		tcp_vs_denypage_assigned_show(rsltBuff, &rsltIndex, optlen, page);
	}

	if (found == 0)
	{
		sprintf(tmpbuf, "\nNo deny pages found");
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
	}

	sprintf(tmpbuf, "\n--------------------------------");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
		
	*length =rsltIndex;
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//	Assign a DenyPage pointer to a tcp_vs_service
//////////////////////////////////////////////////////////////////////////
int aos_deny_page_assign(char* data,
						 unsigned int *length,
						 struct aosKernelApiParms *parms,
						 char* errmsg,
						 const int errlen)
{
	char* tcp_vs_name;
	char* deny_page_name;
	unsigned int tcp_vs_name_length = 0;
	unsigned int deny_page_name_length = 0;
	struct aos_deny_page *page  = 0;
	struct tcp_vs_ident ident;
	struct tcp_vs_service	*svc = 0;
	char *p;
	int i;
	
	tcp_vs_name = parms->mStrings[0];
	deny_page_name = parms->mStrings[1];
	*length = 0;

	if ((tcp_vs_name == 0)||(*tcp_vs_name == '\0'))
	{
		snprintf(errmsg, errlen, "application name error\n");
		return -eAosRc_DenyPageCliErr;
	}
	if ((deny_page_name == 0)||(*deny_page_name == '\0'))
	{
		snprintf(errmsg, errlen, "deny page name error\n");
		return -eAosRc_DenyPageCliErr;
	}

	p = deny_page_name;
	while (*p!='\0')  p++;
	if ((p - deny_page_name)>=eAosDenyPageMaxName)
	{
		snprintf(errmsg, errlen, "deny page name too long\n");
		return -eAosRc_DenyPageCliErr;
	}
	deny_page_name_length = p - deny_page_name + 1;

	p = tcp_vs_name;
	while (*p!='\0') p++;
	if ((p - tcp_vs_name)>= KTCPVS_IDENTNAME_MAXLEN)
	{
		snprintf(errmsg, errlen, "applicaton name error\n");
		return -eAosRc_DenyPageCliErr;
	}
	tcp_vs_name_length = p - tcp_vs_name + 1;


	memset(ident.name, 0, sizeof(ident.name));
	strncpy(ident.name, tcp_vs_name, sizeof(ident.name));

	svc = (struct tcp_vs_service*)tcp_vs_lookup_byident(&ident);
	if (!svc)
	{
		snprintf(errmsg, errlen, "application %s not found", tcp_vs_name);
		return -eAosRc_DenyPageCliErr;
	}

	for(i =0; i< eAosMaxDenyPages; i++)
	{
		page = sg_deny_pages[i];
		if (!page) continue;
		if (strcmp(deny_page_name, page->page_name) == 0)
			break;
	}
	if (i >= eAosMaxDenyPages)
	{
		snprintf(errmsg, errlen, "page %s not found", deny_page_name);
		return -eAosRc_DenyPageCliErr;
	}

	svc->deny_page = sg_deny_pages[i];
	return 0;
}



//////////////////////////////////////////////////////////////////////////
// Remove the DenyPage pointer from a tcv_vs_service
//////////////////////////////////////////////////////////////////////////
int aos_deny_page_deassign(char* data,
						 unsigned int *length,
						 struct aosKernelApiParms *parms,
						 char* errmsg,
						 const int errlen)
{
	char* tcp_vs_name;
	struct tcp_vs_ident ident;
	struct tcp_vs_service	*svc = 0;
	char *p;
	
	tcp_vs_name = parms->mStrings[0];
	*length = 0;

	if ((tcp_vs_name == 0)||(*tcp_vs_name == '\0'))
	{
		snprintf(errmsg, errlen, "application name error\n");
		return -eAosRc_DenyPageCliErr;
	}
	p = tcp_vs_name;
	while (*p!='\0') p++;
	if ((p - tcp_vs_name) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		snprintf(errmsg, errlen, "application name error\n");
		return -eAosRc_DenyPageCliErr;
	}

	memset(ident.name, 0, KTCPVS_IDENTNAME_MAXLEN);
	strncpy(ident.name, tcp_vs_name, KTCPVS_IDENTNAME_MAXLEN);

	svc = (struct tcp_vs_service*)tcp_vs_lookup_byident(&ident);
	if (!svc)
	{
		snprintf(errmsg, errlen, "application %s not found\n", tcp_vs_name);
		return -eAosRc_DenyPageCliErr;
	}
	if (svc->deny_page)
	{
		svc->deny_page = NULL;
	}

	return 0;
}

int aos_deny_page_save_config(char *data, 
						   unsigned int *length, 
						   struct aosKernelApiParms *parms,
						   char *errmsg, 
						   const int errlen)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int optlen = *length;
	unsigned int rsltIndex = 0;
	struct aos_deny_page *page;
	char tmpbuf[256];
	int i;

	for(i =0; i< eAosMaxDenyPages; i++)
	{
			page = sg_deny_pages[i];
			if (!page) continue;
			sprintf(tmpbuf, "<Cmd>deny page import %s %s</Cmd>\n", page->page_name, page->page_path);
			aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, tmpbuf, strlen(tmpbuf));
			tcp_vs_denypage_save_config(rsltBuff, &rsltIndex, optlen, page);
	}
	*length = rsltIndex;
	return eAosRc_Success;
}

int aos_deny_page_clearall_config(char *data, 
							   unsigned int *length, 
							   struct aosKernelApiParms *parms,
							   char *errmsg, 
							   const int errlen)
{
	struct aos_deny_page *page;
	int i;

	*length = 0;

	for(i =0; i< eAosMaxDenyPages; i++)
	{
		page = sg_deny_pages[i];
		if (!page) continue;

		tcp_vs_denypage_remove(page);
		aos_free(page->page_contents);
		aos_free(page);
		sg_deny_pages[i] = NULL;
		
	}
	return eAosRc_Success;
}


