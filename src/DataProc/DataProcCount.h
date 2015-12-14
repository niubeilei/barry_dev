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
// 04/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcCount_h
#define Aos_DataProc_DataProcCount_h

#include "DataProc/DataProc.h"


class AosDataProcCount : virtual public AosDataProc
{
	OmnString			mKeyFieldName;
	int					mKeyFieldIdx;
	OmnString			mValueFieldName;
	int					mValueFieldIdx;
	AosConditionObjPtr	mFilter;
	
	OmnString				mOutputFieldName;		// Ketty 2014/01/09
	int						mOutputFieldIdx;

public:
	AosDataProcCount(const bool flag);
	AosDataProcCount(const AosDataProcCount &proc);
	~AosDataProcCount();

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

	// Ketty 2013/12/27
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
};

#endif
