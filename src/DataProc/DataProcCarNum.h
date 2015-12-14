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
#ifndef Aos_DataProc_DataProcCarNum_h
#define Aos_DataProc_DataProcCarNum_h

#include "DataProc/DataProc.h"
#include "SEUtil/SeTypes.h"

class AosDataProcCarNum : virtual public AosDataProc
{
	RecordFieldInfo				mKeyInput;
	RecordFieldInfo				mTimeInput;
	RecordFieldInfo				mDocidInput;
	RecordFieldInfo				mKeyOutput;
	RecordFieldInfo				mDocidOutput;
	OmnString					mSep;

public:
	AosDataProcCarNum(const bool flag);
	AosDataProcCarNum(const AosDataProcCarNum &proc);
	~AosDataProcCarNum();

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
	
	virtual bool finish(
			const vector<AosDataProcObjPtr> &procs,
			const AosRundataPtr &rdata);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	void			splitWords(
						const OmnString &key,
						set<OmnString> &new_words);
};

#endif
