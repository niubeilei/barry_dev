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
// 07/08/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVGParm.h"

#include "Alarm/Alarm.h"
#include "aos/aosReturnCode.h"
#include "alarm/Alarm.h"
#include "Random/RandomInteger.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"


AosRVGParm::AosRVGParm(const AosCliCmdTorturerPtr &cmd)
:
AosRVG(cmd)
{
}


AosRVGParm::~AosRVGParm()
{
}


AosRVGParmPtr
AosRVGParm::createInstance(
			const AosCliCmdTorturerPtr &cmd,
			const OmnXmlItemPtr &def,
			const OmnString &cmdTag, 
            OmnVList<AosGenTablePtr> &tables)
{
	AosRVGParmPtr inst = OmnNew AosRVGParm(cmd);
	if (!inst)
	{
		OmnAlarm << "Run out of memory" << enderr;
		return 0;
	}

	if (!inst->config(cmd, def, cmdTag, tables))
	{
		OmnAlarm << "Failed to create the parm: " << def->toString() << enderr;
		return 0;
	}

	return inst;
}


bool
AosRVGParm::config(const AosCliCmdTorturerPtr &cmd, 
					const OmnXmlItemPtr &def, 
					const OmnString &cmdTag,
					OmnVList<AosGenTablePtr> &tables)
{
	// 
	// 	<Parm>
	// 		<Method>
	// 		<BadValue>
	// 		<Parms>
	// 			...
	// 		</Parms>
	// 	</Parm>
	//
	if (!AosRVG::config(def, cmdTag, tables)) return false;

	OmnXmlItemPtr parms = def->getItem("MemberParms");
	if (!parms)
	{
		OmnAlarm << "Missing Parms tag: " << def->toString() << enderr;
		return false;
	}

	mBadValue = def->getStr("BadValue", "***BadValue***");
	mMethod = getMethod(def->getStr("Method", "SingleValue"));
	if (mMethod == eInvalidMethod)
	{
		OmnAlarm << "Invalid method: " << def->toString() << enderr;
		return false;
	}

	parms->reset();
	int idx = 0;
	while (parms->hasMore())
	{
		OmnXmlItemPtr parm = parms->next();
		AosRVGPtr pp = AosRVG::createRVG(cmd, parm, cmdTag, tables, false);
		if (!pp)
		{
			OmnAlarm << "Failed to create the parm: " 
				<< parm->toString() << enderr;
			return false;
		}

		u16 weight = parm->getInt("Weight", eDefaultParmWeight);

		mParms.append(pp);
		mWeights[idx++] = weight;
	}

	mIsGood = createSelector();
	return mIsGood;
}


bool
AosRVGParm::createSelector()
{
	int64_t idx[eMaxParms];
	for (int i=0; i<mParms.entries(); i++)
	{
		idx[i] = i;
	}

	mParmSelector = OmnNew AosRandomInteger(idx, 0, mWeights, 
		mParms.entries(), 100);
	aos_assert_r(mParmSelector, false);
	return true;
}


bool 
AosRVGParm::nextStr(OmnString &value,
					 const AosGenTablePtr &table,
					 const AosGenRecordPtr &record,
					 bool &isCorrect, 
					 const bool correctOnly,
					 const bool selectFromRecord,
					 AosRVGReturnCode &rcode,
					 OmnString &errmsg)
{
	switch (mMethod)
	{
	case eSingleValue:
		 return nextSingleStr(value, table, record, isCorrect, correctOnly,
		 			selectFromRecord, rcode, errmsg);
	
	default:
		 OmnAlarm << "Unrecognized Method: " << mMethod << enderr;
		 return mBadValue;
	}

	OmnShouldNeverComeToThisPoint
	return mBadValue;
}


bool 
AosRVGParm::nextSingleStr(OmnString &value,
					 const AosGenTablePtr &table,
					 const AosGenRecordPtr &record,
					 bool &isCorrect, 
					 const bool correctOnly,
					 const bool selectFromRecord,
					 AosRVGReturnCode &rcode,
					 OmnString &errmsg)
{
	aos_assert_r(mParmSelector, false);
	int idx = mParmSelector->nextInt();
	aos_assert_r(idx >= 0 && idx < mParms.entries(), false);
	mLastParm = mParms[idx];
	return mLastParm->nextStr(value, table, record, isCorrect, correctOnly,
				selectFromRecord, rcode, errmsg);
}


OmnString 
AosRVGParm::getNextValueFuncCall() const
{
	return "nextStr";
}


AosRVGParm::Method
AosRVGParm::getMethod(const OmnString &m) const
{
	if (m == "SingleValue") return eSingleValue;

	OmnAlarm << "Unrecognized method: " << m << enderr;
	return eInvalidMethod;
}


bool 
AosRVGParm::getCrtValueAsArg(OmnString &value, 
						  OmnString &decl, 
						  const u32 argIndex) const
{
	if (!mLastParm)
	{
		value = "";
		return true;
	}

	return mLastParm->getCrtValueAsArg(value, decl, argIndex);
}


bool 
AosRVGParm::newRecordAdded(const AosGenRecordPtr &newRecord) const
{
	if (!mLastParm)
	{
		OmnAlarm << "Last Parm is null" << enderr;
		return false;
	}

	return mLastParm->newRecordAdded(newRecord);
}


