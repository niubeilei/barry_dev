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
#include "Conds/CondIILMap.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "IILScanner/IILValueType.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILScannerListener.h"
#include "SEInterfaces/IILScannerObj.h"
#include "SEInterfaces/IILSelector.h"
#include "SmartDoc/SmartDoc.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/HashUtil.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"


static OmnMutex sgLock;
static map<OmnString, AosCondIILStrMapPtr> sgIILMap;


AosCondIILMap::AosCondIILMap(const bool flag)
:
AosCondition(AOSCONDTYPE_IILMAP, AosCondType::eIILMap, flag),
mMapType(eInvalidMap)
{
}


AosCondIILMap::AosCondIILMap(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
AosCondition(AOSCONDTYPE_IILMAP, AosCondType::eIILMap, false),
mMapType(eInvalidMap)
{
	if (!config(def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


AosCondIILMap::~AosCondIILMap()
{
}


bool
AosCondIILMap::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
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
	
	AosXmlTagPtr cfg = def->getFirstChild("iilscanner");
	if (!cfg)
	{
		AosSetErrorU(rdata, "failed to get iilscanner cfg:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mIILScannerCfg = cfg->clone(AosMemoryCheckerArgsBegin);

	///<query iilname="xxxx">
	//</query>
	OmnString iilname = cfg->getAttrStr(AOSTAG_IILNAME, "");
	aos_assert_r(iilname != "", false);

	sgLock.lock();
	map<OmnString, AosCondIILStrMapPtr>::iterator itr = sgIILMap.find(iilname);
	if (itr != sgIILMap.end())
	{
		mIILMap = itr->second;
		sgLock.unlock();
	
		if(mIILMap->size() == 0)
		{
			AosSetErrorU(rdata, "hash map is empty:") << iilname;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;	
		}
		return true;
	}
	sgLock.unlock();

	return true;
}
	

bool
AosCondIILMap::evalCond(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosCondIILMap::evalCond(
		const AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	OmnString key = value.getStr();
	return evalCond(key, rdata);
}


bool
AosCondIILMap::evalCond(
		const char *data,
		int len,
		const AosRundataPtr &rdata)
{
	OmnString key(data, len);
	return evalCond(key, rdata);
}


bool
AosCondIILMap::evalCond(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	if(!mIILMap) 
	{
		bool rslt = createIILMap(rdata);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(mIILMap, false);

	bool found = false;
	mIILMap->mLock->lock();
	map<OmnString, OmnString>::iterator itr = mIILMap->find(value);
	OmnString map_value;
	if(itr != mIILMap->end())
	{
		found = true;	
	}
	mIILMap->mLock->unlock();
		
	if (mMapType == eWhiteMap)
	{
		return found;
	}

	return !found;
}


AosConditionObjPtr
AosCondIILMap::clone() 
{
	try
	{
		AosCondIILMap * cond = OmnNew AosCondIILMap(false);
		cond->mMapType = mMapType;
		cond->mIILMap = mIILMap;
		if(mIILMap && mIILMap->size() == 0)
		{
			OmnAlarm << "hash map is empty" << enderr;
			return 0;
		}
		
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


bool
AosCondIILMap::createIILMap(const AosRundataPtr &rdata)
{
	aos_assert_r(mIILScannerCfg, false);
	
	OmnString iilname = mIILScannerCfg->getAttrStr(AOSTAG_IILNAME, "");
	aos_assert_r(iilname != "", false);
	
	sgLock.lock();
	map<OmnString, AosCondIILStrMapPtr>::iterator itr = sgIILMap.find(iilname);
	if (itr != sgIILMap.end())
	{
		mIILMap = itr->second;
		sgLock.unlock();
		return true;
	}
	
	mIILMap = OmnNew AosCondIILStrMap();
	sgIILMap.insert(make_pair(iilname, mIILMap));
	mIILMap->mLock->lock();
	sgLock.unlock();
	
	AosXmlTagPtr selectors = mIILScannerCfg->getFirstChild(AOSTAG_SELECTORS);
	aos_assert_rl(selectors, mIILMap->mLock, false);

	AosXmlTagPtr selector = selectors->getFirstChild(AOSTAG_SELECTOR);
	aos_assert_rl(selector, mIILMap->mLock, false);

	bool rslt;
	AosIILSelector sel;
	sel.mSelectorType = AosIILEntrySelType::toEnum(selector->getAttrStr(AOSTAG_ENTRYSELECTOR));
	rslt = AosIILEntrySelType::isValid(sel.mSelectorType);
	aos_assert_rl(rslt, mIILMap->mLock, false);

	sel.mValueType = AosIILValueType::toEnum(selector->getAttrStr(AOSTAG_VALUE_TYPE));
	rslt = AosIILValueType::isValid(sel.mValueType);
	aos_assert_rl(rslt, mIILMap->mLock, false);

	sel.mFieldIdx = selector->getAttrInt(AOSTAG_FIELD_IDX, -1);
	sel.mCreateFlag = selector->getAttrBool(AOSTAG_CREATE_FLAG, false);
	if (sel.mCreateFlag)
	{
		AosXmlTagPtr tag = selector->getFirstChild(AOSTAG_DEFAULT_VALUE);
		aos_assert_rl(tag, mIILMap->mLock, false);

		//rslt = sel.mDftValue.configValue(tag, rdata.getPtr());
		aos_assert_rl(rslt, mIILMap->mLock, false);
	}

	sel.mSetDocid = selector->getAttrBool(AOSTAG_SET_DOCID, false);
	sel.mSetRundata = selector->getAttrBool(AOSTAG_SET_RUNDATA, false);

	sel.mSourceFieldIdx = selector->getAttrInt(AOSTAG_SOURCE_FIELD_IDX, -1);
	sel.mSourceSeparator = selector->getAttrStr(AOSTAG_SEPARATOR);

	AosIILScannerObjPtr scanner = AosIILScannerObj::createIILScannerStatic(0, 0, mIILScannerCfg, rdata);
	aos_assert_rl(scanner, mIILMap->mLock, false);

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	aos_assert_rl(query_context, mIILMap->mLock, false);
	
	query_context->setOpr(eAosOpr_an);
	query_context->setReverse(false);
	query_context->setStrValue("*");
	query_context->setBlockSize(100000);
	
	u64 docid;
	OmnString key;
	AosValueRslt value_rslt;
	OmnString newstr;
	bool finished = false;
	AosQueryRsltObjPtr query_rslt;
	while(1)
	{
		query_rslt = AosQueryRsltObj::getQueryRsltStatic();
		query_rslt->setWithValues(true);
		
		rslt = AosQueryColumn(iilname, query_rslt, 0, query_context, rdata);
		aos_assert_rl(rslt, mIILMap->mLock, false);
		
		while(1)
		{
			rslt = query_rslt->nextDocidValue(docid, key, finished, rdata);
			aos_assert_rl(rslt, mIILMap->mLock, false);
			
			if (finished)
			{
				break;
			}

			rslt = AosIILValueType::getValue(sel, value_rslt,
				key.data(), key.length(), docid, scanner, rdata);
			aos_assert_rl(rslt, mIILMap->mLock, false);

			newstr = value_rslt.getStr();
			mIILMap->insert(make_pair(key, newstr));
		}
		
		if (query_context->finished())
		{
			break;
		}
	}
	
	if(mIILMap->size() == 0)
	{
		mIILMap->mLock->unlock();
		AosSetErrorU(rdata, "hash map is empty:") << iilname;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;	
	}

	mIILMap->mLock->unlock();
	return true;
}

