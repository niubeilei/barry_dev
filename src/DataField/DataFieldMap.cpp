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
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldMap.h"


AosDataFieldMap::AosDataFieldMap(const bool reg)
:
AosDataField(AosDataFieldType::eMap, AOSDATAFIELDTYPE_MAP, reg)
{
}


AosDataFieldMap::AosDataFieldMap(const AosDataFieldMap &rhs)
:
AosDataField(rhs),
mDefault(rhs.mDefault)
{
	mMapInfo = rhs.mMapInfo;
	mMap = rhs.mMap;
}


AosDataFieldMap::~AosDataFieldMap()
{
}


bool
AosDataFieldMap::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr dft = def->getFirstChild("default");
	if (dft)
	{
		mDefault = dft->getNodeText();
		mDefault.normalizeWhiteSpace(true, true);  // trim
	}

	AosXmlTagPtr map = def->getFirstChild("map");
	AosXmlTagPtr entry = map->getFirstChild();
	aos_assert_r(entry, false);

	int count = 0;
	while (entry)
	{
		AosMapInfo mi;
		mi.start = entry->getAttrInt64("start", 0);
		mi.end = entry->getAttrInt64("end", 0);
		mMapInfo[count] = mi;
		mMap[count] = entry->getNodeText();
		entry = map->getNextChild();
		count++;
	}

	return true;
}


bool
AosDataFieldMap::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(record, false);
	
	AosValueRslt field_rslt;
	bool rslt = record->getFieldValue(mValueFromFieldIdx, field_rslt, false, rdata);
	aos_assert_r(rslt, false);

	OmnString vv;
	OmnString dd = field_rslt.getStr();
	int64_t uu = atoll(dd.data());
	rslt = mapData(uu, vv);
	aos_assert_r(rslt, false);
	value.setStr(vv);
	return true;
}


AosDataFieldObjPtr 
AosDataFieldMap::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldMap(*this);
}


AosDataFieldObjPtr
AosDataFieldMap::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldMap * field = OmnNew AosDataFieldMap(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}


bool
AosDataFieldMap::mapData(
		const int64_t &source,
		OmnString &dst)
{
	map<int, AosMapInfo>::iterator it;
	AosMapInfo mi;
	map<int, OmnString>::iterator it2;
	for ( it=mMapInfo.begin() ; it != mMapInfo.end(); it++ )
	{
		mi = (*it).second;
		if(source >= mi.start &&  source <= mi.end)
		{
			it2 = mMap.find((*it).first);
			if (it2 == mMap.end())
			{
		 		OmnAlarm << "have no index map : " << (*it).first << enderr;
				return false;
			}
			else
			{
				dst = it2->second;
				return true;
			}
		}
	}
	
	dst = mDefault;
	return true;
}

