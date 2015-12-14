////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommGroup.cpp
// Description:
//	Connections in PacketLink networks are through comm
//  groups. A comm group is created for a specific purpose, such
//  as for exchanging MGCP messages between two PacketLink network nodes.
//  If there are more than one comms in a group, by definition,
//  any comm serves equally well as other comms do. To the 
//  user of this group, it cares nothing about which member comm
//  to use. For instance, if one wants to send an MGCP message to 
//  another SPNR, if it knows through which group to send the message, 
//  it can ask the group to send the message.
//  In this case, it does not care through which comm to 
//  send the message.
//
//	Each Comm Group handles only one type of message category.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/CommGroup.h"

#include "Alarm/Alarm.h"
#include "Network/Network.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/SerialTo.h"
#include "UtilComm/GroupRRobin.h"
#include "UtilComm/GroupReliable.h"
#include "UtilComm/CommMgr.h"
#include "XmlParser/XmlItem.h"
#include "XmlUtil/XmlItemName.h"


extern OmnNetwork *		OmnNetworkSelf;

static int			sgGroupId = 10;
static OmnMutexPtr	sgGroupIdLock = OmnNew OmnMutex();


OmnCommGroup::OmnCommGroup(const OmnCommMgrPtr &commMgr)
:
mName("NoName"),
mProtocol(OmnCommProt::eInvalid),
mTargetId(-1),
mCommMgr(commMgr),
mProxySenderAddr(false)
{
	sgGroupIdLock->lock();
    mGroupId = sgGroupId++;
    sgGroupIdLock->unlock();
}


OmnCommGroup::OmnCommGroup(const OmnString &name,
		const OmnCommProt::E protocol,
		const bool proxySenderAddr,
		const int targetId)
		:
mName(name),
mProtocol(protocol),
mTargetId(targetId),
mProxySenderAddr(proxySenderAddr)
{
	sgGroupIdLock->lock();
	mGroupId = sgGroupId++;
	sgGroupIdLock->unlock();
}


OmnCommGroup::~OmnCommGroup()
{
}


OmnCommGroupPtr
OmnCommGroup::createGroup(const OmnXmlItemPtr &def)
{
	OmnCommMgrPtr mgr;
	return createGroup(mgr, def);
}


OmnCommGroupPtr
OmnCommGroup::createGroup(const OmnCommMgrPtr &commMgr, 
						  const OmnXmlItemPtr &def)
{
	//
	// It assumes 'def' defines a comm group. It contains an item: 
	// <GroupType> that tells the type of the group.
	// This is a static member function to create comm groups based
	// on the type.
	//
	try
	{
		OmnString groupType = def->getStr(OmnXmlItemName::eGroupType, "");
		switch (strToEnum(groupType))
		{
		case eRoundRobin:
			 return	OmnNew OmnGroupRRobin(commMgr, def);

		case eReliable:
			 return OmnNew OmnGroupReliable(commMgr, def);
		
		default:
			 OmnWarn << "Unrecognized Comm Group Type: "
				<< def->toString()
				<< enderr;
			 return 0;
		}
	}

	catch (const OmnExcept &e)
	{
		//
		// The definition has syntax error.
		//
		OmnWarn << "Unrecognized Comm Group Type: " 
                << def->toString()
				<< ". Exeception err: " << e.getErrmsg()
				<< enderr;
		return 0;
	}
}


OmnCommGroup::GroupType
OmnCommGroup::strToEnum(OmnString &name)
{
	name.toLower();
	if (name == OmnString("roundrobin"))
	{
		return eRoundRobin;
	}

	if (name == OmnString("reliable"))
	{
		return eReliable;
	}

	return eInvalidGroupType;
}


OmnString
OmnCommGroup::toString() const
{
	OmnString str;
    str	<< "CommGroup: " << OmnStrUtil::itoa(mGroupId)
				  << "\n	Name: " << mName 
				  << "\n	Protocol: " << OmnStrUtil::itoa(mProtocol)
				  << "\n	TargetId: " << OmnStrUtil::itoa(mTargetId);
	return str;
}
	

int
OmnCommGroup::getNewGroupId()
{
	//
	// The 'tries' is a safe guard. This should never happen.
	//
	int tries = 0;
	while (tries++ < 1000)
	{
		sgGroupIdLock->lock();
    	int groupId = sgGroupId++;
    	sgGroupIdLock->unlock();

		OmnCommGroupPtr group = OmnNetworkSelf->getCommGroup(groupId);
		if (group.isNull())
		{
			//
			// The group id is not used.
			//
			return groupId;
		}
	}

	//
	// If it comes to this point, it means it tried 1000 and still 
	// no idle group ID has been found. This is definitely a problem.
	//
	OmnWarn << "Trying to get a OmnNew comm group id. Tried over"
		<< " 1000 times and still not getting one: "
		<< sgGroupId << enderr;

	return -1;
}	


void
OmnCommGroup::setPeer(const OmnCommGroupPtr &peer)
{
	mPeer = peer;
}


