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
// 2012/12/15 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldIILMap.h"


AosDataFieldIILMap::AosDataFieldIILMap(const bool reg)
:
AosDataField(AosDataFieldType::eIILMap, AOSDATAFIELDTYPE_IILMAP, reg),
mHaveDefault(false),
mEmptyKey(false)
{
}


AosDataFieldIILMap::AosDataFieldIILMap(const AosDataFieldIILMap &rhs)
:
AosDataField(rhs),
mHaveDefault(rhs.mHaveDefault),
mIILName(rhs.mIILName),
mIILMap(rhs.mIILMap),
mEmptyKey(rhs.mEmptyKey),
mDataType(rhs.mDataType)
{
}


AosDataFieldIILMap::~AosDataFieldIILMap()
{
}


bool
AosDataFieldIILMap::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	// 'def' format:
	// 	<field ...
	// 		AOSTAG_IILNAME="xxx"					// Mandatory
	// 		AOSTAG_NEEDSPLIT="true|false"			// default: false
	// 		AOSTAG_SEPARATOR="xxx"					// optional
	// 		AOSTAG_NEEDSWAP="true|false"			// default: false 
	// 		AOSTAG_USE_KEY_AS_VALUE="true|false"	// default: false 
	// 		empty_key="true|false"					// default: false, Chen Ding, 2014/07/31
	// 		...>
	// 	</field>
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	//jackie 2014/01/04
	/*
	AosXmlTagPtr dft = def->getFirstChild("default");
	if (dft)
	{
		mHaveDefault = true;
		mDftValue = dft->getNodeText();
		mDftValue.normalizeWhiteSpace(true, true);  // trim
	}
	*/

	mIILName = def->getAttrStr(AOSTAG_IILNAME);
	if (mIILName == "")
	{
		AosSetErrorU(rdata, "missing iil name:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString datatype = def->getAttrStr("datatype", "string");
	mDataType = AosDataType::toEnum(datatype);
	bool need_split = def->getAttrBool(AOSTAG_NEEDSPLIT, false);
	OmnString sep = def->getAttrStr(AOSTAG_SEPARATOR);
	bool need_swap = def->getAttrBool(AOSTAG_NEEDSWAP, false);
	bool use_key_as_value = def->getAttrBool(AOSTAG_USE_KEY_AS_VALUE, false);
	
	mIILMap = AosIILEntryMapMgr::retrieveIILEntryMap(
		mIILName, need_split, sep, need_swap, use_key_as_value, rdata);
	aos_assert_r(mIILMap, false);

	mIILMap->readlock();
	u32 size = mIILMap->size();
	mIILMap->unlock();

	if (size == 0) 
	{
		OmnScreen << "map size is 0, iilname:" << mIILName << endl;
		//OmnAlarm << "map size is 0, iilname:" << mIILName << enderr;
	}

	mEmptyKey = def->getAttrBool("empty_key", false);
	if (mEmptyKey)
	{
		aos_assert_r(size == 1, false);
	
		mIILMap->readlock();
		map<OmnString, OmnString>::iterator itr = mIILMap->find("__null_entry__");
		if (itr == mIILMap->end())
		{
			mIILMap->unlock();
			OmnAlarm << "error" << enderr;
			return false;
		}
		else 
		{
			mEmptyKeyValue = itr->second;
			mIILMap->unlock();
		}
	}

	return true;
}


bool
AosDataFieldIILMap::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(record, false);
	aos_assert_r(mIILMap, false);

	// Chen Ding, 2014/07/31
	// It supports empty key. If there are no key fields, it always return
	// the value "__null_entry__". This means that the IIL will have at 
	// most one entry. This is needed for the following special case:
	// 	CREATE MAP mymap ON mytable
	// 	KEYS ()
	// 	VALUE xxx;
	
	if (mEmptyKey)
	{
		value.setStr(mEmptyKeyValue);
		return true;
	}
		
	AosValueRslt field_rslt;
	bool rslt = record->getFieldValue(mValueFromFieldIdx, field_rslt, false, rdata);
	aos_assert_r(rslt, false);

	OmnString key = field_rslt.getStr();
	key.normalizeWhiteSpace(true, true);  // trim

	mIILMap->readlock();
	map<OmnString, OmnString>::iterator itr = mIILMap->find(key);
	if (itr != mIILMap->end())
	{
		OmnString vv = itr->second;
		mIILMap->unlock();

		if (vv.length() == 1 && vv[0] == '\b')
		{
			value.setNull();
		}
		else if (mDataType == AosDataType::eString)
		{
			value.setStr(vv);
		}
		else if (mDataType == AosDataType::eNumber)
		{
			value.setDouble(atof(vv.data()));
		}
	}
	else
	{
		mIILMap->unlock();
		OmnScreen << "Failed to find the primary key from joining table: " << key << endl;
		if (mDftValue.getStr() != "")
		{
			value = mDftValue;
		}
		else
		{
			//value.reset();
			value.setNull();
		}
	}

	return true;
}


AosDataFieldObjPtr 
AosDataFieldIILMap::clone(AosRundata *rdata) const
{
	AosDataFieldIILMap * field = OmnNew AosDataFieldIILMap(*this);
	AosIILEntryMapPtr iil_map = field->mIILMap;
	if (!iil_map)
	{
		AosSetErrorU(rdata, "missing iilmap:");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;	
	}
	return field;
}


AosDataFieldObjPtr
AosDataFieldIILMap::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldIILMap * field = OmnNew AosDataFieldIILMap(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}


// Chen Ding, 2014/07/31
bool	
AosDataFieldIILMap::needValueFromField()
{
	return !mEmptyKey;
}

