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
// 06/20/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActGroupbyProc.h"

#include "Alarm/Alarm.h"
#include "Conds/Filter.h"
#include "DataBlob/DataBlob.h"
#include "DataRecord/DataRecord.h"
#include "DataField/DataField.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosActGroupbyProc::AosActGroupbyProc(const bool flag)
:
AosSdocAction(AOSACTTYPE_GROUPBYPROC, AosActionType::eGroupbyProc, flag)
{
}


AosActGroupbyProc::~AosActGroupbyProc()
{
}


bool	
AosActGroupbyProc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


/*
bool
AosActGroupbyProc::run(const AosDataBlobObjPtr &table, const AosRundataPtr &rdata)
{
	// This function combines records in table based on the given criteria. 
	// For example, if the records are in the following format:
	// 		[phonenumber, call-duration]
	// 		[phonenumber, call-duration]
	// 		[phonenumber, call-duration]
	// 		[phonenumber, call-duration]
	//
	// We can combine all the records that have the same phone number into 
	// one by adding their call-duration values.
	aos_assert_rr(table, rdata, false);

	// 1. Sort the table as needed
	if (table->isSorted()) table->sort();

	// 2. Do the group-by operation. 
	bool rslt = false;
	switch (mOpr)
	{
	case eSum:
		 rslt = groupBySum(data, rdata);
		 break;

	case eMaximum:
		 rslt = groupByMax(data, rdata);
		 break;

	case eMinimum:
		 rslt = groupByMin(data, rdata);
		 break;

	case eAverage:
		 rslt = groupByAverage(data, rdata);
		 break;

	default:
		 AosSetErrorU(rdata, "invalid_opr") << ": " << mOpr << enderr;
		 rslt = false;
	}

	return rslt;
}
*/


AosActionObjPtr
AosActGroupbyProc::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActGroupbyProc(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosActGroupbyProc::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	aos_assert_rr(conf, rdata, false);
	mCondRecordPicker = AosGroupbyOpr::toEnum(conf->getAttrStr(AOSTAG_COND_RECORD_PICKER));
	if (!AosGroupbyOpr::isValid(mCondRecordPicker))
	{
		mCondRecordPicker = AosGroupbyOpr::eNone;
	}

	mUncondRecordPicker = AosGroupbyOpr::toEnum(conf->getAttrStr(AOSTAG_UNCOND_RECORD_PICKER));
	if (!AosGroupbyOpr::isValid(mUncondRecordPicker))
	{
		mUncondRecordPicker = AosGroupbyOpr::eNone;
	}

	mGroupStartPos = conf->getAttrInt(AOSTAG_GROUP_STARTPOS, -1);
	if (mGroupStartPos < 0)
	{
		AosSetErrorU(rdata, "invalid_group_startpos:") << conf->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mGroupIdLen = conf->getAttrInt(AOSTAG_GROUPID_LEN, -1);
	if (mGroupIdLen < 0)
	{
		AosSetErrorU(rdata, "invalid_groupid_len:") << conf->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr tag = conf->getFirstChild(AOSTAG_FILTER);
	if (tag)
	{
		try
		{
			mFilter = OmnNew AosFilter(tag, rdata);
		}

		catch (...)
		{
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	// Create the elements
	AosXmlTagPtr elems = conf->getFirstChild(AOSTAG_ELEMENTS);
	AosXmlTagPtr elem = elems->getFirstChild();
	while (elem)
	{
		AosGroupFieldPtr ee = AosGroupField::createFieldStatic(elem, rdata);
		if (!ee)
		{
			AosSetErrorU(rdata, "invalid_elem:") << elem->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		mFields.push_back(ee);
		elem = elems->getNextChild();
	}
	return true;
}

bool
AosActGroupbyProc::processRecord(const AosRundataPtr &rdata)
{
	// 1. Check whether it is in the same group.
	aos_assert_rr(mCrtGroupId && mCrtGroupidLen > 0, rdata, false);	
	mNumRecordsProcessed++;
	checkGroup(rdata);

	// 2. Process unconditional group operators
	// switch (mUncondRecordPicker)
	// {
	// case AosGroupbyOpr::eFirst:
	// 	 break;
	// 
	// case AosGroupbyOpr::eLast:
	// 	 mCrtRecord.assign(data, data_len);
	// 	 break;
	//  
	// default:
	// 	 break;
	// }

	// 3. Filter the record as needed.
	if (mFilter)
	{
		rdata->setCharPtr(0, mCrtRecordChar, mRecordLen);
		if (mFilter->filterData(mCrtRecordChar, mRecordLen, true, rdata))
		{
			// The record is filtered
			if (mNumRecordsProcessed == 1)
			{
				mPreGroupId = 0;
				mPreGroupidLen = 0;
				mNumRecordsProcessed = 0;
			}
			else
			{
				mNumRecordsProcessed--;
			}
			return true;
		}
	}

	// 3. Process the conditional group operators.
	// switch (mCondRecordPicker)
	// {
	// case AosGroupbyOpr::eFirstCond:
	// 	 if (!mFirstRecordCopied)
	// 	 {
	// 	 	 mCrtRecord.assign(data, data_len);
	// 		 mFirstRecordCopied = true;
	// 	 }
	// 	 break;
	// 
	// case AosGroupbyOpr::eLastCond:
	//	 mCrtRecord.assign(data, data_len);
	//	 break;
	// 
	// default:
	// 	 break;
	// }

	// 4. The record passed the filtering. Need to process the record by mFields.
	bool continue_update;
	for (u32 i=0; continue_update && i<mFields.size(); i++)
	{
		bool rslt = mFields[i]->updateData(mRecord, continue_update, rdata);
		if (!rslt) 
		{
			AosSetErrorU(rdata, "failed_update_data");
			OmnAlarm << rdata->getErrmsg() << enderr;
		}
	}

	return true;
}


bool
AosActGroupbyProc::checkGroup(const AosRundataPtr &rdata)
{
	/* Commented out by Chen Ding, 07/19/2012
	// Group has a "Group ID", which is a substring of the current record'. This function
	// checks whether it has valid value. If not, it returns true. Otherwise, 
	// it checks whether it starts a new group.
	aos_assert_rr(mCrtGroupId && mCrtGroupidLen > 0, rdata, false);
	aos_assert_rr(mNumRecordsProcessed > 0, rdata, false);

	if (!mPreGroupId || mPreGroupidLen <= 0) 
	{
		mPreGroupId = mCrtGroupId;
		mPreGroupidLen = mCrtGroupidLen;
		aos_assert_rr(mNumRecordsProcessed == 1, rdata, false);
		return true;
	}

	if (!isSameGroup(rdata))
	{
		// The current group has finished. Normally, this requires generating 
		// a record and reset the operator.
		for (u32 i=0; i<mFields.size(); i++)
		{
			mFields[i]->saveAndClear();
			mFields[i]->setRecordByPrevValue(mRecord.getPtr());
		}

		mTable->rewriteNextRecord(*mRecord, false, rdata);
		mPreGroupId = mCrtGroupId;
		mPreGroupidLen = mCrtGroupidLen;
		mNumRecordsProcessed = 1;
	}
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


