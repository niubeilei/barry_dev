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
#ifndef AOS_Schema_Jimos_SchemaUniLength_h
#define AOS_Schema_Jimos_SchemaUniLength_h

#include "Schema/Schema.h"
#include "SEInterfaces/TaskObj.h"

class AosSchemaUniLength : public AosSchema 
{
	OmnDefineRCObject;

	int						mRecordLen;
	AosDataRecordObjPtr 	mRecord;
	
public:
	AosSchemaUniLength(const int version);
	~AosSchemaUniLength();

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
};
#endif

