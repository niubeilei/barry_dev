////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Schema_Jimos_SchemaFixedLen_h
#define AOS_Schema_Jimos_SchemaFixedLen_h

//#include "RecordScanner/RecordScanner.h"
#include "Schema/Schema.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/DatasetObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/TaskObj.h"

#include <vector>
#include <map>
using namespace std;

class AosSchemaFixedLen : public AosSchema
{
	OmnDefineRCObject;

	enum
	{
		eDftMaxRcdLen = 1000000000
	};

private:
	vector<AosDataRecordObjPtr>		mRecords;
	AosSchemaPickerObjPtr 			mSchemaPicker;
	OmnString						mPattern;
	bool							mLastEntryWithNoPattern;
	int								mPatternLen;
	int64_t							mMaxRcdLen;
	bool 							mSkipInvalidRecords;
	const char *					mCrtData;
	int64_t							mCrtDataLen;
	int64_t							mCrtStartPos;
	int64_t							mCrtPos;
	bool							mIgnoreSubPattern;
	bool							mCheckValidRecord;
	AosConditionObjPtr				mFilter;
	
public:
	AosSchemaFixedLen(const int version);
	~AosSchemaFixedLen();
	
	virtual bool nextRecordset(
						AosRundata *rdata, 
						AosRecordsetObjPtr &recordset, 
						const AosBuffDataPtr &buff, 
						AosDatasetObj *dataset,
						bool &contents_incomplete);
	
	virtual void setTaskDocid(const u64 task_docid);

	virtual bool getRecord(const OmnString &name, AosDataRecordObjPtr &record);
	
	virtual bool getRecords(vector<AosDataRecordObjPtr> &records);

	virtual AosJimoPtr cloneJimo()  const;
	
	virtual bool	config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &worker_doc,
				const AosXmlTagPtr &jimo_doc);
	
private:
	bool handleUnrecogRecord(
				AosRundata *rdata, 
				AosDatasetObj * dataset,
				const AosBuffDataPtr &raw_data,
				const bool incomplete,
				const char * crt_data,
			    int64_t &crt_start_pos,
				int64_t &crt_pos,
				int64_t &crt_data_len);

	bool createRecord(
				AosRundata *rdata, 
				AosDatasetObj* dataset, 
				const AosBuffDataPtr &raw_data,
				AosRecordsetObj *recordset,
				const int rcd_len,
				const char * crt_data,
			    int64_t &crt_start_pos,
				int64_t &crt_pos,
				int64_t &crt_data_len);

	bool procFinish(
				AosRundata *rdata, 
				const AosBuffDataPtr &raw_data, 
				AosDatasetObj *dataset,
				AosRecordsetObj* recordset_raw,
				const int rcd_len,
				bool &contents_incomplete,
				const char * crt_data,
			    int64_t &crt_start_pos,
				int64_t &crt_pos,
				int64_t &crt_data_len);

	bool processLastRecord(
				AosRundata *rdata, 
				const AosBuffDataPtr &raw_data, 
				AosDatasetObj *dataset,
				AosRecordsetObj* recordset_raw,
				const int rcd_len,
				bool &contents_incomplete,
				const char * crt_data,
			    int64_t &crt_start_pos,
				int64_t &crt_pos,
				int64_t &crt_data_len);
};
#endif

