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
// Modification History:
// 07/21/2011   Created by Ketty 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/29
#if 0
#include "DocTrans/DocTransMgr.h"

#include "DocTrans/DocTransProc.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/Siteid.h"
#include "SEUtil/Ptrs.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"

#include <dirent.h>


AosDocTransMgr::AosDocTransMgr(
		const AosXmlTagPtr &config,
		const AosDocTransProcPtr &proc)
:
mInstId(0),
mProc(proc),
mLock(OmnNew OmnMutex())
{
	aos_assert(config);
	aos_assert(proc);
	
	mDirName = OmnApp::getTransDir();
	mDirName << "/" << config->getAttrStr("dirname", "IILTrans");
	mTempFname = config->getAttrStr("fname");
	
	mDocVector.clear();

	recover();
}


AosDocTransMgr::~AosDocTransMgr()
{
}


void
AosDocTransMgr::recover()
{
	DIR *dir;
	struct dirent *file;

	if((dir = opendir(mDirName.data())) == NULL)
	{
		mkdir(mDirName.data(), 00755);
		return;	
	}
		
	while((file = readdir(dir)) != NULL)
	{
		if(strncmp(file->d_name, ".", 1) == 0)  continue;
	    
		OmnString fileName = mDirName;
		fileName << "/" << file->d_name;
		recoverEachFile(fileName);
	}
	closedir(dir);
}


void
AosDocTransMgr::recoverEachFile(const OmnString &fileName)
{
	OmnFilePtr tmpFile = OmnNew OmnFile(fileName, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!tmpFile->isGood()) return;

	u32 tmpFileSize = tmpFile->readBinaryU32(eTmpFileSize, 0);
	if(tmpFileSize <= 0 || tmpFileSize > eMaxFileSize) return;

	AosBuffPtr tmpFileBuff = OmnNew AosBuff(tmpFileSize, 0 AosMemoryCheckerArgs);
	tmpFile->readToBuff(0, tmpFileSize, tmpFileBuff->data());
	tmpFileBuff->setDataLen(tmpFileSize);

	u32 p1 = tmpFileBuff->getU32(0);  
	char status = tmpFileBuff->getChar('a');   
	
	//file is delete 
	if ( (p1 == 0xffffffff) || tmpFileSize == 0) return;
	
	u32 p2 = *(u32 *)(tmpFileBuff->data() + tmpFileSize - sizeof(u32));
	//the tmp file is not correct
	if(p1 != eTmpPoisonNum || p2 != eTmpPoisonNum || status == 'a') return;
	
//	OmnScreen << "--------------------------recover, fileName:" << fileName << endl;

	if(status == 'b')
	{
		//copy tmp header and body
		bool rslt = recoverHeaderBody(tmpFileBuff);
		if(!rslt)
		{
			OmnAlarm << "copy tmp header and body Wrong!" << enderr;
		}
		else
		{
			tmpFile->setInt(0, 0xffffffff, true);
		}
	}
	else if(status == 'c')
	{
		//modify trans status
		tmpFileBuff->setCrtIdx(eTmpNumTrans);
		u32 transNum = tmpFileBuff->getU32(0); 
		
		vector<u64> trans_ids;
		u64 global_tid = 0;
		for(u32 i=0; i<transNum; i++)
		{
			global_tid = tmpFileBuff->getU64(0); 
			trans_ids.push_back(global_tid);
		}
		mProc->finishTrans(trans_ids, 0);
		
		tmpFile->setInt(0, 0xffffffff, true);
	}
}


bool
AosDocTransMgr::recoverHeaderBody(const AosBuffPtr &buff)
{
	//		Byte 0-3	Poison1		to indicate whethe the file is good
	// 		Byte 4		File Status: 
	// 		Byte 5-8	File Size: 
	// 		Byte 9-16	docid (or iilid)
	//		Byte 17-20	blockNum
	// 		Byte 21-25	Number of Transaction IDs
	// 		Byte 26-..	Transactions (u64)
	// 		...
	// 		Bytes		Header size
	// 		Bytes		Header contents
	// 		Bytes		Body size
	// 		Bytes		Body contents
	// 		Byte xxx	Poison 2
	
	buff->setCrtIdx(eTmpEntryId);
	u64 entry_id =  buff->getU64(0);	
	aos_assert_r(entry_id, false);

	int blockNum = buff->getInt(0);
	u32 transNum = buff->getU32(0);

	u32 headerSizeOffset = eTmpTrans + sizeof(u64) * transNum;  
	for(int i=0; i<blockNum; i++)
	{
		buff->setCrtIdx(headerSizeOffset);
		u32 headerSize = buff->getU32(0);		//the real headerSize	
		aos_assert_r(headerSize, false);

		u32 headerOffset = headerSizeOffset + sizeof(u32);	 //sizeof(u32) is the real headerSize
		AosBuffPtr headerBuff = OmnNew AosBuff(buff->data() + headerOffset, headerSize, headerSize, 0 AosMemoryCheckerArgs);

		u32 bodySizeOffset = headerOffset + headerSize;
		buff->setCrtIdx(bodySizeOffset);
		u32 bodySize = buff->getU32(0);	

		u32 bodyOffset = bodySizeOffset + sizeof(u32);
		AosBuffPtr bodyBuff = OmnNew AosBuff(buff->data() + bodyOffset, bodySize, bodySize, 0 AosMemoryCheckerArgs);
	
		//copy tmp header and body
		bool rslt = mProc->recoverHeaderBody(entry_id, headerBuff, bodyBuff);
		aos_assert_r(rslt, false);
	
		headerSizeOffset = bodyOffset + bodySize;
	}	
	buff->setCrtIdx(eTmpNumTrans);
	vector<u64> trans_ids;
	u64 global_tid = 0;
	for(u32 i=0; i<transNum; i++)
	{
		global_tid = buff->getU64(0); 
		trans_ids.push_back(global_tid);
	}
	mProc->finishTrans(trans_ids, 0);
	return true;
}


AosDocTransPtr
AosDocTransMgr::getDocTrans()
{
	mLock->lock();
	AosDocTransPtr doc;
	if (mDocVector.size() > 0)
	{
		doc = mDocVector.back();
		mDocVector.pop_back();
	}
	else
	{
		OmnString fname = getFileName();
		doc = OmnNew AosDocTrans(fname, mProc);
	}
	mLock->unlock();
	aos_assert_r(doc->isGood(), 0);
	return doc;
}

	
OmnString
AosDocTransMgr::getFileName()
{
	u32 instanceId = mInstId++;
	OmnString fileName = mDirName;
	fileName << "/" << mTempFname << "_" << instanceId;
	return fileName;
}


void
AosDocTransMgr::returnDocTrans(const AosDocTransPtr &doc)
{
	mLock->lock();
	mDocVector.push_back(doc);
//OmnScreen << "return docTrans:" << doc->getFilename() << endl;
	mLock->unlock();
}

#endif
