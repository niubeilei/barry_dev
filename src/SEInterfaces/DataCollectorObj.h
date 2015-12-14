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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataCollectorObj_h
#define Aos_SEInterfaces_DataCollectorObj_h

#include "DataCollector/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataCollectorCreatorObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosValueRslt;

class AosDataCollectorObj : virtual public OmnRCObject
{
private:
	static AosDataCollectorCreatorObjPtr		smCreator;

public:
	virtual bool 	serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;
	virtual bool 	serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;
	virtual AosDataCollectorObjPtr clone() = 0;
	virtual bool 	addSendStart(const AosXmlTagPtr &config, const AosRundataPtr &rdata) = 0;
	virtual bool 	addSendFinish(const u32 total, const AosRundataPtr &rdata) = 0;
	virtual bool 	addData(const AosBuffArrayPtr &buff_array, const AosRundataPtr &rdata) = 0;
	virtual bool	finish(const AosRundataPtr &rdata) = 0;
	virtual bool    runAction(const AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;

	virtual AosDataCacherObjPtr  getDataCacher() = 0;
	virtual int getJobServerId() = 0;
	virtual u64 getJobDocid() = 0;

	virtual AosDataCollectorObjPtr createDataCollector(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;

	virtual void setMemory(const u64 maxmem) = 0;
	virtual int getMaxBucket() = 0;

	static AosDataCollectorObjPtr createDataCollectorStatic(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);

	static AosDataCollectorObjPtr serializeFromStatic(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);

	static bool checkConfigStatic(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);

	static AosDataCollectorCreatorObjPtr getDataCollectorCreator() {return smCreator;}
	static void setDataCollectorCreator(const AosDataCollectorCreatorObjPtr &d) {smCreator = d;}
};
#endif
