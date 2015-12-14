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
// 11/24/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Semantics_SemanticRuleCommon_h
#define Aos_Semantics_SemanticRuleCommon_h

#include "SemanticData/Ptrs.h"
#include "Event/EventId.h"
#include "Event/EventUtil.h"
#include "Event/EventRegistrant.h"
#include "Event/ActivityTracker.h"
#include "Event/Ptrs.h"
#include "SemanticRules/SemanticRule.h"
#include "SemanticRules/SemanticRuleUtil.h"
#include "Util/SPtr.h"
#include "Util/string.h"
#include <string>


class AosSemanticRuleCommon : virtual public AosEventRegistrant,
							  public AosSemanticRule
{
protected:
	AosRuleId::E			mRuleId;
	std::string				mName;
	void *					mUserData;
	u32						mDataLen;
	bool					mStatus;	// True is enabled, False is disabled

	AosEventList			mEvents;
	AosStringList			mAllowedActors;
	AosStringList			mDisallowedActors;
	bool					mDefaultPolicy;
	bool					mActivityLogging;
	AosActivityTracker		mActivities;
	bool					mIdentifiedByProgObj;

public:
	AosSemanticRuleCommon(const AosRuleId::E ruleId, 
						  const std::string &name, 
						  void *userData, 
						  u32 dataLen,
						  const bool identifiedByProgObj = true);
	~AosSemanticRuleCommon();

	virtual AosRuleId::E	getRuleId() const {return mRuleId;}
	virtual std::string	getName() const {return mName;}
	virtual std::string	getListenerName() const {return mName;}
	virtual bool		enable();
	virtual bool		disable();
	virtual bool		registerEvents();
	virtual bool		unregisterEvents();
	virtual bool		isIdentifiedByProgObj() const;
	virtual u32			getHashKey() const;
	virtual bool		isSameObj(const AosSemanticRulePtr &rhs) const;
	virtual std::string	toString() const;

protected:
	bool		addAllowedCallers(const std::string &caller);
	bool		removeAllowedCallers(const std::string &caller);
	void		resetAllowedCallers();
	AosStringList	getAllowedCallers();

	bool		addDisallowedCallers(const std::string &caller);
	bool		removeDisallowedCallers(const std::string &caller);
	void		resetDisallowedCallers();
	AosStringList	getDisallowedCallers();
	void 		logActivity(const AosEventId::E eventId, 
							const AosEventPtr &event); 

	void		addEvent(const AosEventId::E event, 
							void *userData, 
							const u32 dataLen);
	void		removeEvent(const AosEventId::E event, 
							void *userData, 
							const u32 dataLen);
};
#endif

