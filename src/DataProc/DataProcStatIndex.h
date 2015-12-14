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
// 05/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcStatIndex_h
#define Aos_DataProc_DataProcStatIndex_h

#include "DataProc/DataProc.h"
#include "SEUtil/SeTypes.h"

class AosDataProcStatIndex : virtual public AosDataProc
{
	struct OutputFieldInfo 
	{
		RecordFieldInfo mKeyOutput;
		RecordFieldInfo mDocidOutput;
	};

	OmnString					mSep;

	//Field						mKey;	
	//Field						mDocid;	
	RecordFieldInfo				mKeyInput;
	RecordFieldInfo				mDocidInput;
	vector<OutputFieldInfo>		mOutputs;

public:
	AosDataProcStatIndex(const bool flag);
	AosDataProcStatIndex(const AosDataProcStatIndex &proc);
	~AosDataProcStatIndex();

	virtual AosDataProcStatus::E procData( 
						const AosDataRecordObjPtr &record,
						const u64 &docid,
						const AosDataRecordObjPtr &output,
						const AosRundataPtr &rdata);
	
	virtual AosDataProcObjPtr cloneProc();

	virtual AosDataProcObjPtr create(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	virtual bool	resolveDataProc(
						map<OmnString, AosDataAssemblerObjPtr> &asms,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);

	virtual bool getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);	

	virtual AosDataProcStatus::E procData(
						AosRundata *rdata_raw,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);
private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
};

#endif
