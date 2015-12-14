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
// 2015/02/11: Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Sequence/Sequence.h"
#include "Util/OmnNew.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/IILName.h"


AosSequence::AosSequence()
:
mInitValue(eDefaultInitValue),
mStepLength(eDefaultStepLength),
mMaxValue(eDefaultMaxValue),
mMinValue(eDefaultMinValue),
mCacheNum(eDefaultCacheNum),
mIsCycle(false),
mIsOrder(false),
mIsBegin(true),
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mCrtValue(0),
mNum(0)
{
}


AosSequence::~AosSequence()
{
}


bool
AosSequence::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	aos_assert_r(def, false);

	OmnString otype = def->getAttrStr(AOSTAG_OTYPE);
	aos_assert_r(otype == "sequence", false);

	OmnString mName = def->getAttrStr("zky_name");
	aos_assert_r(mName != "", false);

	OmnString table_name = AosObjid::getObjidPrefixByJQLDocType(JQLTypes::eSequenceDoc);
	mIILKey = AosIILName::composeAutoIncIILKey(table_name, mName);
	mIILName = AosIILName::composeAutoIncIILName(mIILKey);

	u64 value = 0;
    bool rslt = AosGetDocid(rdata, mIILName, mIILKey, value);
	aos_assert_r(rslt, false);
	mCrtValue = (int)value;

	if (mCrtValue == 0) mCrtValue = mInitValue;

//<doc zky_objid="objid" increment_by="1,-1" start_with="0,10000000000" max_value="xxx" min_value="xxx" is_cycle="true,false" cache_num="20" is_order="true,false" />

	mStepLength = def->getAttrInt("increment_by", eDefaultStepLength);
	mInitValue = def->getAttrInt("start_with",eDefaultInitValue);
	mMaxValue = def->getAttrInt("max_value", eDefaultMaxValue);
	mMinValue = def->getAttrInt("min_value", eDefaultMinValue);
	mIsCycle = def->getAttrBool("is_cycle", false);
    mCacheNum = def->getAttrInt("cache_num", eDefaultCacheNum);
	mIsOrder = def->getAttrBool("is_order", false);

	return true; 
}


bool
AosSequence::getNextValue(
		const AosRundataPtr &rdata,
		int &value)
{
	mLockRaw->lock();
	if(mNum <= 0)
	{
		u64 value = 0;
		bool rslt = AosIncrementKeyedValue(
			mIILName, mIILKey, value, false,
			mStepLength * mCacheNum, mInitValue, true, rdata);
		aos_assert_rl(rslt, mLockRaw, false);
		mCrtValue = (int)value;
		mNum = mCacheNum;
	}
	else
	{
		mCrtValue++;
	}
	aos_assert_rl(mNum > 0, mLockRaw, 0);
	//aos_assert_rl(mCrtValue > 0, mLock, 0);

	//mCrtValue++;
	value = mCrtValue;
	mNum--;
	mLockRaw->unlock();

	return true;
}


bool
AosSequence::getCrtValue(
		const AosRundataPtr &rdata,
		int &value)
{
	mLockRaw->lock();
	value = mCrtValue;
	mLockRaw->unlock();
	return true;
}
/*
bool
AosSequence::getNextValue(i32 &value)
{
	mLockRaw->lock();
	if (mNum <= 0)
	{
		AosRundataPtr rdataPtr(rdata, false);
		bool rslt = AosIncrementKeyedValue(mIILName, mIILKey,
			mCrtValue, false, mIncValue * mCacheNum, mInitValue, true, 0);  
		aos_assert_rl(rslt, mLockRaw, false);
		mNum = mCacheNum;
	}

	value = mCrtValue++;
	mNum--;

	return true;




	if(mIsBegin)
	{
		value = mInitValue;
		mCrtValue = mInitValue;
		mIsBegin = false;
		mLockRaw->unlock();
		return true;
	}
	else
	{
		i32 tempValue = mCrtValue;
    	//wo should juge the sequence is ++ or --
    	tempValue = tempValue + mStepLength;
    	if(mStepLength > 0)
    	{
    		if(tempValue > mMaxValue)
        	{
        		if(mIsCycle)
	        	{
	        		value = mInitValue;
		    		mCrtValue = mInitValue;
	        		mLockRaw->unlock();
					return true; 
	        	} 
	    		mLockRaw->unlock();
				return false;
        	}
    	}
    	else
    	{
			if(tempValue < mMinValue)
	  		{
	    		if(!mIsCycle) 
				{
					mLockRaw->unlock();
					return false;
				}
            	value = mInitValue;
		    	mCrtValue = mInitValue;
		    	mLockRaw->unlock();
				return true;
	    	}
    	}
		value = tempValue;
    	mCrtValue = tempValue;
		mLockRaw->unlock();
    	return true;
	}
}
*/
/*
bool
AosSequence::getCrtValue(i32 &value)
{
	mLockRaw->lock();
	if(mOver)
	{
		mLockRaw->unlock();
		return false;
	}
    i32 tempValue = mCrtValue + mStepLength;
    if(mStepLength > 0)
    {
    	if(tempValue > mMaxValue)
      	{
        	if(mIsCycle)
         	{
            	value = mCrtValue;
	        	mCrtValue = mInitValue;
		    	mLockRaw->unlock();
				return true;
	     	}
			value = mCrtValue;
			mOver = true;
        	mLockRaw->unlock();
			return true;
	  	}
   	}
   	else
   	{
    	if(tempValue < mMinValue)
	  	{
	    	if(mIsCycle)
		 	{
		    	value = mCrtValue;
				mCrtValue = mMaxValue;
				mLockRaw->unlock();
				return true;
		 	}
			value = mCrtValue;
			mOver = true;
			mLockRaw->unlock();
			return true;
	  	}
   	}
   	value = mCrtValue;
   	mCrtValue = tempValue;
	mLockRaw->unlock();
   	return true;
}
*/
/*
bool
AosSequence::getCrtValue(i32 &value)
{
	return mCrtValue

	mLockRaw->lock();
    if(mIsBegin)
	{
		mLockRaw->unlock();
		return false;
	}
	i32 tempValue = mCrtValue;
	mLockRaw->unlock();
	return tempValue;
}
*/

AosSequencePtr
AosSequence::create(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	AosSequencePtr sequence = OmnNew AosSequence();
	aos_assert_r(sequence, 0);
	
	bool rslt = sequence->config(rdata, def);
	aos_assert_r(rslt, 0);
	
	return sequence;
}

