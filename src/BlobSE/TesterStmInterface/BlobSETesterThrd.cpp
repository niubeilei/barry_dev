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
// 2015-11-20 Created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/TesterStmInterface/BlobSETesterThrd.h"

#include "Alarm/Alarm.h"
#include "BlobSE/BlobSE.h"
#include "BlobSE/BlobSEAPI.h"
#include "Debug/Debug.h"
#include "DfmUtil/DfmDoc.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "UtilData/BlobSEReqEntry.h"
#include <arpa/inet.h>
#include <limits.h>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <vector>

extern i64					gTestDuration;

AosBlobSETesterThrd::AosBlobSETesterThrd(
		const u32					ulThreadID,
		AosBlobSEAPIPtr				&pBlobSE,
		AosBlobSETesterController*	pController)
:
mThreadID(ulThreadID)
{
	mLastCreatedDocid = mThreadID;
	mStartSec = OmnTime::getCrtSecond();
	AosXmlTagPtr pConf = OmnApp::getAppConfig();
	aos_assert(pConf.notNull());
	pConf = OmnApp::getAppConfig()->getFirstChild("BlobSETester");
	aos_assert(pConf.notNull());
	mSeedCount = pConf->getAttrU32("seed_count", 100);
	mCreateWeight = pConf->getAttrInt("create_weight", 25);
	mModifyWeight = pConf->getAttrInt("modify_weight", 25);
	mReadWeight = pConf->getAttrInt("read_weight", 25);
	mDeleteWeight = pConf->getAttrInt("delete_weight", 25);
	mApplyWeight = pConf->getAttrInt("apply_weight", 25);
	loadSeed();
	mTestDurationSec = gTestDuration;
	mRundata = OmnApp::getRundata();
	mBlobSE = pBlobSE;
	mController = pController;
}


AosBlobSETesterThrd::~AosBlobSETesterThrd()
{

}
// OmnThreadedObj interface
bool
AosBlobSETesterThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	if (mTestDurationSec > 0)
	{
		// Run the torturer for 'mTestDuration' amount of time.
		while (OmnTime::getCrtSecond() < mStartSec + mTestDurationSec)
		{
			while (state == OmnThrdStatus::eActive)
			{
				if (OmnTime::getCrtSecond() < mStartSec + mTestDurationSec)
				{
					runOneTest();
				}
				else
				{
					OmnScreen << "expired." << endl;
					break;
				}
			}
		}
	}
	else
	{
		OmnScreen << "mTestDurationSec=" << mTestDurationSec << endl;
	}
	state = OmnThrdStatus::eExit;
	mController->incNumFinished();
	return true;
}


bool
AosBlobSETesterThrd::signal(const int threadLogicId)
{
	return true;
}


bool
AosBlobSETesterThrd::runOneTest()
{
	switch (OmnRandom::intByRange(
			eCreate, eCreate, mCreateWeight,
			eDelete, eDelete, mDeleteWeight,
			eModify, eModify, mModifyWeight,
			eRead, eRead, mReadWeight,
			eApply, eApply, mApplyWeight))
	{
	case eCreate:
		mController->incCreateTestCount();
		if( !testCreateDoc() )
		{
			mController->incCreateFailedCount();
		}
		break;

	case eDelete:
		mController->incDelTestCount();
		if( !testDeleteDoc() )
		{
			mController->incDelFailedCount();
		}
		break;

	case eModify:
		mController->incModifyTestCount();
		if( !testModifyDoc() )
		{
			mController->incModifyFailedCount();
		}
		break;

	case eRead:
		mController->incReadTestCount();
		if( !testReadDoc() )
		{
			mController->incReadFailedCount();
		}
		break;

	case eApply:
		applyChanges();
		break;

	default:
		OmnAlarm << "invalid opr" << enderr;
		return false;
		break;
	}
	return true;
}


bool
AosBlobSETesterThrd::testCreateDoc()
{
    // This function creates a new doc. The new doc's body is determined
    // by a seed and a repeat.
	u64 ullDocid = createDocid();
	aos_assert_r(ullDocid > 0, false);
	u32 ulSeedId = 0;
	u32 ulRepeat = 0;
	AosBuffPtr pData = createData(ulSeedId, ulRepeat);
	aos_assert_r(pData.notNull(), false);
	aos_assert_r(0 != ulSeedId, false);
	AosBuffPtr pBuff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	u64 ullTimestamp = 0;
	bool rslt = genBlobSEBuff(ullDocid, AosBlobSEReqEntry::eSave, pData, pBuff, ullTimestamp);
	aos_assert_r(rslt, false);
	u64 ullOprID = 0;
	rslt = mController->appendEntry(pBuff, ullOprID);
	aos_assert_r(rslt, false);
    aos_assert_r(ullOprID, false);
	rslt = saveDocOprInfo(ullDocid, ullOprID, ulSeedId, ulRepeat, ullTimestamp, eCreate);
	aos_assert_r(rslt, false);
	OmnScreen << "doc with docid:" << ullDocid << "  ullOprID:" << ullOprID
			<< "  ullTimestamp:" << ullTimestamp << endl;

	return true;
}


bool
AosBlobSETesterThrd::testModifyDoc()
{
	u64 ullDocid = pickDocid();
	if (0 == ullDocid)
	{
		OmnScreen << "No doc has ever been created, skip modification for now." << endl;
		return true;
	}
	u32 ulSeedId = 0;
	u32 ulRepeat = 0;
	AosBuffPtr pData = createData(ulSeedId, ulRepeat);
	AosBuffPtr pBuff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	u64 ullTimestamp = 0;
	bool rslt = genBlobSEBuff(ullDocid, AosBlobSEReqEntry::eSave, pData, pBuff, ullTimestamp);
	aos_assert_r(rslt, false);
	u64 ullOprID = 0;
	rslt = mController->appendEntry(pBuff, ullOprID);
	aos_assert_r(rslt, false);
    aos_assert_r(ullOprID, false);
	rslt = saveDocOprInfo(ullDocid, ullOprID, ulSeedId, ulRepeat, ullTimestamp, eModify);
	aos_assert_r(rslt, false);
	OmnScreen << "doc with docid:" << ullDocid << "  ullOprID:" << ullOprID
				<< "  ullTimestamp:" << ullTimestamp << endl;
	return true;
}


bool
AosBlobSETesterThrd::testDeleteDoc()
{
	u64 ullDocid = pickDocid();
	if (0 == ullDocid)
	{
		OmnScreen << "No doc has ever been created, skip deletion for now." << endl;
		return true;
	}
	u32 ulSeedId = 0;
	u32 ulRepeat = 0;
	AosBuffPtr pBuff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	u64 ullTimestamp = 0;
	bool rslt = genBlobSEBuff(ullDocid, AosBlobSEReqEntry::eDelete, NULL, pBuff, ullTimestamp);
	aos_assert_r(rslt, false);
	u64 ullOprID = 0;
	rslt = mController->appendEntry(pBuff, ullOprID);
	aos_assert_r(rslt, false);
    aos_assert_r(ullOprID, false);
	rslt = saveDocOprInfo(ullDocid, ullOprID, ulSeedId, ulRepeat, ullTimestamp, eDelete);
	aos_assert_r(rslt, false);
	OmnScreen << "doc deleted with docid:" << ullDocid << "  ullOprID:" << ullOprID
				<< "  ullTimestamp:" << ullTimestamp << endl;
	return true;
}


bool
AosBlobSETesterThrd::testReadDoc()
{
	u64 ullDocid = pickDocid();
	if (0 == ullDocid)
	{
		OmnScreen << "No doc has ever been created, skip reading for now." << endl;
		return true;
	}
	u64 ullOprID = 0;
	u64 ullTimestamp = 0;
	Opr eOpr;
	u64 ullLastAppliedOprID = mController->getOprIdLastApplied();
	if (0 == ullLastAppliedOprID)
	{
		OmnScreen << "0 == ullLastAppliedOprID, cancel the reading." << endl;
		return true;
	}
	bool bHasValidOpr = true;
	bool rslt = mDocOprInfos[ullDocid].pickOpridAndTimestamp(
			ullLastAppliedOprID, ullOprID, ullTimestamp, eOpr, bHasValidOpr);
	aos_assert_r(rslt, false);
	if (!bHasValidOpr)
	{
		OmnScreen << "This docid has no applied operations, reading cancelled." << endl;
		return true;
	}
	aos_assert_r((ullOprID > 0) && (ullTimestamp > 0), false);
	aos_assert_r(eDelete == eOpr || eCreate == eOpr || eModify == eOpr, false);
	AosBuffPtr pBodyBuff, pHeaderCustomData;
	OmnScreen << "trying to read doc with docid:" << ullDocid  << "  ullOprID:" << ullOprID
			<<  "  ullTimestamp:" << ullTimestamp << endl;
	if (!mBlobSE->readDoc(mRundata, 0, 0, ullDocid, ullTimestamp, pBodyBuff, pHeaderCustomData))
	{
		OmnAlarm << "mBlobSE->readDoc failed, docid:" << ullDocid << " timestamp:" << ullTimestamp << enderr;
		return false;
	}
	if (!verifyData(ullDocid, ullOprID, ullTimestamp, pBodyBuff))
	{
		OmnAlarm << "verifyData failed, docid:" << ullDocid << " timestamp:" << ullTimestamp
				<< " oprid:" << ullOprID << enderr;
		return false;
	}
	return true;
}


bool
AosBlobSETesterThrd::applyChanges()
{
	OmnScreen << "applyChanges" << endl;
	return mController->applyChanges();
}


u64
AosBlobSETesterThrd::createDocid()
{
	mLastCreatedDocid += mController->getNumOfThreads();
    return mLastCreatedDocid;
}


u32
AosBlobSETesterThrd::pickSeed()
{
	return rand() % mSeedCount + 1;
}


bool
AosBlobSETesterThrd::loadSeed()
{
	for(u32 i = 1; i <= mSeedCount; i++)
	{
//		mSeedMap[i] = OmnRandom::getRandomStr("", 0, 30);
		mSeedMap[i] = OmnRandom::binaryStr(0, 1000);
//		OmnScreen << "seed[" << i << "]:'" << gSeedMap[i] << "'" << endl;
	}
	return true;
}


AosBuffPtr
AosBlobSETesterThrd::createData(
		u32	&seed_id,
		u32 &repeat)
{
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	seed_id = seed_id == 0 ? pickSeed(): seed_id;
	switch(OmnRandom::percent(50, 50))
	{
	case 0:	//big
		repeat = OmnRandom::nextInt1(30, 300);
		break;

	case 1:	//small
		repeat = OmnRandom::nextInt1(0, 30);
		break;

	default:
		OmnAlarm << "invalid percentage!" << enderr;
	}

	for (u32 i=0; i<repeat; i++)
	{
		buff->setBuff(mSeedMap[seed_id].data(), mSeedMap[seed_id].length());
	}
	buff->setDataLen(mSeedMap[seed_id].length() * repeat);
	return buff;
}


bool
AosBlobSETesterThrd::saveDocOprInfo(
		const u64	ullDocid,
		const u64	ullOprid,
		const u32	ulSeed,
		const u32	ulRepeat,
		const u64	ullTimestamp,
		Opr			eOpr)
{
	itr_t it = mDocOprInfos.find(ullDocid);
	DocOpr sDocOpr;
	sDocOpr.oprid = ullOprid;
	sDocOpr.seed = ulSeed;
	sDocOpr.repeat = ulRepeat;
	sDocOpr.timestamp = ullTimestamp;
	sDocOpr.opr = eOpr;
	if (it == mDocOprInfos.end())
	{
		DocOprInfo sDocOprInfo;
		sDocOprInfo.addOpr(sDocOpr);
		mDocOprInfos[ullDocid] = sDocOprInfo;
	}
	else
	{
		it->second.addOpr(sDocOpr);
	}
	return true;
}


bool AosBlobSETesterThrd::genBlobSEBuff(
		const u64							ullDocid,
		AosBlobSEReqEntry::ReqTypeAndResult	eReqTypeAndResult,
		const AosBuffPtr					&pBodyData,
		AosBuffPtr							&pBuff,
		u64									&ullTimestamp)
{
	u64 ullSnapshotID = 0;
	AosBlobSEReqEntryPtr pAosBlobSEReqEntry = OmnNew AosBlobSEReqEntry(mBlobSE->getConfig().header_custom_data_size);
//	ullTimestamp = OmnTime::getTimestamp();
	ullTimestamp = OmnGetTimestamp();
//	OmnScreen << "got timestamp:" << ullTimestamp << endl;
	pAosBlobSEReqEntry->setTimestamp(ullTimestamp);
	pAosBlobSEReqEntry->setSnapshotID(ullSnapshotID);
	pAosBlobSEReqEntry->setDocid(ullDocid);
	pAosBlobSEReqEntry->setReqTypeAndResult(eReqTypeAndResult);
	AosBuffPtr pHeaderCustomData = OmnNew AosBuff(mBlobSE->getConfig().header_custom_data_size AosMemoryCheckerArgs);
	memset(pHeaderCustomData->data(), 125, pHeaderCustomData->buffLen());	// set it to "{{{" for debugging
	pHeaderCustomData->setDataLen(mBlobSE->getConfig().header_custom_data_size);
	pAosBlobSEReqEntry->setHeaderCustomDataBuff(pHeaderCustomData);
	pAosBlobSEReqEntry->setBodyBuff(pBodyData);
	int iRet = pAosBlobSEReqEntry->serializeToBuff(pBuff);
	if (0 != iRet)
	{
		OmnAlarm << "pAosBlobSEReqEntry->serializeToBuff failed, iRet=" << iRet << enderr;
		return false;
	}
	return true;
}


u64
AosBlobSETesterThrd::pickDocid()
{
	if (mDocOprInfos.empty())
	{
		return 0;
	}
	itr_t it = mDocOprInfos.begin();
	std::advance(it, OmnRandom::nextU32(0, mDocOprInfos.size()));
	return it->first;
}


bool
AosBlobSETesterThrd::verifyData(
		const u64			ullDocid,
		const u64			ullOprid,
		const u64			ullTimestamp,
		const AosBuffPtr	&pBodyData)
{
	itr_t it = mDocOprInfos.find(ullDocid);
	aos_assert_r(it != mDocOprInfos.end(), false);
	vector<DocOpr>::iterator itr = std::find(it->second.operations.begin(), it->second.operations.end(), ullOprid);
	aos_assert_r(itr != it->second.operations.end(), false);
	if (eDelete == itr->opr)
	{
		aos_assert_r(pBodyData.isNull(), false);
		return true;
	}
	aos_assert_r(pBodyData.notNull(),false);
	if (pBodyData->dataLen() != mSeedMap[itr->seed].length() * itr->repeat)
	{
		OmnAlarm << "pBodyData->dataLen():" << pBodyData->dataLen()
				<< " != mSeedMap[itr->seed_id].length() * itr->repeat:"
				<<  mSeedMap[itr->seed].length() * itr->repeat << enderr;
		return false;
	}
	AosBuffPtr buff = OmnNew AosBuff(mSeedMap[itr->seed].length() * itr->repeat AosMemoryCheckerArgs);
	aos_assert_r(buff.notNull(), false);
	for (int i=0; i<itr->repeat; i++)
	{
		buff->setBuff(mSeedMap[itr->seed].data(), mSeedMap[itr->seed].length());
	}
	buff->setDataLen(mSeedMap[itr->seed].length() * itr->repeat);
	if (0 != memcmp(buff->data(), pBodyData->data(), pBodyData->dataLen()))
	{
		OmnAlarm << "data verification failed." << enderr;
		buff->dumpHex("what it should be");
		pBodyData->dumpHex("body buff");
		return false;
	}
	return true;
}
