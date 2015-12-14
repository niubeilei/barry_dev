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
// 01/13/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ValueSel_ValueRandUtil_h
#define AOS_ValueSel_ValueRandUtil_h

#include "Alarm/Alarm.h"
#include "Random/Ptrs.h"
#include "Random/RandomUtil.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"

class AosValueRandUtil
{
public:
	enum
	{
		eMaxDocsPerCtnr
	};

	// This function creates a value selector that selects a single
	static OmnString pickValueSel(
					const OmnString &tagname, 
					const int level,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);
					
	static OmnString pickLhsSelector(
					const OmnString &tagname, 
					const int level,
					const OmnString &dft_value,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

	static OmnString pickRhsSelector(
					const OmnString &tagname, 
					const int level,
					const OmnString &dft_value,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

	static OmnString pickSingleValueSelector(
					const OmnString &tagname,
					const int level,
					const OmnString &dft_value,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

	static OmnString pickMultiValuesSelector(
					const OmnString &tagname,
					const int level,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

	static OmnString pickDataType()
	{
		int start = AosDataType::getFirstEntry();
		int end = AosDataType::getLastEntry();
		int idx = OmnRandom::nextInt1(start, end);
		return AosDataType::getTypeStr((AosDataType::E)idx);
	}

private:
	static OmnString getMulValuesFromUserDomain(
					const OmnString &tagname,
					const int level,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

	static OmnString getMulValuesFromContainer(
					const OmnString &tagname,
					const int level,
					const OmnString &container_objid,
					const OmnString &attrname,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

	static OmnString getSingleValueFromUserDomain(
					const OmnString &tagname,
					const int level,
					const OmnString &dft_value,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

	static OmnString getSingleValueFromContainer(
					const OmnString &tagname,
					const int level,
					const OmnString &container_objid,
					const OmnString &attrname,
					const OmnString &dft_value,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);
};
#endif

