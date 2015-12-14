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
#ifndef Aos_SEInterfaces_SchemaPickerObj_h
#define Aos_SEInterfaces_SchemaPickerObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DatasetObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

#define AOSSCHEMAPICKER_DUMMY		"dummy"
#define AOSSCHEMAPICKER_FIXBIN		"fixbin"
#define AOSSCHEMAPICKER_VARIABLE	"variable"

class AosSchemaPickerObj : public AosJimo
{
public:
	AosSchemaPickerObj(const int version);
	~AosSchemaPickerObj();

	virtual bool addSchemaIndex(const AosXmlTagPtr &tag, const int index) = 0;
	virtual int  pickSchemaIndex(AosRundata *rdata, const char *data, const int rcd_len) = 0;
	virtual void reset() = 0;

	static AosSchemaPickerObjPtr createSchemaPickerStatic(
						AosRundata *rdata,
						const AosXmlTagPtr &worker_doc);
};
#endif
