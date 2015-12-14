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
// 05/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcRange_h
#define Aos_DataProc_DataProcRange_h

#include "DataProc/DataProc.h"


class AosDataProcRange : virtual public AosDataProc
{
	struct composeField
	{
		OmnString			mKeyFieldName;
		int					mKeyFieldIdx;
		bool				mAddToKey;
		bool                mIgnoreEmpty;
		OmnString			mSep;
		AosConditionObjPtr  mFilter;

		bool				mIsRange;

		composeField(){};
		composeField(const AosDataProcRange::composeField &rhs)
		{
			mKeyFieldName = rhs.mKeyFieldName;
			mKeyFieldIdx = rhs.mKeyFieldIdx;
			mAddToKey = rhs.mAddToKey;
			mIgnoreEmpty = rhs.mIgnoreEmpty;
			mIsRange = rhs.mIsRange;
			mSep = rhs.mSep;
			if(rhs.mFilter) mFilter = rhs.mFilter->clone();
		};
	};

	vector<composeField>	mKeyFields;
	OmnString				mValueFieldName;
	int						mValueFieldIdx;
	AosConditionObjPtr  	mValueFilter;

	bool					mHasRange;
	OmnString				mStartFieldName;
	OmnString				mEndFieldName;
	int						mStartFieldIdx;
	int						mEndFieldIdx;
	
	OmnString				mOutputFieldName;		// Ketty 2014/01/09
	int						mOutputFieldIdx;

public:
	AosDataProcRange(const bool flag);
	AosDataProcRange(const AosDataProcRange &proc);
	~AosDataProcRange();

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
	virtual AosDataProcStatus::E procData(
						AosRundata *rdata_raw,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);
	virtual bool 	resolveDataProc(
						const AosRundataPtr &rdata,
						const AosDataRecordObjPtr &input_record,
						const AosDataRecordObjPtr &output_record);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	bool			configNew(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);
};

#endif
