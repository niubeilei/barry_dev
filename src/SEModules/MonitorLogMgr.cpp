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
//	This class is used to manage log and version files, 
//	All documents are stored in 'mVersionDocname' + seqno. There is 
//	a Document Index, that is stored in files 'mVersionDocname' + 
//		'Idx_' + seqno
//
// Modification History:
// 03/03/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SEModules/MonitorLogMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "IdGen/IdGen.h"
#include "Porting/ThreadDef.h"
#include "SEUtil/SeConfig.h"
#include "SEUtil/DiskBlock.h"
#include "SEUtil/DocTags.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/SysLogName.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "Thread/Mutex.h"
#include "Thread/LockMonitorMgr.h"
#include "Thread/LockMonitor.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosMonitorLogMgr::AosMonitorLogMgr()
:
mPreDocLoc(0)
{
}


AosMonitorLogMgr::AosMonitorLogMgr(
		const OmnString &dirname, 
		const OmnString &fname)
:
mPreDocLoc(0)
{
}


void
AosMonitorLogMgr::start()
{
}


AosMonitorLogMgr::~AosMonitorLogMgr()
{
}


bool 	
AosMonitorLogMgr::saveDoc(
		u64 &logid,
		const OmnString &data, 
		const AosRundataPtr &rdata)
{
	// Format:
	//  Byte 0-3:    Doc size
	//  Byte 4-11:   pre Doc Location
	//  Byte 12-15:	 BrowserId
	//  Byte 16-:	 The doc
	AosXmlParser parser;
	AosXmlTagPtr xml_data =  parser.parse(
			data.data(), data.length(), "" AosMemoryCheckerArgs); 
	//logid = AosSeLogClientObj::getLogClient()->addLogWithResp(
			//AOSCTNR_SYSMONITOR, AOSSYSLOGNAME_MONITOR, xml_data, rdata);
	//aos_assert_r(logid>0, false);
	return true;
}


/*
AosXmlTagPtr
AosMonitorLogMgr::readDoc(
			const u32 seqno,
			const u64 offset,
			u64	&preDocLoc,
			u32 &browserId)
{
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	OmnFilePtr file = openFilePriv(seqno);
	if (!file) 
	{
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}

	file->lock();
	AOSLMTR_UNLOCK(mLockMonitor);

	char bb[eDocHeaderSize];
	file->readToBuff(offset, eDocHeaderSize, bb);
	AosBuff buff(bb, eDocHeaderSize, 0);

	int size = buff.getInt(-2);
	preDocLoc = buff.getU64(0);
	browserId = buff.getU32(0);
	if (size == -1)
	{
		file->unlock();
		OmnAlarm << "Record being deleted: "
			<< seqno << ":" << offset << enderr;
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}

	if (size < 0)
	{
		file->unlock();
		OmnAlarm << "Failed to read the file: " << seqno << ":" << offset << enderr;
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}

	if (size == 0) 
	{
		file->unlock();
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}

	if (size >= OmnConnBuff::eMaxBuffSize)
	{
		OmnAlarm << "Data Too big: " << size << enderr;
		file->unlock();
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}

	try
	{
		OmnConnBuffPtr buff = OmnNew OmnConnBuff(size);
		char *data = buff->getData();
		int bytesRead = file->readToBuff(offset + eDocBodyOffset, size, data);
		file->unlock();

		aos_assert_r(bytesRead >= 0 && bytesRead == size, 0); 
		buff->setDataLength(size);

		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(buff, "");
		AOSLMTR_FINISH(mLockMonitor);
		return xml;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Caught exception: " << e.getErrmsg() << enderr;
		AOSLMTR_FINISH(mLockMonitor);
		file->unlock();
		return 0;
	}

	OmnShouldNeverComeHere;
	AOSLMTR_FINISH(mLockMonitor);
	file->unlock();
	return 0;
}
*/

