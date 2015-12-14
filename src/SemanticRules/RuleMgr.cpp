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
// 12/08/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticRules/RuleMgr.h"

#include "alarm/Alarm.h"
#include "SemanticRules/SemanticRule.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/OmnNew.h"

OmnSingletonImpl(AosRuleMgrSingleton,
		 		 AosRuleMgr,
		 		 AosRuleMgrSelf,
		 		 "AosRuleMgr");


AosRuleMgr::AosRuleMgr()
{
	OmnTrace << "To start RuleMgr" << endl;
}


AosRuleMgr::~AosRuleMgr()
{
}


bool
AosRuleMgr::start()
{
	return true;
}


bool
AosRuleMgr::stop()
{
	OmnTrace << "Singleton class AosRuleMgr stopped!" << endl;
	return true;
}
 

OmnRslt
AosRuleMgr::config(const OmnXmlParserPtr &def)
{
	return true;
}
 
 
bool
AosRuleMgr::threadFunc(OmnThrdStatus::E &state, 
								const OmnThreadPtr &thread)
{
	OmnTrace << "Enter AosRuleMgr::threadFunc." << endl;
	
	while (state == OmnThrdStatus::eActive)
	{
		break;
	}
	
	OmnTraceThread << "Leaving AosRuleMgr::threadFunc" << endl;
	return true;
}


bool 
AosRuleMgr::signal(const int threadLogicId)
{
	return true;
}


void 
AosRuleMgr::heartbeat(const int tid)
{
}


bool 
AosRuleMgr::checkThread(OmnString &errmsg, const int tid) const
{
	return true;
}


bool	
AosRuleMgr::addRule(const AosSemanticRulePtr &rule)
{
	aos_assert_r(rule, false);

	if (!mRules.add(rule, false))
	{
		// 
		// The rule is already in the table. 
		//
		OmnAlarm << "Rule is already in table: " 
			<< rule->toString() << enderr;
		return false;
	}

	// 
	// Added successfully. 
	//
	return true;
}


