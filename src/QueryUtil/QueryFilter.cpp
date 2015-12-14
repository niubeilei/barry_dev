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
// 2014/08/19,  Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "QueryUtil/QueryFilter.h"

#include "API/AosApi.h"
#include "IILUtil/IILUtil.h"

	
AosQueryFilterObjPtr
AosQueryFilter::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, 0);

	OmnString type_str = def->getAttrStr("type");
	FilterType type = StrToEnum(type_str);
	aos_assert_r(isValidType(type), 0);

	switch (type)
	{
	case eKeyField : return AosQueryFilterKeyField::create(def, rdata);
	case eStrKey   : return AosQueryFilterStrKey::create(def, rdata);
	case eU64Key   : return AosQueryFilterU64Key::create(def, rdata);
	case eValue    : return AosQueryFilterValue::create(def, rdata);
	default :
		 break;
	}

	return 0;

}

////////////////////////////////////////////////////////////////////////////

AosQueryFilterObjPtr
AosQueryFilterValue::clone() const
{
	AosQueryFilterValue * filter = OmnNew AosQueryFilterValue();
	filter->mOpr = mOpr;
	filter->mValue1 = mValue1;
	filter->mValue2 = mValue2;
	return filter;
}


OmnString	
AosQueryFilterValue::toXmlString()const
{
	OmnString str;
	str << "<query_filter type=\"" << EnumToStr(mFilterType) << "\" "
		<< "opr=\"" << AosOpr_toStr(mOpr) << "\">";
	str	<< "<value1>" << mValue1 << "</value1>";
	str << "<value2>" << mValue2 << "</value2>";
	str << "</query_filter>";
	return str;
}


AosQueryFilterObjPtr
AosQueryFilterValue::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, 0);

	AosQueryFilterObjPtr ff = OmnNew AosQueryFilterValue();
	AosQueryFilterValue * filter = dynamic_cast<AosQueryFilterValue *>(ff.getPtr());
	
	filter->mOpr = AosOpr_toEnum(def->getAttrStr("opr"));
	aos_assert_r(AosOpr_valid(filter->mOpr), 0);

	if (filter->mOpr == eAosOpr_an) return ff;

	AosXmlTagPtr value1 = def->getFirstChild("value1");
	aos_assert_r(value1, 0);

	OmnString vv = value1->getNodeText();
	aos_assert_r(vv.isDigitStr(false, false), 0);
	filter->mValue1 = vv.toU64(0);

	if (AosOpr_isRangeOpr(filter->mOpr))
	{
		AosXmlTagPtr value2 = def->getFirstChild("value2");
		aos_assert_r(value2, 0);

		vv = value2->getNodeText();
		aos_assert_r(vv.isDigitStr(false, false), 0);
		filter->mValue2 = vv.toU64(0);
	}
	
	return ff;
}


bool
AosQueryFilterValue::evalFilter(
		const OmnString &key,
		const u64 &value,
		const AosRundataPtr &rdata) const
{
	return AosIILUtil::valueMatch(value, mOpr, mValue1, mValue2);
}

	
bool
AosQueryFilterValue::evalFilter(
		const u64 &key,
		const u64 &value,
		const AosRundataPtr &rdata) const
{
	return AosIILUtil::valueMatch(value, mOpr, mValue1, mValue2);
}

////////////////////////////////////////////////////////////////////////////

AosQueryFilterObjPtr
AosQueryFilterStrKey::clone() const
{
	AosQueryFilterStrKey * filter = OmnNew AosQueryFilterStrKey();
	filter->mOpr = mOpr;
	filter->mValue1 = mValue1;
	filter->mValue2 = mValue2;
	if (mCond && mCondDef)
	{
		filter->mCond = mCond->clone();
		filter->mCondDef = mCondDef->clone(AosMemoryCheckerArgsBegin);
	}
	return filter;
}


OmnString	
AosQueryFilterStrKey::toXmlString()const
{
	OmnString str;
	str << "<query_filter type=\"" << EnumToStr(mFilterType) << "\" "
		<< "opr=\"" << AosOpr_toStr(mOpr) << "\">";
	if (mCondDef) str << mCondDef->toString();
	str << "<value1>" << mValue1 << "</value1>";
	str << "<value2>" << mValue2 << "</value2>";
	str	<< "</query_filter>";
	return str;
}

	
AosQueryFilterObjPtr
AosQueryFilterStrKey::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, 0);

	AosQueryFilterObjPtr ff = OmnNew AosQueryFilterStrKey();
	AosQueryFilterStrKey * filter = dynamic_cast<AosQueryFilterStrKey *>(ff.getPtr());

	AosXmlTagPtr cond_def = def->getFirstChild("cond");
	if (cond_def)
	{
		AosCondType::E type = AosCondType::toEnum(cond_def->getAttrStr(AOSTAG_ZKY_TYPE));
		if (AosCondType::isValid(type))
		{
			filter->mCond = AosConditionObj::getConditionStatic(cond_def, rdata);
			aos_assert_r(filter->mCond, 0);

			filter->mCondDef = cond_def->clone(AosMemoryCheckerArgsBegin);
			return ff;
		}
	}

	filter->mOpr = AosOpr_toEnum(def->getAttrStr("opr"));
	aos_assert_r(AosOpr_valid(filter->mOpr), 0);

	if (filter->mOpr == eAosOpr_an) return ff;

	AosXmlTagPtr value1 = def->getFirstChild("value1");
	aos_assert_r(value1, 0);

	filter->mValue1 = value1->getNodeText();
	aos_assert_r(filter->mValue1 != "", 0);	

	if (AosOpr_isRangeOpr(filter->mOpr))
	{
		AosXmlTagPtr value2 = def->getFirstChild("value2");
		aos_assert_r(value2, 0);

		filter->mValue2 = value2->getNodeText();
		aos_assert_r(filter->mValue2 != "", 0);	
	}
	
	return ff;
}


bool
AosQueryFilterStrKey::evalFilter(
		const OmnString &key,
		const u64 &value,
		const AosRundataPtr &rdata) const
{
	if (mCond)
	{
		AosValueRslt rslt(key);
		return mCond->evalCond(rslt, rdata);
	}
	return AosIILUtil::valueMatch(key, mOpr, mValue1, mValue2, false);
}

////////////////////////////////////////////////////////////////////////////

AosQueryFilterObjPtr
AosQueryFilterU64Key::clone() const
{
	AosQueryFilterU64Key * filter = OmnNew AosQueryFilterU64Key();
	filter->mOpr = mOpr;
	filter->mValue1 = mValue1;
	filter->mValue2 = mValue2;
	return filter;
}


OmnString	
AosQueryFilterU64Key::toXmlString()const
{
	OmnString str;
	str << "<query_filter type=\"" << EnumToStr(mFilterType) << "\" "
		<< "opr=\"" << AosOpr_toStr(mOpr) << "\">";
	str	<< "<value1>" << mValue1 << "</value1>";
	str << "<value2>" << mValue2 << "</value2>";
	str << "</query_filter>";
	return str;
}


AosQueryFilterObjPtr
AosQueryFilterU64Key::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, 0);

	AosQueryFilterObjPtr ff = OmnNew AosQueryFilterValue();
	AosQueryFilterValue * filter = dynamic_cast<AosQueryFilterValue *>(ff.getPtr());
	
	filter->mOpr = AosOpr_toEnum(def->getAttrStr("opr"));
	aos_assert_r(AosOpr_valid(filter->mOpr), 0);

	if (filter->mOpr == eAosOpr_an) return ff;

	AosXmlTagPtr value1 = def->getFirstChild("value1");
	aos_assert_r(value1, 0);

	OmnString vv = value1->getNodeText();
	aos_assert_r(vv.isDigitStr(false, false), 0);
	filter->mValue1 = vv.toU64(0);

	if (AosOpr_isRangeOpr(filter->mOpr))
	{
		AosXmlTagPtr value2 = def->getFirstChild("value2");
		aos_assert_r(value2, 0);

		vv = value2->getNodeText();
		aos_assert_r(vv.isDigitStr(false, false), 0);
		filter->mValue2 = vv.toU64(0);
	}
	
	return ff;
}

	
bool
AosQueryFilterU64Key::evalFilter(
		const u64 &key,
		const u64 &value,
		const AosRundataPtr &rdata) const
{
	return AosIILUtil::valueMatch(key, mOpr, mValue1, mValue2);
}

////////////////////////////////////////////////////////////////////////////

AosQueryFilterObjPtr
AosQueryFilterKeyField::clone() const
{
	AosQueryFilterKeyField * filter = OmnNew AosQueryFilterKeyField();

	filter->mSep = mSep;
	filter->mFieldIdx = mFieldIdx;
	filter->mOpr = mOpr;
	filter->mValue1 = mValue1;
	filter->mValue2 = mValue2;
	if (mCond && mCondDef)
	{
		filter->mCond = mCond->clone();
		filter->mCondDef = mCondDef->clone(AosMemoryCheckerArgsBegin);
	}
	return filter;
}


OmnString	
AosQueryFilterKeyField::toXmlString()const
{
	OmnString sep = mSep;
	AosUnConvertAsciiBinary(sep);

	OmnString str;
	str << "<query_filter type=\"" << EnumToStr(mFilterType) << "\" "
		<< "opr=\"" << AosOpr_toStr(mOpr) << "\" field_idx=\"" << mFieldIdx << "\" "
		<< AOSTAG_SEPARATOR << "=\"" << sep << "\">";
	if (mCondDef) str << mCondDef->toString();
	str << "<value1>" << mValue1 << "</value1>";
	str << "<value2>" << mValue2 << "</value2>";
	str	<< "</query_filter>";
	return str;
}

	
AosQueryFilterObjPtr
AosQueryFilterKeyField::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, 0);

	AosQueryFilterObjPtr ff = OmnNew AosQueryFilterKeyField();
	AosQueryFilterKeyField * filter = dynamic_cast<AosQueryFilterKeyField *>(ff.getPtr());

	filter->mFieldIdx = def->getAttrInt("field_idx", -1);
	aos_assert_r(filter->mFieldIdx >= 0, 0);

	filter->mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(filter->mSep != "", 0);
	AosConvertAsciiBinary(filter->mSep);

	AosXmlTagPtr cond_def = def->getFirstChild("cond");
	if (cond_def)
	{
		AosCondType::E type = AosCondType::toEnum(cond_def->getAttrStr(AOSTAG_ZKY_TYPE));
		if (AosCondType::isValid(type))
		{
			filter->mCond = AosConditionObj::getConditionStatic(cond_def, rdata);
			aos_assert_r(filter->mCond, 0);

			filter->mCondDef = cond_def->clone(AosMemoryCheckerArgsBegin);
			return ff;
		}
	}

	filter->mOpr = AosOpr_toEnum(def->getAttrStr("opr"));
	aos_assert_r(AosOpr_valid(filter->mOpr), 0);

	if (filter->mOpr == eAosOpr_an) return ff;

	AosXmlTagPtr value1 = def->getFirstChild("value1");
	aos_assert_r(value1, 0);

	filter->mValue1 = value1->getNodeText();
	aos_assert_r(filter->mValue1 != "", 0);	

	if (AosOpr_isRangeOpr(filter->mOpr))
	{
		AosXmlTagPtr value2 = def->getFirstChild("value2");
		aos_assert_r(value2, 0);

		filter->mValue2 = value2->getNodeText();
		aos_assert_r(filter->mValue2 != "", 0);	
	}
	
	return ff;
}

	
bool
AosQueryFilterKeyField::evalFilter(
		const OmnString &key,
		const u64 &value,
		const AosRundataPtr &rdata) const
{
	vector<OmnString> fields;
	int num = AosSplitStr(key, mSep.data(), fields, mFieldIdx + 2);
	aos_assert_r(num >= mFieldIdx + 1, false);

	OmnString field = fields[mFieldIdx];
	if (mCond)
	{
		AosValueRslt rslt(field);
		return mCond->evalCond(rslt, rdata);
	}
	return AosIILUtil::valueMatch(field, mOpr, mValue1, mValue2, false);
}


