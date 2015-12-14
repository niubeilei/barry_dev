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
#ifndef AOS_Schema_Schema_h
#define AOS_Schema_Schema_h

#include "SEInterfaces/SchemaObj.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

class AosSchema : public AosSchemaObj
{
protected:
	OmnString		mJimoSubType;
	OmnString		mSchemaName;

public:
	AosSchema(const OmnString &type, const int version);
	~AosSchema();

	virtual bool getRecord(const OmnString &name, AosDataRecordObjPtr &record){return false;}
	virtual bool getRecords(vector<AosDataRecordObjPtr> &records){return false;}

	virtual bool nextRecordset(
						AosRundata *rdata, 
						AosRecordsetObjPtr &recordset, 
						const AosBuffDataPtr &buff, 
						AosDatasetObj *dataset,
						bool &contents_incomplete){return false;}
	
	virtual bool getNextRecordset(
						AosRundata *rdata, 
						AosRecordsetObjPtr &recordset, 
						AosDatasetObj *dataset,
						bool &contents_incomplete){return false;}

	virtual bool getNextRecord(						// Ketty 2014/09/12
						const AosRundataPtr rdata,
						AosBuff * rcd_buff_data,
						AosDataRecordObj *record){ return false; };
	
	AosXmlTagPtr	getDataRecordConf(
						const AosXmlTagPtr worker_doc,
						AosRundata *rdata);

	virtual int nextRecord(
			AosRundata *rdata,                                      
			char * data,
			int data_len,
			AosMetaData* meta,
			AosDataRecordObj *&rcd,
			int &parsed_len);

	virtual AosSchemaObjPtr clone(AosRundata *rdata AosMemoryCheckDecl) const {return 0;}
};
#endif

