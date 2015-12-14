////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ServerGroup.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Servers/ServerGroup.h"

#include <KernelSimu/string.h>

#include "aos/aosKernelApi.h"
#include "aos/aosReturnCode.h"
#include "aos/aosKernelAlarm.h"
#include "aosUtil/StringUtil.h"
#include "KernelUtil/KernelMem.h"
#include "aosUtil/List.h"
#include "Servers/AppServer.h"

#include "KernelSimu/aosKernelMutex.h"
#include "KernelSimu/aosKernelDebug.h"

#ifdef __KERNEL__
#include "linux/module.h"
#endif

// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>



char *sgMethodNames[] = 
{
	"RR"
};

int aosServerGroup_init()
{
	return 0;
}


int aosServerGroup_getNumGroups(void)
{
	return (int)aosMgdObj_getNumObjs(eAosObjType_ServerGroup);
}


int aosServerGroup_addCli(char *data, 
						  unsigned int *length, 
						  struct aosKernelApiParms *parms, 
						  char *errmsg, 
						  const int errlen)
{
	// 
	// server group add <group-name> [<method>]
	//
	char *gname = parms->mStrings[0];
	char *method = parms->mStrings[1];
	struct aosServerGroup *sg;
	int ret;
	*length = 0;

	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	sg = aosServerGroup_get(gname);
	if (sg)
	{
		aosServerGroup_put(sg);
		sprintf(errmsg, "Server group not found: %s", gname);
		return eAosRc_ServerGroupDefined;
	}

	ret = aosServerGroup_constructor(gname, method, &sg);
	if (!sg)
	{
		sprintf(errmsg, "Failed to create the group");
		return ret;
	}

	// 
	// The application proxy has been created.
	// 
	return 0;
}


int aosServerGroup_showCli(char *data, 
						   unsigned int *length, 
						   struct aosKernelApiParms *parms, 
						   char *errmsg, 
						   const int errlen)
{
	// 
	// server group show [<gname>]
	//
	int numArgs = parms->mNumIntegers;

	if (numArgs == 0)
	{
		return aosServerGroup_list(data, length, parms, errmsg, errlen);
	}

	return aosServerGroup_show(data, length, parms, errmsg, errlen);
}


int aosServerGroup_list(char *data, 
						unsigned int *length, 
						struct aosKernelApiParms *parms,
						char *errmsg, 
						const int errlen)
{
	// 
	// AppName  Type	Addr    Port   Protocol
	// -----------------------------------------------
	// 
	struct aosServerGroup *server;
	struct aosObjListHead *entry;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_ServerGroup);
	char local[200];
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char *rsltBuff = aosKernelApi_getBuff(data);

	if (aosMgdObj_getNumObjs(eAosObjType_ServerGroup) == 0)
	{
	    sprintf(local, "No Server Group found!\n");
        aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
        *length = rsltIndex;
		return 0;
	}

	sprintf(local, "Group Name           Method \n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "-----------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
	aosObjList_for_each(entry, head)
	{
		server = (struct aosServerGroup *)entry;
	    sprintf(local, "%-20s %-10s\n", server->mName, sgMethodNames[server->mMethod]);
        aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}

	*length = rsltIndex;
	return 0;
}


struct aosServerGroup *
aosServerGroup_get(const char *name)
{
	struct aosServerGroup *server;
	struct aosObjListHead *entry;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_ServerGroup);
	aosKernelAssert(head, 0);

	aosObjList_for_each(entry, head)
	{
		server = (struct aosServerGroup *)entry;
		if (strcmp(server->mName, name) == 0)
		{
			atomic_inc(&server->mRefcnt);
			return server;
		}
	}

	// 	
	// Did not find
	//
	return 0;
}


int aosServerGroup_constructor(char *name, 
							   char *method, 
							   struct aosServerGroup **server)
{
	struct aosServerGroup *theServer;
	int m;
	if (strcmp(method, "rr") == 0)
	{
		m = eAosServerGroupMethod_RR;
	}
	else
	{
		*server = 0;
		return eAosRc_InvalidMethod;
	}
		
	theServer = (struct aosServerGroup *)
		aosMgdObj_alloc(eAosObjType_ServerGroup, sizeof(struct aosServerGroup));
	*server = theServer;
	if (!theServer)
	{
		return aosAlarm(eAosAlarmMemErr);
	}
	
	atomic_set(&theServer->mRefcnt, 1);

	strcpy(theServer->mName, name);
	theServer->mMethod = m;
	AOS_INIT_LIST_HEAD(&theServer->mMembers);

	return 0;
}


int aosServerGroup_destructor(struct aosServerGroup *self)
{
	struct aosServerGroupMember *member;
	self->mName[0] = 0;
	self->mMethod = 0;
	
	member = (struct aosServerGroupMember *)self->mMembers.next;
	while (member != (struct aosServerGroupMember *)&self->mMembers)
	{
		// Chen Ding, 03/03/2007
		// aosListDel(&self->mMembers, (struct aosListNode *)member);
		aos_list_del((struct aos_list_head *)member);
		aosServerGroupMember_destructor(member);
		member = (struct aosServerGroupMember *)self->mMembers.next;
	}

	aosMgdObj_release((struct aosMgdObj*)self);
	return 0;
}


int aosServerGroupMember_destructor(struct aosServerGroupMember *self)
{
	if (self->server)
	{
		aosAppServer_put(self->server);
	}

	self->server = 0;
	self->priority = 0;

	aosMgdObj_release((struct aosMgdObj*)self);
	return 0;
}


int aosServerGroup_show(char *data, 
						unsigned int *length, 
						struct aosKernelApiParms *parms,
						char *errmsg, 
						const int errlen)
{
	// 
	// This is to show an individual application proxy. 
	//
	char *groupname = parms->mStrings[0];
	char theName[100];
	struct aosServerGroup *group;
	char local[200];
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	*length = 0;

	group = aosServerGroup_get(groupname);
	if (!group)
	{
		return eAosRc_ServerGroupNotDefined;
	}

	strcpy(theName, groupname);

	sprintf(local, "------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Server Name:                 %-s\n", theName);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Method:                      %-s\n", sgMethodNames[group->mMethod]);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Refcnt:                      %-d\n", atomic_read(&group->mRefcnt)-1);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	// Chen Ding, 03/30/2007
	// if (group->mMembers.qlen == 0)
	if (aos_list_empty(&group->mMembers))
	{
		sprintf(local, "No members\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	else
	{
		struct aosServerGroupMember *member;
		struct aosObjListHead *entry;
		struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_ServerGroupMember);
		sprintf(local, "Members\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		sprintf(local, "   Member Name          Priority\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

		aosKernelAssert(head, eAosAlarmProgErr);
		aosObjList_for_each(entry, head)
		{
			member = (struct aosServerGroupMember *)entry;
			sprintf(local, "   %-20s %d\n", member->server->mName, member->priority);
    		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		}
	}

	sprintf(local, "------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
    *length = rsltIndex;
	aosServerGroup_put(group);
	return 0;
}


int aosServerGroup_delCli(char *data, 
						  unsigned int *length, 
						  struct aosKernelApiParms *parms,
						  char *errmsg, 
						  const int errlen)
{
	// 
	// server group remove <groupname> 
	//
	char *gname = parms->mStrings[0];
	struct aosServerGroup *group;
	*length = 0;

	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	group = aosServerGroup_get(gname);
	if (!group)
	{
		sprintf(errmsg, "Group not found: %s", gname);
		return eAosRc_ServerGroupNotDefined;
	}

	// 
	// Check whether the group is used by others. 
	// 
	if (atomic_read(&group->mRefcnt) > 2)
	{
		aosServerGroup_put(group);
		sprintf(errmsg, "Group used by others");
		return eAosRc_ServerUsedByOthers;
	}

	aosServerGroup_destructor(group);
	return 0;
}
	

int aosServerGroup_clearAllCli(char *data, 
							   unsigned int *length, 
							   struct aosKernelApiParms *parms,
							   char *errmsg, 
							   const int errlen)
{
	struct aosServerGroup *group;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_ServerGroup);
	aosKernelAssert(head, eAosAlarmProgErr);

	*length = 0;
	while (!aosObjList_empty(head))
	{
		group = (struct aosServerGroup *)head->nextobj;
		if (atomic_read(&group->mRefcnt) > 1)
		{
			sprintf(errmsg, "Group used by others: %s", group->mName);
			return eAosRc_ServerGroupUsedByOthers;
		}

		aosServerGroup_destructor(group);
	}

	return 0;
}


int aosServerGroup_addMemberCli(char *data, 
								unsigned int *length, 
								struct aosKernelApiParms *parms,
								char *errmsg, 
								const int errlen)
{
	// 
	// server group add member <group-name> <server-name> <priority>
	// 
	char *gname = parms->mStrings[0];
	char *sname = parms->mStrings[1];
	int priority = parms->mIntegers[0];
	struct aosServerGroup *group;
	struct aosAppServer *server;
	struct aosServerGroupMember *member;
	*length = 0;

	// 
	// Check whether the group has been defined.
	//
	group= aosServerGroup_get(gname);
	if (!group)
	{
		sprintf(errmsg, "Group not found: %s", gname);
		return eAosRc_ServerNotDefined;
	}

	server = aosAppServer_get(sname);
	if (!server)
	{
		// 
		// The server is not defined yet. 
		// 
		aosServerGroup_put(group);
		sprintf(errmsg, "Server not found: %s", sname);
		return eAosRc_AppServerNotDefined;
	}

	if (priority < 0 || priority > eAosServerGroupMaxPriority)
	{
		aosServerGroup_put(group);
		aosAppServer_put(server);
		sprintf(errmsg, "Invalid priority value: %d", priority);
		return eAosRc_InvalidPriority;
	}

	member = aosServerGroupMember_constructor(server, priority);
	if (!member)
	{
		aosServerGroup_put(group);
		aosAppServer_put(server);
		sprintf(errmsg, "Failed to create member");
		return aosAlarm(eAosAlarmMemErr);
	}

	// Chen Ding, 03/03/2007
	// aosListAppend(&group->mMembers, (struct aosListNode *)member);
	aos_list_add_tail((struct aos_list_head *)member, &group->mMembers);

	aosServerGroup_put(group);
	aosAppServer_put(server);
	return 0;
}


struct aosServerGroupMember *
aosServerGroupMember_constructor(struct aosAppServer *server, int priority)
{
	struct aosServerGroupMember *member = (struct aosServerGroupMember *)
		aosMgdObj_alloc(eAosObjType_ServerGroupMember, sizeof(struct aosServerGroupMember));
	if (!member)
	{
		return 0;
	}

	aosAppServer_hold(server);
	member->priority = priority;
	member->server = server;
	
	return member;
}


int aosServerGroup_delMemberCli(char *data, 
								unsigned int *length, 
								struct aosKernelApiParms *parms, 
								char *errmsg, 
								const int errlen)
{
	// 
	// server group member remove <group-name> <server-name>
	// 
	char *gname = parms->mStrings[0];
	char *sname = parms->mStrings[1];
	struct aosServerGroupMember *member;
	struct aosObjListHead *entry;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_ServerGroupMember);
	struct aosServerGroup *group;

	*length = 0;

	// 
	// Check whether the group has been defined.
	//
	group= aosServerGroup_get(gname);
	if (!group)
	{
		sprintf(errmsg, "Group not found: %s", gname);
		return eAosRc_ServerGroupNotDefined;
	}

	aosKernelAssert(head, eAosAlarmProgErr);
	aosObjList_for_each(entry, head)
	{
		member = (struct aosServerGroupMember *)entry;
		if (strcmp(member->server->mName, sname) == 0)
		{
			// Chen Ding, 03/03/2007
			// aosListDel(&group->mMembers, (struct aosListNode *)member);
			aos_list_del((struct aos_list_head *)member);
			aosServerGroupMember_destructor(member);
			aosServerGroup_put(group);
			return 0;
		}
	}

	aosServerGroup_put(group);
	sprintf(errmsg, "Server not in the group: %s", sname);
	return eAosRc_ServerNotInGroup;
}


int aosServerGroup_clearMemberCli(char *data, 
								  unsigned int *length, 
								  struct aosKernelApiParms *parms,
								  char *errmsg, 
								  const int errlen)
{
	// 
	// server group member clear <group-name> 
	// 
	char *gname = parms->mStrings[0];
	struct aosServerGroup *group;
	struct aosServerGroupMember *member;

	*length = 0;

	// 
	// Check whether the group has been defined.
	//
	group= aosServerGroup_get(gname);
	if (!group)
	{
		sprintf(errmsg, "Group not found: %s", gname);
		return eAosRc_ServerGroupNotDefined;
	}

	member = (struct aosServerGroupMember *)group->mMembers.next;
	while (member != (struct aosServerGroupMember *)&group->mMembers)
	{
		// Chen Ding, 03/03/2007
		// aosListDel(&group->mMembers, (struct aosListNode *)member);
		aos_list_del((struct aos_list_head *)member);
		aosServerGroupMember_destructor(member);
		member = (struct aosServerGroupMember *)group->mMembers.next;
	}

	aosServerGroup_put(group);
	return 0;
}


int aosServerGroupMember_hold(struct aosServerGroup *self)
{
	atomic_inc(&self->mRefcnt);
	return 0;
}


int aosServerGroupMember_put(struct aosServerGroup *self)
{
	atomic_dec(&self->mRefcnt);
	return 0;
}


int aosServerGroup_getNextServer(struct aosServerGroup *self, struct aosAppServer **server)
{
	// 
	// It retrieves the next member. This is controlled by the method. 
	// Currently, we simply return the first one.
	// 
	struct aosServerGroupMember *member;
	if ((struct aos_list_head *)self->mMembers.next == &(self->mMembers))
	{
		*server = 0;
		return aosAlarm(eAosAlarm);
	}

	member = (struct aosServerGroupMember *)self->mMembers.next;
	*server = member->server;

	return 0;
}

#ifdef __KERNEL__
EXPORT_SYMBOL( aosServerGroup_get );
EXPORT_SYMBOL( aosServerGroup_getNextServer );
#endif

