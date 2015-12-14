////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DeviceDefTable.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataServer_DeviceDefTable_h
#define Omn_DataServer_DeviceDefTable_h

/*
#include "Database/Ptrs.h"
#include "Debug/Rslt.h"
#include "Network/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "Util/StrHash.h"
#include "XmlParser/Ptrs.h"

OmnDefineSingletonClass(OmnDeviceDefTableSingleton,
		OmnSingletonObjId::eDeviceDefTable,
	   "OmnDeviceDefTable");

class OmnDeviceDefTable	:	public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnValList<OmnSoDeviceDefPtr>	mDefs;
	OmnMutexPtr						mTableLock;

public:
	OmnDeviceDefTable();
	virtual ~OmnDeviceDefTable();

	bool		start() {return true;}
	bool		stop()  {return true;}
	bool		config(const OmnXmlParserPtr &parser) {return true;}

	OmnSoDeviceDefPtr		getDevice(const OmnString &logicalId);
	bool		remove(const OmnString &logicalId);

	bool		deviceAdded(const OmnSoDeviceDefPtr &so);
	bool		deviceDeleted(const OmnSoDeviceDefPtr &so);
	bool		deviceModified(const OmnSoDeviceDefPtr &so);

    virtual OmnString       getSysObjName() const {return "OmnDeviceDefTable";}
	virtual OmnSingletonObjId::E  getSysObjId() const
			{return OmnSingletonObjId::eDeviceDefTable;}
	virtual OmnRslt         config(const OmnSysObjPtr &def) {return false;}

private:
};
*/
#endif
