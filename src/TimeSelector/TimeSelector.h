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
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TimeSelector_TimeSelector_h
#define AOS_TimeSelector_TimeSelector_h

#include "TimeSelector/TimeSelectorType.h"
#include "Alarm/Alarm.h"
#include "TimeSelector/Ptrs.h"
#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "ValueSel/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosTimeSelector : public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosTimeSelectorType::E	mType;
	OmnMutexPtr				mLock;
	static AosDocClientObjPtr	smDocClient;

public:
	enum
	{
		eMax = 100
	};

	AosTimeSelector(
			const OmnString &name, 
			const AosTimeSelectorType::E type, 
			const bool reg);
	~AosTimeSelector();

	virtual bool  run(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata) = 0;

	static bool  getTimeStatic(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

	AosTimeSelectorType::E getType() const
	{
		return mType;
	}

	AosTimeSelectorPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;
		return 0;
	}

	static AosTimeSelectorPtr	getTimeSelector(
			const AosTimeSelectorType::E type, 
			const AosRundataPtr &rdata);

private:
	bool registerSelector(const OmnString &name, const AosTimeSelectorPtr &selector);
};
#endif

