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
// 05/30/2013	Created by Jackie
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/RawFiles.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Rundata/Rundata.h"
#include "Util/File.h"
#include <sys/types.h>
#include <dirent.h>


AosRawFiles::AosRawFiles(const OmnString &dir, const u16 &deviceno)
:
mDeviceNo(deviceno),
mXmlFile(0),
mPrefix("RawFiles"),
mBaseDirName(""),
mLock(OmnNew OmnMutex())
{
	mBaseDirName = dir;
	mBaseDirName << mPrefix;
	init();
}


AosRawFiles::~AosRawFiles()
{
}

bool 
AosRawFiles::init()
{
	mCtrlDoc.xFname = "XmlDoc";
	mCtrlDoc.xOffset = 0;
	mCtrlDoc.cCrtDirSeqno = 0; 
	mCtrlDoc.cCrtFileSeqno = -1; 
	OmnString basepath = mBaseDirName;
	OmnString dirname = basepath;
	DIR *dir;
	dir = opendir(dirname.data());
	if(dir == NULL)
	{	
		OmnString cmd = "mkdir -p ";
		cmd << dirname;
		system(cmd.data());	
	}
	dir = opendir(dirname.data());
	struct stat st;
	struct dirent *ent;
	int ret = 0;
	int dirno = 0;
	OmnString childPath;
	//get great dirno
	while ((ent = readdir(dir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		{
			continue;
		}
		childPath = dirname;
		childPath << "/" << ent->d_name;
		ret = stat(childPath.data(), &st);
		if (ret < 0) 
		{
			continue;
		}
		if (S_ISREG(st.st_mode))
		{
			continue;
		}
		if (S_ISDIR(st.st_mode))
		{
			int num = atoi(ent->d_name);
			if(num > dirno)
			{
				dirno = num;
			}
		}
	}
	closedir(dir);
	//get great fileno
	int fileno = 0;
	basepath = mBaseDirName;
	dirname = basepath << "/" << dirno;
	dir = opendir(dirname.data());
	if(dir == NULL)
	{	
		OmnString cmd = "mkdir -p ";
		cmd << dirname;
		system(cmd.data());	
	}
	dir = opendir(dirname.data());
	ret = 0;
	while ((ent = readdir(dir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		{
			continue;
		}
		childPath = dirname;
		childPath << "/" << ent->d_name;
		ret = stat(childPath.data(), &st);
		if (ret < 0) 
		{
			continue;
		}
		if (S_ISREG(st.st_mode) && (strcmp(ent->d_name, mCtrlDoc.xFname.data()) != 0))
		{
			OmnString seqno(ent->d_name, 5);
			int num = atoi(seqno.data());
			if(num > fileno)
			{
				fileno = num;
			}
			continue;
		}
		if (S_ISDIR(st.st_mode))
		{
			continue;
		}
	}
	closedir(dir);

	mCtrlDoc.cCrtDirSeqno = dirno;;
	mCtrlDoc.cCrtFileSeqno = fileno;
	basepath = mBaseDirName;
	OmnString fname = basepath << "/" << dirno << "/" << mCtrlDoc.xFname;
	mCtrlDoc.xOffset = OmnFile::getFileLengthStatic(fname);

	bool r = readCtrlDoc();
	if(!r)
	{
		OmnAlarm << "Failed to get control doc!" << enderr;	
	}
	return true;
}

bool 
AosRawFiles::saveFile(const AosXmlTagPtr &doc, const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnString rawname = doc->getAttrStr("source_fname");
	aos_assert_r(rawname != "", false);
	int startidx = rawname.find('/', true);
	rawname = rawname.substr(startidx+1);

	mLock->lock();
	OmnString fname = getFileNameLocked(rawname);
	aos_assert_rl(fname != "", mLock, false);

	u64 id = encodeId(mDeviceNo, mCtrlDoc.cCrtDirSeqno, mCtrlDoc.cCrtFileSeqno);
	mLock->unlock();

	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);

	file->lock();
	file->put(0, buff->data(), buff->dataLen(), true);
	file->unlock();

	doc->setAttr("zky_binarydoc_id", id);
	saveCtrlDoc(doc);
	return true;
}

u64
AosRawFiles::encodeId(const u16 &dirno, const u16 &seqno, const u32 &fileno)
{
	u64 value = dirno;
	value = value << 48;
	u64 vv = seqno;
	vv = (vv << 32) | fileno;
	value = value | vv;


u32 fileno1 = (u32) value;
u16 seqno1 = (u16) (value >> 32);
u16 dirno1 = (u16) (value >> 48);
aos_assert_r(fileno == fileno1, false);
aos_assert_r(seqno1 == seqno, false);
aos_assert_r(dirno1 == dirno, false);
	return value;
}


void
AosRawFiles::decodeId(const u64 &id, u16 &seqno, u32 &fileno)
{
	fileno = (u32) id;
	seqno = (u16) (id >> 32);
	u16 dirno = (u16) (id >> 48);
	aos_assert(mDeviceNo == dirno);
}


AosBuffPtr
AosRawFiles::getFile(const AosXmlTagPtr &doc, const u64 &id, const AosRundataPtr &rdata)
{
	OmnString rawname = doc->getAttrStr("source_fname");
	aos_assert_r(rawname != "", 0);
	int startidx = rawname.find('/', true);
	rawname = rawname.substr(startidx+1);
	
	OmnString fname = getFileName(rawname, id);
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file->isGood())
	{
		OmnAlarm << "file is not good: " << fname << enderr;
		return NULL;
	}

	u64 size = file->getLength();
	AosBuffPtr buff = OmnNew AosBuff(size AosMemoryCheckerArgs); 
	int64_t len = file->readToBuff(0, size, buff->data());
	buff->setDataLen(len);
	return buff;
}


OmnString
AosRawFiles::getFileName(const OmnString &rawname, const u64 &id)
{
	u32 file_seqno;
	u16 dir_seqno;
	decodeId(id, dir_seqno, file_seqno);

	char addzero[16];
	sprintf(addzero, "%05d", file_seqno);
	OmnString fseqno(addzero);
	
	OmnString basepath = mBaseDirName;
	basepath << "/" << dir_seqno << "/" << fseqno << "_" << rawname;
	return basepath;
}


OmnString 
AosRawFiles::getFileNameLocked(const OmnString &rawname)
{
	mCtrlDoc.cCrtFileSeqno++;
	OmnString fname;
	if(mCtrlDoc.cCrtFileSeqno > eMaxFiles)
	{
		mCtrlDoc.cCrtDirSeqno++;
		mCtrlDoc.cCrtFileSeqno = 1;
		mCtrlDoc.xOffset = 0;
		OmnString path = mBaseDirName;
		OmnString newdirname = path << "/" << mCtrlDoc.cCrtDirSeqno;
		OmnString cmd = "mkdir -p ";
		cmd << newdirname;
		system(cmd.data());	
		bool r = readCtrlDoc();
		if(!r)
		{
			OmnAlarm << "Failed to get control doc!" << enderr;	
		}
	}
	DIR *dir;
	OmnString basepath = mBaseDirName;
	OmnString dirname = basepath << "/" << mCtrlDoc.cCrtDirSeqno;
	aos_assert_r((dir = opendir(dirname.data())) != NULL, "");
	closedir(dir);
	char addzero[16];
	sprintf(addzero, "%05d", mCtrlDoc.cCrtFileSeqno);
	OmnString seqno(addzero);
	fname << dirname << "/" << seqno << "_" << rawname;
	return fname;
}


bool
AosRawFiles::closeCtrlFile()
{
	return true;
}


bool
AosRawFiles::saveCtrlDoc(const AosXmlTagPtr &doc)
{
	aos_assert_r(mXmlFile && mXmlFile->isGood(), false);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	int length = doc->getDataLength();

	mXmlFile->lock();
	mXmlFile->setU64(mCtrlDoc.xOffset, docid, false);
	mXmlFile->setInt(mCtrlDoc.xOffset+sizeof(u64), length, false);
	mXmlFile->append(doc->toString());

	mCtrlDoc.xOffset += sizeof(u64) + sizeof(int) + length;
	mXmlFile->unlock();
	return true;
}


bool
AosRawFiles::readCtrlDoc()
{
	OmnString basepath = mBaseDirName;
	OmnString dirname = basepath << "/" << mCtrlDoc.cCrtDirSeqno << "/";
	OmnString fname = dirname << mCtrlDoc.xFname;

	mXmlFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (mXmlFile && !mXmlFile->isGood())
	{
		mXmlFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	}
	aos_assert_r(mXmlFile && mXmlFile->isGood(), false);
	return true;
}
