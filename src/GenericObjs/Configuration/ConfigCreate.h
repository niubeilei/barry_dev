////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2014/11/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GenericObjs_Configuration_ConfigCreate_h
#define Aos_GenericObjs_Configuration_ConfigCreate_h

#include "GenericObjs/GenericMethod.h"
#include "DataRecord/RecordParmList.h"
#include "DataRecord/Ptrs.h"
#include "Random/RandomUtil.h"
#include <vector>
#include <map>
#include "SEUtil/ParmNames.h"
#include "SEUtil/ParmValues.h"
#include "GenericObjs/Ptrs.h"
class AosMethodConfigCreate : public AosGenericMethod
{
	OmnDefineRCObject;

protected:

public:
	AosMethodConfigCreate(const int version);
	~AosMethodConfigCreate();

	virtual bool	config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool	proc(
						AosRundata *rdata,
						const OmnString &obj_name, 
						const vector<AosGenericValueObjPtr> &parms);
};
#endif

