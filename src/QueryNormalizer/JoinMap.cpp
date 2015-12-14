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
// A join map is defined by:
// 	1. One or more key fields:
// 			[key1, key2, ...]
//  2. Join type
//  3. Zero or more conditions
//
// Modification History:
// 2015/02/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/JoinMap.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosJoinMap::AosJoinMap(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	if (!parsePrivate(rdata, def))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosJoinMap::~AosJoinMap()
{
}


AosXmlTagPtr
AosJoinMap::createXmlTag(
		AosRundata *rdata, 
		const vector<OmnString> &keys,
		const AosJoinType::E join_type,
		const vector<AosQueryCondPtr> &conds)
{
	if (!AosJoinType::isValid(join_type))
	{
		AosLogError(rdata, true, "invalid_join_type") << enderr;
		return 0;
	}

	if (keys.size() == 0)
	{
		AosLogError(rdata, true, "missing_keys") << enderr;
		return 0;
	}

	OmnString tag_str = "<joinmap keys=\"";
	for (u32 i=0; i<keys.size(); i++)
	{
		if (!isValidKey(keys[i]))
		{
			AosLogError(rdata, true, "invalid_map_key") << enderr;
			return false;
		}
		if (i>0) tag_str << ",";
		tag_str << keys[i];
	}
	tag_str << "\" join_type=\"" << AosJoinType::toStr(join_type) 
		<< "\">";

	if (conds.size() > 0)
	{
		tag_str << "<conds>";
		for (u32 i=0; i<conds.size(); i++)
		{
			tag_str << conds[i]->toXmlStr(rdata, "cond");
		}
		tag_str << "</conds>";
	}
	tag_str << "</joinmap>";

	AosXmlTagPtr doc = AosParseXml(rdata, tag_str);
	aos_assert_rr(doc, rdata, 0);
	return doc;
}


AosJoinMapPtr
AosJoinMap::parseJoinMap(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	try
	{
		return OmnNew AosJoinMap(rdata, def);
	}

	catch (...)
	{
		AosLogError(rdata, true, "invalid_join_map") << enderr;
		return 0;
	}
}


bool
AosJoinMap::parsePrivate(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	// A join map is defined by:
	// 	1. One or more key fields:
	// 			[key1, key2, ...]
	//  2. Join type
	//  3. Zero or more conditions
	//
	// The def is as follows:
	// 	<joinmap keys="xxx,xxx..." join_type="xxx">
	// 		<conds>
	// 			<cond .../>
	// 			<cond .../>
	// 			...
	// 			<cond .../>
	// 		</conds>
	// 	</joinmap>
	aos_assert_rr(def, rdata, false);
	
	if (mParsed) return true;

	NotImplementedYet;
	return false;
}


bool
AosJoinMap::doesJoinMapMatch(
		AosRundata *rdata, 
		const vector<OmnString> &keys,
		const AosJoinType::E join_type,
		const vector<AosQueryCondPtr> &conds)
{
	// This function checks whether the join map
	// [keys, join_type, conds] matches this
	// join map. 
	if (!mParsed) 
	{
		if (!parse(rdata)) return false;
	}

	OmnNotImplementedYet;
	return false;
}

