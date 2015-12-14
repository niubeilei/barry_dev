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
// 06/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Actions/ActDataCombiner.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "XmlUtil/XmlTag.h"


AosActDataCombiner::AosActDataCombiner(const bool flag)
:
AosSdocAction(AOSACTTYPE_DATACOMBINER, AosActionType::eDataCombiner, flag)
{
}


AosActDataCombiner::~AosActDataCombiner()
{
}


bool
AosActDataCombiner::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool	
AosActDataCombiner::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosActDataCombiner::run(const AosDataBlobPtr &table, const AosRundataPtr &rdata)
{
	// This function combines records in table based on the given criteria. 
	// For example, if the records are in the following format:
	// 		[phonenumber, call-duration]
	// 		[phonenumber, call-duration]
	// 		[phonenumber, call-duration]
	// 		[phonenumber, call-duration]
	//
	// We can combine all the records that have the same phone number into 
	// one by adding their call-duration values.
	aos_assert_rr(table, rdata, false);

	// 1. Sort the table as needed
	if (table->isSorted()) table->sort();

	// 2. Do the group-by operation. 
	bool rslt = false;
	switch (mOpr)
	{
	case eSum:
		 rslt = groupBySum(data, rdata);
		 break;

	case eMaximum:
		 rslt = groupByMax(data, rdata);
		 break;

	case eMinimum:
		 rslt = groupByMin(data, rdata);
		 break;

	case eAverage:
		 rslt = groupByAverage(data, rdata);
		 break;

	default:
		 AosSetErrorU(rdata, "invalid_opr") << ": " << mOpr << enderr;
		 rslt = false;
	}

	return rslt;
}


AosActionObjPtr
AosActDataCombiner::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActDataCombiner(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
	}

}


bool
AosActDataCombiner::groupBySum(const AosDataBlobPtr &data, const AosRundataPtr &rdata)
{
	// For each entry, group it into a group. Then calculate its sum.
}
#endif
