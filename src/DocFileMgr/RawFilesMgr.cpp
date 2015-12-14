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
// 	Created: 05/28/2013 by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/RawFilesMgr.h"

#include "DocFileMgr/RawFiles.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "SingletonClass/SingletonImpl.cpp"

OmnSingletonImpl(AosRawFilesMgrSingleton, AosRawFilesMgr, AosRawFilesMgrSelf, "AosuRawFilesMgr");

AosRawFilesMgr::AosRawFilesMgr()
:
mLock(OmnNew OmnMutex()),
mRawFilesSeqno(0),
mLoopIndex(0)
{
	initRawFiles();
}


AosRawFilesMgr::~AosRawFilesMgr()
{
}

bool 
AosRawFilesMgr::start()
{
	return true;
}

bool 
AosRawFilesMgr::stop()
{
	return true;
}

bool 
AosRawFilesMgr::config(const AosXmlTagPtr &config)
{
	return true;
}

bool 
AosRawFilesMgr::initRawFiles()
{
	AosXmlTagPtr config = OmnApp::getAppConfig();
	aos_assert_r(config, false);
	AosXmlTagPtr stm_cfg = config->getFirstChild(AOSCONFIG_STORAGEMGR);
	aos_assert_r(stm_cfg, false);

	AosXmlTagPtr dev_config = stm_cfg->getFirstChild(AOSCONFIG_DEVICE);
	aos_assert_r(dev_config, false);

	AosXmlTagPtr partition_config = dev_config->getFirstChild(AOSCONFIG_PARTITION);
	while(partition_config)
	{
		OmnString path;
		OmnString userdir = partition_config->getAttrStr("userdir", "");
		if(userdir == "" ) 
		{
			partition_config = dev_config->getNextChild(AOSCONFIG_PARTITION);
			continue;
		}

		u32 len = userdir.length();
		if(userdir.data()[len-1] != '/')   userdir << "/";
		if(userdir.data()[0] == '/')   
		{
			path << userdir;
		}
		else
		{
			path << OmnApp::getAppBaseDir() << "/" << userdir; 
		}
		AosRawFilesPtr rawfiles = OmnNew AosRawFiles(path, mRawFilesSeqno);
		aos_assert_r(rawfiles, false);
		mFiles[mRawFilesSeqno] = rawfiles;
		mRawFilesSeqno++;
		partition_config = dev_config->getNextChild(AOSCONFIG_PARTITION);
	}
	return true;
}

bool
AosRawFilesMgr::saveFile(const AosXmlTagPtr &doc, const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	mLock->lock();
	u16 k = mLoopIndex % mRawFilesSeqno;	
	aos_assert_rl(k >= 0 && k < mRawFilesSeqno, mLock, false);

	AosRawFilesPtr rawfiles = mFiles[k];
	mLoopIndex ++;
	if (mLoopIndex >= mRawFilesSeqno) mLoopIndex = 0;
	mLock->unlock();

	aos_assert_r(rawfiles, 0);
	rawfiles->saveFile(doc, buff, rdata);
	return true;
}

AosBuffPtr
AosRawFilesMgr::getFile(const AosXmlTagPtr &doc, const u64 &id, const AosRundataPtr &rdata)
{
	u16 deviceno = (u16) (id >> 48);
	aos_assert_r(deviceno < mRawFilesSeqno, 0);

	AosRawFilesPtr rawfiles = mFiles[deviceno];
	aos_assert_r(rawfiles, 0);

	AosBuffPtr buff = rawfiles->getFile(doc, id, rdata);
	aos_assert_r(buff, 0);
	return buff;
}

