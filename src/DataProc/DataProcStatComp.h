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
// 2013/03/18 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcStatComp_h
#define Aos_DataProc_DataProcStatComp_h

#include "CounterUtil/StatTypes.h"
#include "DataProc/DataProc.h"
#include "UtilTime/TimeGran.h"


class AosDataProcStatComp : virtual public AosDataProc
{
	struct composeField
	{
		OmnString			mKeyFieldName;
		int					mKeyFieldIdx;
		AosConditionObjPtr  mFilter;

		composeField(){};
		composeField(const AosDataProcStatComp::composeField &rhs)
		{
			mKeyFieldName = rhs.mKeyFieldName;
			mKeyFieldIdx = rhs.mKeyFieldIdx;
			if(rhs.mFilter) mFilter = rhs.mFilter->clone();
		};
	};

	vector<composeField>	mKeyFields;
	AosStatType::E			mStatType;
	AosTimeGran::E			mTimeGran;
	int						mTimeFieldIdx;
	OmnString				mSep;
	OmnString				mValueFieldName;
	int						mValueFieldIdx;
	int						mReserveNum;
	
	OmnString				mOutputFieldName;		// Ketty 2014/01/09
	int						mOutputFieldIdx;

public:
	AosDataProcStatComp(const bool flag);
	AosDataProcStatComp(const AosDataProcStatComp &proc);
	~AosDataProcStatComp();

	virtual AosDataProcStatus::E procData(
				const AosDataRecordObjPtr &record,			
				const u64 &docid,
				const AosDataRecordObjPtr &output,			
				const AosRundataPtr &rdata);

	virtual AosDataProcObjPtr cloneProc();
	
	virtual AosDataProcObjPtr create(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);
	
	virtual bool resolveDataProc(
	 					map<OmnString, AosDataAssemblerObjPtr> &asms,
 						const AosDataRecordObjPtr &record,
 						const AosRundataPtr &rdata);

	// Ketty 2013/12/19
//	virtual AosDataProcStatus::E procData(
//						AosRundata *rdata_raw,
//						AosDataRecordObj **input_records,
//						AosDataRecordObj **output_records);
	virtual bool 	resolveDataProc(
						const AosRundataPtr &rdata,
						const AosDataRecordObjPtr &input_record,
						const AosDataRecordObjPtr &output_record);

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
};

#endif
