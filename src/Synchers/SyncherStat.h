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
#ifndef AOS_Synchers_SyncherStat_h
#define AOS_Synchers_SyncherStat_h

#include "SEInterfaces/AggregationType.h"
#include "SEInterfaces/SyncherType.h"
#include "SEInterfaces/DataFieldType.h"
#include "Synchers/Syncher.h"
#include "SEUtil/DeltaBeanOpr.h"
#include "SEUtil/FieldOpr.h"

class AosSyncherStat : public AosSyncher
{
private:
	struct StatMeasure 
	{
		OmnString  				mFieldName;
		OmnString				mEscapeName;
		AosAggregationType::E	mAggrType;
		AosDataType::E			mDataType;
	};

private:
	OmnMutexPtr						mLock;

	vector<OmnString>				mKeys;
	vector<StatMeasure>				mMeasures;

	// for time field
	OmnString						mTimeField;
	OmnString 						mFormat;
	OmnString						mTimeUnit;

	AosDataRecordObjPtr				mOutputRecord;		

public:

	AosSyncherStat(const AosSyncherStat &rhs);
	AosSyncherStat(const int version);
	virtual ~AosSyncherStat();

	virtual AosSyncherType::E getType() { return AosSyncherType::eStat; }
	virtual bool proc();
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual AosJimoPtr cloneJimo() const;

	virtual int getDestCubeID() { return 0; }

public:
	bool 	config(AosRundata *rdata, const AosXmlTagPtr &def);
	bool	proc(AosRundata *rdata, const AosDatasetObjPtr &dataset);
	bool	procData(AosRundata *rdata, AosDataRecordObj *record);
	bool 	createOutput(AosRundata *rdata, AosDataRecordObj *input_records);
	bool 	flushDeltaBeans(AosRundata *rdata);

	bool 	proc(AosRundata *rdata,
				const AosXmlTagPtr &dsconf, 
				const AosBuffPtr &buff);
					
	bool 	procData(AosRundata *rdata, AosDataRecordObj **input_records);
	bool 	procDataByAdd(AosRundata *rdata, AosDataRecordObj **input_records);
	bool 	procDataByModify(AosRundata *rdata, AosDataRecordObj **input_records);
	bool 	procDataByDelete(AosRundata *rdata, AosDataRecordObj **input_records);
	AosDataFieldType::E dataTypeToDataFieldType(const AosDataType::E type);

private: 
	bool sendTransNeedResp(AosRundata *rdata,
					const AosTransPtr &trans,
					AosBuffPtr &resp);
};

#endif
