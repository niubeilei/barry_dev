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
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataParserObj_h
#define Aos_SEInterfaces_DataParserObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DatasetObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosDataParserObj : public AosJimo
{
public:
	AosDataParserObj(const int version);
	~AosDataParserObj();

	virtual void setTask(const AosTaskObjPtr &t) = 0;
	virtual bool getRecord(const OmnString &name, AosDataRecordObjPtr &record) = 0;
	virtual bool getRecords(vector<AosDataRecordObjPtr> &records) = 0;

	virtual bool nextRecordset(
						AosRundata *rdata, 
						AosRecordsetObjPtr &recordset, 
						const AosBuffDataPtr &buff, 
						AosDatasetObj *dataset,
						bool &contents_incomplete) = 0;
	
	virtual bool getNextRecordset(
						AosRundata *rdata, 
						AosRecordsetObjPtr &recordset, 
						AosDatasetObj *dataset,
						bool &contents_incomplete) = 0;
	
	virtual bool getNextRecord(					// Ketty 2014/09/12
						const AosRundataPtr rdata,
						AosBuff * rcd_buff_data,
						AosDataRecordObj *record) = 0;
	
	static AosDataParserObjPtr createParserStatic(
						AosRundata *rdata,
						const AosXmlTagPtr &worker_doc);
};
#endif
