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
// 12/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/VersionDocMgr.h"

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
#include "SEUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/LockMonitorMgr.h"
#include "Thread/LockMonitor.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"

const bool sgWriteSpace = true;

AosVersionDocMgr::AosVersionDocMgr()
:
mLock(OmnNew OmnMutex()),
mCrtSeqno(0),
mMaxFilesize(eAosMaxFileSize)
{
	memset(mFilesizes, 0, 4 * eMaxFiles);
	mLockMonitor = OmnNew AosLockMonitor(mLock);
	mLock->mMonitor = mLockMonitor;
	AosLockMonitorMgr::getSelf()->addLock(mLockMonitor);
}


AosVersionDocMgr::AosVersionDocMgr(
		const OmnString &dirname, 
		const OmnString &fname)
:
mLock(OmnNew OmnMutex()),
mCrtSeqno(0),
mMaxFilesize(eAosMaxFileSize),
mDirname(dirname),
mFilename(fname)
{
	memset(mFilesizes, 0, 4 * eMaxFiles);
	mLockMonitor = OmnNew AosLockMonitor(mLock);
	mLock->mMonitor = mLockMonitor;
	AosLockMonitorMgr::getSelf()->addLock(mLockMonitor);
}


AosVersionDocMgr::~AosVersionDocMgr()
{
}


bool 		
AosVersionDocMgr::init(
	const OmnString &dirname, 
	const OmnString &fname)
{
	mMaxFilesize = eAosMaxFileSize;
	mDirname = dirname;
	mFilename = fname;
	aos_assert_r(mDirname != "", false);
	aos_assert_r(mFilename != "", false);
	OmnString cmd = "mkdir -p ";
	cmd << mDirname;
	system(cmd.data());
	return true;
}


bool
AosVersionDocMgr::findFilePriv(const u64 &newsize, u32 seqno)
{
	// This function finds a file that has the room
	// for 'newsize'. It searches from the first file and returns
	// [seqno, offset] that can grow 'newsize'. 
	// It will also update the index record in the file
	// If no such files, it tries to create new one. If it 
	// exceeds the maximum, it returns false. 
	// 
	// IMPORTANT: the caller should have locked the class.
	aos_assert_r(seqno < eMaxFiles, false);
	OmnFilePtr file;
	if (!mFiles[seqno]) 
	{
		file = openFilePriv(seqno AosMemoryCheckerArgs);
		aos_assert_r(file, false);
	}
	while (seqno < eMaxFiles)
	{
		if (mFilesizes[seqno] + newsize + eDocOffset < mMaxFilesize)
		{
			// The current file is good enough
			seqno = mCrtSeqno;
			return true;
		}

		// Need to create a new log file
		seqno++;
		file = openFilePriv(seqno AosMemoryCheckerArgs);
		aos_assert_r(file, false);
	}

	OmnShouldNeverComeHere;
	return false;
}


AosXmlTagPtr
AosVersionDocMgr::getXmlDoc2(const u32 seqno, const u32 offset)
{
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	OmnFilePtr file = openFilePriv(seqno AosMemoryCheckerArgs);
	if (!file) 
	{
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}

	file->lock();
	AOSLMTR_UNLOCK(mLockMonitor);

	int size = file->readBinaryInt(offset + eDocSizeOffset, -2);
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

	OmnConnBuffPtr buff = OmnNew OmnConnBuff(size);
	char *data = buff->getData();
	int bytesRead = file->readToBuff(offset + eDocOffset, size, data);
	file->unlock();

	aos_assert_r(bytesRead >= 0 && bytesRead == size, 0); 
	buff->setDataLength(size);

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(buff, "" AosMemoryCheckerArgs);
	AOSLMTR_FINISH(mLockMonitor);
	return xml;
}


bool 	
AosVersionDocMgr::saveDoc(
	u32 &seqno,
	u64 &offset,
	const u64 docsize,
	const char *data)
{

	// This is used by the version manager. Each record is:
	//  Byte 0-3:   Doc size
	//  Byte 4-:    The doc
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	seqno = mCrtSeqno;
	aos_assert_rk(findFilePriv(docsize, seqno), mLock, false);
	OmnFilePtr ff = mFiles[seqno];
	aos_assert_rk(ff, mLock, false);

	offset = mFilesizes[seqno];
	mFilesizes[seqno] += docsize + eDocSizeOffset;

	ff->lock();
	ff->setInt(offset, docsize, false);
	ff->put(offset+eDocSizeOffset, data, docsize, true);
	ff->unlock();

	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
	return true;
}


bool
AosVersionDocMgr::stop()
{
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	for (int i=0; i<eMaxFiles; i++)
	{
		if (mFiles[i])
		{
			//mFiles[i]->closeFile();
			mFiles[i] = 0;
		}
	}
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
	return true;
}


AosXmlTagPtr
AosVersionDocMgr::readDoc(const u32 seqno, const u32 offset)
{
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	OmnFilePtr file = openFilePriv(seqno AosMemoryCheckerArgs);
	if (!file) 
	{
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}

	file->lock();
	AOSLMTR_UNLOCK(mLockMonitor);

	int size = file->readBinaryInt(offset, -2);
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
		int bytesRead = file->readToBuff(offset + eDocSizeOffset, size, data);
		file->unlock();

		aos_assert_r(bytesRead >= 0 && bytesRead == size, 0); 
		buff->setDataLength(size);

		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(buff, "" AosMemoryCheckerArgs);
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


OmnFilePtr
AosVersionDocMgr::openFilePriv(const u32 seqno AosMemoryCheckDecl) 
{
	// This is a private member function. The caller should
	// have locked the class. It opens the file 'seqno'. If the 
	// file does not exist, it will create it.
	OmnString fname = mDirname;
	fname << "/" << mFilename << "_" << seqno;
	mServerReadonly = (OmnApp::getAppConfig()->getAttrStr("serverReadonly") == "true");
	OmnFilePtr ff;
	if (mServerReadonly)
	{
		ff = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerFileLine);
	}
	else
	{
		ff = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerFileLine);
		if (!ff->isGood())
		{
			// The file has not been created yet. Create it.
			ff = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerFileLine);
			if (!ff->isGood())
			{
				OmnAlarm << "Failed to open the file: " << fname << enderr;
				return NULL;
			}
		}
	}

	mCrtSeqno = seqno;
	mFiles[seqno] = ff;
	u64 crtPos = ff->getLength();
	aos_assert_r(crtPos >= 0, 0);
    mFilesizes[seqno] = (u64)(crtPos==0)?eDocOffset:crtPos;	
	return ff;
}

bool 	
AosVersionDocMgr::saveDocASC(
	u32 &seqno,
	u64 &offset,
	u64 &docsize,
	const OmnString &data)
{
	// This is used by the version manager. Each record is:
	// docsize : title
	// the doc
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	seqno = mCrtSeqno;
	aos_assert_rk(findFilePriv(docsize, seqno), mLock, false);
	OmnFilePtr ff = mFiles[seqno];
	aos_assert_rk(ff, mLock, false);

	offset = mFilesizes[seqno];
	mFilesizes[seqno] += docsize + eDocSizeOffset;
	ff->lock();
	//version doc.
	OmnString buff;
	buff << docsize << ":" << data;
	ff->append(buff, true);
	ff->unlock();

	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
	return true;	
}

AosXmlTagPtr
AosVersionDocMgr::readDocASC(const u32 seqno, const u32 offset)
{
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	OmnFilePtr file = openFilePriv(seqno AosMemoryCheckerArgs);
	if (!file) 
	{
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}
	file->lock();
	AOSLMTR_UNLOCK(mLockMonitor);

	//int size = file->readBinaryInt(offset, -2);
	//james 2010-12-07
	int size = file->readU64(offset, -2);
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
		int bytesRead = file->readToBuff(offset + eDocSizeOffset, size, data);
		file->unlock();
		aos_assert_r(bytesRead >= 0 && bytesRead == size, 0); 
		buff->setDataLength(size);
		//2010-12-07 james
		OmnString str(data, size);
		OmnString docstr;
		int sidx = str.indexOf(0, '\n') + 1;
		docstr << str.substr(sidx, str.length() - 1);			
		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
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
