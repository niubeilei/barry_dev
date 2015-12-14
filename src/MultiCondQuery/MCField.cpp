////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/03/11 Created by liuwei	
////////////////////////////////////////////////////////////////////////////
#include"MultiCondQuery/MCField.h"

#include "API/AosApi.h"
#include "Util/File.h"

#include <fstream>
AosMCField::AosMCField()
{
	mZeroStr = "00000000000000000000";
}

AosMCField::AosMCField(const AosXmlTagPtr &config_xml)
{
	mZeroStr = "00000000000000000000";
	config(config_xml);
}

AosMCField::~AosMCField()
{
}

bool
AosMCField::config(const AosXmlTagPtr &config_xml)
{

	//	mActions mRevActions  clear();

	//AosXmlTagPtr Actions = config_xml->get// get node Actions
	//	AosXmlTagPtr Action = config_xml->get// get node Action
	// get attr   type
	// get mPos1, mPos2 for switch, add a same one to mRevActions
	// get mPos1 for rotate, add a same one to mRevActions 
	// get mDelta for add, add a eRemove one to mRevActions
	//RevActions should be reverse order as Actions
	AosXmlTagPtr action = config_xml->getFirstChild("action");
	int i=0;
	bool found = false;
	OmnString pos1;
	OmnString pos2;
	OmnString delta;
	while(action)
	{
		mActions.resize(i+1);
		OmnString tmp = action->getAttrStr("type","");
		//u64 type = strToU64(tmp);
		//ActionType type1 = (ActionType)type;
		//mActions[i].mType = type1;
		mActions[i].mType = (ActionType)strToU64(tmp);
//		pos1 = action->getNextAttrStr("pos1",found);
//		aos_assert_r(found,false);
//		mActions[i].mPos1 = pos1.toU64(0);
		mActions[i].mPos1 = action->getAttrU64("pos1",0);
		mActions[i].mPos2 = action->getAttrU64("pos2",0);
		mActions[i].mDelta = action->getAttrU64("delta",0);
		action = config_xml->getNextChild("action");
		i++;
	}

	int size = mActions.size();
	mRevActions.resize(size);
	for(int i=size-1; i>=0; i--)
	{
		mRevActions[size-i-1].mType = mActions[i].mType;
		mRevActions[size-i-1].mPos1 = mActions[i].mPos1;
		mRevActions[size-i-1].mPos2 = mActions[i].mPos2;
		mRevActions[size-i-1].mDelta = mActions[i].mDelta;
		if(mRevActions[size-i-1].mType == eAdd)
		{
			mRevActions[size-i-1].mType = eRemove;
			continue;
		}
		if(mRevActions[size-i-1].mType == eRemove)
		{
			mRevActions[size-i-1].mType = eAdd;
		}
	}
}

u64
AosMCField::getValueByTID(const u64 &tid)
{

	u64 rslt = tid;
	for(int i = 0;i < mActions.size();i++)
	{
		switch(mActions[i].mType)
		{
			case eSwitch:
				bitSwitch(rslt, mActions[i].mPos1, mActions[i].mPos2);
				break;
			case eRotate:
				bitRotate(rslt, mActions[i].mPos1);
				break;
			case eAdd:
				tidAdd(rslt, mActions[i].mDelta);
				break;
			case eRemove:
				tidRemove(rslt, mActions[i].mDelta);
				break;
			default:
				break;
		}
	}
	return rslt;
}

u64 
AosMCField::getTIDByValue(const u64 &value)
{
	u64 rslt = value;
	for(int i=0; i<mRevActions.size(); i++)
	{
		switch(mRevActions[i].mType)
		{
			case eSwitch:
				bitSwitch(rslt, mRevActions[i].mPos1, mRevActions[i].mPos2);
				break;
			case eRotate:
				bitRotate(rslt, mRevActions[i].mPos1);
				break;
			case eAdd:
				tidAdd(rslt, mRevActions[i].mDelta);
				break;
			case eRemove:
				tidRemove(rslt, mRevActions[i].mDelta);
				break;
			default:
				break;
		}
	}
	return rslt;
}

void
AosMCField::bitSwitch(
		u64 &rslt,
		const u64 &pos1,
		const u64 &pos2)
{
	u64 pos1_bit = (rslt>>pos2) & 1ULL;
	u64 pos2_bit = (rslt>>pos1) & 1ULL;
	if(pos1_bit)
	{
		rslt |= 1<<pos1;
	}
	else
	{
		rslt &= ~(1<<pos1);
	}

	if(pos2_bit)
	{
		rslt |= 1<<pos2;
	}
	else
	{
		rslt &= ~(1<<pos2);
	}
}

void
AosMCField::bitRotate(
		u64 &rslt,
		const u64 &pos1)
{
	rslt ^= (1<<pos1);
}

void
AosMCField::tidAdd(
		u64 &rslt,
		const u64 &mDelta)
{
	rslt += mDelta;
}

void
AosMCField::tidRemove(
		u64 &rslt,
		const u64 &mDelta)
{
	rslt -= mDelta;
}

OmnString
AosMCField::toString(u64 orig_val)
{
	OmnString orig_str;
	orig_str << orig_val;
	OmnString rslt_str;
	rslt_str.setLength(20);
	u64 n = orig_str.length();
	rslt_str.replace(0, 20, mZeroStr);
	rslt_str.replace(20-n, n, orig_str);

	//strcpy(rslt_str, mZeroStr);
	//strcpy(rslt_str+(20-n), orig_str);
	
	return rslt_str;
}

u64
AosMCField::strToU64(OmnString orig)
{
	u64 rslt;
	if(orig=="eSwitch") rslt = 0;
	if(orig=="eRotate") rslt = 1;
	if(orig=="eAdd") rslt = 2;
	if(orig=="eRemove") rslt = 3;
	return rslt;
}

void 
AosMCField::setQueryConds()
{
	mOpr = selectOpr();
	mValue1 = getValue();
	mValue2 = getValue();
	changeValue();
}

u64
AosMCField::getValue()
{
	return rand()%5000+1000;
	d64 aa;
	aa = ((((u64)rand())<<33) |(((u64)rand()) << 2) | (((u64)rand()) >> 29));
	return aa;
}

void
AosMCField::changeValue()
{
	if(mValue1>mValue2)
	{
		u64 tmp = mValue1;
		mValue1 = mValue2;
		mValue2 = tmp;
	}
}

AosOpr
AosMCField::selectOpr()
{
	int opr_sel = rand()%11+1;
	//int opr_sel = 11;
	switch(opr_sel)
	{
		case 1:	//eq
			mOpr = eAosOpr_eq;
			break;
		case 2:	//gt
			mOpr = eAosOpr_gt;
			break;
		case 3:	//lt
			mOpr = eAosOpr_lt;
			break;
		case 4:	//ge
			mOpr = eAosOpr_ge;
			break;
		case 5:	//le
			mOpr = eAosOpr_le;
			break;
		case 6:	//ne
//			mOpr = eAosOpr_ne;
			selectOpr();
			break;
		case 7:	//gt_lt
			mOpr = eAosOpr_range_gt_lt;
			if(mValue1 == mValue2)mValue2+=2048;
			break;
		case 8:	//ge_lt
			mOpr = eAosOpr_range_ge_lt;
			if(mValue1 == mValue2)mValue2+=2048;
			break;
		case 9:	//ge_le
			mOpr = eAosOpr_range_ge_le;
			break;
		case 10:	//gt_le
			mOpr = eAosOpr_range_gt_le;
			if(mValue1 == mValue2)mValue2+=2048;
			break;
		case 11:
//			mOpr = eAosOpr_in;
			selectOpr();
			break;
		default:
			OmnAlarm << "====================picking opr error======================"<< enderr;
	}
	OmnScreen << "mOpr:" << AosOpr_toStr(mOpr) << endl;
	return mOpr;
}

OmnString
AosMCField::getWhereStr(const OmnString field_name)
{
	OmnString opr1;
	OmnString opr2;
	OmnString rslt;
	bool double_opr = true;
	switch(mOpr)
	{
		case eAosOpr_eq:
			opr1 << "=";
			double_opr = false;
			break;
		case eAosOpr_gt:
			opr1 << ">";
			double_opr = false;
			break;
		case eAosOpr_lt:
			opr1 << "<";
			double_opr = false;
			break;
		case eAosOpr_ge:
			opr1 << ">=";
			double_opr = false;
			break;
		case eAosOpr_le:
			opr1 << "<=";
			double_opr = false;
			break;
		case eAosOpr_ne:
			opr1 << "!=";
			double_opr = false;
			break;
//		case eAosOpr_in:
//			opr1 << "";
//			double_opr = false;
//			break;
		default:
			break;
	}
	if(!double_opr)
	{
		rslt << field_name << opr1 << mValue1; 
	}
	else
	{
		switch(mOpr)
		{
			case eAosOpr_range_gt_lt:
				opr1 << ">";
				opr2 << "<";
				break;
			case eAosOpr_range_ge_lt:
				opr1 << ">=";
				opr2 << "<";
				break;
			case eAosOpr_range_gt_le:
				opr1 << ">";
				opr2 << "<=";
				break;
			case eAosOpr_range_ge_le:
				opr1 << ">=";
				opr2 << "<=";
				break;
			default:
				break;
		}
		rslt << field_name << opr1 << mValue1 << " and " << field_name << opr2 << mValue2;
	}
	return rslt;
}

bool
AosMCField::matchCond(const u64 &value)
{
	return AosIILUtil::valueMatch(value,mOpr,mValue1,mValue2);
}
