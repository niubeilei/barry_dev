////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IpBubble.h
// Description:
//	An IP Bubble is an isolated IP network. SPNRs are used to connect
//  these isolated IP networks. This class defines these IP bubbles.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Network_IpBubble_h
#define Omn_Network_IpBubble_h


#include "Network/NetEntity.h"

#include "DataServer/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/StrHashTable.h"
#include "Util/String.h"
#include "Util/RCObjImp.h"




class OmnIpBubble	:	public OmnNetEntity
{
	OmnDefineRCObject;

private:


public:
	OmnIpBubble(const OmnXmlItemPtr &def);
	~OmnIpBubble();

	//
	// member functions
	//

    virtual OmnNetEtyType::E	getEntityType() const {return OmnNetEtyType::eIpBubble;} 

	//bool			addToTable(const OmnString &termEID,
	//						   const OmnMgcpEndpointPtr &endpoint);
	//OmnString		removeFromTable(const OmnString &termEID);
	//OmnString		lookupTable(const OmnString &termEID);

private:
	OmnRslt		config(const OmnXmlItemPtr &def);
};
#endif
