////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DvBlackList.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataServer_DvBlackList_h
#define Omn_DataServer_DvBlackList_h


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

OmnDefineSingletonClass(OmnDvBlackListSingleton,
						OmnDvBlackList,
						OmnDvBlackListSelf,
						OmnSingletonObjId::eDvBlackList,
	   					"OmnDvBlackList");

class OmnSoDevice;

class OmnDvBlackList	:	public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnStrHash<OmnIntegerPtr, 4096>	mHashTable;

	OmnMutexPtr						mTableLock;

public:
	OmnDvBlackList();
	~OmnDvBlackList();

	bool		start() {return true;}
	bool		stop()  {return true;}
	bool		config(const OmnXmlParserPtr &parser) {return true;}

	bool		dvAdded(const OmnString &deviceId);
	bool		dvDeleted(const OmnString &oldDeviceId);
	bool		dvModified(const OmnString &deviceId);

	void		add(const OmnString &deviceId); 
	bool		isOnList(const OmnString &priEid); 

    virtual OmnString       getSysObjName() const {return "OmnEpBlackTable";}
	virtual OmnSingletonObjId::E  getSysObjId() const
			{return OmnSingletonObjId::eDvBlackList;}
	virtual OmnRslt         config(const OmnSysObjPtr &def) {return false;}

private:
};
#endif
