////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetEntityMgr.h
// Description:
//	This class is used to describe an adjacent Network.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Network_NetEntityMgr_h
#define Omn_Network_NetEntityMgr_h

#include "Debug/Rslt.h"
#include "Debug/Rslt.h"
#include "Message/MsgId.h"
#include "Network/Ptrs.h"
#include "Network/NetEnum.h"
#include "Network/NetEtyType.h"
#include "Thread/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/ValList.h"
#include "XmlParser/Ptrs.h"
#include <vector>

class OmnNetEntityMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnNetEntityPtr				mEntities[OmnNetEnum::eMaxNumEntity];
	OmnIpBubblePtr				mIpBubbles[OmnNetEnum::eMaxNumEntity];

	std::vector<OmnNetEntityPtr>	mAdjEntities;

	int							mMyNetId;
	bool						mAmIRoot;

	OmnMutexPtr					mEntryListLock;

public:
	OmnNetEntityMgr();
	~OmnNetEntityMgr();

	OmnRslt				config(const OmnXmlItemPtr &configItem);

	//
	// gets
	//
	int					getMyNetId() const { return mMyNetId; }
	bool				amIRoot() const { return mAmIRoot; }
	OmnNetEtyType::E	getEntityType(const int id) const;

	// 
	// member functions
	//

	OmnRslt				addEntity(const OmnNetEntityPtr &entity);
	bool				removeEntity(const int netId);
	bool				isEntityDefined(const int entityId) const;


	OmnNetEntityPtr		getNetEntity(const int netEntityId);
	OmnIpBubblePtr		getIpBubble(const int bubbleId);

    // 
    // Protocol based entities
    //
    OmnRslt         getAdjEntities(const OmnMsgId::E protocol, 
						std::vector<OmnNetEntityPtr> &entities);
    OmnRslt         getAdjIpBubbles(const OmnMsgId::E protocol, 
						std::vector<OmnIpBubblePtr> &ipBubble);
    
private:
};
#endif

