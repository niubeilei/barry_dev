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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_DataRecordXml_h
#define Aos_DataRecord_DataRecordXml_h

#include "DataRecord/DataRecord.h"

class AosRecordXmlSimple : public AosDataRecord
{
private:
	AosXmlTagPtr	mRecord;
public:
	AosRecordXmlSimple(AosMemoryCheckDeclBegin);
	~AosRecordXmlSimple();

	virtual bool	getFieldValue(
					const OmnString &field_name,
					AosValueRslt &value, 
					const bool copy_flag,
					AosRundata* rdata);

	void setDoc(AosXmlTagPtr record);
	
	AosJimoPtr cloneJimo()const { return NULL; } 
	bool isFixed()const { return false; }
	int getRecordLen() { return 0; }
	int getEstimateRecordLen() { return 0; }
	bool setData(char*, int, AosMetaDataPtr, int64_t) { return false; }
	AosDataRecordObjPtr clone(AosRundata* AosMemoryCheckDecl) const { return NULL; }
	AosDataRecordObjPtr create( const AosXmlTagPtr&, const AosTaskObjPtr&, AosRundata*) const { return NULL; }
};
#endif

