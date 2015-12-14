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
// 09/03/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataColComp/DataColCompMap.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"

AosDataColCompMap::AosDataColCompMap(const bool flag)
:
AosDataColComp(AOSDATACOLCOMP_MAP, AosDataColCompType::eMap, flag),
mLock(OmnNew OmnMutex())
{
}

AosDataColCompMap::AosDataColCompMap(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataColComp(AOSDATACOLCOMP_MAP, AosDataColCompType::eMap, false),
mLock(OmnNew OmnMutex())
{
	bool rslt = config(conf, rdata);
	if (!rslt) OmnThrowException(rdata->getErrmsg());
}

AosDataColCompMap::AosDataColCompMap(const AosDataColCompMap &rhs)
:
AosDataColComp(rhs),
mLock(OmnNew OmnMutex()),
mDataType(rhs.mDataType)
{
	mMap = rhs.mMap;
}

bool
AosDataColCompMap::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnString map_objid = def->getAttrStr("zky_map_objid");
	if (map_objid == "")
	{
		AosSetErrorU(rdata, "missing_map_values") << def->toString() << enderr;
		return false;
	}
	
	// The map is defined by a doc.
	AosXmlTagPtr doc = AosRetrieveDocByObjid(map_objid, false, rdata);
	if (!doc)
	{
		AosSetErrorU(rdata, "failed_retr_obj_byobjid") << map_objid << enderr;
		return false;
	}

	// 'doc' should be in the form:
	//  <doc ...>
	//      <records zky_type="char">
	//          <record AOSTAG_KEY = "" AOSTAG_VALUE = ""></record>
	//          <record AOSTAG_KEY = "" AOSTAG_VALUE = ""></record>
	//          ...
	//      </records>
	//  </doc>

	AosXmlTagPtr tags = doc->getFirstChild("records");
	if (!tags)
	{
		AosSetErrorU(rdata, "missing_map_values") << doc->toString() << enderr;
		return false;
	}

	OmnString map_key = def->getAttrStr("zky_mapkey_attr", AOSTAG_KEY);
	OmnString map_value  = def->getAttrStr("zky_mapvalue_attr", AOSTAG_VALUE);

	OmnString value_type = tags->getAttrStr(AOSTAG_TYPE);
	aos_assert_r(value_type != "", false);
	mDataType = AosDataType::toEnum(value_type); 
	if (!AosDataType::isValid(mDataType))
	{
		AosSetErrorU(rdata, "invalid_data_type") << def->toString() << enderr;
		return false;
	}
	
	AosXmlTagPtr tag = tags->getFirstChild();
	while (tag)
	{
		OmnString key = tag->getAttrStr(map_key, "");
		aos_assert_r(key != "", false);
		OmnString value = tag->getAttrStr(map_value, "");
		aos_assert_r(value != "", false);
		mLock->lock();
		mMap.insert(make_pair(key, value));
		mLock->unlock();
		tag = tags->getNextChild();
	}
	aos_assert_r(mMap.size() > 0, false);
	return true;
	
//	bool  reverse = def->getAttrBool("key_reverse", false); 
//	AosBuffPtr buff = tags->getNodeTextBinaryCopy(AosMemoryCheckerArgsBegin);
//	aos_assert_r(buff, false);
//	while (buff->getCrtIdx() < buff->dataLen())
//	{
//		OmnString key;
//		buff->getStr(key); 
//		aos_assert_r(key != "", false);
//		OmnString value;
//		value << buff->getU64(0);
//		aos_assert_r(value != "", false);
//
//		if (!reverse)
//		{
//			mLock->lock();
//			mMap.insert(make_pair(key, value));
//			mLock->unlock();
//		}
//		else
//		{
//			mLock->lock();
//			mMap.insert(make_pair(value, key));
//			mLock->unlock();
//		}
//	}
//	aos_assert_r(mMap.size() > 0, false);
//	return true;
}


AosDataColCompMap::~AosDataColCompMap()
{
}

AosDataColCompPtr
AosDataColCompMap::clone() const
{
	try
	{
		return OmnNew AosDataColCompMap(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object " << enderr;
		return 0;
	}
}

AosDataColCompPtr
AosDataColCompMap::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataColCompMap(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}

bool
AosDataColCompMap::convertToStr(
		const char *data,
		const int data_len,
		AosValueRslt &valueRslt,
		const AosRundataPtr &rdata)
{
	int kk = getValue(data);
	aos_assert_r(kk >= 0, false);
	OmnString key;
	key << kk;
	mLock->lock();
	AosStr2StrItr_t itr = mMap.find(key);
	if (itr == mMap.end())
	{
		mLock->unlock();
		OmnAlarm << "invalid" << enderr;
		return false;
	}
	mLock->unlock();
	OmnString vv = itr->second;
	aos_assert_r(vv != "", false);
	valueRslt.setStr(vv);
	return true;
}


int
AosDataColCompMap::getValue(const char *data)
{
	int kk = -1;
	switch (mDataType)
	{
	case AosDataType::eU8:
		 kk = *(u8 *) data;
		 break;
	case AosDataType::eU16:
		 kk = *(u16 *) data;
		 break;
	default:
		OmnAlarm << "Invalid data type:" << enderr;
		return -1;
	}
	return kk;
}


bool 
AosDataColCompMap::convertToInteger(
		const char *data, 
		const int data_len, 
		AosValueRslt &valueRslt,
		AosDataType::E &data_type,
		const AosRundataPtr &rdata)
{
	OmnString key(data, data_len);
	mLock->lock();
	AosStr2StrItr_t itr = mMap.find(key);
	if (itr == mMap.end())
	{
		mLock->unlock();
		OmnAlarm << "invalid" << enderr;
		return false;
	}
	mLock->unlock();
	OmnString vv = itr->second;
	aos_assert_r(vv != "", false);
	valueRslt.setStr(vv);
	data_type = mDataType; 
	return true;
}
	
//	mLock->lock();
//	char c = data[data_len];
//	((char*)data)[data_len] = 0;
//	AosStr2StrItr_t itr = mMap.find(data);
//	((char*)data)[data_len] = c;
//
//	if (itr == mMap.end())
//	{
//		mLock->unlock();
//		OmnAlarm << "invalid" << enderr;
//		return false;
//	}
//	mLock->unlock();
//	//valueRslt.setValue(itr->second.data(), itr->second.length());
//	OmnString value = itr->second;
//	int ll = itr->second.length();
//	valueRslt.setValue(value);
//	return true;


