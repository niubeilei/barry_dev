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
// How to torture: 
// 1. Create a table of records randomly.
// 2. Determine one column as the 'driving' column. This is the matched key.
// 3. Separate one or more columns into one IIL. 
//   
//
// Modification History:
// 06/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "VirtualFile/Testers/VirtualFileTester.h"

#include "API/AosApi.h"
#include "Random/RandomUtil.h"
#include "Tester/TestMgr.h"
#include "VirtualFile/VirtualFile.h"
#include "VirtualFile/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

AosVirtualFileTester::AosVirtualFileTester()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);
	mRundata->setCid("307");
	mRundata->setUserid(307);
	aos_assert(createFiles());
}

 
AosVirtualFileTester::~AosVirtualFileTester()
{
}


bool 
AosVirtualFileTester::createFiles()
{
	// This if a record file.
	mRcdFile = OmnNew OmnFile("/home/brian/Testers/file1.txt", OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!mRcdFile || !mRcdFile->isGood())
	{
		mRcdFile = OmnNew OmnFile("/home/brian/Testers/file1.txt", OmnFile::eCreate);
		while (true)
		{
			OmnString str = OmnRandom::digitStr(20);
			aos_assert_r(str != "", false);
			str << " ";
			mRcdFile->append(str);
			if (mRcdFile->getLength() > 2000000000) break;
		}
		mRcdFile->flushFileContents();
	}
	if (!mRcdFile) return false;

	// This if a split file.
	mSplitFile = OmnNew OmnFile("/home/brian/Testers/file2.txt", OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!mSplitFile || !mSplitFile->isGood())
	{
		mSplitFile = OmnNew OmnFile("/home/brian/Testers/file2.txt", OmnFile::eCreate);
		while (true)
		{
			OmnString str = OmnRandom::digitStr(20);
			aos_assert_r(str != "", false);
			str << "|";
			mSplitFile->append(str);
			if (mSplitFile->getLength() > 2000000000) break;
		}
		mSplitFile->flushFileContents();
	}
	if (!mSplitFile) return false;

	mNormalFile = OmnNew OmnFile("/home/brian/Testers/file3.txt", OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!mNormalFile || !mNormalFile->isGood())
	{
		mNormalFile = OmnNew OmnFile("/home/brian/Testers/file3.txt", OmnFile::eCreate);
		while (true)
		{
			OmnString str = OmnRandom::digitStr(20);
			aos_assert_r(str != "", false);
			mNormalFile->append(str);
			if (mNormalFile->getLength() > 2000000000) break;
		}
		mNormalFile->flushFileContents();
	}
	if (!mNormalFile) return false;

	return true;
}

bool 
AosVirtualFileTester::start()
{
	cout << "Start	Virtfile test..." << endl;
	basicTest();
	return true;
}


bool 
AosVirtualFileTester::basicTest()
{
	int i=0;
	while(1)
	{
OmnScreen << "Tries: " << i++ << endl;
		aos_assert_r(createVFile(mRundata), false);
		aos_assert_r(retrVFileAndAppend(mRundata), false);
		aos_assert_r(retrVFileAndImport(mRundata), false);
		aos_assert_r(exportFile(mRundata), false);
		aos_assert_r(retrieveBlock(mRundata), false);
		aos_assert_r(checkBlock(mRundata), false);
	}
	return true;
}


bool 
AosVirtualFileTester::createVFile(const AosRundataPtr &rdata)
{
	int idx = 2;
	for (int i=0; i<idx; i++)
	{
		OmnString objid = OmnRandom::letterStr(1, 10);
		if (mObjidMap.find(objid) != mObjidMap.end()) continue;
		OmnString seg_type = getSegmentType();
		AosXmlTagPtr conf = initConfig(objid, seg_type, rdata);
		aos_assert_r(conf, false);
		AosVirtualFilePtr vf = OmnNew AosVirtualFile(conf, 0, rdata);
		aos_assert_r(vf, false);
		mObjidMap[objid] = conf;
		switch(rand()%4)
		{
		case 0:
			aos_assert_r(importFile(seg_type, vf, rdata), false);
			break;
		case 1:
			aos_assert_r(appendFile(seg_type, vf, rdata), false);
			break;
		case 2:
			aos_assert_r(appendAndImportFile(seg_type, vf, rdata), false);
			break;
		case 3:
			aos_assert_r(importAndAppendFile(seg_type, vf, rdata), false);
			break;
		default:
			OmnAlarm << "Inter error" << enderr;
			return false;
		}
	}
	return true;
}


OmnString
AosVirtualFileTester::getSegmentType()
{
	int idx = rand() % 3;
	switch(idx)
	{
	case 0:
		return AOSSEGMENTTYPE_BYRECORDLEN;
	case 1:
		return AOSSEGMENTTYPE_BYSEGMENT_SIZE;
	case 2:
		return AOSSEGMENTTYPE_BYLINEDELIMITRT;
	}

	return "";
}

AosXmlTagPtr 
AosVirtualFileTester::initConfig(
		const OmnString &objid,
		const OmnString &seg_type,
		const AosRundataPtr &rdata)
{
	aos_assert_r(objid != "", 0);	
	OmnString str = "<virfile ";
	str << "zky_vfobjid=\"" << objid << "\" "
		<< "zky_blkodrmode=\"false\" "
		<< "zky_is_public=\"true\" "
		<< "zky_need_compress=\"true\" "
		<< "zky_compress_type=\"gzip\" "
		<< "zky_seg_size=\"" << getSegmentSize() << "\" "
		<< "segment_type=\"" << seg_type << "\" "
		<< "zky_rcdlen=\"" << 21 << "\">"                 
		<< "<zky_sep><![CDATA[|]]></zky_sep>"
		<< "</virfile>";
	AosXmlParser parser;                                                        
	AosXmlTagPtr conf = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(conf, 0);
	return conf;
}


int
AosVirtualFileTester::getSegmentSize()
{
	return 120000000;
}

bool
AosVirtualFileTester::importFile(
		const OmnString &seg_type,
		const AosVirtualFilePtr &file,
		const AosRundataPtr &rdata)
{
	aos_assert_r(file, false);
OmnScreen << "ImportFile: " << seg_type << endl;	
	if (seg_type == AOSSEGMENTTYPE_BYRECORDLEN) return file->importFile(mRcdFile->getFileName(), rdata);
	if (seg_type == AOSSEGMENTTYPE_BYSEGMENT_SIZE) return file->importFile(mNormalFile->getFileName(), rdata);
	if (seg_type == AOSSEGMENTTYPE_BYLINEDELIMITRT) return file->importFile(mSplitFile->getFileName(), rdata);
	return false;
}


bool
AosVirtualFileTester::appendFile(
		const OmnString &seg_type,
		const AosVirtualFilePtr &file,
		const AosRundataPtr &rdata)
{
	aos_assert_r(file, false);
OmnScreen << "AppendFile: " << seg_type << endl;	
	
	if (seg_type == AOSSEGMENTTYPE_BYRECORDLEN) 
	{
		return appendFileByRcd(mRcdFile, file, rdata);
	}
	else if (seg_type == AOSSEGMENTTYPE_BYSEGMENT_SIZE) 
	{
		return appendFileBySize(mNormalFile, file, rdata);
	}
	else if (seg_type == AOSSEGMENTTYPE_BYLINEDELIMITRT) 
	{
		return appendFileBySpliter(mSplitFile, file, rdata);
	}

	return false;
}


bool
AosVirtualFileTester::appendFileByRcd(
		const OmnFilePtr &file,
		const AosVirtualFilePtr &vf,
		const AosRundataPtr &rdata)
{
	aos_assert_r(file && file->isGood(), false);
	aos_assert_r(vf, false);
	
	u64 startpos = 0;
	while (true)
	{
		AosBuffPtr buff = OmnNew AosBuff(128000010, AosMemoryCheckerArgsBegin);
		aos_assert_r(buff, false);
		int64_t readsize = file->readToBuff(startpos, 128000000, buff->data());
		if (readsize <= 0) break;
		buff->setDataLen(readsize);
		int pos = 0;
		while (true)
		{
			AosBuffPtr newbuff = OmnNew AosBuff(&buff->data()[pos], 30, 21, 0, AosMemoryCheckerArgsBegin);
			aos_assert_r(newbuff, false);
			aos_assert_r(vf->appendData(newbuff, rdata), false);
			if (pos + 21 > buff->dataLen()) break;
			pos += 21;
		}
		if (readsize < 1000000) break;
		startpos += readsize;
	}
	return true;
}


bool
AosVirtualFileTester::appendFileBySize(
		const OmnFilePtr &file,
		const AosVirtualFilePtr &vf,
		const AosRundataPtr &rdata)
{
	aos_assert_r(file && file->isGood(), false);
	aos_assert_r(vf, false);
	
	u64 startpos = 0;
	while (true)
	{
		AosBuffPtr buff = OmnNew AosBuff(128000010, AosMemoryCheckerArgsBegin);
		aos_assert_r(buff, false);
		int64_t readsize = file->readToBuff(startpos, 128000000, buff->data());
		if (readsize <= 0) break;
		buff->setDataLen(readsize);
		int pos = 0;
		while (true)
		{
			u64 size = rand() % 10000 + 20;
			if (pos + size >= readsize) size = readsize - pos;
			if (size <= 0) break;
			AosBuffPtr newbuff = OmnNew AosBuff(&buff->data()[pos], size+10, size, 0, AosMemoryCheckerArgsBegin);
			aos_assert_r(newbuff, false);
			aos_assert_r(vf->appendData(newbuff, rdata), false);
			if ((u64)pos + size > (u64)(buff->dataLen())) break;
			pos += size;
		}
		if (readsize < 1000000) break;
		startpos += readsize;
	}
	return true;
}


bool
AosVirtualFileTester::appendFileBySpliter(
		const OmnFilePtr &file,
		const AosVirtualFilePtr &vf,
		const AosRundataPtr &rdata)
{
	aos_assert_r(file && file->isGood(), false);
	aos_assert_r(vf, false);

	u64 startpos = 0;
	while (true)
	{
		AosBuffPtr buff = OmnNew AosBuff(128000010, AosMemoryCheckerArgsBegin);
		aos_assert_r(buff, false);
OmnScreen << "Read File: " << file->getFileName() << " : " << startpos << endl;
		int64_t readsize = file->readToBuff(startpos, 128000000, buff->data());
		if (readsize <= 0) break;
		buff->setDataLen(readsize);
		u64 pos = 0;
		u64 rcdpos = 0;
		while (true)
		{
			AosBuffPtr newbuff = getSplitBuff(pos, rcdpos, buff);
			if (!newbuff) break;
			aos_assert_r(vf->appendData(newbuff, rdata), false);
		}
		if (readsize < 1000000) break;
		startpos = (startpos + rcdpos);
	}
	return true;
}


AosBuffPtr
AosVirtualFileTester::getSplitBuff(
		u64 &startpos,
		u64 &rcdpos,
		const AosBuffPtr &buff)
{
	int size = 0;
	u64 pos = startpos;
	u64 len = buff->dataLen();
	aos_assert_r(len > 0, 0);
	while (true)
	{
		if ((startpos < len) && (buff->data()[startpos] != '|')) 
		{
			startpos ++;
			size ++;
		}
		else
		{
			if (startpos >= len) return 0;
			rcdpos = startpos + 1;
			break;
		}
	}
if (size != 20)
	OmnMark;
	AosBuffPtr newbuff = OmnNew AosBuff(&buff->data()[pos], size+10, size, 0, AosMemoryCheckerArgsBegin);
	aos_assert_r(newbuff, 0);
	startpos ++;
	return newbuff;
}


bool
AosVirtualFileTester::appendAndImportFile(
		const OmnString &seg_type,
		const AosVirtualFilePtr &file,
		const AosRundataPtr &rdata)
{
	if (seg_type == AOSSEGMENTTYPE_BYRECORDLEN)
	{
		aos_assert_r(appendFile(seg_type, file, rdata), false);
		return file->importFile(mRcdFile->getFileName(), rdata);
	}
	if (seg_type == AOSSEGMENTTYPE_BYSEGMENT_SIZE) 
	{
		aos_assert_r(appendFile(seg_type, file, rdata), false);
		return file->importFile(mNormalFile->getFileName(), rdata);
	}
	if (seg_type == AOSSEGMENTTYPE_BYLINEDELIMITRT) 
	{
		aos_assert_r(appendFile(seg_type, file, rdata), false);
		return file->importFile(mSplitFile->getFileName(), rdata);
	}

	return false;
}


bool
AosVirtualFileTester::importAndAppendFile(
		const OmnString &seg_type,
		const AosVirtualFilePtr &file,
		const AosRundataPtr &rdata)
{
	if (seg_type == AOSSEGMENTTYPE_BYRECORDLEN)
	{
		aos_assert_r(file->importFile(mRcdFile->getFileName(), rdata), false);
		return appendFile(seg_type, file, rdata);
	}
	if (seg_type == AOSSEGMENTTYPE_BYSEGMENT_SIZE) 
	{
		aos_assert_r(file->importFile(mNormalFile->getFileName(), rdata), false);
		return appendFile(seg_type, file, rdata);
	}
	if (seg_type == AOSSEGMENTTYPE_BYLINEDELIMITRT) 
	{
		aos_assert_r(file->importFile(mSplitFile->getFileName(), rdata), false);
		return appendFile(seg_type, file, rdata);
	}

	return false;
}

bool
AosVirtualFileTester::retrVFileAndAppend(const AosRundataPtr &rdata)
{
OmnScreen << "Retrieve And Append: " << endl;
	int idx = rand() % mObjidMap.size();
	map<OmnString, AosXmlTagPtr>::iterator itr = mObjidMap.begin();
	for (int i=0; i<idx; i++) itr++;
	AosVirtualFilePtr vf = OmnNew AosVirtualFile(itr->first, rdata);
	aos_assert_r(vf, false);
	
	vf->changeMode();	

	AosVirtualFile::SegmentType type = vf->getSegmentType();
	switch (type)
	{
	case AosVirtualFile::eSegmentType_ByRecordLen:
		return appendFileByRcd(mRcdFile, vf, rdata);

	case AosVirtualFile::eSegmentType_BySegmentSize:
		return appendFileBySize(mNormalFile, vf, rdata);
	
	case AosVirtualFile::eSegmentType_ByLineDelimiter:
		return appendFileBySpliter(mSplitFile, vf, rdata);
	default:
		OmnAlarm << "Wrong segment type" << enderr;
		return false;
	}

	return false;
}

	
bool
AosVirtualFileTester::retrVFileAndImport(const AosRundataPtr &rdata)
{
OmnScreen << "Retrieve And Import: " << endl;
	int idx = rand() % mObjidMap.size();
	map<OmnString, AosXmlTagPtr>::iterator itr = mObjidMap.begin();
	for (int i=0; i<idx; i++) itr++;
	AosVirtualFilePtr vf = OmnNew AosVirtualFile(itr->first, rdata);
	aos_assert_r(vf, false);
	
	vf->changeMode();	

	OmnString typestr;
	AosVirtualFile::SegmentType type = vf->getSegmentType();
	switch (type)
	{
	case AosVirtualFile::eSegmentType_ByRecordLen:
		typestr = AOSSEGMENTTYPE_BYRECORDLEN;
		break;

	case AosVirtualFile::eSegmentType_BySegmentSize:
		typestr = AOSSEGMENTTYPE_BYSEGMENT_SIZE;
		break;
	case AosVirtualFile::eSegmentType_ByLineDelimiter:
		typestr = AOSSEGMENTTYPE_BYLINEDELIMITRT;
		break;
	default:
		OmnAlarm << "Wrong segment type" << enderr;
		return false;
	}
	
	return importFile(typestr, vf, rdata);
}

	
bool
AosVirtualFileTester::exportFile(const AosRundataPtr &rdata)
{
	u64 file_len;
	int idx = rand() % mObjidMap.size();
	map<OmnString, AosXmlTagPtr>::iterator itr = mObjidMap.begin();
	for (int i=0; i<idx; i++) itr++;
	AosVirtualFilePtr vf = OmnNew AosVirtualFile(itr->first, rdata);
	aos_assert_r(vf, false);
	aos_assert_r(vf->exportFile(file_len, "/home/brian/Testers/export.txt", rdata), false);
	OmnString str = "rm -rf /home/brian/Testers/export.txt";
	system(str.data());
	return true;
}

	
bool
AosVirtualFileTester::retrieveBlock(const AosRundataPtr &rdata)
{
OmnScreen << "RetrieveBlock" << endl;
	int idx = rand() % mObjidMap.size();
	map<OmnString, AosXmlTagPtr>::iterator itr = mObjidMap.begin();
	for (int i=0; i<idx; i++) itr++;
	AosVirtualFilePtr vf = OmnNew AosVirtualFile(itr->first, rdata);
	aos_assert_r(vf, false);
	u64 size = vf->getSegmentSize();
	aos_assert_r(size > 0, false);

	AosBuffPtr buff = OmnNew AosBuff(size, AosMemoryCheckerArgsBegin);
	aos_assert_r(buff, false);

	bool finished;
	aos_assert_r(vf->getFirstSegment(buff, rdata), false);
	while (!finished)
	{
		if (!vf->nextSegment(buff, finished, rdata)) break;
	}
	return true;
}

	
bool
AosVirtualFileTester::checkBlock(const AosRundataPtr &rdata)
{
OmnScreen << "Check Block" << endl;
	int idx = rand() % mObjidMap.size();
	map<OmnString, AosXmlTagPtr>::iterator itr = mObjidMap.begin();
	for (int i=0; i<idx; i++) itr++;
	AosVirtualFilePtr vf = OmnNew AosVirtualFile(itr->first, rdata);
	aos_assert_r(vf, false);
	u64 size = vf->getSegmentSize();
	aos_assert_r(size > 0, false);

	AosBuffPtr buff = OmnNew AosBuff(size+10, AosMemoryCheckerArgsBegin);
	aos_assert_r(buff, false);

	vector<u64> docids = vf->getAllSegment(rdata);
	aos_assert_r(docids.size() > 0, false);

	for (int i=0; i< docids.size(); i++)
	{
		AosXmlTagPtr doc = AosRetrieveBinaryDoc(docids[i], buff, rdata);
		aos_assert_r(doc, false);
	}
	return true;
}
