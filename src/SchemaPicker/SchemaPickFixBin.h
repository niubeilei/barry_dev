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
#ifndef AOS_SchemaPicker_SchemaPickFixBin_h
#define AOS_SchemaPicker_SchemaPickFixBin_h

#include "SchemaPicker/SchemaPicker.h"
#include <map>
using namespace std;

class AosSchemaPickFixBin : public AosSchemaPicker
{
	OmnDefineRCObject;

private:
	int										mKeyIdx;
	int										mKeyLen;
	map<OmnString, int>						mMap;

public:
	AosSchemaPickFixBin(const u32 version); 
	~AosSchemaPickFixBin();
	virtual void reset();	

	virtual int pickSchemaIndex(
						AosRundata *rdata,
						const char *data,
						const int rcd_len);

	virtual bool addSchemaIndex(const AosXmlTagPtr &conf,
								const int index);
	virtual AosJimoPtr cloneJimo()  const;

	virtual bool config(const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

private:
	bool 	getMap(const vector<AosDataRecordObjPtr> &records,
				   map<OmnString, AosDataRecordObj *> &rcdMap);
};
#endif

