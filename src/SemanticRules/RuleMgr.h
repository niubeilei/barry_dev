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
// 12/08/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticRules_RuleMgr_h
#define Aos_SemanticRules_RuleMgr_h

#include "SemanticRules/Ptrs.h"
#include "SemanticRules/SemanticRuleUtil.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashObj.h"
#include "XmlParser/Ptrs.h"
#include <queue>

OmnDefineSingletonClass(AosRuleMgrSingleton,
		  				AosRuleMgr,
		  				AosRuleMgrSelf,
		  				OmnSingletonObjId::eRuleMgr,
						"RuleMgr");

typedef AosSemanticRulePtr (*aos_rule_creator_func)();

extern bool aos_register_rule_creator(
					const std::string &name, 
					aos_rule_creator_func func);
						
class AosRuleMgr : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

private:
	OmnObjHash<AosSemanticRulePtr, 0xfff>	mRules;

public:
	AosRuleMgr();
	virtual ~AosRuleMgr();

	// 
	// Singleton Class interface
	//
	static AosRuleMgr*	getSelf();
	virtual bool		start();
	virtual bool		stop();
	virtual OmnRslt		config(const OmnXmlParserPtr &configData);

	//  
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state,
						    const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
	virtual void heartbeat(const int tid);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;

	bool	addRule(const AosSemanticRulePtr &rule);

private:
};
#endif

