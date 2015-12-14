////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommGroupMgr.cpp
// Description:
//	This class is used to manage a group of OmnCommGroups. Internally,
//  it stores all comm groups in a list. When one creates a group, 
//  it should add the group to this class. From this class, one can 
//  retrieve the desired groups.    
//
// Modification History:
// 		Added getGroupByTargetId(...).
//
//		When creating a group, we need to check group ID to make
//		sure it is not bigger than the maximum.  
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/CommGroupMgr.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/ErrId.h"
#include "Network/Network.h"
#include "Network/NetEnum.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/CommGroup.h"
#include "UtilComm/CommMgr.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/XmlItem.h"
#include "XmlUtil/XmlItemName.h"
#include "XmlParser/XmlParser.h"




OmnCommGroupMgr::OmnCommGroupMgr(const OmnCommMgrPtr &commMgr)
:
mGroupLock(OmnNew OmnMutex()),
mCommMgr(commMgr)
{
	//
	// Initialize mCommGroups
}


OmnCommGroupMgr::~OmnCommGroupMgr()
{
}


OmnRslt
OmnCommGroupMgr::config(const OmnXmlItemPtr &def)
{
	//
	// It is used to create comm groups based on the parameter 'def'.
	// It assumes the following:
	//
	// 	<CommGroupMgr>
	//		<CommGroups>
	//			<CommGroup>
	//				...				; Refer to OmnCommGroup::config(...)
	//			</CommGroup>
	//			...					; One for each comm group.
	//		</CommGroups>
	//		</Routes>
	//	</CommGroupMgr>
	//

	mGroupLock->lock();

	//
	// Retrieve comm group definitions
	//
	OmnXmlItemPtr commGroupDef = def->getItem(OmnXmlItemName::eCommGroups);

	while (commGroupDef->hasMore())
	{
		OmnXmlItemPtr item = commGroupDef->next();
		OmnCommGroupPtr group = OmnCommGroup::createGroup(mCommMgr, item);
		if (group.isNull())
		{
			//
			// Failed to create the comm group.
			//
			mGroupLock->unlock();
			return OmnAlarm << "Failed To Create Comm Group: "
				<< commGroupDef->toString() << enderr;
		}

		int groupId = group->getGroupId();
			
		// 
		// Chen Ding, 05/22/2003, Change-0002
		// Making sure the group ID is not bigger than the maximum.
		//
		if (groupId < 0 || groupId >= OmnNetEnum::eMaxNumGroup)
		{
			OmnAlarm << OmnErrId::eAlarmConfigError
				<< "Invalid group ID: " << groupId
				<< ". " << commGroupDef->toString() << enderr;
			return false;
		}

		mCommGroups[groupId] = group;
	}

	mGroupLock->unlock();
	return true;
}


OmnCommGroupPtr		
OmnCommGroupMgr::getGroup(const int groupId)
{
	//
	// It retrieves the specified comm groups. If not found, 
	// 0 is returned.
	//
	OmnRslt rslt = checkGroupId(groupId);
	if (!rslt)
	{
		return 0;
	}

	mGroupLock->lock();
	OmnCommGroupPtr group = mCommGroups[groupId];
	mGroupLock->unlock();
	return group;
}


OmnCommGroupPtr
OmnCommGroupMgr::getGroup(const OmnString &def)
{
    //
    // The string config contains the configuration information
    // to create a comm group.
    //
    try
    {
        OmnXmlItemPtr item = OmnNew OmnXmlItem(def);
		return getGroup(item);
    }
    
    catch (const OmnExcept &e)
    {   
        OmnWarn << "Failed To Create Comm Group: "
           << "Group configuration: \n"
           << def 
           << ". Error message: " << e.toString()
           << enderr;
		return 0;
    }
}

OmnCommGroupPtr
OmnCommGroupMgr::getGroup(const OmnXmlItemPtr &def)
{
    //
    // It retrieves the specified comm groups. If not found,
    // 0 is returned.
    //
	int groupId = def->getInt(OmnXmlItemName::eCommGroupId, -1);
	OmnCommGroupPtr group = getGroup(groupId);
	if (!group.isNull())
	{
		return group;
	}

	//
	// Need to create the group
	//
    group = OmnCommGroup::createGroup(mCommMgr, def);
    if (group.isNull())
    {
        //
        // Failed to create the comm group.
        //
        OmnAlarm << "Failed To Create Comm Group: "
                << def->toString() << enderr;
		return 0;
	}

	groupId = group->getGroupId();
	mGroupLock->lock();
    mCommGroups[groupId] = group;
	mGroupLock->unlock();

	return group;
}


OmnRslt
OmnCommGroupMgr::removeGroup(const int groupId)
{
	OmnRslt rslt = checkGroupId(groupId);
	if (!rslt)
	{
		return rslt;
	}

	mGroupLock->lock();
	// 
	// Chen Ding, 05/21/2003
	//
	// if (!mCommGroups[groupId])
	if (mCommGroups[groupId].isNull())
	{
		mGroupLock->unlock();
		return OmnAlarm << "CommGroup: " << groupId 
			<< " not defined!" << enderr;
	}

	mCommGroups[groupId] = 0;

	//
	// Remove the group from all oter arrays
	//
	//removeCommGroup(mCommsMgcp, groupId);
	//removeCommGroup(mCommsNms, groupId);

	mGroupLock->unlock();
	return true;	
}


/*
OmnRslt
OmnCommGroupMgr::removeCommGroup(REntry theArray[], const int groupId)
{
	//
	// It removes the commm group identified by 'groupId' from the array
	// 'theArray'.
	//
	// The caller should have locked the array.
	//
	OmnCommGroupPtr group;
	for (int i=0; i<OmnNetEnum::eMaxNumEntity; i++)
	{
		group = theArray[i].mCommGroup;
		if (!group)
		{
			continue;
		}

		if (group->getGroupId() == groupId)
		{
			theArray[i].mCommGroup = 0;
		}
	}

	return true;
}
*/


bool				
OmnCommGroupMgr::addGroup(const OmnCommGroupPtr &group)
{
	//
	// If the group is already in the list, this is an error. Report
	// the error and do nothing.
	//
	// Otherwise, add the group.
	//

	int groupId = group->getGroupId();
	int targetId = group->getTargetId();

	//
	// Sanity Checking
	//
	if (groupId < 0 || groupId >= OmnNetEnum::eMaxNumGroup)
	{
		OmnAlarm << "Invalid Group Id: " << groupId << enderr;
		return false;
	}

	if (targetId < 0 || targetId >= OmnNetEnum::eMaxNumEntity)
	{
		OmnAlarm << "Invalid Entity Id: " << targetId << enderr;
		return false;
	}

	mGroupLock->lock();
	mCommGroups[groupId] = group;
	mGroupLock->unlock();
	return true;
}


OmnRslt
OmnCommGroupMgr::checkGroupId(const int groupId)
{
	if (groupId < 0 || groupId >= OmnNetEnum::eMaxNumGroup)
	{
		return false;
	}

	return true;
}


OmnRslt
OmnCommGroupMgr::stopReadingComm(const int groupId,
								 const OmnCommListenerPtr &requester)
{
 	//
	// Sanity checking
	//
	if (groupId < 0 || groupId >= OmnNetEnum::eMaxNumGroup)
	{
		return OmnAlarm << "Invalid Group Id: " << groupId << enderr;
	}

	mGroupLock->lock();
	OmnCommGroupPtr group = mCommGroups[groupId];
	mGroupLock->unlock();

	if (group.isNull())
	{
		return OmnAlarm << "Program Error: "
			<< "To stop reading a comm group but the comm group not found: " 
			<< groupId << enderr;
	}

	return group->stopReading(requester);
}


OmnRslt
OmnCommGroupMgr::startReadingComm(const int groupId, const OmnCommListenerPtr &requester)
{
    //
    // Sanity checking
    //
    if (groupId < 0 || groupId >= OmnNetEnum::eMaxNumGroup)
    {
        return OmnAlarm << "Invalid Group Id: " << groupId << enderr;
    }

    mGroupLock->lock();
	OmnCommGroupPtr group = mCommGroups[groupId];
	mGroupLock->unlock();

	if (!group)
	{
		return OmnAlarm << "Program Error: "
			<< "To start reading a comm group but not defined: " 
			<< groupId << enderr;
	}

	return group->startReading(requester);
}


bool                
OmnCommGroupMgr::isCommGroupDefined(const int groupId) const
{
	if (groupId < 0 || groupId >= OmnNetEnum::eMaxNumGroup)
	{
		return false;
	}

	return !mCommGroups[groupId].isNull();
}


bool                
OmnCommGroupMgr::usingSameProtocol(const int group1, const int group2) const
{
	//
	// Make sure the two are valid group IDs.
	//
	if (group1 < 0 || group1 >= OmnNetEnum::eMaxNumGroup ||
	    group2 < 0 || group2 >= OmnNetEnum::eMaxNumGroup)
	{
		return false;
	}

	if (mCommGroups[group1].isNull() || mCommGroups[group2].isNull())
	{
		//
		// They are not valid group IDs.
		//
		return false;
	}

	return mCommGroups[group1]->getProtocol() 
				== mCommGroups[group2]->getProtocol();
}


OmnCommProt::E         
OmnCommGroupMgr::getCommGroupProtocol(const int groupId) const
{
    //
    // Make sure the two are valid group IDs.
    //
    if (groupId < 0 || groupId >= OmnNetEnum::eMaxNumGroup)
    {
		OmnAlarm << "Invalid Comm Group Id: "
			<< groupId << enderr;
		return OmnCommProt::eInvalid;
    }

    if (mCommGroups[groupId].isNull())
    {
        //
        // Not valid group IDs.
        //
		OmnAlarm << "Invalid Comm Group Id: " << groupId << enderr;
		return OmnCommProt::eInvalid;
    }

    return mCommGroups[groupId]->getProtocol();
}


// 
// Chen Ding, 05/10/2003
//
OmnCommGroupPtr 
OmnCommGroupMgr::getGroupByTargetId(const int targetId)
{
	for (int i=0; i<OmnNetEnum::eMaxNumGroup; i++)
	{
		// 
		// Chen Ding, 05/21/2003
		//
		// if (mCommGroups[i]->getTargetId() == targetId)
		if (!mCommGroups[i].isNull() && 
			mCommGroups[i]->getTargetId() == targetId)
		{
			return mCommGroups[i];
		}
	}

	return 0;
}

