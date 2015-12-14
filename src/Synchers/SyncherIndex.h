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
#ifndef AOS_Synchers_SyncherIndex_h
#define AOS_Synchers_SyncherIndex_h

#include "SEInterfaces/SyncherType.h"
#include "SEInterfaces/DataFieldType.h"
#include "Synchers/Syncher.h"
#include "SEUtil/DeltaBeanOpr.h"


class AosSyncherIndex : public AosSyncher
{
private:
	OmnMutexPtr					mLock;
	OmnString 					mIILName;	// _zt4g_idxmgr_...
	AosDataFieldType::E 		mIndexKeyDataType;
	vector<AosExprObjPtr>		mInputKeys;
	AosExprObjPtr				mInputDocid;
	AosDataRecordObjPtr 		mOutputRecord;
	AosExprObjPtr       		mCondition;
	AosBuffPtr 					mDeltaBeansBuff;

public:
	AosSyncherIndex(const int version);
	virtual ~AosSyncherIndex();

	virtual AosSyncherType::E getType() { return AosSyncherType::eIndex; }
	virtual bool proc();
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual AosJimoPtr cloneJimo() const;
	virtual int getDestCubeID() { return 0; }

	virtual bool config(AosRundata *rdata, const AosXmlTagPtr &def);
	virtual bool procData(AosRundata *rdata, AosDataRecordObj *record);
	virtual bool flushDeltaBeans(AosRundata *rdata);
public:
	bool	proc(AosRundata *rdata, const AosDatasetObjPtr &dataset);
	bool 	appendDeltaBean(AosRundata *rdata, AosDataRecordObj *record);
	bool 	createOutput(const OmnString &dpname, AosRundata *rdata);
	AosXmlTagPtr generatorIndexDatasetConf(AosRundata *rdata);

};

#endif
