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
#include "Conds/CondRegExp.h"

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
#include <boost/regex.hpp>


AosCondRegExp::AosCondRegExp(const bool flag)
:
AosCondition(AOSCONDTYPE_REGEXP, AosCondType::eRegExp, flag),
mMapType(eInvalidMap)
{
}


AosCondRegExp::AosCondRegExp(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
AosCondition(AOSCONDTYPE_REGEXP, AosCondType::eRegExp, false),
mMapType(eInvalidMap)
{
	if (!config(def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


AosCondRegExp::~AosCondRegExp()
{
}


bool
AosCondRegExp::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// 'def' should be:
	// 	<cond AOSTAG_ZKY_TYPE=AOSCONDTYPE_CHARSTRMAP
	// 		  maptype="black|white">
	// 	xxxxxx
	// 	</cond>
	aos_assert_rr(def, rdata, false);

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

	// Retrieve maptype
	mRegExp = def->getNodeText();
	aos_assert_r(mRegExp != "", false);
	return true;
}


bool
AosCondRegExp::evalCond(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

	
bool
AosCondRegExp::evalCond(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnString data = value.getStr();
	return evalCond(data.data(), data.length(), rdata);
}


bool
AosCondRegExp::evalCond(const char *data, int len, const AosRundataPtr &rdata)
{

	string r = mRegExp.data(); 
	try 
	{ 
		boost::regex reg(r); 

		if (mMapType == eWhiteMap)
		{
	 		return boost::regex_match(data, reg);
		}
	 	return !boost::regex_match(data, reg);
	} 
	catch(const boost::bad_expression& e)
	{ 
		OmnAlarm << "That's not a valid regular expression!" << e.what() << enderr;
		return false;
	}
}


AosConditionObjPtr
AosCondRegExp::clone() 
{
	try
	{
		AosCondRegExp * cond = OmnNew AosCondRegExp(false);
		cond->mRegExp = mRegExp;
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

