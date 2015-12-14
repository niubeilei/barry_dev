////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_StorageEngine_SengineFixedDispatch_h
#define AOS_StorageEngine_SengineFixedDispatch_h

#include "Rundata/Rundata.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlDoc.h"

class AosSengineFixedDispatch : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	OmnString							mId;
	u32									mModifyTime;
	u64									mFileLength;

public:
	AosSengineFixedDispatch(
			const OmnString &id,
			const u32 &time,
			const u64 &file_size)
	:
	mId(id),
	mModifyTime(time),
	mFileSize(file_size)
	{
	}

	~AosSengineFixedDispatch(){}

	friend bool operator < (const AosSengineFixedDispatchPtr &p1, const AosSengineFixedDispatchPtr &p2) 
	{
		u32 crt_time = OmnGetSecond();
		u64 one_minute_convert_size = AosStorageEngineMgr::getSelf()->getOneMinuteConvertSize(); 
		u64 p1_size = ((crt_time - p1->getModifyTime())/60 * one_minute_convert_size) + p1->getFileLength();
		u64 p2_size = ((crt_time - p2->getModifyTime())/60 * one_minute_convert_size) + p2->getFileLength();
		return (p1_size < p2_size);
	}

	u32 getModifyTime() {return mModifyTime;}
	
	u64 getFileLength() {return mFileLength;}

	OmnString getJobSizeId() {return mId;}

};


#endif
#endif
