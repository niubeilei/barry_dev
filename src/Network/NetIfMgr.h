////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetIfMgr.h
// Description:
//	The class that manages all network interfaces for a given SPNR.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Network_NetIfMgr_h
#define Omn_Network_NetIfMgr_h

#include "Debug/Rslt.h"
#include "Network/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/ValList.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"
#include "XmlParser/Ptrs.h"
#include <vector>

class OmnNetIfMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	std::vector<OmnNetIfPtr>			mNetIfs;
	OmnMutexPtr					mLock;


public:
	OmnNetIfMgr();
	~OmnNetIfMgr();

	OmnRslt				config(const OmnXmlItemPtr &def);

	OmnNetIfPtr			getNetInterface(const int niid) const;
	OmnIpAddr			getIpAddr(const int niid) const;
	OmnRslt				removeNetInterface(const int niid);
	OmnRslt				addNetInterface(const OmnNetIfPtr &net);

	bool				isNiidDefined(const int niid) const;


private:
};
#endif
