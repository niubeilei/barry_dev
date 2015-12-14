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
// This class is the same as AosRVGStr except that its correct value
// will be generated from a list.  For more information about this class, 
// refer to the comments for AosRVGStr.
//
// Modification History:
// 02/06/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVGStrPair.h"

#include "Alarm/Alarm.h"
#include "aos/aosReturnCode.h"
#include "aosUtil/Alarm.h"
#include "Random/RandomSelStr.h"
#include "Random/RandomStr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"
#include <limits>

AosRVGStrPair::AosRVGStrPair(const AosCliCmdTorturerPtr &cmd)
:
AosRVGStr(cmd),
mUseSingleValuePct(eDefaultUseSingleValuePct)
{
}


AosRVGStrPair::~AosRVGStrPair()
{
}


AosRVGStrPairPtr
AosRVGStrPair::createInstance(
			const AosCliCmdTorturerPtr &cmd,
			const OmnXmlItemPtr &def,
			const OmnString &cmdTag, 
            OmnVList<AosGenTablePtr> &tables)
{
	AosRVGStrPairPtr inst = OmnNew AosRVGStrPair(cmd);
	if (!inst)
	{
		OmnAlarm << "Run out of memory" << enderr;
		return 0;
	}

	if (!inst->config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to create the parm: " << def->toString() << enderr;
		return 0;
	}

	return inst;
}


bool
AosRVGStrPair::config(const OmnXmlItemPtr &def, 
					const OmnString &cmdTag,
					OmnVList<AosGenTablePtr> &tables)
{
	// 
	// 	<Parm>
	// 		<type>
	// 		<Name>
	// 		<DataType>
	// 		<FieldName>
	// 		<KeyType>
	// 		<BadValue>
	// 		<CorrectOnly>
	// 		<StrType>
	// 		<Separator>
	// 		<PairType>
	// 		<MinValue>
	// 		<MaxValue>
	// 		<TableName>
	// 		<SingleValuePct>
	// 		...
	// 	</Parm>
	//
	if (!AosRVGStr::config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to parse parm: " << def->toString() << enderr;
		return false;
	}

	OmnXmlGetStr(mSeparator, def, "Separator", "", false);
	mPairType = def->getStr("PairType", "Digit");
	if (mPairType == "DigitPair")
	{
		mStrType = eAosStrType_DigitStr;
		mMinValue = def->getInt64("MinValue", LONG_MIN);
		// mMinValue = def->getInt64("MinValue", numeric_limits<long long>::min());
		mMaxValue = def->getInt64("MaxValue", LONG_MAX);
	}
	else if (mPairType == "StringPair")
	{
	}
	else
	{
		OmnAlarm << "Pair Type not supported: " << mPairType << enderr;
		return false;
	}

	mOperator = AosTorCondOpr_toEnum(def->getStr("Operator", "!="));
	if (mOperator == eAosTortCondOpr_Invalid)
	{
		OmnAlarm << "Invalid Operator: " << def->toString() << enderr;
		return false;
	}

	mUseSingleValuePct = def->getInt("SingleValuePct", eDefaultUseSingleValuePct);
	if (mUseSingleValuePct < 0 || mUseSingleValuePct > 100)
	{
		OmnAlarm << "Invalid SingleValuePct: " << def->toString() << enderr;
		return false;
	}

	mIsGood = createSelectors();
	return mIsGood;
}


OmnString
AosRVGStrPair::genValue()
{
	OmnString str;
	const u32 tries = 10;
	if (aos_next_pct(mUseSingleValuePct))
	{
		if (mPairType == "DigitPair")
		{
			int64_t v1 = aos_next_int64(mMinValue, mMaxValue);
			str << v1;
			return str;
		}
		else if (mPairType == "StringPair")
		{
			return mStrGen->nextStr();
		}
		else
		{
			OmnAlarm << "Unrecognized PairType: " << mPairType << enderr;
			return mBadValue;
		}
	}

	if (mPairType == "DigitPair")
	{
		int64_t v1 = aos_next_int64(mMinValue, mMaxValue);
		int64_t v2;
		switch (mOperator)
		{
		case eAosTortCondOpr_Less:
			 if (v1 == mMaxValue)
			 {
			 	if (v1 == mMinValue)
				{
					// 
					// Cannot generate a correct value
					//
					OmnAlarm << "Cannot generate correct value. "
						<< "MinValue: " << mMinValue
						<< "MaxValue: " << mMaxValue
						<< "Operator: " << mOperator << enderr;
					return mBadValue;
				}

				v1--;
			 }
			 v2 = aos_next_int64(v1+1, mMaxValue);
			 break;

		case eAosTortCondOpr_LessEqual:
			 v2 = aos_next_int64(v1, mMaxValue);
			 break;

		case eAosTortCondOpr_Equal:
			 v2 = v1;
			 break;

		case eAosTortCondOpr_LargerEqual:
			 v2 = aos_next_int64(mMinValue, v1);
			 break;
			 
		case eAosTortCondOpr_Larger:
			 if (v1 == mMinValue)
			 {
			 	if (mMinValue == mMaxValue)
				{
					// 
					// Cannot generate a correct value
					//
					OmnAlarm << "Cannot generate correct value. "
						<< "MinValue: " << mMinValue
						<< "MaxValue: " << mMaxValue
						<< "Operator: " << mOperator << enderr;
					return mBadValue;
				}
				v1++;
			 }
			 v2 = aos_next_int64(mMinValue, v1-1);
			 break;

		case eAosTortCondOpr_NotEqual:
			 if (mMinValue == mMaxValue)
			 {
			 	OmnAlarm << "Cannot generate correct value "
					<< "because MinValue and MaxValue are the same."
					<< enderr;
				return mBadValue;
			 }

			 for (u32 i=0; i<tries; i++)
			 {
			 	v2 = aos_next_int64(mMinValue, mMaxValue);
				if (v1 != v2) break;
			 }

			 for (v2=mMinValue; v2<=mMaxValue; v2++)
			 {
			 	if (v1 != v2) break;
			 }
			 break;
			 	
		default:
			 OmnAlarm << "Unrecognized Operator Type: " << mOperator << enderr;
			 return mBadValue;
		}

		str << v1 << mSeparator << v2;
		return str;
	}
	else if (mPairType == "StringPair")
	{
		OmnString v1, v2;
		switch (mOperator)
		{
		case eAosTortCondOpr_Less:
		case eAosTortCondOpr_LessEqual:
		case eAosTortCondOpr_Equal:
		case eAosTortCondOpr_LargerEqual:
		case eAosTortCondOpr_Larger:
			 {
				OmnNotImplementedYet;
			 }
			 return mBadValue;

		case eAosTortCondOpr_NotEqual:
			 for (u32 i=0; i<tries; i++)
			 {
			 	v1 = mStrGen->nextStr();	
			 	v2 = mStrGen->nextStr();	
				if (v1 != v2) break;
			 }
			 // 
			 // This should never happen
			 //
			 OmnAlarm << "Failed to generate string pair" << enderr;
			 return mBadValue;

		default:
			 OmnAlarm << "Unrecognized Operator type: " << mOperator << enderr;
			 return mBadValue;
		}
		
		str << v1 << mSeparator << v2;
		return str;
	}

	OmnAlarm << "Pair Type not supported: " << mPairType << enderr;
	return mBadValue;
}

