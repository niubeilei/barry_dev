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
// There are two IILs to manage versions. 
//
// Version Count IIL
// This is a string IIL. The value portion is objid and the docid portion
// is the number of versions for that doc. The IIL name is:
// 		AosIILName::composeVersionCountIILName(mPctrObjid);
//
// Version Listing IIL
// This is a string IIL. The value portion is:
// 		objid + "_" + version
// The docid portion is the logid. Its name is:
// 		AosIILName::composeVersionIILName(mPctrObjid);
//
// 09/18/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeLogSvr/SeLogEntryVersion.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "SEUtil/IILName.h"
#include "ReliableFile/ReliableFile.h"
#include "SeLogSvr/Ptrs.h"
#include "SeLogSvr/SeLog.h"
#include "SeLogUtil/VersionId.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/Buff.h"

extern int gAosLogLevel;

AosSeLogEntryVersion::AosSeLogEntryVersion()
:
AosSeLogEntry(AosLogType::eVersion)
{
	mMaxVersions = AosSeLog::eDftMaxVersions;
	mMaxEntrySize = AosSeLog::eDftMaxLogLength;
}


AosSeLogEntryVersion::AosSeLogEntryVersion(const int maxEntrySize,AosBuff &buff)
:
AosSeLogEntry(AosLogType::eVersion)
{
	mMaxEntrySize = maxEntrySize;
	if (mMaxEntrySize <= 0)
	{
		OmnAlarm << "Invalid max entry size: " << mMaxEntrySize << enderr;
		mMaxEntrySize = AosSeLog::eDftMaxLogLength;
	}
	mMaxVersions = buff.getInt(-1);
}


AosSeLogEntryVersion::AosSeLogEntryVersion(
		const AosXmlTagPtr &ctnr_doc,
		const OmnString &container,
		const OmnString &logname,
		const AosRundataPtr &rdata)
:
AosSeLogEntry(AosLogType::eVersion)
{
	mMaxVersions = AosSeLog::eDftMaxVersions;
	mMaxEntrySize = AosSeLog::eDftMaxLogLength;
	if(ctnr_doc)
	{
		mMaxVersions = ctnr_doc->getAttrInt(AOSTAG_MAX_VERSIONS, eDftMaxVersions);
		mMaxEntrySize = ctnr_doc->getAttrInt(AOSTAG_MAX_LOG_LENGTH, eDftMaxLogLength);
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


AosSeLogEntryVersion::~AosSeLogEntryVersion()
{
}


AosSeLogEntryPtr
AosSeLogEntryVersion::clone()
{
	AosSeLogEntryVersionPtr entry = OmnNew AosSeLogEntryVersion();
	entry->mRefCount = mRefCount;
	entry->mFlags = mFlags;
	entry->mEntryLength = mEntryLength;
	entry->mDoc = mDoc;
	entry->mSignature = mSignature;
	entry->mCanDeletable = mCanDeletable;
	entry->mSignatureFlag = mSignatureFlag;
	entry->mSignatureFlag = mSignatureFlag;
	entry->mMaxEntrySize = mMaxEntrySize;
	entry->mMaxVersions = mMaxVersions;
	return entry;
}


u64 
AosSeLogEntryVersion::appendToFile(
		const AosSeLogPtr &selog,
		const u32 &seqno,
		const AosXmlTagPtr &log,
		//const OmnFilePtr &file,
		const AosReliableFilePtr &file,
		u64 &writePos,
		const AosRundataPtr &rdata)
{
	// 	<request ...
	// 		AOSTAG_LOG_PCTROBJID="pctr_objid"
	// 		logname="xxx"
	//		<objdef>
	//			...
	//		</objdef>
	// 
	// There is an IIL to keep track of versioned docs. It is a numerical
	// alphabetical string IIL. The value portion is 'objid' + "_" + version.
	// The docid portion is the log entry logic.
	//
	// The transaction format is:
	// 	<trans ...>
	// 		<objdef>
	// 			<the doc>
	// 		</objdef>
	// 		...
	// 	</trans>
	aos_assert_rr(mIILName != "", rdata, 0);

	OmnString ctnr_objid = selog->getPctrObjid();
	aos_assert_rr(ctnr_objid != "", rdata, 0);
	if (!log)
	{
		AosSetError(rdata, AosErrmsgId::eMissingVersionDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosXmlTagPtr child = log->getFirstChild();
	aos_assert_r(child, false);
	OmnString objid = child->getAttrStr(AOSTAG_OBJID);
	aos_assert_rr(objid != "", rdata, 0);

	// Add the entry to the file.
	u64 logid = AosSeLogEntry::appendToFile(selog, seqno, log, file, writePos, rdata);
	if (logid == 0)
	{
		AosSetError(rdata, AosErrmsgId::eFailedSaveToLogFile);
		OmnAlarm << rdata->getErrmsg() << ". Doc: " << log->toString()
			<< ". Seqno: " <<  seqno
			<< ". FileName: " << file->getFileName() << enderr;
		return 0;
	}

	// Add the entry [objid + "_" + version, logid] to the IIL.
	OmnString iilname = AosIILName::composeVersionIILName(ctnr_objid);
	if (iilname == "")
	{
		AosSetError(rdata, AosErrmsgId::eFailedCreateIILName);
		OmnAlarm << rdata->getErrmsg() << ". Ctnr objid: " << ctnr_objid << enderr;
		return 0;
	}
	//OmnString key = composeVersionListingKey(objid, child->getAttrStr(AOSTAG_VERSION));
	if (gAosLogLevel >= 100) 
	{
		OmnScreen << "-----------------------------------------------------------------" << endl;
		OmnScreen << "IILName: " << iilname << ". Key: " << objid << ". Logid: " << logid << endl;
		OmnScreen << "-----------------------------------------------------------------" << endl;
	}
//OmnScreen << "================" << endl;
	//bool rslt = AosIILClient::getSelf()->addStrValueDoc(
	//				iilname, true, key, logid, true, false, rdata);
	bool rslt = AosIILClientObj::getIILClient()->addStrValueDocToTable(
					iilname, objid, logid, false, false, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eFailedAddValueToIIL);
		OmnAlarm << rdata->getErrmsg() << iilname << enderr;
		return 0;
	}
	
	// Modify the count in iil [objid, count]
	u64 count = 0;
	OmnString iilname1 = AosIILName::composeVersionCountIILName(ctnr_objid);
	aos_assert_rr(iilname1 != "", rdata, 0);
//OmnScreen << "================" << endl;
// Chen Ding, 02/13/2012 
// Commented out by Chen Ding
	// rslt = AosIILClient::getSelf()->incrementDocidToTable(iilname1, objid, count, 1, 0, true, rdata);
	// aos_assert_rr(rslt, rdata, 0);

	if (++count > (u64)mMaxVersions)
	{
		checkNumVersions(ctnr_objid, objid, count, rdata);
	}

	return logid;
}


bool
AosSeLogEntryVersion::checkNumVersions(
		const OmnString &ctnr_objid,
		const OmnString &objid,
		const u64 &old_count,
		const AosRundataPtr &rdata)
{
	// This function checks whether the number of versions for the doc
	// 'objid' exceeds the limit. If yes, it removes some older versions.
	// Version counts are stored in IIL.
	aos_assert_rr(mMaxVersions > 0, rdata, false);
	
	// There are too many. Need to remove some.
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_rslt->setWithValues(true);
	
	query_context->setPageSize(1);
	// Chen Ding, 03/04/2012
	// ICE_WARN, how to set page size. Need review!!!!!!!
	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(objid);
	OmnString iilname = AosIILName::composeVersionIILName(ctnr_objid);
	bool rslt = AosQueryColumn(iilname, query_rslt, 0, query_context, rdata);
	aos_assert_rr(rslt, rdata, false);
	u64 logid;
	OmnString value;
	query_rslt->reset();
	rslt = query_rslt->nextDocidValue(logid, value, rdata);
	aos_assert_rr(rslt, rdata, false);
	if (logid == 0)
	{
		AosSetError(rdata, AosErrmsgId::eFailedDeleteFromIIL); 
		OmnAlarm << "Failed removing a version!" << enderr;
		return true;
	}

	AosXmlTagPtr log = AosSeLogEntry::readFromFile(logid, rdata);
	if (log && canDelete())
	{
		rslt = deleteLog(logid, ctnr_objid, log, rdata);
		if (rslt)
		{
			// Remove the entry from the iil
//OmnScreen << "================" << endl;
			rslt = AosIILClientObj::getIILClient()->removeStrValueDoc(iilname, objid, logid, rdata);
			if (!rslt)
			{
				AosSetError(rdata, AosErrmsgId::eFailedDeleteFromIIL);
				OmnAlarm << rdata->getErrmsg() << ". IILName: " 
					<< iilname << " .Logid: " << logid << enderr;
				return false;
			}
		}
		else
		{
			AosSetError(rdata, AosErrmsgId::eFailedDeleteLog);
			OmnAlarm << rdata->getErrmsg() << ". IILName: " 
				<< iilname << " .Logid: " << logid << enderr;
			return false;
		}
	}
	return true;
}

