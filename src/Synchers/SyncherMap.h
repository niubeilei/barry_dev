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
// 2015/09/20 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Synchers_SyncherMap_h
#define AOS_Synchers_SyncherMap_h

#include "SEInterfaces/SyncherType.h"
#include "SEInterfaces/DataFieldType.h"
#include "Synchers/Syncher.h"
#include "SEUtil/DeltaBeanOpr.h"
#include "SEUtil/FieldOpr.h"


class AosSyncherMap : public AosSyncher
{
private:
	OmnMutexPtr					mLock;
	OmnString 					mIILName;	// _zt4g_idxmgr_...
	AosDataFieldType::E			mIILMapKeyType;
	vector<AosExprObjPtr> 		mInputKeys;
	AosExprObjPtr				mInputValue;
	AosDataRecordObjPtr 		mOutputRecord;
	AosExprObjPtr       		mCondition;

	AosFieldOpr::E 				mIILMapValueAgrType;
	AosDataType::E				mMapValueType;
	AosBuffPtr 					mDeltaBeansBuff;
	OmnString 					mMapKeyIILName;


public:
	AosSyncherMap(const int version);
	virtual ~AosSyncherMap();

	virtual AosSyncherType::E getType() { return AosSyncherType::eMap; }
	virtual bool proc();
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual AosJimoPtr cloneJimo() const;
	virtual int getDestCubeID() { return 0; }

public:
	bool 	config(AosRundata *rdata, const AosXmlTagPtr &def);
	bool	proc(AosRundata *rdata, const AosDatasetObjPtr &dataset);
	bool	procData(AosRundata *rdata, AosDataRecordObj *record);
	bool 	flushDeltaBeans(AosRundata *rdata);
	bool 	appendDeltaBean(AosRundata *rdata, AosDataRecordObj *record);
	bool 	createOutput(const OmnString &dpname, AosRundata *rdata);
	AosXmlTagPtr generatorIndexDatasetConf(AosRundata *rdata);
	bool 	repairValue(AosRundata *rdata, 
					const AosDeltaBeanOpr::E &opr,
					AosValueRslt &value);
	AosSyncherObjPtr getExecutor(AosRundata *rdata);
};

#endif
