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
#ifndef AOS_Schema_Jimos_SchemaRecordByLenId_h
#define AOS_Schema_Jimos_SchemaRecordByLenId_h

#include "Schema/Schema.h"
#include "SEInterfaces/TaskObj.h"

class AosSchemaRecordByLenId : public AosSchema 
{
	OmnDefineRCObject;

private:
	AosDataRecordObjPtr 	mRecord;
	u64 					mTaskDocid;
	
	// for mutil record
	u64				mSchemaDocid;
	vector<AosDataRecordObjPtr>     mRecords;
	map<u64, int>                   mRecordDocids;  // key:schema_id, value:record index      
	bool							mIsMultiRecord;

public:
	AosSchemaRecordByLenId(const int version);
	AosSchemaRecordByLenId(const AosSchemaRecordByLenId &rhs, AosRundata *rdata AosMemoryCheckDecl);
	~AosSchemaRecordByLenId();

	virtual bool nextRecordset(
						AosRundata *rdata, 
						AosRecordsetObjPtr &recordset, 
						const AosBuffDataPtr &buffdata,
						AosDatasetObj *dataset,
						bool &contents_incomplete);

	virtual void setTaskDocid(const u64 task_docid);

	virtual bool getRecord(const OmnString &name, AosDataRecordObjPtr &record);
	
	virtual bool getRecords(vector<AosDataRecordObjPtr> &records);

	virtual AosJimoPtr cloneJimo()  const;
	
	virtual bool config(const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);
	int nextRecord(
			AosRundata *rdata,                                      
			char * data,
			int data_len,
			AosMetaData* meta,
			AosDataRecordObj *&rcd,
			int &parsed_len);

	bool getIsMultiRecord() {return mIsMultiRecord;}

	virtual AosSchemaObjPtr clone(AosRundata *rdata AosMemoryCheckDecl) const;

private:
	bool nextRecordsetByMulti(
			AosRundata *rdata, 
			AosRecordsetObjPtr &recordset, 
			const AosBuffDataPtr &buffdata,
			AosDatasetObj *dataset,
			bool &contents_incomplete);

	int getRecordIndex(
			AosRundata *rdata, 
			const u64 &docid, 
			AosRecordsetObj *recordset_raw);

};
#endif

