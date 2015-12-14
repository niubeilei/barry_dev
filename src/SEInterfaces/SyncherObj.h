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
// 2015/01/24 Created by Chen Ding
// 2015/09/15 Modified by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SyncherObj_h
#define Aos_SEInterfaces_SyncherObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/SyncherType.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosSyncherType;

class AosSyncherObj : public AosJimo
{
	OmnDefineRCObject;

public:
	AosSyncherObj(const int version);

	virtual void setRundata(const AosRundataPtr &rdata) = 0;
	virtual AosSyncherType::E getType() = 0;

	virtual bool proc() = 0;
	virtual bool isValid() const = 0;
	virtual bool isExpired() const = 0;
	virtual AosBuffPtr serializeToBuff() = 0;
	virtual bool serializeFromBuff(const AosBuffPtr &buff) = 0;
	virtual AosSyncherObjPtr clone() const = 0;
	virtual AosJimoPtr cloneJimo() const;
	virtual int getDestCubeID() = 0;

	// Add by Young, 2010/10/30
	virtual bool appendRecord(AosRundata *rdata, AosDataRecordObj *record) = 0;
	virtual bool config(AosRundata *rdata, const AosXmlTagPtr &def) = 0;
	virtual bool procData(AosRundata *rdata, AosDataRecordObj *record) = 0;
	virtual bool flushDeltaBeans(AosRundata *rdata) = 0;
								
};
#endif

