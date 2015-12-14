////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: EpBlackList.h
// Description:
// 	To prevent programs constantly looking into database for undefined
// 	endpoints, programs should first look up an endpoint in this 
// 	list. If it is found, we know the endpoint is not defined in 
// 	the system. There is no need to look up the database anymore.
//
// 	To prevent the memory growing too big, this list gets cleaned
// 	based on the activities. If an endpoint was not looked up 
// 	in the last x number of seconds, it is deleted from this list.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataServer_EpBlackList_h
#define Omn_DataServer_EpBlackList_h


#include "Database/Ptrs.h"
#include "Debug/Rslt.h"
#include "MgcpEpMgr/Ptrs.h"
#include "Network/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "SysObj/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/StrHash.h"
#include "Util/IpPortHash.h"
#include "XmlParser/Ptrs.h"

OmnDefineSingletonClass(OmnEpBlackListSingleton,
						OmnEpBlackList,
						OmnEpBlackListSelf,
						OmnSingletonObjId::eEpBlackList,
	   					"OmnEpBlackList");

class OmnSoMgcpEndpoint;

class OmnEpBlackList	:	public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnStrHash<OmnIntegerPtr, 4096>	mPriHash;
	OmnStrHash<OmnIntegerPtr, 4096>	mPubHash;

	OmnMutexPtr						mTableLock;

public:
	OmnEpBlackList();
	~OmnEpBlackList();

	bool		start() {return true;}
	bool		stop()  {return true;}
	bool		config(const OmnXmlParserPtr &parser) {return true;}

	bool		epAdded(const OmnSoMgcpEndpoint &so);
	bool		epDeleted(const OmnSoMgcpEndpoint &so);
	bool		epModified(const OmnSoMgcpEndpoint &so);

	void		removeEp(const OmnSoMgcpEndpoint &so);

	void		addPri(const OmnString &priEid); 
	void		addPub(const OmnString &pubEid);
	bool		isPriOnList(const OmnString &priEid); 
	bool		isPubOnList(const OmnString &pubEid);

    virtual OmnString       getSysObjName() const {return "OmnEpBlackTable";}
	virtual OmnSingletonObjId::E  getSysObjId() const
			{return OmnSingletonObjId::eEpBlackList;}
	virtual OmnRslt         config(const OmnSysObjPtr &def) {return false;}

private:
};
#endif
