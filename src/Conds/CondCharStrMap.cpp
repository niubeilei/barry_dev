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
// This filter filters data through a hash map. The map is defined by 
// a smart doc. Callers should call:
// 		evalCond(const char *data, const int len, ...);
// 'data' points to the beginning of a record; 'len' is the length of the
// record. The condition matches the substring:
// 		[data[mStartPos], data[mStartPos] + mMatchedLength]
//
// Modification History:
// 05/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondCharStrMap.h"

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


AosCondCharStrMap::AosCondCharStrMap(const bool flag)
:
AosCondition(AOSCONDTYPE_CHARSTRMAP, AosCondType::eCharStrMap, flag),
mMapType(eInvalidMap),
mStartPos(-1),
mMatchedLength(-1),
mMaxSkip(-1),
mSkipChar(0),
mLock(OmnNew OmnMutex())
{
}


AosCondCharStrMap::AosCondCharStrMap(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
AosCondition(AOSCONDTYPE_CHARSTRMAP, AosCondType::eCharStrMap, false),
mMapType(eInvalidMap),
mStartPos(-1),
mMatchedLength(-1),
mMaxSkip(-1),
mSkipChar(0),
mLock(OmnNew OmnMutex())
{
	if (!config(def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


AosCondCharStrMap::~AosCondCharStrMap()
{
}


bool
AosCondCharStrMap::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
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

	// Retrieve separator
	OmnString separator = def->getAttrStr("separator");
	if (separator == "") separator = "|$|";

	// Chen Ding, 12/24/2012
	AosConvertAsciiBinary(separator);

	// Retrieve start position
	mStartPos = def->getAttrInt("start_pos", -1);
	if (mStartPos < 0)
	{
		AosSetErrorU(rdata, "invalid_start_pos:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve matched length
	mMatchedLength = def->getAttrInt("matched_len", -1);
	if (mMatchedLength <= 0)
	{
		AosSetErrorU(rdata, "invalid_matched_len:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString map_objid = def->getAttrStr("map_objid");
	OmnString words;
	if (map_objid != "")
	{
		// The map is defined by a doc.
		AosXmlTagPtr doc = AosRetrieveDocByObjid(map_objid, false, rdata);
		if (!doc)
		{
			AosSetErrorU(rdata, "failed_retr_obj_byobjid:") << map_objid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		// 'doc' should be in the form:
		// 	<doc ...>
		// 		<AOSTAG_MAP_VALUES>xxx,xxx,...</AOSTAG_MAP_VALUES>
		// 	</doc>
		AosXmlTagPtr tag = doc->getFirstChild(AOSTAG_MAP_VALUES);
		if (!tag)
		{
			AosSetErrorU(rdata, "missing_map_values:") << doc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		words = tag->getNodeText();
	}
	else
	{
		// It is defined in 'def'.
		words = def->getNodeText();
	}

	if (words == "")
	{
		AosSetErrorU(rdata, "map_is_empty:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnStrParser1 parser(words, separator);
	OmnString word;
	//mMap.clear();
	mSet.clear();
	while ((word = parser.nextWord()) != "")
	{
		//char *mem = OmnNew char[word.length()+1];
		//strcpy(mem, word.data());
		//mMap[mem] = 1;
		mSet.insert(word);
//		OmnScreen << "add map, mem:" << (long)mem << ", this :" << this << endl;
	}
	
	mMaxSkip = def->getAttrInt("max_skip", 10);
	mSkipChar = def->getAttrChar("skip_char", 0);
	
	return true;
}


bool
AosCondCharStrMap::evalCond(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

	
bool
AosCondCharStrMap::evalCond(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	int len;
	OmnString data = value.getStr();
	return evalCond(data.data(), len, rdata);
}


bool
AosCondCharStrMap::evalCond(const char *data, int len, const AosRundataPtr &rdata)
{
	// This function uses 'mMap' to determine whether 'data' should be 
	// filtered. It uses a substring of 'data', defined by [mStartPos, mMatchedLength]
	// to search the map. 
	//
	// The function returns false if the condition evalutes to false. Otherwise, 
	// it returns true.
	if (mStartPos < 0 || !data || len <= 0) return false;

	int start_pos = mStartPos;
	// Chen Ding, 12/24/2012
	// if (mSkipChar)
	if (mSkipChar && mMaxSkip > 0)
	{
		// Skip the leading characters that are the same as mSkipChar, up to mMaxSkip
		while (start_pos <= len && (start_pos - mStartPos <= mMaxSkip) && data[start_pos] == mSkipChar)
		{
			start_pos++;
		}
	}
	
	int end_pos = start_pos + mMatchedLength;
	if(end_pos > len) return false;

	char vv[mMatchedLength + 1];
	strncpy(vv, &data[start_pos], mMatchedLength);
	vv[mMatchedLength] = 0;

	mLock->lock();
	//AosCharStr2IntItr itr = mMap.find(vv);
	//if (itr == mMap.end())
	set<OmnString>::iterator itr = mSet.find(vv);
	if (itr == mSet.end())
	{
		mLock->unlock();

		// Did not find it.
		if (mMapType == eWhiteMap)
		{
			// It is a white map. Data are filtered out if they are not in the map.
			return false;
		}
		return true;
	}

	mLock->unlock();

	// Found it. 
	if (mMapType == eBlackMap)
	{
		// It should be filtered out.
		return false;
	}

	// it is accepted.
	return true;
}


AosConditionObjPtr
AosCondCharStrMap::clone() 
{
	try
	{
		AosCondCharStrMap * cond = OmnNew AosCondCharStrMap(false);
		//cond->mMap = mMap;
		cond->mSet = mSet;
		cond->mMapType = mMapType;
		cond->mStartPos = mStartPos;
		cond->mMatchedLength = mMatchedLength;
		cond->mMaxSkip = mMaxSkip;
		cond->mSkipChar = mSkipChar;
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

