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
#ifndef Aos_DataProc_DataProcCompose_h
#define Aos_DataProc_DataProcCompose_h

#include "DataProc/DataProc.h"

class AosDataProcCompose : virtual public AosDataProc
{
	struct composeField
	{
		OmnString			mKeyFieldName;
		int					mKeyFieldIdx;
		bool				mAddToKey;
		bool                mIgnoreEmpty;
		OmnString			mSep;
		AosConditionObjPtr  mFilter;

		RecordFieldInfo		mInput;		// Ketty 2014/01/13

		composeField(){};
		composeField(const AosDataProcCompose::composeField &rhs)
		{
			mKeyFieldName = rhs.mKeyFieldName;
			mKeyFieldIdx = rhs.mKeyFieldIdx;
			mAddToKey = rhs.mAddToKey;
			mIgnoreEmpty = rhs.mIgnoreEmpty;
			mSep = rhs.mSep;
			if(rhs.mFilter) mFilter = rhs.mFilter->clone();
			
			mInput = rhs.mInput;		// Ketty 2014/01/13
		};
	};

	vector<composeField>	mKeyFields;
	OmnString				mValueFieldName;
	int						mValueFieldIdx;
	AosConditionObjPtr  	mValueFilter;
	
	RecordFieldInfo			mOutput;		// Ketty 2014/01/13
	
public:
	AosDataProcCompose(const bool flag);
	AosDataProcCompose(const AosDataProcCompose &proc);
	~AosDataProcCompose();

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
	//virtual bool resolveDataProc(
	//			const AosRundataPtr &rdata,
	//			const AosDataEngineObjPtr &data_engine);
	virtual bool getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);	
	virtual AosDataProcStatus::E procData(
				AosRundata *rdata_raw,
				AosDataRecordObj **input_records,
				AosDataRecordObj **output_records);

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
	bool	configNew(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &def);

	bool	configV1(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
	bool 	configInputs(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

};

#endif
