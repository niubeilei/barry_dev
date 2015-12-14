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
//
// Modification History:
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/Tester2/StorageMgrAyscIoTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/ActImportDoc.h"
#include "Debug/Debug.h"
#include "DocFileMgr/DocFileMgr.h"
#include "DocFileMgr/FileGroupMgr.h"
#include "DocFileMgr/StorageApp.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "TaskMgr/TaskData.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "StorageMgr/StorageMgr.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageMgr/Tester2/Ptrs.h"
#include "StorageMgr/StorageMgrAyscIo.h"
#include "StorageMgr/AioRequest.h"
#include "StorageMgr/Ptrs.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/VirtualFileObj.h"
#include "VirtualFile/VirtualFile.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "Util1/Time.h"
#include "Util/ReadFile.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>


AosStorageMgrAyscIoTester::AosStorageMgrAyscIoTester()
:
mReadId(0),
mCrtLastOffset(0),
mCrtVidIdx(0)
{
	mBuffLen = 100000000;
	mBuff = OmnNew AosBuff(mBuffLen+10 AosMemoryCheckerArgs);
}


AosStorageMgrAyscIoTester::~AosStorageMgrAyscIoTester()
{
}


bool 
AosStorageMgrAyscIoTester::start()
{
	AosRundataPtr rdata = OmnApp::getRundata()->clone(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	basicTest(rdata);
	return true;
}


bool
AosStorageMgrAyscIoTester::basicTest(const AosRundataPtr &rdata)
{
	mStartDocid = 432340000000000000;
	mNumDocids = 0;
	mCheckNum = 0;
	//readyData(rdata);
	for (int i = 0; i < 1; i++)
	{
OmnScreen << "===============: " << i << endl;
		createData(rdata);
		readData(rdata);
	}
	return true;
}


bool
AosStorageMgrAyscIoTester::asyncReadFile(const AosRundataPtr &rdata)
{
	int idx = rand()%mFileId.size();
	u64 file_id = mFileId[idx];
	OmnFilePtr ff ;//= AosVfsMgrObj::getVfsMgr()->openFile(file_id, rdata AosMemoryCheckerArgs);
	aos_assert_r(ff && ff->isGood(), false);
	AosStorageMgrAyscIoTesterPtr thisptr(this, false);
	u32 offset = rand()%(ff->getLength() - 10000000);
	int physicalid = 0;
	bool rslt = false; //AosNetFileMgr::getSelf()->asyncReadFile(file_id, physicalid, offset, 10000000, thisptr, rdata);
	aos_assert_r(rslt, false);
}


void
AosStorageMgrAyscIoTester::addReq(const AosRundataPtr &rdata)
{
	int idx = rand()%mFileId.size();
	u64 file_id = mFileId[idx];

	OmnString records = "<records> ";
	int serverid = AosGetSelfServerId();
	for (int idx = 0; idx < mFileId.size(); idx ++)
	{
		records << "<" << AOSTAG_TASKDATA_TAGNAME << " "
			<< AOSTAG_PHYSICALID << "=\"" << serverid << "\" " 
			<< AOSTAG_SERVERID << "=\"" << serverid << "\" " 
			<< AOSTAG_SIZEID << "=\"" << 12 << "\" "
			<< AOSTAG_RECORD_LEN << "=\"" << 200 << "\" "
			<< AOSTAG_STORAGE_FILEID << "=\"" << mFileId[idx] << "\" "
			<< "/>";
	}
	records << "</records>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(records, "" AosMemoryCheckerArgs);
	AosXmlTagPtr sdoc = root->getFirstChild();
	aos_assert(sdoc);

	AosActImportDocPtr ipdoc = OmnNew AosActImportDoc(false);
	aos_assert(ipdoc);
	AosTaskDataObjPtr pp;
	aos_assert(ipdoc->run(0, sdoc, pp, rdata));
}


void
AosStorageMgrAyscIoTester::createData(const AosRundataPtr &rdata)
{
	while(1)
	{
		readyData();
		if (mCrtLastOffset + mDataBuff->dataLen() > eMaxFileSize) break;
		if (!mFile || mCrtLastOffset + mDataBuff->dataLen() > eMaxFileSize)
		{
			bool rslt = createFileLocked(rdata);      
			aos_assert(rslt);
		}

OmnScreen << "write write write mCrtLastOffset: " << mCrtLastOffset << " , " << mDataBuff->dataLen() << endl;
		mFile->writelock();
		mFile->write(mCrtLastOffset, mDataBuff->data(), mDataBuff->dataLen());
		//mFile->append(mDataBuff->data(), mDataBuff->dataLen());
		mFile->unlock();

		mCrtLastOffset += mDataBuff->dataLen();
		mSize[mIndex] = mCrtLastOffset;
	}
	mFile = 0;
	mCrtLastOffset = 0;
}


bool
AosStorageMgrAyscIoTester::createFileLocked(const AosRundataPtr &rdata) 
{
	OmnString fname_prefix = "temp_";
	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	aos_assert_r(total_vids.size()>0 && mCrtVidIdx < (int)total_vids.size(), false);
	u32 virtual_id = total_vids[mCrtVidIdx];
	mCrtVidIdx++;
	if(mCrtVidIdx == (int)total_vids.size())  mCrtVidIdx = 0;

	u64 file_id;
	virtual_id = 0;
	//mFile = OmnNew AosRaidFile(rdata, virtual_id, file_id, fname_prefix, eMaxFileSize, false);
	mFile = AosVfsMgrObj::getVfsMgr()->createRaidFile(rdata, virtual_id, file_id, fname_prefix, eMaxFileSize, false);
	mCrtLastOffset = 0;
	mIndex = mFileId.size();	
	mFileId.push_back(file_id);
	mSize.push_back(0);
	OmnScreen << "file_id: " << file_id << endl;
	return true;
}

bool
AosStorageMgrAyscIoTester::readyData(const AosRundataPtr &rdata)
{
	u32 bytes_to_read = 1000000;
	mDataBuff = OmnNew AosBuff(eMaxReadSize, 0 AosMemoryCheckerArgs);
	mRecordLen = 81;
	while(1)
	{
		if (mDataBuff->dataLen() + mRecordLen + sizeof(int) > eMaxReadSize) break;
		u64 docid = mStartDocid + mNumDocids;
		OmnString str;
		str << docid << ":" << "12345abcde"
			<< ":" << "xyzrsw98765" <<":"
			<< docid*424 << ":" << docid;
		aos_assert_r(str.length() == 81, false);
		mDataBuff->setOmnStr(str);
		mNumDocids++;
	}
	u64 docid = mStartDocid;
	//checkData(mDataBuff, docid);
	return true;
}
AosBuffPtr
AosStorageMgrAyscIoTester::readFile(
		const OmnString &fname,
		int64_t &seekPos,
		const u32 bytes_to_read,
		const AosRundataPtr &rdata)
{
//OmnScreen << "fname : " << fname << " , seekpos: " << seekPos << endl; 
	AosReadFilePtr file = OmnNew AosReadFile(fname);
	aos_assert_r(file && file->isGood(), 0);
	AosBuffPtr buff = OmnNew AosBuff(bytes_to_read, 0 AosMemoryCheckerArgs);
	file->lock();
	bool rslt = file->seek(seekPos);
	if (!rslt)
	{
		file->unlock();
		return 0;
	}

	u32 bytes_read = file->readToBuff(bytes_to_read, buff->data());
	file->unlock();
	seekPos += bytes_read;

	buff->setDataLen(bytes_read);
	return buff;
}

bool
AosStorageMgrAyscIoTester::readData(const AosRundataPtr &rdata)
{
	int idx = rand()%mFileId.size();
idx = mFileId.size()-1;
	u64 file_id = mFileId[idx];
	
	OmnLocalFilePtr file = AosVfsMgrObj::getVfsMgr()->openFile(file_id, rdata AosMemoryCheckerArgs);
	int64_t offset = 0;

	u64 docid = mStartDocid;
	//int64_t len = (eMaxReadSize - 7000000)/8;
	//int64_t len = eMaxReadSize /(mRecordLen + sizeof(int));
	//len = len * (mRecordLen + sizeof(int)); 
	int64_t len = eMaxReadSize / sizeof(u64);
	len = len * sizeof(u64); 
	int64_t size = len;
	while (offset < mSize[idx])
	{
		AosBuffPtr buff = OmnNew AosBuff(size AosMemoryCheckerArgs);
		memset(buff->data(), 0, size);
		file->readlock();
		int64_t bytes_read = file->read(offset, buff->data(), size);
		if (bytes_read != size)
		{
			aos_assert_r(mSize[idx] - offset == bytes_read, false);
		}
		buff->setDataLen(bytes_read);
OmnScreen << " read read read read offset: " << offset << " , " << bytes_read << endl;
		//aos_assert_r(checkData(buff, docid), false);
		aos_assert_r(checkData(buff, offset), false);
		file->unlock();
		//offset += len;

		offset = rand()% mSize[idx];
		if (offset % sizeof(u64) != 0)
		{
			offset = offset - (offset % sizeof(u64));
			aos_assert_r(offset %sizeof(u64) == 0, false);
		}

		size = rand()% mSize[idx];
		size = size > (mSize[idx] - offset)?(mSize[idx] - offset):size;
		size = ((size % sizeof(u64)) != 0)?(size - (size % sizeof(u64))): size;
	}
//	aos_assert_r(offset == mSize[idx], false);
	AosVfsMgrObj::getVfsMgr()->removeFile(file_id, rdata);
	return true;
}

bool
AosStorageMgrAyscIoTester::checkData(const AosBuffPtr &buff, const int64_t &offset)
{
	buff->reset();
	aos_assert_r(offset % sizeof(u64) == 0, false);
	u64 vv = offset / sizeof(u64);
	while(buff->getCrtIdx() < buff->dataLen())
	{
		u64 num = buff->getU64(0);
		aos_assert_r(vv == num, false);
		vv ++;
	}
	return true;
}

bool
AosStorageMgrAyscIoTester::checkData(const AosBuffPtr &buff, u64 &docid)
{
	buff->reset();
	OmnString sep = ":";
	OmnString f1 = "12345abcde";
	OmnString f2 = "xyzrsw98765";
	int loop = 0;
	while (buff->getCrtIdx() < buff->dataLen())
	{
		OmnString str = buff->getOmnStr("");
		aos_assert_r(str != "", false);
		aos_assert_r(str.length() == mRecordLen, false);

		vector<OmnString> vv;

		AosStrSplit::splitStrBySubstr(str.data(), sep.data(), vv, 100);
		aos_assert_r(vv.size() == 5, false);

		OmnString docidstr;
		docidstr << docid;
		OmnString docidstr1;
		docidstr1 << docid*424;

		aos_assert_r(docidstr == vv[0], false);
		aos_assert_r(f1 == vv[1], false);
		aos_assert_r(f2 == vv[2], false);
		aos_assert_r(docidstr1== vv[3], false);
		aos_assert_r(docidstr == vv[4], false);

		docid ++;
		if (docid == mStartDocid + mNumDocids ) docid = mStartDocid;
		loop ++;
	}
	return true;
}

bool
AosStorageMgrAyscIoTester::readyData()
{                                                                                   
	mDataBuff = 0;
	mDataBuff = OmnNew AosBuff(eMaxReadSize, 0 AosMemoryCheckerArgs);
	while(1)
	{
		if (mDataBuff->dataLen() +  sizeof(u64) > eMaxReadSize) break;
		mDataBuff->setU64(mCheckNum);
		mCheckNum ++;
	}
	return true;
}


/*
	OmnString path = "/home/linda/unicomdata/cu_data1"; 
	DIR * pDir;
	struct dirent *ent;
	pDir = opendir(path.data());
	aos_assert_r(pDir, false);
	while((ent = readdir(pDir)) != NULL)
	{
		OmnString dname = ent->d_name;
		if (dname == "." || dname  == "..")
		{
			continue;
		}
	
		OmnString vir_path = path;
		vir_path << "/" << dname;
		DIR * vir_Dir;
		struct dirent *vir_ent;
		vir_Dir = opendir(vir_path.data());
		aos_assert_r(vir_Dir, false);
		while((vir_ent = readdir(vir_Dir)) != NULL)
		{
			dname = vir_ent->d_name;
			if (dname == "." || dname  == "..")
			{
				continue;
			}

			if (vir_ent->d_type & DT_REG)
			{
				dname = vir_ent->d_name;
				OmnString file_path = vir_path;
				file_path << "/" << dname;
				u64 len = OmnFile::getFileLengthStatic(file_path);
				int64_t i = 0;
				while (1)
				{
					AosBuffPtr bb = readFile(file_path, i, bytes_to_read, rdata); 
					if (!bb) break;
					mDataBuff->setBuff(bb->data(), bb->dataLen());
					if (mDataBuff->dataLen() + bytes_to_read > eMaxReadSize + bytes_to_read)
					{
						break;
					}
					if (i >= len) break;
				}

			}
			if (mDataBuff->dataLen() + bytes_to_read > eMaxReadSize + bytes_to_read)
			{
				break;
			}
		}
		if (mDataBuff->dataLen() + bytes_to_read > eMaxReadSize + bytes_to_read)
		{
			break;
		}
	}
	OmnScreen << "datalen :" << mDataBuff->dataLen()  << endl;
	return true;
}
*/
