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
// 04/11/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtilServer/IdGenFileMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "SEUtil/DocTags.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "SiteMgr/SyncServer.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

extern AosXmlTagPtr gAosAppConfig;

OmnSingletonImpl(AosIdGenFileMgrSingleton,
	AosIdGenFileMgr,
	AosIdGenFileMgrSelf,
	"AosIdGenFileMgr");

const OmnString AosIdGenFileMgr::sgFileName = "idGen";

AosIdGenFileMgr::AosIdGenFileMgr()
:mLock(OmnNew OmnMutex())
{
}


AosIdGenFileMgr::~AosIdGenFileMgr()
{
}


bool
AosIdGenFileMgr::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	OmnString docstr;
	
	mFilePath = config->getAttrStr(AOSCONFIG_DIRNAME);
	u64 crtDocid = config->getAttrU64(AOSCONFIG_STARTDOCID, eDftStartDocid);
	u64 crtIILId = config->getAttrU64(AOSCONFIG_STARTIILID, eDftStartIILId);
	u64 crtImgId = config->getAttrU64(AOSCONFIG_STARTIMGID, eDftStartImgId);
	u64 dftDocidBlocksize = config->getAttrU64(AOSCONFIG_DFTDOCID_BLOCKSIZE,
		eDftDocidBlocksize);
	u64 dftIILIdBlocksize = config->getAttrU64(AOSCONFIG_DFTIILID_BLOCKSIZE,
		eDftIILIdBlocksize);
	u64 dftImgIdBlocksize = config->getAttrU64(AOSCONFIG_DFTIMGID_BLOCKSIZE,
		eDftImgIdBlocksize);

	docstr = "<idgenmgr ";
	docstr << AOSTAG_OBJID << "=\"" << "zky_idgenmgr@sys"
		   << "\" " << AOSTAG_SITEID << "=\"" << AOS_DFTSITEID
			   << "\" " << AOSTAG_OTYPE << "=\"" << AOSOTYPE_IDGENMGR
		   << "\">";
	docstr << "<docid " << AOSTAG_CRTID << "=\"" << crtDocid
		   << "\" " << AOSTAG_BLOCKSIZE << "=\"" << dftDocidBlocksize << "\"/>";
	docstr << "<iilid " << AOSTAG_CRTID << "=\"" << crtIILId
			   << "\" " << AOSTAG_BLOCKSIZE << "=\"" << dftIILIdBlocksize << "\"/>";
	docstr << "<imageid " << AOSTAG_CRTID << "=\"" << crtImgId
		   << "\" " << AOSTAG_BLOCKSIZE << "=\"" << dftImgIdBlocksize << "\"/>"
			   << "</idgenmgr>";

	OmnString filename;
	filename << mFilePath << "/" << sgFileName;
	mFile = OmnNew OmnFile(filename, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!mFile->isGood())
	{
		mFile = OmnNew OmnFile(filename, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_rl(mFile->isGood(), mLock, false);
		writeRecord(0, docstr, false);
	}
	return true;
}


bool
AosIdGenFileMgr::start()
{
	return true;
}


bool
AosIdGenFileMgr::stop()
{
	// If mControlDoc is not defined yet, it is the time to define it. 
	// Note that AosDocServer should be stopped after this class.
	mLock->lock();
	//mFile->closeFile();
	mFile = 0;
	mLock->unlock();

	return true;
}


AosXmlTagPtr
AosIdGenFileMgr::readRecord(const int id)
{
	mLock->lock();
	u64 offset = id*eRecordSize+eRecordStart;

	mFile->lock();
	u32 length = mFile->readBinaryU32(offset, 0);
	if (length == 0 || length > eMaxDocSize)
	{
		mFile->unlock();
		mLock->unlock();
		OmnAlarm << "Doc too long: " << length << enderr;
		return 0;
	}

	char *buff = OmnNew char[length+1];
	aos_assert_rl(buff, mLock, 0);

	u32 len = mFile->readToBuff(offset+eRecordBodyStart, length, buff);
	mFile->unlock();

	aos_assert_rl(len == length, mLock, 0);
	OmnString str(buff, len);
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_rl(xml, mLock, 0);
	OmnDelete [] buff;
	mLock->unlock();
	return xml;
}


bool
AosIdGenFileMgr::writeRecord(const int id, const OmnString &data, bool needLock = true)
{
	if (needLock) mLock->lock();
	u64 offset = id*eRecordSize+eRecordStart;
	u32 length = data.length();

	mFile->lock();
	bool rest = mFile->setU32(offset, length, false);
	if (!rest)
	{
		if (needLock) mLock->unlock();
		aos_assert_r(rest, false);
		return false;
	}
	rest = mFile->put(offset+eRecordBodyStart, data.data(), length, true);
	mFile->unlock();
	if (!rest)
	{
		if (needLock) mLock->unlock();
		aos_assert_r(rest, false);
		return false;
	}

	if (needLock) mLock->unlock();
	return true;
}
