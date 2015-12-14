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
#ifndef AOS_SchemaPicker_SchemaPicker_h
#define AOS_SchemaPicker_SchemaPicker_h

#include "SEInterfaces/SchemaPickerObj.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

class AosSchemaPicker : public AosSchemaPickerObj
{
	OmnDefineRCObject;

	OmnString			mJimoSubType;

public:
	AosSchemaPicker(
			const OmnString &type,
			const u32 version);
	~AosSchemaPicker();

	/*
	virtual AosDataRecordObj * pickSchema(
				AosRundata *rdata,
				const char *data,
				const int rcd_len){return 0;}

	virtual bool holdRecords(
				const vector<AosDataRecordObjPtr> &records,
				AosRundata *rdata){return false;}
	*/

	virtual bool addSchemaIndex(
				const AosXmlTagPtr &conf,
				const int index){return false;}
	
	virtual int pickSchemaIndex(
				AosRundata *rdata,
				const char *data,
				const int rcd_len){return -1;}

	virtual void reset(){return;}
	/*
	virtual bool addSchema(
				AosDataRecordObj * schema,
				const AosXmlTagPtr &conf,
				AosRundata *rdata){return false;}
	*/
};
#endif

