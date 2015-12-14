////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
Moved to StorageEngineObj
#ifndef Aos_SEInterfaces_StorageEngine_h
#define Aos_SEInterfaces_StorageEngine_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "StorageEngine/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/DocTypes.h"

class AosStorageEngine : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	enum
	{
		eDftMaxFixedSizeFileSize = 400000000			// 400M
	};

	AosDocType::E		mType;
	static u64			smMaxFixedSizeFileSize;

public:
	AosStorageEngine(const AosDocType::E type, const bool flag);

	virtual bool saveToFile(
							const u64 &docid, 
							const AosXmlTagPtr &doc,
							const bool savenew, 
							const AosDocFileMgrObjPtr &docfilemgr, 
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr readFromFile(const u64 &docid, 
							const AosDocFileMgrObjPtr &docfilemgr, 
							const AosRundataPtr &rdata) = 0;

	virtual bool deleteDoc(const u64 &docid, 
							const AosDocFileMgrObjPtr &docfilemgr, 
							const AosRundataPtr &rdata) = 0;

	static AosStorageEnginePtr getStorageEngine(const AosDocType::E type);
	static bool init();

private:
	bool registerStorageEngine(const AosStorageEnginePtr &storage_eng);
};
#endif

#endif

