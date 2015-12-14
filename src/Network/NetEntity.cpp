////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetEntity.cpp
// Description:
//	This class is used to describe an adjacent Network.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Network/NetEntity.h"

#include "Alarm/Alarm.h"
#include "Debug/Error.h"
#include "Debug/ErrId.h"
#include "Network/IpBubble.h"
#include "Network/NetIf.h"
#include "UtilComm/CommGroup.h"
#include "UtilComm/CommGroupMgr.h"
#include "XmlParser/XmlItemName.h"
#include "XmlParser/XmlItem.h"


OmnNetEntity::OmnNetEntity(const OmnNetEtyType::E type)
:
mEntityId(-1),
mLocationId(-1),
mEntityType(type)
{
}



OmnNetEntity::~OmnNetEntity()
{
}


OmnNetEntityPtr
OmnNetEntity::createEntity(const OmnXmlItemPtr &def)
{
	//
	// This function creates a new net entity based on the definition
	// 'def'. This function assumes the following:
	//
	//      <EntityDef>
	//			<Name>
	//			<DomainName>
    //          <EntityId>
    //          <EntityType>
    //          <LocationId>
    //          <LocationDesc>
    //          <EntityDesc>
    //      </EntityDef>
	//

	// Retrieve EntityType
	OmnString entityTypeStr = def->getStr(OmnXmlItemName::eEntityType, "");
	OmnNetEtyType::E entityType = OmnNetEtyType::nameToEnum(entityTypeStr);

	//
	// Branch on the type
	//
	switch (entityType)
	{
	case OmnNetEtyType::eIpBubble:
		 return OmnNew OmnIpBubble(def);

	default:
		 OmnAlarm << entityTypeStr << enderr;
		 return 0;
	}
}


OmnRslt
OmnNetEntity::config(const OmnXmlItemPtr &def)
{
    //
    // This function assumes the following:
    //
    //      <EntityDef>
	//			<Name>
    //          <EntityId>
    //          <LocationId>
    //          <LocationDesc>
    //          <EntityDesc>
	//			...				; the rest shall be handled by individual type
    //      </EntityDef>
    //

	// Retrieve Name
	mName = def->getStr(OmnXmlItemName::eName, "NoName");

    // Retrieve EntityId
    mEntityId = def->getInt(OmnXmlItemName::eEntityId);

    // Retrieve Location Id (optional)
    mLocationId = def->getInt(OmnXmlItemName::eLocationId, -1);

    // Retrieve Location Description
    mLocationDesc = def->getStr(OmnXmlItemName::eLocationDesc, "Not Specified");

    // Retrieve Description
    mDesc = def->getStr(OmnXmlItemName::eEntityDesc, "Not Specified");

    return true;
}
