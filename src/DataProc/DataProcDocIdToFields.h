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
#ifndef Aos_DataProc_DataProcDocIdToFields_h
#define Aos_DataProc_DataProcDocIdToFields_h

#include "DataProc/DataProc.h"
#include "SEUtil/SeTypes.h"

class AosDataProcDocIdToFields : virtual public AosDataProc
{
	vector<Field>		mFields;
	int					mDocidFieldIdx;
	RecordFieldInfo		mOutputDocid;

public:
	AosDataProcDocIdToFields(const bool flag);
	AosDataProcDocIdToFields(const AosDataProcDocIdToFields &proc);
	~AosDataProcDocIdToFields();

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

	virtual AosDataProcStatus::E procData(
						AosRundata *rdata_raw,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);

	virtual bool 	resolveDataProc(
						const AosRundataPtr &rdata,
						const AosDataRecordObjPtr &input_record,
						const AosDataRecordObjPtr &output_record);

	static u32 genFieldFunc(const u64 &docid, int swap1, int swap2);
	bool docIdToFile(u64 docid, OmnString fname);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	
};

#endif
