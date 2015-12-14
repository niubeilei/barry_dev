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
// 09/18/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeLogSvr/SeLogEntryNorm.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "SeLogSvr/Ptrs.h"
#include "SeLogSvr/SeLog.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/Buff.h"
#include "SEUtil/IILName.h"


AosSeLogEntryNorm::AosSeLogEntryNorm()
:
AosSeLogEntry(AosLogType::eNorm)
{

}

	
AosSeLogEntryNorm::AosSeLogEntryNorm(const int maxEntrySize)
:
AosSeLogEntry(AosLogType::eNorm)
{
	mMaxEntrySize = maxEntrySize;
}


AosSeLogEntryNorm::AosSeLogEntryNorm(
		const AosXmlTagPtr &ctnr_doc,
		const OmnString &container,
		const OmnString &logname,
		const AosRundataPtr &rdata)
:
AosSeLogEntry(AosLogType::eNorm)
{
	mMaxEntrySize = AosSeLog::eDftMaxLogLength;
	if (ctnr_doc)
	{
		mMaxEntrySize = ctnr_doc->getAttrInt(AOSTAG_MAX_LOG_LENGTH, AosSeLog::eDftMaxLogLength);
		if (mMaxEntrySize <= 0)
		{
			OmnAlarm << "Invalid max entry size: " << mMaxEntrySize << enderr;
			mMaxEntrySize = AosSeLog::eDftMaxLogLength;
		}
	}
	mIILName = AosIILName::composeLogListingIILName(
			rdata->getSiteid(), container, logname);
	aos_assert(mIILName != "");
}


AosSeLogEntryNorm::AosSeLogEntryNorm(
		const int maxEntrySize,
		const OmnString &iilname)
:
AosSeLogEntry(AosLogType::eNorm)
{
	mIILName = iilname;
	aos_assert(mIILName != "");
	mMaxEntrySize = maxEntrySize;
}


AosSeLogEntryNorm::~AosSeLogEntryNorm()
{
}


AosSeLogEntryPtr
AosSeLogEntryNorm::clone()
{
	AosSeLogEntryNormPtr entry = OmnNew AosSeLogEntryNorm(mMaxEntrySize, mIILName);
	entry->mRefCount = mRefCount;
	entry->mFlags = mFlags;
	entry->mEntryLength = mEntryLength;
	entry->mDoc = mDoc;
	entry->mSignature = mSignature;
	entry->mCanDeletable = mCanDeletable;
	entry->mSignatureFlag = mSignatureFlag;
	entry->mMaxEntrySize = mMaxEntrySize;
	return entry;
}

