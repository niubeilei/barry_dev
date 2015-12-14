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
#ifndef Aos_SEInterfaces_StatMeasureObj_h
#define Aos_SEInterfaces_StatMeasureObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Opr.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

using namespace std;


class AosStatMeasureObj : public AosJimo
{
public:
	AosStatMeasureObj(const int version);
	virtual ~AosStatMeasureObj();

	static AosStatMeasureObjPtr createStatMeasureStatic(
					AosRundata *rdata, 
					const AosXmlTagPtr &def);

	virtual OmnString getName() const = 0;
	virtual AosAggrFuncObjPtr getAggrFunc() const = 0;

	virtual AosStatMeasureObjPtr createStatMeasure(
					AosRundata *rdata, 
					const AosXmlTagPtr &def) = 0;
};

#endif
