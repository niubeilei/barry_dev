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
// 05/23/2012, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Groupby/GroupbyProc.h"

#include "Conds/Filter.h"
#include "Rundata/Rundata.h"
#include "Groupby/GroupbyOpr.h"
#include "Thread/Mutex.h"


AosGroupbyProc::AosGroupbyProc(
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mRecordPicker(AosGroupbyOpr::eNone),
mGroupStartPos(-1),
mGroupIdLen(-1),
mFirstRecordCopied(false),
mNumGroupsProcessed(0),
mFirstRcdInGroup(true)
{
}


AosGroupbyProc::~AosGroupbyProc()
{
}


bool 
AosGroupbyProc::config(
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(conf, rdata, false);
	mRecordPicker = AosGroupbyOpr::toEnum(conf->getAttrStr("zky_"));
	if (!AosGroupbyOpr::isValid(mRecordPicker))
	{
		mRecordPicker = AosGroupbyOpr::eNone;
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
		AosGroupFieldPtr field = AosGroupField::createFieldStatic(elem, rdata);
		if (!field)
		{
			AosSetErrorU(rdata, "invalid_elem:") << elem->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		mFields.push_back(field);
		elem = elems->getNextChild();
	}
	return true;
}


AosDataProcStatus::E
AosGroupbyProc::procRecord(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	// This class is responsible for processing groups. Group level processing
	// is broken down into two levels: 
	// 		a. Record Level 
	// 		b. Field Level
	// The record level is normally responsible for picking/dropping records. 
	// The field level is responsible for calculating values that may be used
	// to assemble a final record. 
	// In addition, group-by operations may filter records. 
	// The function returns one of the following:
	// 1. eOK: 			the record is valid and should be included as the final data
	// 2. eRejected: 	the record should be dropped.
	// 3. eError: 		some errors occurred during the processing.
	
	// 1. Ignore all the records whose group id is empty or not big enough.
	//OmnScreen << "Process groupby record" << endl;
	bool need_record = false;
	char *data = record->getData(rdata.getPtr());
	int data_len = record->getRecordLen();
	bool continue_update = true;
	if (!data[mGroupStartPos])
	{
		// The group is empty. It is ignored.
	//	OmnScreen << "group is empty!!!" << endl;
		return AosDataProcStatus::eRejected;
	}

	// Chen Ding, 06/08/2012
	if (!mGroupStartPos + mGroupIdLen > data_len)
	{
	//	OmnScreen << "group is not big enough: " << mGroupStartPos + mGroupIdLen
	//		<< ":" << data_len << endl;
		return AosDataProcStatus::eRejected;
	}

	//OmnScreen << "Process groupby record: " << data << ":" << data_len << endl;
	// 2. Check whether it is in the same group. If it is not, 
	//    finish the current group.
	mLock->lock();
	if (!isSameGroup(data, data_len))
	{
		// It is not the same group. Finish the current group and start a new group.
	//	OmnScreen << "Starting a new group: " << mCrtGroupId.data() << endl;

		// Chen Ding, 06/08/2012
		if (mNumGroupsProcessed > 0)
		{
			// need_record = (mNumGroupsProcessed > 0)?true:false;
			if (mCrtGroupId.length() <= 0)
			{
				OmnAlarm << "Internal error" << enderr;
			}
			else
			{
				need_record = true;
			}
			finishAndStartGroupLocked(data);
		}
		else
		{
			// Chen Ding, 06/09/2012
			// mNumGroupsProcessed++;
			need_record = false;
		}
		mNumGroupsProcessed = 1;
		mFirstRcdInGroup = true;
	}
	else
	{
		mNumGroupsProcessed++;
		mFirstRcdInGroup = false;
	}

	// 3. Process unconditional group operators
	AosDataProcStatus::E status = AosDataProcStatus::eRejected;
	switch (mRecordPicker)
	{
	case AosGroupbyOpr::eFirst:
		 status = AosDataProcStatus::eRejected;
		 goto copy_record;
		 break;

	case AosGroupbyOpr::eLast:
		 mCrtRecord.assign(data, data_len);
		 status = AosDataProcStatus::eRejected;
		 goto copy_record;
		 break;

	default:
		 break;
	}

	// 4. Filter the record as needed.
	if (mFilter)
	{
		// Chen Ding, 06/08/2012
		rdata->setCharPtr(0, data, data_len);
		if (mFilter->filterData(data, data_len, true, rdata))
		{
			//OmnScreen << "Record filtered: " << data << ":" << data_len << endl;
			status = AosDataProcStatus::eRejected;

			// Chen Ding, 06/08/2012
			if (mFirstRcdInGroup)
			{
				// This means that the current record should not contribute
				// to the group. Need to reset the group. 
				mCrtGroupId = "";
				mNumGroupsProcessed = 0;
				mFirstRcdInGroup = false;
			}
			else
			{
				mNumGroupsProcessed--;
			}
			goto copy_record;
		}
	}

	// 5. Process the conditional group operators.
	switch (mRecordPicker)
	{
	case AosGroupbyOpr::eFirstCond:
		 if (!mFirstRecordCopied)
		 {
		 	 mCrtRecord.assign(data, data_len);
			 mFirstRecordCopied = true;
		 }
		 status = AosDataProcStatus::eRejected;
		 goto copy_record;
		 break;

	case AosGroupbyOpr::eLastCond:
		 mCrtRecord.assign(data, data_len);
		 status = AosDataProcStatus::eRejected;
		 goto copy_record;
		 break;

	default:
		 break;
	}

	// 6. Process Fields
	for (u32 i=0; continue_update && i<mFields.size(); i++)
	{
		//OmnScreen << "Process groupby elem: " << i << endl;
		bool rslt = mFields[i]->updateData(record, continue_update, rdata);
		if (!rslt) 
		{
			AosSetErrorU(rdata, "failed_update_data");
			OmnAlarm << rdata->getErrmsg() << enderr;

			status = AosDataProcStatus::eError;
			goto copy_record;
		}
	}

	// 7. Create the record, if needed.
copy_record:
	if (need_record)
	{
		// Need to modify the record based on the collected information
		for (u32 i=0; i<mFields.size(); i++)
		{
			mFields[i]->setRecordByPrevValue(record, rdata);
		}

		// Chen Ding, 06/28/2012
		// !!!!!! This is incorrect
		// record->setRecord(mPreGroupId);
		// Chen Ding, 2013/11/29
		// record->setData(mPreGroupId, false);

		// jimodb-1301
		int status;
		record->setData(mPreGroupId.getBuffer(), mPreGroupId.length(), 0, status);
		mLock->unlock();
		return AosDataProcStatus::eOk;
	}

	mLock->unlock();
	return AosDataProcStatus::eRejected;
}


bool
AosGroupbyProc::isSameGroup(const char *record, const int len)
{
	// Group has a "Group ID", which is a substring of 'record'. This function
	// checks whether it has valid value. If not, it returns true. Otherwise, 
	// it compares the groupid with 'record'. If they are the same, return 
	// true (same group). Otherwise, it returns false.
	if (mCrtGroupId.length() == 0)
	{
		// It does not have valid value yet. 
		mCrtGroupId.assign(&record[mGroupStartPos], mGroupIdLen);
		return false;
	}

	return strncmp(mCrtGroupId.data(), &record[mGroupStartPos], mGroupIdLen) == 0;
}


bool
AosGroupbyProc::finishAndStartGroupLocked(const char *record)
{
	// The current group has finished. Normally, this requires generating 
	// a record and reset the operator.
	if (mNumGroupsProcessed > 0)
	{
		if (mCrtGroupId.length() > 0)
		{
			for (u32 i=0; i<mFields.size(); i++)
			{
				mFields[i]->saveAndClear();
			}
		}
	}

	// mNumGroupsProcessed++;
	mPreGroupId = mCrtGroupId;
	mCrtGroupId.assign(&record[mGroupStartPos], mGroupIdLen);
	return true;
}


bool
AosGroupbyProc::modifyRecord(
		const char *record,
		const int len,
		const AosRundataPtr &rdata)
{
	return true;
}


bool
AosGroupbyProc::procFinished(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mCrtGroupId.length() > 0)
	{
		finishAndStartGroupLocked(record->getData(rdata.getPtr()));
		for (u32 i=0; i<mFields.size(); i++)
		{
			mFields[i]->setRecordByPrevValue(record.getPtr(), rdata);
		}
		// Chen Ding, 06/28/2012
		// !!!!!!!!!!!!!!!! This is incorrect
		// record->setData(mPreGroupId, false);
		// Chen Ding, 2013/11/29
		//
		// jimodb-1301
		int status;
		record->setData(mPreGroupId.getBuffer(), mPreGroupId.length(), 0, status);
	}
	else
	{
		// Chen Ding, 06/28/2012
		// !!!!!!!!!!!!!!!! This is incorrect
		// record->setRecord("");	
		OmnString v = "";
		// Chen Ding, 2013/11/29
		// record->setData(v, false);

		// jimodb-1301
		int status;
		record->setData(v.getBuffer(), v.length(), 0, status);
	}
	mLock->unlock();
	return true;
}

	
AosGroupbyProcPtr
AosGroupbyProc::createGroupbyProc(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosGroupbyProcPtr proc = OmnNew AosGroupbyProc(def, rdata);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}

