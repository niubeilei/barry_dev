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
// This action filters data through a hash map. The map is defined by 
// a smart doc. Callers should call:
// 		evalCond(const char *data, const int len, ...);
// 'data' points to the beginning of a record; 'len' is the length of the
// record. The condition matches the substring:
// 		[data[mStartPos], data[mStartPos] + mMatchedLength]
//
// Modification History:
// 05/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondEmpty.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/HashUtil.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"


AosCondEmpty::AosCondEmpty(const bool flag)
:
AosCondition(AOSCONDTYPE_EMPTY, AosCondType::eEmpty, flag),
mMapType(eInvalidMap)
{
}


AosCondEmpty::AosCondEmpty(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
AosCondition(AOSCONDTYPE_EMPTY, AosCondType::eEmpty, false),
mMapType(eInvalidMap)
{
	if (!config(def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


AosCondEmpty::~AosCondEmpty()
{
}


bool
AosCondEmpty::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// 'def' should be:
	// 	<cond AOSTAG_ZKY_TYPE=AOSCONDTYPE_CHARSTRMAP
	// 		maptype="black|white"
	// 		separator=","
	// 		start_pos="xxx"
	// 		matched_len="xxx"
	// 		map_objid="xxx">xxx,xxx,...
	// 	</cond>
	aos_assert_rr(def, rdata, false);

	// Retrieve maptype
	OmnString ss = def->getAttrStr("maptype");
	if (ss == "black")
	{
		mMapType = eBlackMap;
	}
	else if (ss == "white")
	{
		mMapType = eWhiteMap;
	}
	else
	{
		AosSetErrorU(rdata, "invalid_map_type:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool
AosCondEmpty::evalCond(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

	
bool
AosCondEmpty::evalCond(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnString data = value.getStr();
	return evalCond(data.data(), data.length(), rdata);
}


bool
AosCondEmpty::evalCond(const char *data, int len, const AosRundataPtr &rdata)
{
	// Did not find it.
	if (mMapType == eWhiteMap)
	{
		return len == 0;
	}

	return len != 0;
}


AosConditionObjPtr
AosCondEmpty::clone() 
{
	try
	{
		AosCondEmpty * cond = OmnNew AosCondEmpty(false);
		cond->mMapType = mMapType;
		return cond;
	}

	catch (...)
	{
		OmnAlarm << "failed create cond" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

