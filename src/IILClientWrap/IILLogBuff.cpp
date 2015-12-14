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
// This class implements an array that holds a given number of entries. 
// When there are too many entries, some of the contents are flushed to
// file to prevent the oustanding logs from using too much memory.
//
// In addition, this class ensures all logs are for the same type of 
// IIL. 
//
// It holds all the entries for a given IIL only.
//
// Modification History:
// 03/27/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILClient/IILLogBuff.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "IILClient/IILLogFile.h"
#include "IILUtil/IILLogType.h"
#include "Util/OmnNew.h"
#include "Util/File.h"



AosIILLogBuff::AosIILLogBuff()
:
mIILType(eInvalid)
{
}


AosIILLogBuff::~AosIILLogBuff()
{
}


bool
AosIILLogBuff::addEntry(
		const u32 crt_pos, 
		const u32 size, 
		const OmnFilePtr &file)
{
	AosBuffPtr buff = OmnNew AosBuff(size+sizeof(u32), 0);
	char *nnn = buff->data();
	int num_bytes = file->readToBuff(crt_pos, size, nnn);
	aos_assert_r(num_bytes > 0 && (u32)num_bytes == size, false);

	// Check the tail poison
	u32 poison = *(u32 *)(&nnn[size-sizeof(u32)]);
	aos_assert_r(poison == AosIILLogFile::eTailPoison, false);
	
	// All logs are in the form:
	// 		transid		(4 bytes)
	//      iilid       (8 bytes)
	//      operation 	(4 bytes)
	//      ...
	buff->getU32(0);
	u64 iilid   = buff->getU64(0);
	AosIILLogType::E type = (AosIILLogType::E)buff->getU32(0);

	switch (type)
	{
	case AosIILLogType::eHitAddDoc:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eHit;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eHit, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eHitRemoveDoc:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eHit;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eHit, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;
		
		 // Str
	case AosIILLogType::eStrAddValueDoc:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eStr;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eStr, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;
	
	case AosIILLogType::eStrModifyValueDoc:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eStr;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eStr, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;
	
	case AosIILLogType::eStrRemoveValueDoc:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eStr;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eStr, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eStrIncrementCounter:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eStr;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eStr, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

		 // U64
	case AosIILLogType::eU64AddValueDoc:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eStr;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eU64ModifyValueDoc:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eU64RemoveValueDoc:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eU64IncrementDocid:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eU64UpdateDoc:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

		 ////have mistake
	case AosIILLogType::eU64ModifyDocid:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eU64IncrementCounter:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;
	
	case AosIILLogType::eU64UpdateDoc3:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eU64UpdateDoc1:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;
	
	case AosIILLogType::eU64UpdateDoc4:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eU64AppendManualOrder:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eU64MoveManualOrder:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eU64SwapManualOrder:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	case AosIILLogType::eU64RemoveManualOrder:
		 if (mIILType == eInvalid) 
		 {
			 mIILType = eU64;
			 mIILID = iilid;
		 }
		 else 
		 {
			 aos_assert_r(mIILType == eU64, false);
		 }
		 aos_assert_r(mIILID == iilid, false);
		 mVector.push_back(buff);
		 break;

	default:
		 OmnAlarm << "Unrecognized IIL Log Type: " << mIILType
			 << ":" << type << enderr;
		 return false;
	}

	return true;
}


bool
AosIILLogBuff::sortEntries()
{
	if (mIILType == eHit || mIILType == eU64)
	{
		stable_sort(mVector.begin(), mVector.end(), u64_sort_opr);
	}
	else if (mIILType == eStr)
	{
		stable_sort(mVector.begin(), mVector.end(), str_sort_opr);
	}
	else
	{
		OmnAlarm << "Unrecognized IIL type: " << mIILType << enderr;
		return false;
	}
	return true;
}

