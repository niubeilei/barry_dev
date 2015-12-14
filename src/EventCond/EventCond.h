////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_EventCond_EventCond_h
#define Aos_EventCond_EventCond_h

#include "EventCond/EventCondType.h"
#include "EventCond/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Security/Ptrs.h"
#include "SmartDoc/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/XmlRc.h"

#include <vector>
using namespace std;

class AosEventCond : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosEventCondType::E	mType;
	OmnMutexPtr			mLock;

public:
	AosEventCond(const AosEventCondType::E type, const bool reg);
	AosEventCond();
	~AosEventCond();

	virtual bool
	evalCond(const AosXmlTagPtr &def, 
			bool &rslt, 
			const AosRundataPtr &rdata) = 0;

	static bool evalConds(
			const AosXmlTagPtr &conds, 
			bool &rslt,
			const AosRundataPtr &rdata);
	
	AosEventCondType::E	getType() const {return mType;}

	static bool registerEventCond(AosEventCond *cond);

private:
	static bool logicOpr(
			const OmnString &logic,
			const bool &b1,
			const bool &b2);
};
#endif

