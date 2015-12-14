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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/SengTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"

#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

// To run a single tester, set the tester ID to the following 
// global variable. Otherwise, set it to AosTesterId::eInvalid;
//static AosTesterId::E 	sgSingleTesters[AosSengTester::eMaxSingles]; 
//static int 				sgSingleTries[AosSengTester::eMaxSingles]; 

AosStr2U32_t		sgNameMap;
AosStr2U32_t		sgNameMapE;
AosSengTesterPtr	AosSengTester::smTesters[AosTesterId::eMax];
int 				AosSengTester::smTotalWeights = 0;
AosTesterId::E	*	AosSengTester::smTesterInsts = 0;


// The following are all testers. If the parameter is true, 
// the tester will be used in this testing. Otherwise, it is
// ignored.
// static AosAccessCtrlTester      	sgAccessCtrlTester(false);
// static AosCreateCtnrTester   		sgCreateCtnrTester(false);
// static AosCreateObjTester 			sgCreateObjTester(true);
// static AosCreateUserDomainTester	sgCreateUserDomainTester(false);
// static AosCreateUserNewTester   	sgCreateUserTester(false);
// static AosDeleteObjTester 			sgDeleteTester(true);
// static AosGetDomainTester			sgGetDomainTester(false);
// static AosLoginTester      			sgLoginTester(false);
// static AosLogoutTester      		sgLogoutTester(false);
// static AosModifyAttrStrTester		sgModifyAttrStr(false);
// static AosModifyObjTester 			sgModifyObjTester(true);
// static AosPartialDocTester			sgPartialDocTester(false);
// static AosManualOrderTester			sgManualOrderTester(false);
// static AosSdocTriggeringTester		sgSdocTriggeringTester(false);
// static AosOperationsTester			sgOperationsTester(false);
// static AosDocLockTester				sgDocLockTester(false);


AosSengTester::AosSengTester(
		const OmnString &name,
		const AosTesterId::E id,
		const bool reg)
:
mLock(OmnNew OmnMutex()),
mWeight(eDefaultWeight),
mTesterId(id),
mName(name)
{
	if (reg)
	{
		AosSengTesterPtr thisptr(this, false);
		registerTester(id, thisptr);
	}
}


AosSengTester::AosSengTester(
		const OmnString &name,
		const OmnString &weight_tagname, 
		const AosTesterId::E id)
:
mLock(OmnNew OmnMutex()),
mWeight(eDefaultWeight),
mTesterId(id),
mName(name)
{
}


bool 	
AosSengTester::configStatic(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosSengTester::configTester(const AosXmlTagPtr &config)
{
	// This function assumes the following:
	// 	<tester weight="xxx" .../>
	if (!config) return true;
	mWeight = config->getAttrInt("weight", mWeight);
	if (mWeight < 0)
	{
		mWeight = eDefaultWeight;
		return true;
	}

	return true;

	/*
	// The configure is in the form:
	//  <config ...>
	//      <AOSCONFIG_TORTURER ...>
	//          <AOSCONFIG_WEIGHT>
	//          	<testerid>xxx</testerid>
	//          	<testerid>xxx</testerid>
	//              ...
	//          	<testerid>xxx</testerid>
	//          </AOSCONFIG_WEIGHT>
	//      </AOSCONFIG_TORTURER>
	//      ...
	//  </config>
	
	aos_assert_r(config, false);
	for (int i=1; i<AosTesterId::eMax; i++)
	{
		if (smTesters[i]) smTesters[i]->configTester(config);
	}

	for (int i=0; i<eMaxSingles; i++) sgSingleTesters[i] = AosTesterId::eInvalid;

	AosXmlTagPtr torturer_config = config->getFirstChild(AOSCONFIG_TORTURER);
	if (torturer_config)
	{
		AosXmlTagPtr singles = torturer_config->getFirstChild("singles");
		if (singles && singles->getAttrBool("with_singles", false))
		{
			AosXmlTagPtr tester = singles->getFirstChild();
			int idx = 0;
			while (idx < eMaxSingles && tester)
			{
				OmnString name = tester->getAttrStr("name");
				sgSingleTesters[idx] = AosTesterId::toEnum(name); 
				sgSingleTries[idx] = tester->getAttrInt("tries", 1000);
				tester = singles->getNextChild();
				idx++;
			}
			sgSingleTesters[idx] = AosTesterId::eInvalid;
		}
	}
		
	// There may be many testers. Each tester is assigned a weight. Tester
	// weights are used to control how often a tester is picked to run. 
	// Initially, all testers are set to 'eDefaultWeight'. This value
	// can be overridden through the configuration. After that, all 
	// weights are added up; an array[total_weight] is created. If a
	// tester has weight W, there are W number of cells in the array 
	// assigned to that tester.
	for (int i=0; i<AosTesterId::eMax; i++)
	{
		if (smTesters[i]) smTesters[i]->setWeight(eDefaultWeight);
	}

	if (torturer_config)
	{
	    AosXmlTagPtr weights = torturer_config->getFirstChild(AOSCONFIG_WEIGHT);
	    if (weights)
	    {
			AosXmlTagPtr weight = weights->getFirstChild();
			while (weight)
			{
				OmnString name = weight->getTagname();
				AosTesterId::E id = AosTesterId::toEnum(name);
				aos_assert_r(AosTesterId::isValid(id), false);
				int ww = atoi(weight->getNodeText().data());
				if (ww > 0 && ww < eMaxWeight && smTesters[id])
				{
					smTesters[id]->setWeight(ww);
				}
				weight = weights->getNextChild();
			}
	    }
	}

	// Calculate the total weights
	smTotalWeights = calculateTotalWeights();
	aos_assert_r(smTotalWeights > 0, false);
	smTesterInsts = OmnNew AosTesterId::E[smTotalWeights];
	int idx = 0;
	for (int i=AosTesterId::eInvalid; i<AosTesterId::eMax; i++)
	{
		if (smTesters[i]) 
		{
			int ww = smTesters[i]->getWeight();
			aos_assert_r(idx < smTotalWeights, false);
			for (int k=0; k<ww; k++)
			{
				aos_assert_r(idx < smTotalWeights, false);
				smTesterInsts[idx++] = (AosTesterId::E)i;
			}
		}
	}
	return true;
	*/
}


AosSengTester::~AosSengTester()
{
}


bool
AosSengTester::registerTester(
		const AosTesterId::E id, 
		const AosSengTesterPtr &tester)
{
	aos_assert_r(id, false);
	mLock->lock();
	if (smTesters[id])
	{
		mLock->unlock();
		OmnAlarm << "Tester already registered: " << id << enderr;
		return false;
	}

	smTesters[id] = tester;
	AosTesterId::addName(mName, id);
	mLock->unlock();
	return true;
}


bool
AosSengTester::cloneTesters(
		const AosSengTestThrdPtr &thread, 
		AosSengTesterPtr *testers,
		const AosSengTesterMgrPtr &mgr)
{
	// Clone all the testers
	/*
	for (int i= AosTesterId::eInvalid+1; i < AosTesterId::eMax; i++)
	{
	 	if (smTesters[i])
	 	{
	 		testers[i] = smTesters[i]->clone();
	 		testers[i]->config(thread, mgr);
	 	}
	}
	*/
	return true;
}


bool
AosSengTester::startTestStatic(
		const AosSengTestThrdPtr &thread,
		const AosSengTesterMgrPtr &mgr)
{
	/*
	// Check whether it is to test a single testers.
	if (AosTesterId::isValid(sgSingleTesters[0]))
	{
		for (int i=0; i<eMaxSingles; i++)
		{
			if (smTesters[sgSingleTesters[i]])
			{
				smTesters[sgSingleTesters[i]]->config(thread, mgr);
				//for (int k=0; k<sgSingleTries[k]; k++)
				for (int k=0; k<sgSingleTries[i]; k++)
				{
					OmnScreen << smTesters[sgSingleTesters[i]]->getName() << ":" << k << endl;
					smTesters[sgSingleTesters[i]]->test();
				}
			}
		}
		return true;
	}

	AosSengTesterPtr *testers = thread->getTesters(); 
	
	int tries = thread->getTries();
	if (tries <= 0) tries = eDftTries;
	int group = 10000;
	int groupSize = 200;
	OmnScreen << "tries: " << tries << ":" << thread->getThreadid() << endl;
	for (int i =0; i< tries; i++)
	{
		if (group >= groupSize)
		{
			OmnScreen << "Tester tries: " << i << endl;
			group = 0;
		}

		group++;
		int ww = rand() % smTotalWeights;
		AosTesterId::E tester_id = smTesterInsts[ww];
		aos_assert_r(AosTesterId::isValid(tester_id), false);
		if (testers[tester_id])
		{
			OmnScreen << "Run test(thd" << thread->getThreadid() << "):" 
				<< i << ":" << testers[tester_id]->getName() << endl;
			testers[tester_id]->test();
		}
	}

	OmnScreen <<"Finished basic testing!" << endl;
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosSengTester::testStatic(
		const AosSengTestThrdPtr &thread,
		const AosSengTesterMgrPtr &mgr,
		AosTesterId::E id)
{
	return smTesters[id]->test();
}


int 
AosSengTester::calculateTotalWeights()
{
	int total = 0;
	for (int i=AosTesterId::eInvalid; i<AosTesterId::eMax; i++)
	{
		if (smTesters[i]) 
		{
			int ww = smTesters[i]->getWeight();
			total += ww;
		}
	}

	return total;
}


/*
bool 
AosSengTester::config(const AosSengTestThrdPtr &thread, const AosSengTesterMgrPtr &mgr)
{
	mThread = thread;
	mMgr = mgr;
	mThreadId = mThread->getThreadId();
	return true;
}
*/

void 	
AosSengTester::setTesterMgr(const AosSengTesterMgrPtr &mgr)
{
	mMgr = mgr;
}
