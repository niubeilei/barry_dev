////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/06/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryContextObj.h"

#include "SEInterfaces/QueryDistrMapObj.h"
#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"


AosQueryContextObjPtr AosQueryContextObj::smQueryContextObj;

OmnString
AosQueryContextObj::GroupFilterType_toString(const GroupFilterType type)
{
	if (type == eGroupFilterKey)		  return "key";
	if (type == eGroupFilterFirstStrKey)  return "firststrkey";
	if (type == eGroupFilterSecondStrKey) return "secondstrkey";
	if (type == eGroupFilterFirstU64Key)  return "firstu64key";
	if (type == eGroupFilterSecondU64Key) return "secondu64key";

	OmnAlarm << "error GroupFilterType, type:" << type << enderr;
	return "key";
}


AosQueryContextObj::GroupFilterType
AosQueryContextObj::GroupFilterType_toEnum(const OmnString &type)
{
	if (type == "key")			return eGroupFilterKey;
	if (type == "firststrkey")	return eGroupFilterFirstStrKey; 
	if (type == "secondstrkey") return eGroupFilterSecondStrKey;
	if (type == "firstu64key")	return eGroupFilterFirstU64Key; 
	if (type == "secondu64key") return eGroupFilterSecondU64Key;

	OmnAlarm << "error GroupFilterType, typestr:" << type << enderr;
	return eGroupFilterKey;
}


AosQueryContextObj::QueryKeyGroupingType
AosQueryContextObj::QueryKeyGroupingType_toEnum(const OmnString &type)
{
	if (type == "full")  return eFullKey;
	if (type == "first") return eFirstKey;
		
	OmnAlarm << "error QueryKeyGroupingType, typestr:" << type << enderr;
	return eFullKey;
}


OmnString
AosQueryContextObj::QueryKeyGroupingType_toString(const QueryKeyGroupingType type)
{
	if (type == eFullKey)  return "full";
	if (type == eFirstKey) return "first";

	OmnAlarm << "error QueryKeyGroupingType, type:" << type << enderr;
	return "full";
}

	
AosQueryContextObj::QueryValueGroupingType
AosQueryContextObj::QueryValueGroupingType_toEnum(const OmnString &type)
{
	if (type == "first") return eFirstEntry;
	if (type == "last")	 return eLastEntry;
	if (type == "max")	 return eMaxEntry;
	if (type == "min")	 return eMinEntry;
	if (type == "sum")	 return eSum;
	if (type == "avg")	 return eAvg;	 
		
	OmnAlarm << "error QueryValueGroupingType, typestr:" << type << enderr;
	return eFirstEntry;
}

OmnString
AosQueryContextObj::QueryValueGroupingType_toString(const QueryValueGroupingType type)
{
	if (type == eFirstEntry) return "first";
	if (type == eLastEntry)	 return "last";
	if (type == eMaxEntry)	 return "max";
	if (type == eMinEntry)	 return "min";
	if (type == eSum)		 return "sum";
	if (type == eAvg)		 return "avg";
		
	OmnAlarm << "error QueryValueGroupingType, type:" << type << enderr;
	return "first";
}
	

AosQueryContextObj::SplitValueType
AosQueryContextObj::SplitValueType_toEnum(const OmnString &type)
{
	if (type == "key") return eKey;
	if (type == "keyvalue") return eKeyValue;
	if (type == "firstfiled") return eFirstField;
	if (type == "firstsecondfield") return eFirstSecondField;
		
	OmnAlarm << "error SplitValueType, type:" << type << enderr;
	return eKey;
}


OmnString
AosQueryContextObj::SplitValueType_toString(const SplitValueType type)
{
	if (type == eKey) return "key";
	if (type == eKeyValue)	 return "keyvalue";
	if (type == eFirstField)	 return "firstfiled";
	if (type == eFirstSecondField)	 return "firstsecondfield";
		
	OmnAlarm << "error SplitValueType, type:" << type << enderr;
	return "key";
}


bool
AosQueryContextObj::copyMemberData(AosQueryContextObj * const obj) const
{
	obj->mReverse = mReverse;
	obj->mIndex = mIndex;
	obj->mIILIndex = mIILIndex;
	obj->mPageSize = mPageSize;
	obj->mBlockSize = mBlockSize;
	obj->mOpr = mOpr;
	obj->mStrValue = mStrValue;	
//	obj->mU64Value = mU64Value;
	obj->mStrValue2 = mStrValue2;	
//	obj->mU64Value2 = mU64Value2;
	obj->mCrtDocid = mCrtDocid;
	obj->mCrtValue = mCrtValue;
//	obj->mCrtU64Value = mCrtU64Value;
	obj->mFinished = mFinished;
	obj->mCheckRslt = mCheckRslt;
	obj->mMoveOnly = mMoveOnly;
	obj->mNumDocChecked = mNumDocChecked;
	obj->mTotalDocInIIL = mTotalDocInIIL;
	obj->mTotalDocInRslt = mTotalDocInRslt;
	obj->mGrpFirstEntry = mGrpFirstEntry;
	obj->mGrpCmpKey = mGrpCmpKey;
	obj->mGrpCurKey = mGrpCurKey;
	obj->mGrpCmpU64Key = mGrpCmpU64Key;
	obj->mGrpCurU64Key = mGrpCurU64Key;
	obj->mGrpCrtValue = mGrpCrtValue;
	obj->mGrpCrtNum = mGrpCrtNum;
	obj->mSaperator = mSaperator;
	obj->mGrouping = mGrouping;	
	obj->mKeyGroupingType = mKeyGroupingType;
	obj->mValueGroupingType = mValueGroupingType;
	obj->mSplitValueType = mSplitValueType;
	obj->mSplitValueStr = mSplitValueStr;
	obj->mParalIILName = mParalIILName;
//	obj->mPageStart = mPageStart;
	obj->mIsAlphaNum = mIsAlphaNum;
	obj->mEstimate = mEstimate;
	obj->mQueryCursor = mQueryCursor;
	if(!mDistrMap)
	{
		obj->mDistrMap = 0;
	}
	else
	{
		if(!obj->mDistrMap)
		{
			obj->mDistrMap = AosQueryDistrMapObj::getObject();
		}
		obj->mDistrMap->clone(mDistrMap);
	}
	obj->mIILType = mIILType;
	return true;
}


bool 
AosQueryContextObj::setBackward(const AosQueryContextObjPtr &new_context)
{
	mIndex = new_context->mIndex;
	mIILIndex = new_context->mIILIndex;
	mCrtDocid = new_context->mCrtDocid;
	mCrtValue = new_context->mCrtValue;
	//mCrtU64Value = new_context->mCrtU64Value;

	mNumDocChecked = new_context->mNumDocChecked;
	mTotalDocInIIL = new_context->mTotalDocInIIL;
	mTotalDocInRslt = new_context->mTotalDocInRslt;
//	mPageStart = new_context->mPageStart;
	mIsAlphaNum = new_context->mIsAlphaNum;
	mQueryCursor = new_context->mQueryCursor;

	return true;
}


bool
AosQueryContextObj::merge(const AosQueryContextObjPtr &rhs)
{
	aos_assert_r(rhs, 0);
	
	mTotalDocInIIL += rhs->mTotalDocInIIL;
	mTotalDocInRslt += rhs->mTotalDocInRslt;

	mFinished = mFinished && rhs->mFinished;
	if (mFinished) return true;

	switch(mIILType)
	{
		case eAosIILType_Str:
		case eAosIILType_BigStr:				
			if (mCrtValue != "" && rhs->mCrtValue != "")
			{
				
				if(mCrtValue > rhs->mCrtValue)
				{
					mCrtValue = rhs->mCrtValue;
					mCrtDocid = rhs->mCrtDocid;
				}
				else if (mCrtValue == rhs->mCrtValue && mCrtDocid > rhs->mCrtDocid)
				{
					mCrtDocid = rhs->mCrtDocid;
				}
			}
			break;
		case eAosIILType_U64:				
		case eAosIILType_BigU64:				
			if (mCrtValue != "" && rhs->mCrtValue != "")
			{
				u64 val_a = mCrtValue.toU64(0);
				u64 val_b = rhs->mCrtValue.toU64(0);
				
				if(val_a > val_b)
				{
					mCrtValue = rhs->mCrtValue;
					mCrtDocid = rhs->mCrtDocid;
				}
				else if (val_a == val_b && mCrtDocid > rhs->mCrtDocid)
				{
					mCrtDocid = rhs->mCrtDocid;
				}
			}
			break;
		case eAosIILType_BigI64:				
			if (mCrtValue != "" && rhs->mCrtValue != "")
			{
				i64 val_a = mCrtValue.toI64(0);
				i64 val_b = rhs->mCrtValue.toI64(0);
				
				if(val_a > val_b)
				{
					mCrtValue = rhs->mCrtValue;
					mCrtDocid = rhs->mCrtDocid;
				}
				else if (val_a == val_b && mCrtDocid > rhs->mCrtDocid)
				{
					mCrtDocid = rhs->mCrtDocid;
				}
			}
			break;
		case eAosIILType_BigD64:				
			if (mCrtValue != "" && rhs->mCrtValue != "")
			{
				d64 val_a = mCrtValue.toD64(0);
				d64 val_b = rhs->mCrtValue.toD64(0);
				
				if(val_a > val_b)
				{
					mCrtValue = rhs->mCrtValue;
					mCrtDocid = rhs->mCrtDocid;
				}
				else if (val_a == val_b && mCrtDocid > rhs->mCrtDocid)
				{
					mCrtDocid = rhs->mCrtDocid;
				}
			}
			break;
		default:
			break;
	}
	return true;
}

void		
AosQueryContextObj::setDistrMap(vector<OmnString> &values,  
								vector<u64> &docids,
								vector<u64> &num_entries)
{
//	AosQueryDistrMapObjPtr	mDistrMap;
	if(!mDistrMap)
	{
		mDistrMap = AosQueryDistrMapObj::getObject();
	}
	
	mDistrMap->setInfo(values, docids, num_entries);
}


void		
AosQueryContextObj::setDistrMap(vector<u64> &values,  
								vector<u64> &docids,
								vector<u64> &num_entries)
{
	OmnNotImplementedYet;
	return;
}

void		
AosQueryContextObj::setDistrMap(vector<i64> &values,  
								vector<u64> &docids,
								vector<u64> &num_entries)
{
	OmnNotImplementedYet;
	return;
}

void		
AosQueryContextObj::setDistrMap(vector<d64> &values,  
								vector<u64> &docids,
								vector<u64> &num_entries)
{
	OmnNotImplementedYet;
	return;
}

