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
#if 0
#include "VirtualFile/Testers/ActVFTester.h"
#include "VirtualFile/VirtualFile.h"

#include "VirtualFile/Ptrs.h"
#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbRecord.h"
#include "Database/DbTable.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "IILClient/Ptrs.h"
#include "IILAssembler/IILAssembler.h"
#include "Job/Job.h"
#include "QueryUtil/QueryContext.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosVFTester::AosVFTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mName = "ActUnicomTester";
	mRdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
}


AosVFTester::~AosVFTester()
{
}


bool 
AosVFTester::start()
{
	cout << "Start	Virtfile test..." << endl;
	basicTest();
	return true;
}


bool 
AosVFTester::BasicTest()
{
	while(1)
	{
		config();
		clearData();
		createFiles();
		splitFiles();
		readFilesBlocks();
		compareResults();
		testXmlConfigRead();
		testSegmentRead();
		sleep(1);
	}
	return true;
}


bool 
AosVFTester::config()
{
	mSegmentSize = OmnRandom::nextInt(1, 2000);
	mLastSegmentSize = OmnRandom::nextInt(1, mSegmentSize);
	mNumSegment = OmnRandom::nextInt(1, 50);
	mFileSize = (mNumSegment - 1) * mSegmentSize;
	mAppendFile = OmnNew AosVirtualFile("ApendTestFile.txt", AosVirtualFile::eCreate, mRdata);
	return true;
}


bool 
AosVFTester::splitFiles()
{
	aos_assert_r(mVirtualFile, false);
	
	int64_t filesize = 0;
	mAppendFileData = "";

	while (filesize < mFileSize)
	{
		OmnString str = "";
		// Randomly create some contents and append the contents to the file.
		mApendFileData->appendBlock(str);
		mApendFileData << str;
	}
	
	
	aos_assert_r(mVirtualFile, false);
	return VirtualFile->splitFile(mRundata);
}


bool
AosVFTester::createFiles()
{
	switch (rand() % mTotalSplitMethods)
	{
	case eNoSplit:
		 return createSingleFile();

	case eEvenlyDistribute:
		 return evenlyDistribute();

	default:
		 OmnAlarm << "Invalid method: " << enderr;
		 return false;
	}

	/*
	mFileData = "";
	for(int i 0 ; i < mNumSegment - 1; i++)
	{
		OmnString segmentFileData = OmnRandom::binaryStr(mSegmentSize, mSegmentSize);
		mFileData << segmentFileData;
		mSegmentData.push_back(segmentFileData);
		
	}
	OmnString segmentFileData = OmnRandom::binaryStr(mLastSegmentSize, mLastSegmentSize);
	mFileData << segmentFileData;
	mSegmentData.push_back(segmentFileData);

	OmnFilePtr file = OmnNew OmnFile("File.txt", OmnFile::eCreate);
	if( file && file->isGood() )
	{
		file->put(0, segmentFileData.data(), segmentFileData.length(), true);
		file->close();
	}
	else
	{
		OmnScreen << "file error ." << endl;
		exit(1);
	}
	return true;
	*/
}


bool
AosVFTester::clearData()
{
	OmnFilePtr file = OmnNew OmnFile("File.txt", OmnFile::eReadWrite);
	if( file && file->isGood() )
	{
		file->deleteFile();
	}

	mSegmentDataRead.clear();
	mSegmentData.clear();
	mFileData.clear();
}


bool
AosVFTester::compareResults()
{
	/*
	if(mSegmentDataRead.size() != mSegmentData.size()|| mSegmentData.size() != mNumSegment)
	{
		OmnScreen << "Numbers of seg error. Check it." << endl;
		exit(1);
	}

	for(int i = 0;i< mSegmentData.size(); i++)
	{
		if((mSegmentData[i]) !=  (mSegmentDataRead[i]))
		{
			OmnScreen << "Segment:" << i << "seg error. Check it." << endl;
			exit(1);
		}
	}
	*/

	if (mFileDataRead!=mFileData)
	{
		OmnScreen << "File read error. Check it." << endl;
		exit(1);
	}
}


bool
AosVFTester::readFilesBlocks()
{
	mFileDataRead = "";
	AosSyncTokenPtr fileV = OmnNew AosVirtualFile(mObjid, AosVirtualFile::eReadOnly, mRdata);
	AosBuffPtr buff = OmnNew AosBuff(100000000, 100000000);
	while(fileV->getNextBlock(buff, mRdata))		// the file must return false at the end .
	{
		mFileDataRead << buff.data();		// need confire . may be a bug .
	}

	return true;
}


bool
AosVFTester::testXmlConfigRead()
{
	OmnNotImplementedYet;
	return false;
	return true;
}


bool
AosVFTester::testSegmentRead()
{
	OmnNotImplementedYet;
	return false;
	return true;
}
#endif
