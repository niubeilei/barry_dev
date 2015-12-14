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
// 2014/08/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_AggrFuncMgrObj_h
#define AOS_SEInterfaces_AggrFuncMgrObj_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
//#include "SEInterfaces/AggregationType.h"
#include "Util/Ptrs.h"
#include "Util/DataTypes.h"
#include "XmlUtil/Ptrs.h"

class AosRundata;

class AosAggrFuncMgrObj : public AosJimo
{
public:
	AosAggrFuncMgrObj(const int version);

	~AosAggrFuncMgrObj();

	virtual AosAggrFuncObjPtr pickAggrFunc(
				const AosRundataPtr &rdata,
				const OmnString &func_name) = 0;
	
	virtual bool isFuncValid(
				const AosRundataPtr &rdata,
				const OmnString &func_name) = 0;

};
#endif

