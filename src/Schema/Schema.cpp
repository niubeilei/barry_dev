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
#include "Schema/Schema.h"

#include "API/AosApi.h"

AosSchema::AosSchema(
		const OmnString &type,
		const int version)
:
AosSchemaObj(version),
mJimoSubType(type)
{
}


AosSchema::~AosSchema()
{
}


AosXmlTagPtr
AosSchema::getDataRecordConf(
		const AosXmlTagPtr worker_doc,
		AosRundata *rdata)
{
	AosXmlTagPtr rcd_conf = worker_doc->getFirstChild("datarecord");
	if (!rcd_conf)
	{
		AosSetEntityError(rdata, "schemaunilen_missing_record", "Schema", mSchemaName) 
			<< worker_doc->toString();
		return 0;
	}

	OmnString name = rcd_conf->getAttrStr("zky_datarecord_name");
	if (name != "")
	{
	/*
		vector<OmnString> keys;
		keys.push_back("zky_task_name");
		keys.push_back("zky_otype");

		vector<OmnString> values;
		values.push_back(name);
		values.push_back("task");
	
		AosXmlTagPtr task_sdoc = AosGetDocByKey(rdata, "", keys, values);
		aos_assert_r(task_sdoc, 0);
	*/
		AosXmlTagPtr record = AosGetDocByKey(rdata, "", "zky_datarecord_name", name);
		aos_assert_r(record, 0);
		
		return record;
	}

	return rcd_conf;
}

/*
AosSchemaObjPtr
AosSchema::createSchema(
		AosRundata *rdata,
		const AosXmlTagPtr &worker_doc)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	if (!jimo) return 0;

	AosSchemaObjPtr schema = dynamic_cast<AosSchemaObj *>(jimo.getPtr());
	aos_assert_rr(schema, rdata, 0);
	return schema;
}
*/
// jimodb-953, 2015.10.17
int 
AosSchema::nextRecord(
		AosRundata *rdata,                                      
		char * data,
		int data_len,
		AosMetaData* meta,
		AosDataRecordObj *&rcd,
		int &parsed_len)
{
	return false;
}
