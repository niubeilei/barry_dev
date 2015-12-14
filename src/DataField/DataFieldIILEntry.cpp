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
#include "DataField/DataFieldIILEntry.h"

#include "API/AosApi.h"


AosDataFieldIILEntry::AosDataFieldIILEntry(const bool reg)
:
AosDataField(AosDataFieldType::eIILEntry, AOSDATAFIELDTYPE_IILENTRY, reg),
mHaveDefault(false),
mEmptyKey(false)
{
}


AosDataFieldIILEntry::AosDataFieldIILEntry(const AosDataFieldIILEntry &rhs)
:
AosDataField(rhs),
mHaveDefault(rhs.mHaveDefault),
mIILName(rhs.mIILName),
mEmptyKey(rhs.mEmptyKey),
mDataType(rhs.mDataType)
{
}


AosDataFieldIILEntry::~AosDataFieldIILEntry()
{
}


bool
AosDataFieldIILEntry::config(
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

	mNeedSplit = def->getAttrBool(AOSTAG_NEEDSPLIT, false);
	if (mNeedSplit)
	{
		mSep = def->getAttrStr(AOSTAG_SEPARATOR);
		AosConvertAsciiBinary(mSep);
		aos_assert_r(mSep != "", false);	
	}

	mNeedSwap = def->getAttrBool(AOSTAG_NEEDSWAP, false);
	aos_assert_r(!mNeedSwap, false);

	mUseKeyAsValue = def->getAttrBool(AOSTAG_USE_KEY_AS_VALUE, false);
	
	mEmptyKey = def->getAttrBool("empty_key", false);
	if (mEmptyKey)
	{
		aos_assert_r(!mNeedSplit, false);
		aos_assert_r(!mNeedSwap, false);
		aos_assert_r(!mUseKeyAsValue, false);

		AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
		aos_assert_r(query_context, false);
		query_context->setOpr(eAosOpr_an);
		query_context->setStrValue("*");
		query_context->setBlockSize(10);
			
		AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
		query_rslt->setWithValues(true);

		bool rslt = AosQueryColumn(mIILName,
			query_rslt, 0, query_context, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(query_context->getTotalDocInIIL() == 1, false);
		aos_assert_r(query_rslt->getNumDocs() == 1, false);

		bool finished;
		OmnString key;
		u64 value;
		rslt = query_rslt->nextDocidValue(value, key, finished, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(key == "__null_entry__", false);

		mEmptyKeyValue = "";
		mEmptyKeyValue << value;
	}

	return true;
}


bool
AosDataFieldIILEntry::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(record, false);

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

	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	aos_assert_r(query_context, false);

	if (mNeedSplit)
	{
		key << mSep;
		query_context->setOpr(eAosOpr_prefix);
	}
	else
	{
		query_context->setOpr(eAosOpr_eq);
	}
		
	query_context->setStrValue(key);
	query_context->setBlockSize(10);

	rslt = AosQueryColumn(mIILName, query_rslt, 0, query_context, rdata);
	aos_assert_r(rslt, false);

	if (query_rslt->getNumDocs() <= 0)
	{
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
		return true;	
	}

	bool finished;
	u64 v;
	rslt = query_rslt->nextDocidValue(v, key, finished, rdata);
	aos_assert_r(rslt, false);

	OmnString vv;
	if (mNeedSplit)
	{
		vector<OmnString> keys;
		AosSplitStr(key, mSep.data(), keys, 10);
		aos_assert_r(keys.size() == 2, false);
		aos_assert_r(keys[0] != "" && keys[1] != "", false);
		key = keys[0];
		vv = keys[1];
	}
	else
	{
		vv << (int64_t)v;	
	}

	if (mUseKeyAsValue)
	{
		vv = key;
	}

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

	return true;
}


AosDataFieldObjPtr 
AosDataFieldIILEntry::clone(AosRundata *rdata) const
{
	AosDataFieldIILEntry * field = OmnNew AosDataFieldIILEntry(*this);
	return field;
}


AosDataFieldObjPtr
AosDataFieldIILEntry::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldIILEntry * field = OmnNew AosDataFieldIILEntry(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}


// Chen Ding, 2014/07/31
bool	
AosDataFieldIILEntry::needValueFromField()
{
	return !mEmptyKey;
}

