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
//	07/29/2011 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilTime_CalendarMgr_h
#define Omn_UtilTime_CalendarMgr_h

#include "aosUtil/Types.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "Util/Locale.h"
#include "UtilTime/Ptrs.h"
#include "UtilTime/Calendar.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

OmnDefineSingletonClass(AosCalendarMgrSingleton,
		AosCalendarMgr,
		AosCalendarMgrSelf,
		OmnSingletonObjId::eCalendarMgr,
		"CalendarMgr");

class AosCalendarMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosCalendarMgr();
	~AosCalendarMgr();

	// Singleton class interface
	static AosCalendarMgr*       getSelf();
	virtual bool        start();
	virtual bool        stop();
	virtual bool		config(const AosXmlTagPtr &def);

	AosCalendarPtr getCalendar(const OmnString &objid);
	AosCalendarPtr getDefaultCalendar(const AosRundataPtr &rdata);

};
#endif
