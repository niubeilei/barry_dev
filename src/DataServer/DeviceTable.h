////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DeviceTable.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataServer_DeviceTable_h
#define Omn_DataServer_DeviceTable_h


#include "Database/Ptrs.h"
#include "Debug/Rslt.h"
#include "Device/Ptrs.h"
#include "Network/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/StrHash.h"
#include "XmlParser/Ptrs.h"

OmnDefineSingletonClass(OmnDeviceTableSingleton,
						OmnDeviceTable,
						OmnDeviceTableSelf,
						OmnSingletonObjId::eDeviceTable,
	   					"OmnDeviceTable");

class OmnDeviceTable	:	public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnStrHash<OmnDevicePtr, 4096>	mHash;
	OmnMutexPtr						mTableLock;

public:
	OmnDeviceTable();
	~OmnDeviceTable();

	bool		start();
	bool		stop()  {return true;}
	bool		config(const OmnXmlParserPtr &parser) {return true;}

	bool		addToTables(const OmnDevicePtr &device);
	bool		removeFromTables(const OmnString &physicalId);

	OmnDevicePtr		getDevice(const OmnString &deviceId);

	bool		deviceAdded(const OmnSoDevicePtr &so);
	bool		deviceDeleted(const OmnSoDevicePtr &so);
	bool		deviceModified(const OmnString &oldDeviceId);

    virtual OmnString       getSysObjName() const {return "OmnDeviceTable";}
	virtual OmnSingletonObjId::E  getSysObjId() const
			{return OmnSingletonObjId::eDeviceTable;}
	virtual OmnRslt         config(const OmnSysObjPtr &def) {return false;}

private:
};
#endif
