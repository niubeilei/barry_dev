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
//	mLastSeqno, mLastFile and mWritePos have not inited.
//
// Modification History:
// Tom 06/23/2011 
////////////////////////////////////////////////////////////////////////////
#include "SeLogSvr/SeLog.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "ReliableFile/ReliableFile.h"
#include "SearchEngine/DocMgr.h"
#include "SEUtil/SeTypes.h"
#include "SeLogSvr/SeLogEntry.h"
#include "SeLogSvr/SeLogEntryVersion.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "SeLogUtil/LogNames.h"
#include "Security/SecurityMgr.h"
#include "Util/StrSplit.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "UtilHash/HashedObj.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/AccessRcd.h"
#include  "SEUtil/IILName.h"

	
static bool sgFilesSanityCheck = true;

AosSeLog::AosSeLog()
:
AosHashedObj("", AosHashedObj::eInvalid),
mNumFiles(0),
mLastSeqno(0),
mLogType(AosLogType::eInvalid),
mMaxLogLength(eDftMaxLogLength),
mModuleId(0),
mLock(OmnNew OmnMutex(true)),
mWritePos(0),
mNumRecord(0),
mErrid(AosErrmsgId::eInvalid),
mCreationSec(0),
mMdlLogSvr(0),
mHashLogInitFlag(false)
{
	//OmnScreen << "SeLog created: " << this << endl;
}


AosSeLog::AosSeLog(
		const OmnString &key,
		const AosErrmsgId::E errid)
:
AosHashedObj(key, AosHashedObj::eInvalid),
mNumFiles(0),
mLastSeqno(0),
mLogType(AosLogType::eInvalid),
mMaxLogLength(eDftMaxLogLength),
mModuleId(0),
mLock(OmnNew OmnMutex(true)),
mWritePos(0),
mNumRecord(0),
mErrid(errid),
mCreationSec(OmnGetSecond()),
mMdlLogSvr(0),
mHashLogInitFlag(false)
{
	//OmnScreen << "SeLog created: " << this << endl;
}


AosSeLog::AosSeLog(AosBuff &buff)
:
AosHashedObj("", AosHashedObj::eInvalid),
mNumFiles(0),
mLastSeqno(0),
mLogType(AosLogType::eNorm),
mMaxLogLength(eDftMaxLogLength),
mModuleId(0),
mLock(OmnNew OmnMutex(true)),
mWritePos(0),
mNumRecord(0),
mErrid(AosErrmsgId::eInvalid),
mCreationSec(0),
mMdlLogSvr(0),
mHashLogInitFlag(false)
{
	// This function creates a new SeLog. The SeLog definition
	// is stored in 'buff', which is in the following format:
	// 		pctr_objid 
	// 		logname
	// 		num of files
	// 		last seqno
	// 		log type
	// 		max loglength
	// 		aging...
	mPctrObjid = buff.getOmnStr("");
	mLogName = buff.getOmnStr("");
	mNumFiles = buff.getInt(-1);
	mLastSeqno = buff.getU32(0);
	mLogType = (AosLogType::E)buff.getChar(0);
	mMaxLogLength  = buff.getInt(-1);
	mModuleId  = buff.getInt(-1);

	// Chen Ding, 12/06/2011
	mLogEntry = AosSeLogEntry::getLogEntry(mMaxLogLength, buff, mLogType);
	mLogEntry->setIILName(mIILName);

	mAging = AosLogAging::getAging(buff);
	aos_assert(mAging && mLogEntry);
	//OmnScreen << "SeLog created: " << this << endl;
}


bool
AosSeLog::initHashSeLog(const AosRundataPtr &rdata)
{
	mMdlLogSvr = AosPhyLogSvr::getMdlLogSvr(mModuleId);
	aos_assert_r(mMdlLogSvr, false);
	bool rslt = init(rdata);
	if (!rslt)
	{
		OmnAlarm << "selog init failed: " << enderr;
		return false;
	}
	aos_assert_r(checkSiteId(rdata), false);
	aos_assert_r(mLogEntry, false);
	OmnString iilname = AosIILName::composeLogListingIILName(
			        	rdata->getSiteid(), mPctrObjid, mLogName);
	aos_assert_r(iilname != "", false);
	mLogEntry->setIILName(iilname);
	return true;
}

AosSeLog::AosSeLog(
	const OmnString &key,
	const OmnString &pctr_objid,
	const OmnString &log_name,
	const int &moduleId,
	const AosRundataPtr &rdata)
:
AosHashedObj(key, AosHashedObj::eInvalid),
mPctrObjid(pctr_objid),
mLogName(log_name),
mNumFiles(0),
mLastSeqno(0),
mLogType(AosLogType::convertLognameToLogType(log_name)),
mMaxLogLength(eDftMaxLogLength),
mModuleId(moduleId),
mLock(OmnNew OmnMutex(true)),
mWritePos(0),
mNumRecord(0),
mErrid(AosErrmsgId::eInvalid),
mCreationSec(0),
mMdlLogSvr(0),
mHashLogInitFlag(false)
{
	// This constructor is build for root_ctnr.
	//
	//OmnScreen << "SeLog created: " << this << ":" << mWritePos << endl;
	mMdlLogSvr = AosPhyLogSvr::getMdlLogSvr(mModuleId);
	aos_assert(mMdlLogSvr);
	bool rslt = init(rdata);
	if (!rslt)
	{
		OmnAlarm << "Selog init failed: " << mKey << enderr;
		return;
	}

	// Just for add version, we will do not add log for root_ctnr.
	mLogEntry = AosSeLogEntry::getLogEntry(AosLogType::eVersion, rdata);
	//mLogEntry = AosSeLogEntry::getLogEntry(0, mPctrObjid, 
	//		mLogName, AosLogType::eVersion, rdata);

	// Create a default aging.
	mAging = AosLogAging::getAging(0, rdata);
	aos_assert(mAging && mLogEntry);
}


AosSeLog::AosSeLog(
	const OmnString &key,
	const OmnString &pctr_objid,
	const OmnString &log_name,
	const AosXmlTagPtr &ctnr_doc,
	const int &moduleId,
	const AosRundataPtr &rdata)
:
AosHashedObj(key, AosHashedObj::eInvalid),
mPctrObjid(pctr_objid),
mLogName(log_name),
mNumFiles(0),
mLastSeqno(0),
mLogType(AosLogType::convertLognameToLogType(log_name)),
mMaxLogLength(eDftMaxLogLength),
mModuleId(moduleId),
mLock(OmnNew OmnMutex(true)),
mWritePos(0),
mNumRecord(0),
mErrid(AosErrmsgId::eInvalid),
mCreationSec(0),
mMdlLogSvr(0),
mHashLogInitFlag(false)
{
	// Check whether there are definitions for the log. 
	//OmnScreen << "SeLog created: " << this << ":" << mWritePos << endl;
	mMdlLogSvr = AosPhyLogSvr::getMdlLogSvr(mModuleId);
	aos_assert(mMdlLogSvr);
	aos_assert(ctnr_doc);
	AosXmlTagPtr log_tag = ctnr_doc->getFirstChild(AOSTAG_LOG_DEFINITION);
	AosXmlTagPtr logdef_tag;
	if (log_tag)
	{
		AosXmlTagPtr manage_tag = log_tag->getFirstChild(AOSTAG_LOG_MANAGE);
		if (manage_tag)
		{
			logdef_tag = manage_tag->getFirstChild(log_name); 
		}
	}
	
	bool rslt = init(rdata);
	if (!rslt)
	{
		OmnAlarm << "selog init failed: " << mKey << enderr;
		return;
	}

	mLogEntry = AosSeLogEntry::getLogEntry(logdef_tag, mPctrObjid, mLogName, mLogType, rdata);
	mAging = AosLogAging::getAging(logdef_tag, rdata);
	aos_assert(mAging && mLogEntry);
}


bool
AosSeLog::init(const AosRundataPtr &rdata)
{
	if (mHashLogInitFlag) return true;
	mHashLogInitFlag = true;
	if (mPctrObjid == "")
	{
		AosSetError(rdata, AosErrmsgId::eObjidIsNull);
		OmnAlarm << rdata->getErrmsg() << ". Log name: " 
			<< mLogName << enderr;
		return false;
	}

	if (mLogName == "")
	{
		AosSetError(rdata, AosErrmsgId::eLogNameIsNull);
		OmnAlarm << rdata->getErrmsg() << ". Container: " 
			<< mPctrObjid << enderr;
		return false;
	}
	
	if (mLastSeqno > 0)
	{
		mLastFile = mMdlLogSvr->getLogFile(mLastSeqno, mModuleId, rdata AosMemoryCheckerArgs);
		if (!mLastFile)
		{
			AosSetError(rdata, AosErrmsgId::eFailedOpenLogFile);
			OmnAlarm << rdata->getErrmsg() << ". LastSeqno: " 
				<< mLastSeqno << enderr;
			return false;
		}

		// Retrieve the (file size-endflag) as the value of mWritePos
		mWritePos = mLastFile->getLength() - sizeof(eEndFlag);
		if (mWritePos < eBodyStart)
		{
			initLogFile(rdata);
		}
	}
	else
	{
		if (!createNewLogFile(true, rdata))
		{
			AosSetError(rdata, AosErrmsgId::eLogInitFileFailed);
			OmnAlarm << rdata->getErrmsg() << ". Container: " 
				<< mPctrObjid << enderr;
			return false;
		}
	}

	if (mIILName == "")
	{
		mIILName = AosIILName::composeLogListingIILName(
							rdata->getSiteid(), mPctrObjid, mLogName);
	}

	return true;	
}


bool
AosSeLog::checkSiteId(const AosRundataPtr &rdata)
{
	aos_assert_r(mLastFile, false);
	OmnString siteid = OmnString(mLastFile->readStr(0, 100, "", rdata.getPtr()).data());
	//OmnString siteid = mLastFile->readStr(0, 100, "", rdata);
	OmnString site_str;
	site_str << rdata->getSiteid();
	aos_assert_r(siteid == site_str,false);
	return true;
}


AosSeLog::~AosSeLog()
{
}


bool
AosSeLog::filesSanityCheck(const u32 seqno, const AosRundataPtr &rdata)
{
	// This sanity check ensures mFiles[seqno] is null.
	if (!sgFilesSanityCheck) return true;
	return true;
}


bool
AosSeLog::createNewLogFile(
		const bool first,
		const AosRundataPtr &rdata)
{
	// Whenever a new log file is created, this function should be
	// called. This function is responsible for setting up the
	// following:
	// 	1. Create a new log file
	// 	2. Update 'mLastSeqno' and 'mWritePos'
	// 	3. Save this object.
	mLastFile = mMdlLogSvr->createNewLogFile( 
						mLastSeqno, mPctrObjid, mLogName, mModuleId, rdata AosMemoryCheckerArgs);
	if (!mLastFile)
	{
		AosSetError(rdata, AosErrmsgId::eFailedCreateLogFile);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	initLogFile(rdata);
	mNumFiles++;
	if (first) return true;
	AosSeLogPtr thisptr(this, false);
	bool rslt = mMdlLogSvr->saveSeLog(thisptr, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosSeLog::initLogFile(const AosRundataPtr &rdata)
{
	aos_assert_r(mLastFile, false);
	mWritePos = eBodyStart;

	// Set container, logname and siteid to Header of file
	OmnString site_str;
	site_str << rdata->getSiteid();
	
	// Ketty 2012/12/05
	//mLastFile->setStr(0, site_str, false);
	//mLastFile->setStr(101, mPctrObjid, false);
	//mLastFile->setStr(201, mLogName, false);
	mLastFile->put(0, site_str.data(), site_str.length(), false, rdata.getPtr());
	mLastFile->put(101, mPctrObjid.data(), mPctrObjid.length(), false, rdata.getPtr());
	mLastFile->put(201, mLogName.data(), mLogName.length(), false, rdata.getPtr());

	// set endflg to startPos
	mLastFile->setU32(mWritePos, (u32)eEndFlag, true, rdata.getPtr());
	return true;
}


// Ketty 2013/03/20
bool 
AosSeLog::addLog(
		//const AosXmlTagPtr &trans, 
		const AosXmlTagPtr &loginfo, 
		u64 &logid,
		const AosRundataPtr &rdata)
{
	// 1. File, if not, create one.
	// 2. Current size, 
	// 3. Docid
	// 4. Modify the log with the docid
	// 5. Save the log
	//AosXmlTagPtr child;
	//if(trans->isRootTag())
	//{
	//	child = trans->getFirstChild();
	//}
	//else
	//{
	//	child = trans;   
	//}
	//AosXmlTagPtr loginfo = child->getFirstChild("loginfo");
	//aos_assert_rr(loginfo, rdata, false);
	aos_assert_rr(loginfo, rdata, false);
	aos_assert_rr(mAging, rdata, false);
	aos_assert_rr(mLogEntry, rdata, false);

	//if (loginfo->getDataLength() <= 0)
	//{
	//	AosSetError(rdata, AosErrmsgId::eInternalError);
	//	OmnAlarm << rdata->getErrmsg() 
	//		<< ". Log length invalid." << enderr;
	//	return false;
	//}

	//if (loginfo->getDataLength() > mMaxLogLength)
	if (loginfo->getDataLength() > mMaxLogLength)
	{
		AosSetError(rdata, AosErrmsgId::eLogTooLong);
		OmnAlarm << rdata->getErrmsg() << ". Length: " << loginfo->getDataLength() << enderr;
		return false;
	}

	mLock->lock();
	// Check whether it needs to create a new log file.
	AosSeLogPtr thisptr(this, false);
	if (mAging->isFull(mWritePos + loginfo->getDataLength(), thisptr, rdata))
	{
		bool rslt = createNewLogFile(false, rdata);
		if (!rslt)
		{
			OmnAlarm << rdata->getErrmsg() << enderr;
			mLock->unlock();
			return false;
		}
	}
	
	//OmnScreen << "ktttt: SeLog:"
	//		<< "; log_name:" << mLogName
	//		<< "; virtual_id:" << mModuleId
	//		<< "; last_seqno:" << mLastSeqno
	//		<< "; write_pos:" << mWritePos
	//		<< endl;

	logid = mLogEntry->appendToFile(thisptr, mLastSeqno, loginfo, mLastFile, mWritePos, rdata);
	aos_assert_rl(logid>0, mLock, false);
	mNumRecord++;
	
	if (AosPhyLogSvr::isChecking())
	{
		// OmnScreen << "Add log: " << mModuleId << " : " 
		// 	<< mLastSeqno << " : " << rdata->getSiteid()
		// 	<< " : " << mWritePos << " : " << logid << endl;
	}

	// Kttt temp. 
	//OmnNotImplementedYet;
	//AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	//resp_buff->setU8(true);
	//resp_buff->setU64(logid);
	
	// Create the response
	//OmnString ss = "<Contents><![CDATA[";
	//ss << logid << "]]></Contents>";
	//rdata->setContents(ss);
	
	mLock->unlock();
	return true;
}


// Ketty 2013/03/20
bool
AosSeLog::procRequest(
		const AosLogOpr::E opr,
		const AosXmlTagPtr &loginfo, 
		u64 &logid,
		const AosRundataPtr &rdata)
{
	switch (opr)
	{
	case AosLogOpr::eAddLog:
		 return addLog(loginfo, logid, rdata);

	case AosLogOpr::eAddVersion:
		 aos_assert_rr(mLogEntry, rdata, false);
		 if (!mLogEntry->isVersionEntry())
		 {
			 addLog(loginfo, logid, rdata);
			 AosSetError(rdata, AosErrmsgId::eLogTypeMismatch);
			 OmnAlarm << rdata->getErrmsg() << ". LogType: " << mLogEntry->getType() << enderr;
			 return false;
		 }
		 return addLog(loginfo, logid, rdata);

	case AosLogOpr::eMarkVersion:
		 aos_assert_rr(mLogEntry, rdata, false);
		 if (!mLogEntry->isVersionEntry())
		 {
			 addLog(loginfo, logid, rdata);
			 AosSetError(rdata, AosErrmsgId::eLogTypeMismatch);
			 OmnAlarm << rdata->getErrmsg() << ". LogType: " << mLogEntry->getType() << enderr;
			 return false;
		 }
		 return true;//((AosSeLogEntryVersion*)mLogEntry.getPtr())->markVersion(trans, rdata);

	default:
		 rdata->setError() << "Unrecognized operator: " 
			 << opr;
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return false;
	}

	rdata->setOk();
	return true;
}


AosHashedObjPtr 
AosSeLog::clone()
{
	AosSeLogPtr selog = OmnNew AosSeLog();
	selog->mPctrObjid = mPctrObjid;
	selog->mLogName = mLogName;
	selog->mNumFiles = mNumFiles;
	selog->mLastSeqno = mLastSeqno;
	selog->mLogType = mLogType;
	selog->mMaxLogLength = mMaxLogLength;
	selog->mLastFile = mLastFile;
	selog->mWritePos = mWritePos;
	selog->mNumRecord = mNumRecord;
	selog->mErrid = mErrid;

	if (mAging) selog->mAging = mAging->clone();
	if (mLogEntry) selog->mLogEntry = mLogEntry->clone();
	return selog.getPtr();
}


bool 
AosSeLog::reset()
{
	mPctrObjid = "";
	mLogName = "";
	mNumFiles = 0;
	mLastSeqno = 0;
	mLogType = AosLogType::eInvalid;
	mMaxLogLength = -1;
	mLastFile = 0;
	mWritePos = 0;
	mAging = 0;
	mNumRecord = 0;
	mErrid = AosErrmsgId::eNotDefined;
	if (mLogEntry) mLogEntry = 0;
	return true;
}


bool 
AosSeLog::isSame(const AosHashedObjPtr &rhs) const
{
	if (!rhs) return false;
	AosSeLog *rr = (AosSeLog *)rhs.getPtr();
	return (mPctrObjid == rr->mPctrObjid && mLogName == rr->mLogName);
}


int  
AosSeLog::doesRecordMatch(
			AosBuff &buff,
			const OmnString &key_in_buff,
			const OmnString &expected_key,
			AosHashedObjPtr &obj)
{
	// This function checks whether the current entry in 
	// buff is the same as 'expected_key'. For SeLog, 
	// both 'key_in_buff' and 'expected_key' is the key
	// for SeLog. If 'key_in_buff' is the same as 
	// 'expected_key', it is the object being searched for.
	int idx = buff.getCrtIdx();
	if (key_in_buff != expected_key) 
	{
		obj = 0;
	}
	else
	{
		// This means the current object is the SeLog we are looking
		// for. Create the object.
		if (!obj)
		{
			obj = OmnNew AosSeLog(buff);
			return buff.getCrtIdx() - idx;
		}
	}
	// Just for calulate the length of obj
	buff.getOmnStr("");
	buff.getOmnStr("");
	buff.getInt(-1);
	buff.getU32(0);
	char logtype = (AosLogType::E)buff.getChar(0);
	buff.getInt(-1);
	buff.getInt(-1);
	if (logtype == AosLogType::eVersion)
	{
		buff.getInt(-1);
	}
	buff.getChar(AosLogAging::eInvalidAging);
	buff.getU32(0);
	buff.getInt(-1);

	return buff.getCrtIdx() - idx;
}


int 
AosSeLog::setContentsToBuff(AosBuff &buff)
{
	buff.setOmnStr(mPctrObjid);
	buff.setOmnStr(mLogName);
	buff.setInt(mNumFiles);
	buff.setU32(mLastSeqno);
	buff.setChar((char)mLogType);
	buff.setInt(mMaxLogLength);
	buff.setInt(mModuleId);
	
	if (mLogType == AosLogType::eVersion)
	{
		aos_assert_r(mLogEntry, false);
		mLogEntry->setContentsToBuff(buff);
	}

	int idx = buff.getCrtIdx();
	if (!mAging)
	{
		buff.setChar((char)AosLogAging::eInvalidAging);
	}
	else
	{
		mAging->setContentsToBuff(buff);
	}
	return buff.getCrtIdx() - idx;
}


bool
AosSeLog::removeOldestFile(const AosRundataPtr &rdata)
{
	mLock->lock();
	aos_assert_r(mMdlLogSvr, false);
	bool rslt = mMdlLogSvr->removeOldestFile(mPctrObjid, mLogName, mModuleId, rdata);
	if (rslt) mNumFiles--;
	mLock->unlock();
	return rslt;
}

