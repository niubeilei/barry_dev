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
// 2013/05/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DLLCondObj_h
#define Aos_SEInterfaces_DLLCondObj_h

#include "SEInterfaces/DLLObj.h"
#include "SEInterfaces/ConditionObj.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"

class AosDLLCondObj : public AosConditionObj, public AosDLLObj
{
private:
	OmnDefineRCObject;

protected:
	OmnString	mCondName;

public:
	AosDLLCondObj(const OmnString &name, 
			const OmnString &libname, 
			const OmnString &method,
			const OmnString &version);
	~AosDLLCondObj();

	// ConditionObj Interface
	virtual AosConditionObjPtr clone();
	virtual bool evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool evalCond(const AosRundataPtr &rdata);
	virtual bool evalCond(const char *record, const int len, const AosRundataPtr &rdata);
	virtual bool evalCond(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	virtual bool evalCond(const AosValueRslt &value, const AosRundataPtr &rdata);
	virtual AosConditionObjPtr getCondition(const AosXmlTagPtr &conf, const AosRundataPtr &);
	virtual bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif

