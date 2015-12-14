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
// 09/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_EventObj_h
#define Aos_SEInterfaces_EventObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/EventCreatorObj.h"
#include "SEInterfaces/EventType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosEventObj : public OmnRCObject
{
	OmnDefineRCObject

public:
	enum ContextType
	{
		eContextType_Invalid,

		eContextType_Event,
		eContextType_Stage,
		eContextType_Thread,
		eContextType_Task,
		eContextType_Job,

		eContextType_Max
	};

protected:
	AosEventType::E		mType;

	static AosEventCreatorObjPtr		smCreator;

public:
	AosEventObj();
	AosEventObj(
			const OmnString &name, 
			const AosEventType::E type, 
			const bool flag);
	~AosEventObj();

	AosEventType::E	getType() const {return mType;}

	virtual AosEventObjPtr clone() const = 0;
	virtual AosEventObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const = 0;
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata) const = 0;

	static AosEventObjPtr createEvent(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);
	
	static bool registerEvent(const OmnString &name, AosEventObj *cacher);

	static AosEventObjPtr serializeFromStatic(const AosBuffPtr &buff, const AosRundataPtr &rdata);

	static void setCreator(const AosEventCreatorObjPtr &creator) {smCreator = creator;}
	static AosEventCreatorObjPtr getCreator() {return smCreator;}
};

#endif

