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
#include "ValueSel/ValueSelGetCounter.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Random/CommonValues.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocSelObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "CounterUtil/CounterQueryType.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/ResolveCounterName.h"
#include "UtilTime/TimeUtil.h"

static AosDocSelObjPtr sgDocSelector;



AosValueSelGetCounter::AosValueSelGetCounter(const bool reg)
:
AosValueSel(AOSACTOPRID_GETCOUNTER, AosValueSelType::eGetCounter, reg)
{
}


AosValueSelGetCounter::AosValueSelGetCounter(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_GETCOUNTER, AosValueSelType::eGetCounter, false)
{
}


AosValueSelGetCounter::AosValueSelGetCounter(
		const OmnString &value, 
		const OmnString &xpath, 
		const bool copydoc,
		const AosXmlTagPtr &doc_selector)
:
AosValueSel(AOSACTOPRID_GETCOUNTER, AosValueSelType::eGetCounter, false),
mValue(value),
mXPath(xpath),
mCopyDoc(copydoc),
mDocSelector(doc_selector)
{
}


AosValueSelGetCounter::~AosValueSelGetCounter()
{
}


bool
AosValueSelGetCounter::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// 	<valuesel 
	// 		AOSTAG_VALUE_TYPE="getcount" 
	// 		AOSTAG_DATA_TYPE="xxx"
	// 		sum="true|false"
	// 		avg="true|false"
	// 		max="true|false"
	// 		min="true|false"
	// 		zky_counter_id="xxx" >
	//
	// 		<counter_names>
	// 		     <termname zky_value_type="attr" zky_xpath="xxxx">
	//         	  	<zky_docselector zky_docselector_type="retrieveddoc"/>
	// 	   	     </termname>
	// 		     <termname type="const">xxx</termname>
	// 	     			....
	// 	   	   	 <termname type="const">xxx</termname>
	// 	 	</counter_names>
	// 		<zky_time_condition zky_timeformat="xxx" zky_starttime="xxx" "zky_endtime="xxx" zky_timegran = "yer|mnt|day".../>
	// 	</valuesel>
	//
	// If AOSTAG_DATA_TYPE is not specified, it defaults to string.
	aos_assert_r(sdoc, false);
	aos_assert_r(rdata, false);
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eInt64;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorU(rdata, "the data type is wrong!") << ": " << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	CounterInfo counterobj;	 
	counterobj.mDftValue = 0;
	bool r = parse(counterobj, sdoc, rdata);
	aos_assert_r(r, false);

	AosBuffPtr buff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
	bool rslt = AosRetrieveSingleCounter(
			counterobj.mCounterId, counterobj.mCounterName, counterobj.mStatTypes, counterobj.mStartTime, counterobj.mEndTime, 
			counterobj.mDftValue, buff, rdata);
	aos_assert_r(rslt, false);

	int64_t vv;
	if (!AosTime::isValidTimeRange(counterobj.mStartTime, counterobj.mEndTime))
	{
		 vv = buff->getI64(counterobj.mDftValue);
	}
	else
	{
		while(1)
		{
			OmnString name = buff->getOmnStr("");
			if (name == "") break;
			vv += buff->getI64(counterobj.mDftValue);
		}
	}
	OmnString value;
	value << vv;
	valueRslt.setStr(value);
	return true;
}

bool
AosValueSelGetCounter::parse(struct CounterInfo &counterobj, const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	aos_assert_r(term, 0);

	AosStatType::retrieveStatTypes(counterobj.mStatTypes, term);

	// Get the counter id
	counterobj.mCounterId = term->getAttrStr(AOSTAG_COUNTER_ID);
	if (counterobj.mCounterId == "") counterobj.mCounterId = term->getNodeText(AOSTAG_COUNTER_ID);
	if (counterobj.mCounterId == "")
	{
		AosSetErrorU(rdata, "missing_counter_id") << ": " << term->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	counterobj.mCounterId = AosIILName::composeCounterIILName(counterobj.mCounterId);

	AosTimeUtil::parseTimeNew(term, AOSTAG_TIME_CONDITION, 
			counterobj.mStartTime, counterobj.mEndTime, counterobj.mTimeGran, counterobj.mTimeFormat, false, rdata);

	bool need_proc_member = false;
	if (!AosTime::isValidTimeRange(counterobj.mStartTime, counterobj.mEndTime)) need_proc_member = true; 

	// Retrieve the counter name
	bool rslt = AosResolveCounterName::resolveCounterName(
			term, AOSTAG_COUNTERNAMES, counterobj.mCounterName, need_proc_member, rdata);

	if (!rslt || counterobj.mCounterName == "")
	{
		return false;
	}
	return true;
}

AosValueSelObjPtr
AosValueSelGetCounter::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelGetCounter(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

