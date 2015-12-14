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
// This is a utility to select docs.
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Conds_Filter_h
#define AOS_Conds_Filter_h

#include "Conds/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosFilter : public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosConditionObjPtr		mCondition;
	bool				mIsBlackList;

public:
	AosFilter();
	AosFilter(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	virtual ~AosFilter();

	bool filterData(
			const char *record, 
			const int len, 
			const bool dft, 
			const AosRundataPtr &rdata);

	bool filterData(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
};
#endif

