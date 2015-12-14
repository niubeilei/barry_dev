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
// 03/27/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILUtil/IILLog.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"

AosIILLog::AosIILLog()
{
}


AosIILLog::~AosIILLog()
{
}


bool
AosIILLog::parse(AosBuff &buff)
{
	// All logs are in the format:
	//  transid (4 bytes)
	// 	IILID	(8 bytes)
	// 	type	(4 byte)
	// 	...
	mTransid = buff.getU32(0);
	mIILID = buff.getU64(0);
	aos_assert_r(mIILID, false);
	mLogType = (AosIILLogType::E)buff.getU32(0);
	aos_assert_r(AosIILLogType::isValid(mLogType), false);

	switch (mLogType)
	{
	case AosIILLogType::eHitAddDoc:
		 mDocid = buff.getU64(0);
		 aos_assert_r(mDocid, false);
	 	 addLog("Hit");
		 break;

	case AosIILLogType::eHitRemoveDoc:
	 	 mDocid = buff.getU64(0);
	 	 addLog("Hit");
	 	 break;
	 	
	case AosIILLogType::eStrAddValueDoc:
	 	 mValueStr = buff.getOmnStr("");
	 	 mDocid = buff.getU64(0);
	 	 mValueUnique = (bool)buff.getChar(false);
	 	 mDocidUnique = (bool)buff.getChar(false);
	 	 addLog("Str");
	 	 break;
	
	case AosIILLogType::eStrRemoveValueDoc:
	 	 mValueStr = buff.getOmnStr("");
	 	 mDocid = buff.getU64(0);
	 	 addLog("Str");
	 	 break;

	case AosIILLogType::eU64AddValueDoc:
	 	 mValueU64 = buff.getU64(0);
	 	 mDocid = buff.getU64(0);
	 	 mValueUnique = (bool)buff.getChar(false); 
	 	 mDocidUnique = (bool)buff.getChar(false);
	  	 addLog("U64");
	 	 break;

	case AosIILLogType::eU64RemoveValueDoc:
	 	 mValueU64 = buff.getU64(0);
	 	 mDocid = buff.getU64(0);
	 	 addLog("U64");
	 	 break;

	case AosIILLogType::eU64IncrementDocid:
	 	 mValueU64 = buff.getU64(0);
	 	 mDocid = buff.getU64(0);
		 mValue2U64 = buff.getU64(0);
	 	 addLog("U64");
	 	 break;

	case AosIILLogType::eStrIncrementDocid:
	 	 mValueStr = buff.getOmnStr("");
	 	 mDocid = buff.getU64(0);
	 	 addLog("Str");
	 	 break;

	case AosIILLogType::eU64UpdateKeyedValue:
	 	 mValueU64 = buff.getU64(0);
	 	 mDocid = buff.getU64(0);
	 	 mNeedUpdate = (bool)buff.getChar(false);
	 	 addLog("U64");
	 	 break;
	
	case AosIILLogType::eU64AppendManualOrder:
	 	 mDocid = buff.getU64(0);
	 	 mValueU64 = buff.getU64(0);
	  	 addLog("U64");
	 	 break;

	case AosIILLogType::eU64MoveManualOrder:
	 	 mValueU64 = buff.getU64(0);
	 	 mDocid = buff.getU64(0);
	 	 mValue2U64 = buff.getU64(0);
	 	 mDocid2 = buff.getU64(0);
	  	 addLog("U64");
	 	 break;

	case AosIILLogType::eU64SetDocid:
		 mValueU64 = buff.getU64(0);
		 mDocid = buff.getU64(0);
		 break;

	case AosIILLogType::eU64SetValueDoc:
		 mValueU64 = buff.getU64(0);
		 mValue2U64 = buff.getU64(0);
		 break;

	default:
		 OmnAlarm << "Unrecognized IIL Log Type: " << mLogType << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILLog::addLog(OmnString type)
{

	mType = type;
	AosIILLogPtr thisPtr(this, false);
	const char* temp = type.data();
	
	switch(temp[0])
	{
	case 'H':
		mHitList.push_back(thisPtr);
		break;
	
	case 'S':
		mStrList.push_back(thisPtr);
		break;
	
	case 'U':
		mU64List.push_back(thisPtr);
		break;

	default:
		 OmnAlarm << "Unrecognized Type: " << type << enderr;
		 return false;
	}

	return true;
}

bool
AosIILLog::sort()
{
	const char* temp = mType.data();
	
	switch(temp[0])
	{
	case 'H':
		stable_sort(mHitList.begin(), mHitList.end(), sort_hit);
		break;
	
	case 'S':
		stable_sort(mStrList.begin(), mStrList.end(), sort_hit);
		break;
	
	case 'U':
		stable_sort(mU64List.begin(), mU64List.end(), sort_hit);
		break;

	default:
		 OmnAlarm << "Unrecognized Type: " << mType << enderr;
		 return false;
	}
	
	return true;
}

bool
AosIILLog::exec()
{
	// By IILid get IILPtr
	// call IILMgr procIILLogs();
	// clear vector
	mHitList.clear();
	mStrList.clear();
	mU64List.clear();
	return false;	
}

	
