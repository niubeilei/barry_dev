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
#ifndef Aos_SEInterfaces_StatModelObj_h
#define Aos_SEInterfaces_StatModelObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Opr.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

using namespace std;


class AosStatModelObj : public AosJimo
{
public:
	AosStatModelObj(const int version);
	virtual ~AosStatModelObj();

	static AosStatModelObjPtr createStatModelStatic(
					AosRundata *rdata, 
					const AosXmlTagPtr &def);

	virtual OmnString getName() const = 0;
	virtual OmnString getTimeFieldName() const = 0;
	virtual vector<AosStatMeasureObjPtr> getMeasures() const = 0;
	virtual AosStatMeasureObjPtr getMeasure(const OmnString &name) const = 0;

	virtual AosStatModelObjPtr createStatModel(
					AosRundata *rdata, 
					const AosXmlTagPtr &def) = 0;
};

#endif
