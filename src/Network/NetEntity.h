////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetEntity.h
// Description:
//	This class is used to describe an adjacent Network. This Spnr box
//	is usually connected to a few adjacent Spnr boxes, or a few IP 
//  bubbles, or mix of both (Spnr boxes and Ip bubbles). Each connection
//  is through a group of network interface cards (there may be only one
//  NIC in a NI group.
//
//  We call those adjacent entities "NetEntity". Thus, there are two
//  types of NetEntities: SpnrBox and IpBubble. Also, there are four
//  types of adjacencies: Parent indicates that the adj NetEntity is
//  the parent SpnrBox of the box on which this Spnr software is running.
//  Peer indicates a peer SpnrBox, Mate means a mate SpnrBox for 
//  redundant installation (in active or standby mode). Child means a
//  child SpnrBox or a connected IP bubble.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Network_NetEntity_h
#define Omn_Network_NetEntity_h

#include "Debug/Rslt.h"
#include "Network/Ptrs.h"
#include "Network/NetEtyType.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"
#include <vector>


typedef std::vector<OmnNetIfPtr>	OmnNetInterfList;

class OmnNetEntity	:	public OmnRCObject
{
protected:
	OmnString			mName;
	int					mEntityId;				// Id of the network entity
	int					mLocationId;
	OmnString			mLocationDesc;
	OmnString			mDesc;
	OmnNetInterfList	mNetInterfaces;
	OmnString			mDomainName;
	OmnNetEtyType::E 	mEntityType;

public:
	OmnNetEntity(const OmnNetEtyType::E type);
	virtual ~OmnNetEntity();

	static OmnNetEntityPtr	createEntity(const OmnXmlItemPtr &def);


	//
	// The OmnRCObject Interface implementation
	//
	OmnString		getName() const {return mName;}
	int				getEntityId() const { return mEntityId; }
	int				getLocationId() const {return mLocationId;}
	OmnString		getLocationDesc() const {return mLocationDesc;}
	OmnString		getDesc() const {return mDesc;}
    OmnString   	getDomainName() const {return mDomainName;}

	void			setEntityId(const int i) {mEntityId = i;}
							
	OmnNetEtyType::E 	getEntityType() const {return mEntityType;}

protected:
	OmnRslt			config(const OmnXmlItemPtr &def);
};
#endif
