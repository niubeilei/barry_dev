////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetEntityMgr.cpp
// Description:
//   
//
// Modification History:
// 		Added getCAs(...) 
////////////////////////////////////////////////////////////////////////////

#include "Network/NetEntityMgr.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Network/IpBubble.h"
#include "Network/NetEntity.h"
#include "Network/NetIf.h"
#include "Thread/Mutex.h"
#include "UtilComm/CommGroup.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"



OmnNetEntityMgr::OmnNetEntityMgr()
:
mEntryListLock(OmnNew OmnMutex())
{
}



OmnNetEntityMgr::~OmnNetEntityMgr()
{
}


OmnRslt
OmnNetEntityMgr::addEntity(const OmnNetEntityPtr &netEntity)
{
	int entityId = netEntity->getEntityId();
	if (entityId < 0 || entityId >= OmnNetEnum::eMaxNumEntity)
	{
		return OmnAlarm << entityId << enderr;
	}

	mEntryListLock->lock();
	mEntities[entityId] = netEntity;

	switch (netEntity->getEntityType())
	{
    case OmnNetEtyType::eIpBubble:
         mIpBubbles[entityId] = (OmnIpBubble*)netEntity.getPtr();
         break;

	default:
		 //OmnWarn << OmnErrId::eWarnProgError
		//	<< "Unsupported: " << netEntity->getEntityType()
		//	<< enderr;
		 break;
	}

	mEntryListLock->unlock();
	return true;
}


OmnRslt
OmnNetEntityMgr::config(const OmnXmlItemPtr &def)
{
    //
    // It assumes the following
    //
	//	<EntityDefs>
    //     	<EntityDef>
    //     	 	<EntityId>
    //     	 	<EntityType>
    //         	<LocationId>
    //         	<LocationDesc>
    //         	<EntityDesc>
    //     	</EntityDef>
	//		...						; One per net entity
    //	</EntityDefs>
	//

    def->reset();

	//
	// Process EntityDefs
	//
    while (def->hasMore())
    {
		//
		// Retrieve the next entity definition
		//
        OmnXmlItemPtr entityDef = def->next();

		//
		// Create the entity
		//
        OmnNetEntityPtr netEntity = OmnNetEntity::createEntity(entityDef);
        if (netEntity.isNull())
        {
            //
            // Did not create. Return
            //
            return OmnAlarm << OmnErrId::eAlarmNetworkError 
				<< "Failed To Create Net Entity: "
                << entityDef->toString() << enderr;
        }

		//
		// Creation successful. Insert the entity
		//
		addEntity(netEntity);
	}

    return true;
}


bool
OmnNetEntityMgr::isEntityDefined(const int entityId) const
{
	if (entityId <= 0 || entityId >= OmnNetEnum::eMaxNumEntity)
	{
		return false;
	}

	return !mEntities[entityId].isNull();
}


OmnNetEtyType::E
OmnNetEntityMgr::getEntityType(const int entityId) const
{
	if (entityId <= 0 || entityId >= OmnNetEnum::eMaxNumEntity)
    {
        return OmnNetEtyType::eInvalidNetEntity;
    }

    if (mEntities[entityId].isNull())
	{
        return OmnNetEtyType::eInvalidNetEntity;
    }

	return mEntities[entityId]->getEntityType();
}



OmnIpBubblePtr
OmnNetEntityMgr::getIpBubble(const int bubbleId)
{
	if (bubbleId < 0 || bubbleId >= OmnNetEnum::eMaxNumEntity)
	{
		OmnAlarm << OmnErrId::eAlarmNetworkError 
			<< "InvalidEntityId: " << bubbleId << enderr;
		return 0;
	}

    if (bubbleId== 0)
    {
        //
        // Entity 0 is reserved as an Invalid Entity ID. It always return
        // null pointer, but it is not considered an error. Therefore,
        // no error reporting is done.
        //
        return 0;
    }

	return mIpBubbles[bubbleId];
}


OmnNetEntityPtr
OmnNetEntityMgr::getNetEntity(const int entityId)
{
	if (entityId < 0 || entityId >= OmnNetEnum::eMaxNumEntity)
    {
        OmnAlarm << "OmnErrId::eInvalidEntityId: " << entityId << enderr;
        return 0;
    }

	if (entityId == 0)
	{
		//
		// Entity 0 is reserved as an Invalid Entity ID. It always return 
		// null pointer, but it is not considered an error. Therefore, 
		// no error reporting is done.
		//
		return 0;
	}
	
	return mEntities[entityId];
}


