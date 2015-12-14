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
#ifndef AOS_Schema_SchemaDummy_h
#define AOS_Schema_SchemaDummy_h

#include "Schema/Schema.h"

class AosSchemaDummy : virtual public AosSchema
{
	OmnDefineRCObject;

public:
	AosSchemaDummy(const int version);
	~AosSchemaDummy();

	//virtual bool nextRecordset(
	 //					AosRundata *rdata, 
	 //					const AosRecordsetObjPtr &recordset, 
	 //					const AosBuffPtr &buff, 
	 //					int64_t &offset);
	
	virtual bool appendData(
				AosRundata *rdata, 
				const AosBuffDataPtr &buffdata);
	
	virtual bool getNextRecordset(
				AosRundata *rdata, 
				AosRecordsetObjPtr &recordset, 
				AosDatasetObj *dataset,
				bool &contents_incomplete);

	virtual void setTaskDocid(const u64 task_docid){OmnNotImplementedYet;}
	virtual AosJimoPtr cloneJimo()  const{OmnNotImplementedYet; return NULL;};

};
#endif

