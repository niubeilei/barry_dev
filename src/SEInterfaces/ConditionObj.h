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
// 02/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ConditionObj_h
#define Aos_SEInterfaces_ConditionObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/CondTypes.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"

class AosConditionObj : virtual public OmnRCObject
{
private:
	static AosConditionObjPtr smCondition;

protected:
	AosCondType::E	mType;

public:
	virtual AosConditionObjPtr clone() = 0;
	virtual bool evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata) = 0;
	virtual bool evalCond(const AosRundataPtr &rdata) = 0;
	virtual bool evalCond(const char *record, const int len, const AosRundataPtr &rdata) = 0;
	virtual bool evalCond(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata) = 0;
	virtual bool evalCond(const AosValueRslt &value, const AosRundataPtr &rdata) = 0;
	virtual AosConditionObjPtr getCondition(const AosXmlTagPtr &conf, const AosRundataPtr &) = 0;
	virtual bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata) = 0;

	AosCondType::E getType() const {return mType;}

	static AosConditionObjPtr getCondition() {return smCondition;}
	static void setCondition(const AosConditionObjPtr &d) {smCondition = d;}
	static AosConditionObjPtr getConditionStatic(const AosXmlTagPtr &conf, const AosRundataPtr &);
};

#endif

