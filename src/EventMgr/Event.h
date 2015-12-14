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
// 10/25/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_EventMgr_Event_h
#define Omn_EventMgr_Event_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "EventMgr/EventTriggers.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/EventObj.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


#define AOSINPUTTYPE_DOC       			"doc"
#define AOSINPUTTYPE_TIMER     			"timer"
#define AOSINPUTTYPE_SYSTEM    			"system"
#define AOSINPUTTYPE_OPERATION 			"operation"

class AosEvent : public AosEventObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxEvents = 50
	};

public:
	AosEvent();
	~AosEvent();

	static bool checkEvent1(
					const AosXmlTagPtr &olddoc, 
					const AosXmlTagPtr &newdoc, 
					const OmnString &trigger,
					const AosRundataPtr &rdata);

private:
	static bool		procEvent(
					const AosXmlTagPtr &olddoc,
					const AosXmlTagPtr &newdoc,
					const OmnString &trigger,
					const AosXmlTagPtr &event_xml,
					const AosRundataPtr &rdata);


	static bool     tryToFireEvent(
					const AosXmlTagPtr &event_def,
					const AosXmlTagPtr &olddoc,
					const AosXmlTagPtr &newdoc,
					const OmnString &trigger,
					const AosRundataPtr &rdata);
};
#endif

