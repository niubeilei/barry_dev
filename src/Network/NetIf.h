////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetIf.h
// Description:
//	Each SPNR has multiple network interface. This class is used to define
//  these network interfaces.
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Network_NetIf_h
#define Omn_Network_NetIf_h

#include "Message/MsgId.h"
#include "Network/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommProt.h"
#include "XmlParser/Ptrs.h"

typedef OmnVList<OmnCommGroupPtr>	OmnCommGroupList;

class OmnNetIf	: public OmnRCObject
{
	OmnDefineRCObject;

private:
	int					mNiid;
	// int					mEntityId;
	OmnIpAddr			mIpAddr;
	OmnString			mDesc;
	int					mBandwidth;
	OmnString			mInterfaceName;

	OmnMutexPtr			mCommLock;
	OmnCommGroupList	mComms;

public:
	OmnNetIf(const OmnXmlItemPtr &def); 
	~OmnNetIf();

	//
	// gets
	//
	int				getNiid() const { return mNiid; }
	OmnIpAddr		getIpAddr() const { return mIpAddr; }
	int				getBandwidth() const { return mBandwidth; }
	OmnString		getInterfaceName() const {return mInterfaceName;}
	OmnString		getDesc() const {return mDesc;}
	// int				getEntityId() const {return mEntityId;}

	OmnRslt			config(const OmnXmlItemPtr &def);


	//
	// Comm related functions
	//
	void			addCommGroup(const OmnCommGroupPtr &comm);	// Chen Ding, 6/13/2002
	OmnCommGroupPtr	getCommGroup(const OmnCommProt::E prot);	// Chen Ding, 6/13/2002
};
#endif
