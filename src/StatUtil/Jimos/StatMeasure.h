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
// 2015/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatUtil_StatMeasure_h
#define Aos_StatUtil_StatMeasure_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "SEInterfaces/StatMeasureObj.h"
#include "SEInterfaces/AggrFuncObj.h"
#include <vector>

using namespace std;


class AosStatMeasure : public AosStatMeasureObj
{
	OmnDefineRCObject;

private:
	OmnString mName;
	AosAggrFuncObjPtr mAggrFuncPtr;
	OmnString mFieldName;

public:
	AosStatMeasure(const int version);
	virtual ~AosStatMeasure();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	virtual OmnString getName() const {return mName; }
	virtual OmnString getFieldName() {return mFieldName; }
	virtual AosAggrFuncObjPtr getAggrFunc() const {return mAggrFuncPtr; }

	virtual AosStatMeasureObjPtr createStatMeasure(
					AosRundata *rdata, 
					const AosXmlTagPtr &def);
};

#endif
