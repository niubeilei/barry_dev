////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2015/10/15 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SyncEngineClientObj_h
#define Aos_SEInterfaces_SyncEngineClientObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/SyncherType.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosSyncEngineClientObj : public AosJimo
{
	OmnDefineRCObject;

private:
	static AosSyncEngineClientObjPtr smClient;

public:
	//virtual bool docCreated(AosRundata *rdata, const AosXmlTagPtr &doc) = 0;
	//virtual bool docCreated(AosRundata *rdata, const OmnString &tablename, const AosDataRecordObj* record) = 0;

	static void setSyncEngineClient(const AosSyncEngineClientObjPtr &d);
	static AosSyncEngineClientObjPtr getSyncEngineClient() { return smClient; }
	virtual AosSyncherObjPtr getSyncher(AosRundata *rdata,
					const OmnString &sync_name,
					const AosSyncherType::E type) = 0;
};
#endif

