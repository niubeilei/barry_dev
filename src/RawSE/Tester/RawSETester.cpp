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
// 1. It maintains a list of seeds of fixed length for each file.
// 2. When adding new data to a file, it randomly determines the number of
//    seeds to add: "0123456789". "222222222222" "33333"
//    files[0] = "xxxx";
//    files[1] = "xxxx";
//
//
// Modification History:
// 2014/11/28 Created by White
////////////////////////////////////////////////////////////////////////////
#include "RawSE/Tester/RawSETester.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Random/RandomUtil.h"
#include "RawSE/RawFile.h"
#include "RawSE/RawSE.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include <arpa/inet.h>
#include <limits.h>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

extern AosXmlTagPtr	gApp_config;
extern i64			gTestDuration;
extern u32			gRandomSeed;
AosBuffPtr	AosRawSETester::ptrBuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);
AosRawSETester::AosRawSETester()
{
	mName = "RawSETester";
	mTries = gApp_config->getAttrInt("tries", 100);
	mBatchSize = gApp_config->getAttrInt("batch_size", 10);
	mMaxSiteID = gApp_config->getAttrU32("max_siteid", 10);
	mMaxCubeID = gApp_config->getAttrU32("max_cubeid", 10);
	mMaxAseID = gApp_config->getAttrU64("max_aseid", 10);
	mSeedCount = gApp_config->getAttrU32("seed_count", 100);
	mRundata = OmnNew AosRundata();
	mRawSE = OmnNew AosRawSE();
	mFile = OmnNew OmnFile(AosMemoryCheckerArgsBegin);
	mTestDurationSec = gTestDuration;
	mTestCount = 1;
	muFileCreated = 0;
	mReadTestCount = 0;
	mCreateTestCount = 0;
	mDelTestCount = 0;
	mAppendTestCount = 0;
	mReadFailedCount = 0;
	mCreateFailedCount = 0;
	mDelFailedCount = 0;
	mAppendFailedCount = 0;
}


bool
AosRawSETester::start()
{
	return basicTest();
}


bool
AosRawSETester::basicTest()
{
	if (!loadSeed())
	{
		OmnScreen << "loadSeed failed." << endl;
		return false;
	}
	if (0 < gRandomSeed)
	{
		OmnScreen << "Seed for this trial: '" << gRandomSeed << "'" << endl;
		OmnRandom::setSeed(gRandomSeed);
	}
	else
	{
		u32 uTime = time(NULL);
		OmnScreen << "Seed for this trial: '" << uTime << "'" << endl;
		OmnRandom::setSeed(uTime);
	}

	OmnScreen << "Weights for this trial, read:'" << gApp_config->getAttrInt("read_weight", 25)
			<< "' create:'" << gApp_config->getAttrInt("create_weight", 25)
			<< "' append:'" << gApp_config->getAttrInt("append_weight", 25)
			<< "' delete:'" << gApp_config->getAttrInt("delete_weight", 25) << "'" << endl;

	if (mTestDurationSec > 0)
	{
		// Run the torturer for 'mTestDuration' amount of time.
		i64 crt_sec = OmnTime::getCrtSec();
//		i64 crt_sec = OmnGetSecond();
		while (OmnTime::getSecTick() < crt_sec + mTestDurationSec)
		{
			runOneTest();
		}
	}
	else
	{
		if (mTries <= 0) mTries = eDefaultTries;
		for (int i=0; i<mTries; i += mBatchSize)
		{
			oneBatchTests(mBatchSize);
		}
	}

	OmnScreen << "Read Test Count:" << mReadTestCount << " Failed Count:" << mReadFailedCount << endl;
	OmnScreen << "Create Test Count:" << mCreateTestCount << " Failed Count:" << mCreateFailedCount << endl;
	OmnScreen << "Append Test Count:" << mAppendTestCount << " Failed Count:" << mAppendFailedCount << endl;
	OmnScreen << "Del Test Count:" << mDelTestCount << " Failed Count:" << mDelFailedCount << endl;
	OmnScreen << "Total Test Count:" << mReadTestCount + mCreateTestCount + mAppendTestCount + mDelTestCount
			<< " Total Failed Count:" << mReadFailedCount + mCreateFailedCount + mAppendFailedCount + mDelFailedCount << endl;

	return true;
}


bool
AosRawSETester::oneBatchTests(const int batch_size)
{
	for (int i=0; i<batch_size; i++)
	{
		switch (OmnRandom::intByRange(
				0, 0, gApp_config->getAttrInt("create_weight", 25),
				1, 1, gApp_config->getAttrInt("append_weight", 25),
				2, 2, gApp_config->getAttrInt("read_weight", 25),
				3, 3, gApp_config->getAttrInt("delete_weight", 25)))
		{
		case 0:
			mCreateTestCount++;
			if (!testCreateFile())
			{
				mCreateFailedCount++;
			}
			 break;

		case 1:
			mAppendTestCount++;
			if (!testAppendData())
			{
				mAppendFailedCount++;
			}
			 break;

		case 2:
			mReadTestCount++;
			if (!testReadData())
			{
				mReadFailedCount++;
			}
			 break;

		case 3:
			mDelTestCount++;
			if (!testDeleteFile())
			{
				mDelFailedCount++;
			}
			 break;

		default:
			 OmnShouldNeverComeHere;
			 break;
		}
	}

	return true;
}


bool
AosRawSETester::testCreateFile()
{
	// This function creates a raw file.
	OmnScreen << "Test No." << mTestCount++ << ":file creation." << endl;
	u32 siteid = pickSiteid();
	if(0 >= siteid)
	{
		OmnScreen << "pickSiteid failed." << endl;
		return false;
	}
	u32 cubeid = pickCubeid();
	if(0 >= cubeid)
	{
		OmnScreen << "pickCubeid failed." << endl;
		return false;
	}
	u64 aseid  = pickAseid();
	if(0 >= aseid)
	{
		OmnScreen << "pickAseid failed." << endl;
		return false;
	}
	u64 rawfid = 0;
	AosRawFilePtr raw_file = mRawSE->createRawFile(mRundata.getPtrNoLock(), siteid, cubeid, aseid,
			AosMediaType::eDisk, AosRawFile::eReadWrite, rawfid);
	if (raw_file.isNull())
	{
		OmnScreen << "file creation failed, rawfid allocted:'" << rawfid << "'" << endl;
		return false;
	}
//	OmnScreen << "File No." << ++muFileCreated << " '" << raw_file->getFileNameWithFullPath()
//			  << "' @" << raw_file.getPtr() << " created with rawfid:'" << rawfid << "' siteid:'" << siteid
//			  << "' cubeid:'" << cubeid << "' aseid:'" << aseid  << "'" << endl;
	mvFile.push_back(AosRawFile::Identifier(siteid, cubeid, aseid, rawfid));
	return true;
}


bool
AosRawSETester::testAppendData()
{
	//	randomly pick a file;
	//	randomly pick a repeat;
	//	append data;
	OmnScreen << "Test No." << mTestCount++ << ":appending data." << endl;
	int iRet = 0;
	if (0 >= mvFile.size())	//no file has been created yet
	{
		OmnScreen << "appending canceled because no file has been created yet" << endl;
		return true;
	}
	AosRawFile::Identifier raw_file = mvFile[rand() % mvFile.size()];
//	OmnScreen << "Trying to append data to file with rawfile id:'" << raw_file.rawFileID
//			  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
	OmnString data = createData(raw_file);
	AosRawFilePtr rawfile = mRawSE->getRawFile(mRundata.getPtrNoLock(), raw_file.siteID,
			raw_file.cubeID, raw_file.aseID, raw_file.rawFileID, AosRawFile::eReadWrite);
	if (!rawfile)
	{
		OmnScreen << "Failed to get raw file obj for appending with rawfile id:'" << raw_file.rawFileID
				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
		return false;
	}
//	OmnScreen << "Trying to append data to file '" << rawfile->getFileNameWithFullPath() << "'" << endl;
	if (0 != (iRet = rawfile->append(mRundata.getPtrNoLock(), data.getBuffer(), data.length())))
	{
		OmnScreen << "can not append to file with rawfile id:'" << raw_file.rawFileID << "' siteid '" << raw_file.siteID
				<< "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "' iRet=" << iRet << endl;
		return false;
	}
//	OmnScreen << "data appended to file with rawfile id:'" << raw_file.rawFileID
//			<< "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
//	for (v_seed_repeat_t::iterator it = mRawFileSeedMap[raw_file].begin(); it != mRawFileSeedMap[raw_file].end(); it++)
//	{
//		OmnScreen << "seed id:" << it->first << " repeat time:" << it->second <<" seed content:'" << mSeedMap[it->first] << "'" << endl;
//	}
	return true;
}


bool
AosRawSETester::testDeleteFile()
{
	OmnScreen << "Test No." << mTestCount++ << ":file deletion." << endl;
	int iRet = 0;
	if (0 >= mvFile.size())	//no file has been created yet
	{
		OmnScreen << "deleting canceled because no file has been created yet" << endl;
		return true;
	}
	u64 uFileIndex = rand() % mvFile.size();
	AosRawFile::Identifier raw_file = mvFile[uFileIndex];
//	OmnScreen << "Trying to delete file with rawfile id:'" << raw_file.rawFileID
//			  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
	AosRawFilePtr rawfile = mRawSE->getRawFile(mRundata.getPtrNoLock(), raw_file.siteID,
			raw_file.cubeID, raw_file.aseID, raw_file.rawFileID, AosRawFile::eReadWrite);
	if (rawfile.isNull())
	{
		OmnScreen << "Failed to get raw file obj for check deletion result with rawfile id:'" << raw_file.rawFileID
				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
		return false;
	}
	struct stat buffer;
	if (0 != stat (rawfile->getFileNameWithFullPath().data(), &buffer))	//file does not exist
	{
		OmnScreen << "file '" << rawfile->getFileNameWithFullPath() << "' with rawfile id:'" << raw_file.rawFileID
				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID
				  << "' got from RawSE does not exist" << endl;
		return false;
	}
	else
	{
		if (0 != (iRet = mRawSE->deleteFile(mRundata.getPtrNoLock(), raw_file.siteID, raw_file.cubeID, raw_file.aseID, raw_file.rawFileID)))
		{
			OmnScreen << "can not delete file with rawfile id:'" << raw_file.rawFileID << "' siteid '" << raw_file.siteID
					<< "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "' iRet=" << iRet << endl;
			return false;
		}
		else
		{
			if (0 != stat(rawfile->getFileNameWithFullPath().data(), &buffer))	//file does not exist
			{
				mvFile.erase(mvFile.begin() + uFileIndex);
//				OmnScreen << "Deletion succeeded." << endl;
				return true;
			}
			else
			{
				OmnScreen << "Deletion failed for file '" << rawfile->getFileNameWithFullPath() << "' with rawfile id:'" << raw_file.rawFileID
				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << endl;
				return false;
			}
		}
	}
	return true;
}


bool
AosRawSETester::testReadData()
{
//	randomly pick a file;
//	randomly determine read position
//	randomly determine read length;
	OmnScreen << "Test No." << mTestCount++ << ":reading data." << endl;
	if (0 >= mvFile.size())	//no file has been created yet
	{
//		OmnScreen << "reading canceled because no file has been created yet" << endl;
		return true;
	}
	AosRawFile::Identifier raw_file = mvFile[rand() % mvFile.size()];
	AosRawFilePtr rawfile = mRawSE->getRawFile(mRundata.getPtrNoLock(), raw_file.siteID,
			raw_file.cubeID, raw_file.aseID, raw_file.rawFileID, AosRawFile::eReadWrite);
	if (!rawfile)
	{
		OmnScreen << "Failed to get raw file obj for reading with rawfile id:'" << raw_file.rawFileID
				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
		return false;
	}
	u64 ullOffset = rand();
	u64 ullReadLen = rand() % eMaxBuffSize;
//	OmnScreen << "Trying to read data from file '" << rawfile->getFileNameWithFullPath()
//			<< "' wiht offset:'" << ullOffset << "' length:'" << ullReadLen << "'" << endl;
	int iRet = mRawSE->readRawFile(mRundata.getPtrNoLock(), raw_file.siteID,
			raw_file.cubeID, raw_file.aseID, raw_file.rawFileID, ullOffset, ullReadLen, ptrBuff);
	u64 ullFileLen = rawfile->getLength();
	if (ullFileLen < ullOffset)
	{
		if (0 == iRet)
		{
			OmnScreen << "iRet should not be 0." << endl;
			return false;
		}
		else
		{
			return true;
		}
	}

//	if (!rawfile->read(mRundata.getPtrNoLock(), ullOffset, ullReadLen, ptrBuff))
//	{
//		OmnScreen << "can not read data from file '" << rawfile->getFileNameWithFullPath() << "' with rawfile id:'" << raw_file.rawFileID
//				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
//		return false;
//	}
//	if(!rawfile->isGood())
//	{
//		OmnScreen << "last append to file with rawfile id:'" << raw_file.rawFileID
//				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
//		return false;
//	}

	//check the content
	//construct the content of the file
	OmnString sContent = "";
	itr_t it = mRawFileSeedMap.find(raw_file);
	if (mRawFileSeedMap.end() == it)	//the file has just been created and has no data in it yet
	{
		return true;
	}
	v_seed_repeat_t* vSeedPair = &it->second;
	v_seed_repeat_t::iterator v_it = vSeedPair->begin();
	u32 seed_id = v_it->first;
	u32 repeat_time = v_it->second;
	for (v_it = vSeedPair->begin(); v_it != vSeedPair->end(); v_it++)
	{
		seed_id = v_it->first;
		repeat_time = v_it->second;
		for (int i = 0; i < repeat_time; ++i)
		{
			sContent << mSeedMap[seed_id];
		}
	}

	//compare
	OmnString sShoulBe = sContent.subString(ullOffset, ullReadLen);
	if (0 != sShoulBe.compare1(ptrBuff->data()))
	{
		OmnScreen << "Read validation failed for file '" << rawfile->getFileNameWithFullPath() << "' with rawfile id:'" << raw_file.rawFileID
				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
		OmnScreen << "Content read from file:'" << ptrBuff->data() <<"'." << endl;
		OmnScreen << "Content that should be:'" << sShoulBe <<"'." << endl;
		OmnScreen << "File Content should be:'" << sContent <<"'." << endl;
		return false;
	}
	else
	{
//		OmnScreen << "Read validation passed for file '" << rawfile->getFileNameWithFullPath() << "' with rawfile id:'" << raw_file.rawFileID
//				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
		return true;
	}
	return true;
}


u32
AosRawSETester::pickSiteid()
{
	if (0 < mMaxSiteID)
	{
		return rand() % mMaxSiteID + 1;
	}
	else
	{
		return 0;
	}
}


u32
AosRawSETester::pickCubeid()
{
	if (0 < mMaxCubeID)
	{
		return rand() % mMaxCubeID + 1;
	}
	else
	{
		return 0;
	}
}


u64
AosRawSETester::pickAseid()
{
	if (0 < mMaxAseID)
	{
		return rand() % mMaxAseID + 1;
	}
	else
	{
		return 0;
	}
}


bool
AosRawSETester::runOneTest()
{
	return oneBatchTests(1);
}


u32
AosRawSETester::pickSeed()
{
	return rand() % mSeedCount;
}


bool
AosRawSETester::loadSeed()
{
	OmnString seed_file = gApp_config->getAttrStrSimp("seed_file", "");
	if (!mFile->openFile1(seed_file, OmnFile::eReadOnly))
	{
		OmnScreen << "can not open seed file '" << seed_file << "'" << endl;
		return false;
	}
	bool finished;
	for(u32 i = 0; i < mSeedCount; i++)
	{
		mSeedMap[i] = mFile->getLine(finished);
		if (finished)
		{
			break;
		}
	}
	mFile->closeFile();
	return true;
}


OmnString
AosRawSETester::createData(const AosRawFile::Identifier &raw_file)
{
	u32 seed_id = pickSeed();
	u32 repeat_time = rand() % 10 + 1;
	mRawFileSeedMap[raw_file].push_back(make_pair(seed_id, repeat_time));
	OmnString buff;
	for (int i = 0; i < repeat_time; ++i)
	{
		buff << mSeedMap[seed_id];
	}
	return buff;
}



