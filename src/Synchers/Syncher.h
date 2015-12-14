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
// 2015/01/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Synchers_Syncher_h
#define AOS_Synchers_Syncher_h

#include "SEInterfaces/SyncherObj.h"
#include "SEInterfaces/SyncherType.h"
#include "SEInterfaces/DatasetObj.h"
#include "Util/Ptrs.h"
#include "SEUtil/DeltaBeanOpr.h"

class AosSyncher : public AosSyncherObj
{
protected:
	AosRundataPtr 				mRundata;
	AosXmlTagPtr 				mDatasetDef;
	AosBuffPtr 					mDeltasBuff;

public:
	AosSyncher(const int version);
	virtual ~AosSyncher();

	virtual void setRundata(const AosRundataPtr &rdata) {mRundata = rdata;}
	virtual bool isValid() const {return true;}
	virtual bool isExpired() const {return false;}
	virtual bool proc();
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual AosSyncherObjPtr clone() const ;

	// Add by Young
	virtual bool config(AosRundata *rdata, const AosXmlTagPtr &def);
	virtual bool appendRecord(AosRundata *rdata, AosDataRecordObj *record);
	virtual bool procData(AosRundata *rdata, AosDataRecordObj *record);
	virtual bool flushDeltaBeans(AosRundata *rdata);

public:
	AosDatasetObjPtr getSyncherDataset(AosRundata *rdata,
						const AosXmlTagPtr &dataset_def, 
						const AosBuffPtr &buff);

	AosDeltaBeanOpr::E getDeltaBeanOpr(AosRundata *rdata, AosDataRecordObj *record);
	AosIILType getIILType(const AosDataType::E type);
};

#endif
