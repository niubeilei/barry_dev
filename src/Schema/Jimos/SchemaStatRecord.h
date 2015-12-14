////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Statistics.h
// Description:
//   
//
// Modification History:
// 2014/01/22 Created by Ketty
// 
////////////////////////////////////////////////////////////////////////////

#ifndef AOS_Schema_Jimos_SchemaStatRecord_h
#define AOS_Schema_Jimos_SchemaStatRecord_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Schema/Schema.h"
#include "StatUtil/StatFieldDef.h"
#include "StatUtil/Ptrs.h"

#include <vector>
using namespace std;


class AosSchemaStatRecord : public AosSchema
{
	OmnDefineRCObject;

private:
	AosDataRecordObjPtr	mDataRecord;
	AosDataRecordObj *	mDataRecordRaw;
	
	vector<AosStatFieldDef>	mStatFieldDefs;
	vector<AosStatFieldDef>	mRcdFieldDefs;

public:
	AosSchemaStatRecord(
		const OmnString &type,
		const int version);
	AosSchemaStatRecord(const int version);
	~AosSchemaStatRecord();

	virtual AosJimoPtr cloneJimo()  const;
	
	virtual bool config( 
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);
	
	virtual bool nextRecordset(
						const AosRundataPtr &rdata, 
						AosRecordsetObjPtr &recordset, 
						const AosBuffDataPtr &buffdata,
						AosDatasetObj *dataset,
						bool &contents_incomplete){ return true; };

	virtual bool getNextRecord(
				const AosRundataPtr rdata,
				AosBuff * rcd_buff_data,
				AosDataRecordObj *record);
	
	virtual void setTaskDocid(const u64 task_docid){ return; };
	virtual bool getRecord(const OmnString &name, AosDataRecordObjPtr &record);

private:
	bool 	initDataRecord(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	bool 	initStatFieldDef(const AosXmlTagPtr &conf);
	bool 	initRcdFieldDef();

	bool 	getEntryFieldValue(
				const AosRundataPtr &rdata,
				AosBuff * rcd_buff_data,
				const u32 idx,
				AosValueRslt &value_rslt);

	bool 	getCalendarTime(
				const int64_t epoch_time,
				AosValueRslt &value_rslt);
	
};

#endif
