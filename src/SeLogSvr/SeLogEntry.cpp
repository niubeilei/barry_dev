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
#include "SeLogSvr/SeLogEntry.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/IILClientObj.h"
//#include "DocProc/DocProc.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "ReliableFile/ReliableFile.h"
#include "SearchEngine/DocServerCb.h"
#include "SearchEngine/DocReq.h"
#include "SeLogSvr/MdlLogSvr.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "SeLogSvr/SeLogEntryNorm.h"
#include "SeLogSvr/SeLogEntryVersion.h"
#include "SeLogUtil/LogId.h"
#include "SeLogUtil/LogType.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/Buff.h"

AosSeLogEntryPtr AosSeLogEntry::smVersionReader = OmnNew AosSeLogEntryVersion();
AosSeLogEntryPtr AosSeLogEntry::smNormReader = OmnNew AosSeLogEntryNorm();

extern int gAosLogLevel;
	
AosSeLogEntry::AosSeLogEntry(const AosLogType::E type) 
:
mRefCount(0),
mFlags(0),
mEntryLength(0),
mLogType(type),
mCanDeletable(false),
mMaxEntrySize(eDftMaxLogLength),
mSignatureFlag(false)
{
}


AosSeLogEntry::~AosSeLogEntry()
{
}


AosSeLogEntryPtr 
AosSeLogEntry::getLogEntry(const AosLogType::E type, const AosRundataPtr &rdata)
{
	switch (type)
	{
	case AosLogType::eVersion:
		 return smVersionReader;

	case AosLogType::eNorm:
		 return smNormReader;
	 
	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return 0;
}


AosSeLogEntryPtr 
AosSeLogEntry::getLogEntry(
		const int maxEntrySize,
		AosBuff &buff,
		const AosLogType::E type)
{
	try
	{
		switch (type)
		{
		case AosLogType::eVersion:
			 return OmnNew AosSeLogEntryVersion(maxEntrySize, buff);

		case AosLogType::eNorm:
			 return OmnNew AosSeLogEntryNorm(maxEntrySize);
		 
		default:
			 break;
		}
	}

	catch (...)
	{
		OmnAlarm << ". Type: " << type
			<< ". Max Entry Length: " << maxEntrySize << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


AosSeLogEntryPtr 
AosSeLogEntry::getLogEntry(
		const AosXmlTagPtr &ctnr_doc,
		const OmnString &container,
		const OmnString &logname,
		const AosLogType::E type,
		const AosRundataPtr &rdata)
{
	try
	{
		switch (type)
		{
		case AosLogType::eVersion:
			 return OmnNew AosSeLogEntryVersion(ctnr_doc, container, logname, rdata);

		case AosLogType::eNorm:
			 return OmnNew AosSeLogEntryNorm(ctnr_doc, container, logname, rdata);
		 
		default:
			 break;
		}
	}

	catch (...)
	{
		AosSetError(rdata, AosErrmsgId::eExceptionCreateLogEntry);
		OmnAlarm << rdata->getErrmsg() << ". Type: " << type << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosSeLogEntry::calculateSignature(
		AosBuff &buff, 
		const OmnString &logstr)
{
	// Calculate the signature. Not implemented yet.
	return true;
}


AosXmlTagPtr
AosSeLogEntry::getDoc() const
{
	return mDoc;
}


AosXmlTagPtr
AosSeLogEntry::readFromFile(
		const u64 &logid,
		const AosRundataPtr &rdata)
{
	// This function reads the log entry identified by 'logid'.
	// Since logs can be in different format, reading log entries
	// should be handled by the corresponding log entry class. 
	// This function will retrieve the type first. It uses the 
	// type to get the corresponding subclass of 'AosSeLogEntry'.
	//
	// File format is:
	// 		length 			(4 bytes)
	// 		entry type		(char)
	// 		front poison	(u32)
	// 		reference count	(u32)
	// 		flags			(u32)
	// 		data			(variable)
	// 		signature		(optional)
	// 		length			(4 bytes)
	u32 seqno, offset, moduleid;
	AosLogId::decomposeLogid(logid, seqno, offset, moduleid);

	AosMdlLogSvrPtr logsvr = AosPhyLogSvr::getMdlLogSvr(moduleid);
	aos_assert_r(logsvr, 0);
	//OmnFilePtr file = logsvr->getLogFile(seqno, moduleid, rdata AosMemoryCheckerArgs);
	AosReliableFilePtr file = logsvr->getLogFile(seqno, moduleid, rdata AosMemoryCheckerArgs);
	if (!file)
	{
		AosSetError(rdata, AosErrmsgId::eLogNotFound);
		OmnAlarm << rdata->getErrmsg() << ". Seqno: " << seqno << enderr;
		return 0;
	}

	if (AosPhyLogSvr::isChecking())
	{
		OmnScreen << "Retrieve log: " << moduleid 
			<< " : " << seqno 
			<< " : " << rdata->getSiteid() 
			<< " : " << offset 
			<< " : " << logid << endl;
	}

	u32 length;
	return readFromFile(file, offset, length, rdata);
}


AosXmlTagPtr
AosSeLogEntry::readBody(
		//const OmnFilePtr &file,
		const AosReliableFilePtr &file,
		const u64 &offset,
		const u32 length,
		const AosRundataPtr &rdata)
{
	// This function reads in the log identified by 'logid'.
	// The header has already read in 'buff'.
	// 
	// Note that this is a virtual function. Different types of log entries
	// may need to override this function.
	//
	// File format is:
	// 		length 			(4 bytes)
	// 		entry type		(char)
	// 		front poison	(u32)
	// 		reference count	(u32)
	// 		flags			(u32)
	// 		data			(variable)
	// 		signature		(optional)
	// 		length			(4 bytes)
	
	int datalen = length - eHeaderLength - getSignatureLen() - sizeof(u32);
	u64 start = offset + eHeaderLength;
	if (gAosLogLevel >= 100) OmnScreen << "Read log length: " << datalen 
				<< ". FileName: " << file->getFileName()
				<< ". Offset: " << offset << endl;
	
	AosBuff buff(datalen + 20, 0 AosMemoryCheckerArgs);
	char *data = buff.data();
	//int bytesRead = file->readToBuff(start, datalen, data);
	//aos_assert_rr(bytesRead == int(datalen), rdata, 0);
	bool rslt = file->readToBuff(start, datalen, data, rdata.getPtr());
	aos_assert_rr(rslt, rdata, 0);
	buff.setDataLen(datalen);
	buff.reset();
	OmnString str = buff.getOmnStr("");
	aos_assert_r(str != "", 0);

	AosXmlTagPtr doc = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	return doc;
}


AosXmlTagPtr
AosSeLogEntry::readFromFile(
		//const OmnFilePtr &file,
		const AosReliableFilePtr &file,
		const u64 &offset,
		u32 &length,
		const AosRundataPtr &rdata)
{
	return readHeaderStatic(file, offset, length, rdata);
}


u64
AosSeLogEntry::appendToFile(
		const AosSeLogPtr &selog,
		const u32 &seqno,
		const AosXmlTagPtr &log,
		//const OmnFilePtr &file,
		const AosReliableFilePtr &file,
		u64 &writePos,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(file, rdata, 0);
	aos_assert_rr(log, rdata, 0);
	aos_assert_rr(mIILName != "", rdata, 0);

	//AosXmlParser parser;
	//AosXmlTagPtr log = parser.parse(log_str, "" AosMemoryCheckerArgs);
	//aos_assert_r(log,  0);
	AosXmlTagPtr child = log->getFirstChild();
	aos_assert_r(child, false);

	// Retrieve the reference count
	mRefCount = child->getAttrU32(AOSTAG_REFCOUNT, 0);

	// Determine the flags
	mFlags = 0;
	//if (!log->getAttrBool(AOSTAG_DELETABLE, true)) 
	//{
		mCanDeletable = true;
	//}

	// Set the creation time
	//u64 crtLogTime = OmnGetTimestamp();
	//log->setAttr(AOSTAG_LOGCREATE_TIME, crtLogTime);

	// Determine the logid
	u64 logid = AosLogId::composeLogid(seqno, writePos, selog->getModuleId());
	
	//OmnScreen << "ktttt : logid:" << logid << endl;

	child->setAttr(AOSTAG_LOGID, logid);
	child->setAttr(AOSTAG_OTYPE, AOSOTYPE_LOG);
	// Ready to save the log into the file.
	// File format is:
	// 		length 			(4 bytes)
	// 		entry type		(char)
	// 		front poison	(u32)
	// 		reference count	(u32)
	// 		flags			(u32)
	// 		data			(variable)
	// 		signature		(optional)
	// 		length			(4 bytes)
	OmnString logstr = log->toString();
	int loglength = logstr.length();
	aos_assert_rr(loglength > 0, rdata, 0);

	if (gAosLogLevel >= 100) OmnScreen << "Add log: " << loglength << ". Seqno: " << seqno
		<< ". Offset: " << writePos << ". Logid: " << logid << endl;

	int siglen = (withSignature()) ? eSignatureLen : 0;
	mEntryLength = eHeaderLength + siglen + loglength + sizeof(u32) + AosBuff::getStrSize(0);
	aos_assert_rr(mEntryLength > 0, rdata, 0);
	if (mEntryLength > (u32)mMaxEntrySize) 
	{
		AosSetError(rdata, AosErrmsgId::eLogTooLong);
		OmnAlarm << rdata->getErrmsg() << ". Length: " << mEntryLength << enderr;
		return 0;
	}

	// Create the header and save it.
	AosBuff buff(mEntryLength+sizeof(u32) AosMemoryCheckerArgs);
	buff.setU32(mEntryLength);
	buff.setChar(mLogType);
	buff.setU32(ePosition);
	buff.setU32(mRefCount);
	buff.setU32(mFlags);
	buff.setOmnStr(logstr);
	if (withSignature())
	{
		//setSignatureFlag(mFlags);
	}
	buff.setU32(mEntryLength);
	buff.setU32((u32)eEndFlag);
	aos_assert_rr(writePos >= 1024, rdata, 0);
	file->put(writePos, buff.data(), mEntryLength+sizeof(u32), true, rdata.getPtr()); 

	writePos += mEntryLength;

	u64 crtLogTime = child->getAttrU64(AOSTAG_CT_EPOCH, 0);
	aos_assert_rr(crtLogTime > 0, rdata, 0);
	// add time iil
//OmnScreen << "-----------: " << crtLogTime << ":" << logid << endl;
	//bool rslt = AosIILClient::getSelf()->addU64ValueDocToTable(
	bool rslt = AosIILClientObj::getIILClient()->addU64ValueDocToTable(
			mIILName, crtLogTime, logid, false, false, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to create iil for log";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	
	// 	<container ...>
	// 		<AOSTAG_LOG_DEFINITION>zky_log_def
	// 			<AOSTAG_LOG_INDEX>zky_log_index
	// 				.....
	// 			</AOSTAG_LOG_INDEX>
	// 			<AOSTAG_LOG_MANAGE>zky_log_manage
	// 				<AOSTAG_DFT_LOGNAME 
	// 					AOSTAG_AGING_TYPE="agecap"
	// 					max_filesize="50000000"
	// 					max_files="6"/>
	//				<test_1 
	// 					AOSTAG_AGING_TYPE="agecap"
	// 					max_filesize="20000000"
	// 					max_files="1"/>
	//				<test_2 
	// 					AOSTAG_AGING_TYPE="agecap"
	// 					max_filesize="600"
	// 					max_files="100"/>
	//			</AOSTAG_LOG_MANAGE>
	// 		</AOSTAG_LOG_DEFINITION>
	// 	</container>
	AosXmlTagPtr parent_doc = AosDocClientObj::getDocClient()->getDocByObjid(selog->getPctrObjid(), rdata);
	if (parent_doc)
	{
		AosXmlTagPtr logdef = parent_doc->getFirstChild(AOSTAG_LOG_DEFINITION);
		if (logdef)
		{
			AosXmlTagPtr idxdef = logdef->getFirstChild(AOSTAG_LOG_INDEX);
			if (idxdef)
			{
				AosXmlTagPtr logbody;
				if (log->getTagname() == "loginfo") 
				{
					logbody = log->getFirstChild();
				}
				else
				{
					logbody = log;
				}
				// Ketty 2013/02/22
				// Check the container, if need to index, send it to DocProc, need to modify.
				//AosTaskTransPtr task_trans = OmnNew AosTaskTrans(rdata, logbody->toString() AosMemoryCheckerArgs);
				//AosIILClient::getSelf()->getTaskSubIds(task_trans); 
				
				//AosDocReqPtr docreq = AosDocProc::getSelf()->addCreateLogRequest(
				//		          rdata->getUserid(), logbody, rdata, task_trans);
				rslt = AosDocClientObj::getDocClient()->addCreateLogRequest(
					rdata->getUserid(), logbody, rdata);
				aos_assert_r(rslt, false);
			}
		}
	}

	return logid;
}


bool
AosSeLogEntry::deleteLog(
		const u64 &logid,
		const OmnString &ctnr_objid,
		const AosXmlTagPtr &log,
		const AosRundataPtr &rdata)
{
	// Now we don't delete form file, we just set flag 0.
	aos_assert_r(ctnr_objid != "", false);
	AosXmlTagPtr parent_doc = AosDocClientObj::getDocClient()->getDocByObjid(ctnr_objid, rdata);
	if (parent_doc)
	{
		AosXmlTagPtr logdef = parent_doc->getFirstChild(AOSTAG_LOG_DEFINITION);
		if (logdef)
		{
			AosXmlTagPtr idxdef = logdef->getFirstChild(AOSTAG_LOG_INDEX);
			if (idxdef)
			{
				bool rslt = AosDocClientObj::getDocClient()->addDeleteLogRequest(
			    	               log, rdata->getUserid(), rdata);
				aos_assert_rr(rslt, rdata, false);
			}
		}
	}

	mFlags = 0;
	u32 seqno, offset, moduleid;
	AosLogId::decomposeLogid(logid, seqno, offset, moduleid);
	// TSK001NOTE
	// Why 'moduleid > 0'? Can 'moduleid' be 0?
	// aos_assert_rr(seqno > 0 && offset >= AosSeLog::eBodyStart && moduleid > 0, rdata, false);
	aos_assert_rr(seqno > 0 && offset >= AosSeLog::eBodyStart && 
			AosLogUtil::isValidModuleId(moduleid), rdata, false);
	
	AosMdlLogSvrPtr logsvr = AosPhyLogSvr::getMdlLogSvr(moduleid);
	aos_assert_r(logsvr, false);
	//OmnFilePtr file = logsvr->getLogFile(seqno, moduleid, rdata AosMemoryCheckerArgs);
	AosReliableFilePtr file = logsvr->getLogFile(seqno, moduleid, rdata AosMemoryCheckerArgs);
	aos_assert_rr(file && file->isGood(), rdata, false);
	
	u64 startPos = offset + eHeaderLength - sizeof(u32);
	aos_assert_rr(startPos > AosSeLog::eBodyStart, rdata, false);
	aos_assert_rr(file->setU32(startPos, eDeleteFlag, true, rdata.getPtr()), rdata, false);
	return true;
}


void 
AosSeLogEntry::setSignature(AosBuff &buff, const OmnString &logstr)
{
	OmnNotImplementedYet;
	return;
}


AosXmlTagPtr
AosSeLogEntry::readHeaderStatic(
		//const OmnFilePtr &file,
		const AosReliableFilePtr &file,
		const u64 &offset,
		u32 &length,
		const AosRundataPtr &rdata)
{
	// File format is:
	// 		length 			(4 bytes)
	// 		entry type		(char)
	// 		front poison	(u32)
	// 		reference count	(u32)
	// 		flags			(u32)
	// 		data			(variable)
	// 		signature		(optional)
	// 		length			(4 bytes)
	char mem[eHeaderLength];
	bool rslt = file->readToBuff(offset, eHeaderLength, mem, rdata.getPtr());
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eFailedReadingLogEntry);
		OmnAlarm << rdata->getErrmsg() << ". File: " << file->getFileName()
			<< ". Offset: " << offset << enderr;
		return 0;
	}
	AosBuff buff(mem, eHeaderLength, eHeaderLength, false AosMemoryCheckerArgs);
	length = buff.getU32(0);
	if (length == eEndFlag)
	{
		// This is the end of the log. 
		length = 0;
		return 0;
	}

	AosLogType::E type = (AosLogType::E)buff.getChar(0);
	u32 poison = buff.getU32(0);
	aos_assert_rr(poison == ePosition, rdata, 0);
	buff.getU32(0);		// refcount
	u32 flags = buff.getU32(0);
	if (flags == 1)
	{
		AosSetError(rdata, AosErrmsgId::eDocAlreadyDeleted);
		OmnAlarm << rdata->getErrmsg() 
			<< ".FileName: " << file->getFileName() << enderr;
		return 0;
	}
	AosSeLogEntryPtr reader = getLogEntry(type, rdata);
	if (!reader)
	{
		AosSetError(rdata, AosErrmsgId::eFailedToGetLogEntry);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	return reader->readBody(file, offset, length, rdata);
}

