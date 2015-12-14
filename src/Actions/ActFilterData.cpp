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
// This action filters data through a hash map.
//
// Modification History:
// 05/06/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActFilterData.h"

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


AosActFilterData::AosActFilterData(const bool flag)
:
AosSdocAction(AOSACTTYPE_FILTERDATA, AosActionType::eFilterData, flag),
mMapType(eInvalidMap),
mLock(OmnNew OmnMutex())
{
}


AosActFilterData::AosActFilterData(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_FILTERDATA, AosActionType::eFilterData, false),
mMapType(eInvalidMap),
mLock(OmnNew OmnMutex())
{
	if (!config(def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


AosActFilterData::~AosActFilterData()
{
	AosCharS2U8Itr_t itr = mMap.begin();
	for (; itr != mMap.end(); itr++)
	{
		OmnDelete [] itr->first;
	}
}


bool
AosActFilterData::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
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

	OmnString separator = def->getAttrStr("separator");
	if (separator == "") separator = "|$|";

	mStartPos = def->getAttrInt("start_pos", -1);
	if (mStartPos < 0)
	{
		AosSetErrorU(rdata, "invalid_start_pos:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

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
		OmnString words = def->getNodeText();
	}

	if (words == "")
	{
		AosSetErrorU(rdata, "map_is_empty:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnStrParser1 parser(words, separator);
	OmnString word;
	while ((word = parser.nextWord()) != "")
	{
		char *mem = OmnNew char[word.length()+1];
		strcpy(mem, word.data());
		mMap[mem] = 1;
	}

	return true;
}


bool
AosActFilterData::run(const char *&data, int &len, const AosRundataPtr &rdata)
{
	// This function uses 'mMap' to determine whether 'data' should be 
	// filtered. It uses a substring of 'data', defined by [mStartPos, mMatchedLength]
	// to search the map. 
	aos_assert_rr(data, rdata, false);
	int end_pos = mStartPos + mMatchedLength;
	aos_assert_rr(mStartPos >= 0 && end_pos <= len, rdata, false);
	if (len <= 0) return true;

	mLock->lock();
	char c = data[end_pos];
	((char*)data)[end_pos] = 0;
	AosCharS2U8Itr_t itr = mMap.find(data);
	((char*)data)[end_pos] = c;
	if (itr == mMap.end())
	{
		// Did not find it.
		if (mMapType == eWhiteMap)
		{
			// It is a white map. Data are filtered out if they are not in the map.
			data = 0;
			len = 0;
		}
		mLock->unlock();
		return true;
	}

	mLock->unlock();

	// Found it. 
	if (mMapType == eBlackMap)
	{
		// It should be filtered out.
		data = 0;
		len = 0;
		return true;
	}

	// it is accepted.
	return true;
}


bool	
AosActFilterData::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return true;
}


AosActionObjPtr
AosActFilterData::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActFilterData(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

