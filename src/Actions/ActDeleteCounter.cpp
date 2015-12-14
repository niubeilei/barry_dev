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
// 06/13/2011	Created by Lynch Yang 
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActDeleteCounter.h"

#include "API/AosApi.h"
#include "CounterUtil/StatTypes.h"
#include "CounterClt/CounterClt.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "ValueSel/ValueSel.h"
#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Actions/ActUpdateCounter.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

using namespace std;

AosActDeleteCounter::AosActDeleteCounter(const bool flag)
:
AosSdocAction(AOSACTTYPE_DELETECOUNTER, AosActionType::eDeleteCounter, flag)
{
}



AosActDeleteCounter::~AosActDeleteCounter()
{
}

bool
AosActDeleteCounter::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function updates a counter based on the smartdoc configuration.
	// sdoc:Action Doc
	// Action Doc format
	// 	<action zky_type = "deletecounter"> 
	// 		<counter_names>
	// 			<counter_name zky_value_type="attr" zky_xpath="city">
	// 				 <zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 			</counter_name>
	// 			<counter_name zky_value_type="attr" zky_xpath="industry">
	// 				 <zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 			</counter_name>
	// 			<counter_name zky_value_type="attr" zky_xpath="company">
	// 			     <zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	//			</counter_name>
	//			or
	//			<counter_name zky_value_type="attr" zky_xpath="xxx" zky_names_flag = "true">
	//			    <zky_docselector zky_type="workingdoc" zky_doc_xpath="entries"></zky_docselector>
	//		    </counter_name>
	// 		</counter_names>
	// 		<counter_value zky_value_type="attr" zky_xpath="value" zky_values_flag="true" zky_data_type="int64|double" factor="100">
	// 		     <zky_docselector zky_docselector_type="receiveddoc"  zky_doc_xpath="entries"></zky_docselector>
	// 		</counter_value>
	// 		<counter_stattype zky_value_type="const"><![CDATA[max|sum|min|avg]]></counter_stattype>
	// 		<counter_operation zky_value_type="const">add|set</counter_operation>
	// 		<counter_time zky_value_type="attr" zky_xpath="time">
	// 			<zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 		</counter_time>
	// 		<counter_type zky_value_type="const">yer|mnt|day</counter_type>
	// 		<counter_timeformat zky_value_type="const">%Y-%m-%d</counter_timeformat>
	// 	</action>
	
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		rdata->setError() << "Missing smartdoc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// gets docid from smart doc. It's necessary.
//	AosXmlTagPtr counterIdChild = sdoc->getFirstChild(AOSTAG_COUNTERID);
//	if (!counterIdChild)
//	{
//		rdata->setError() << "Missing counter docid node";
//		OmnAlarm << rdata->getErrmsg() << enderr;
//		return false;
//	}
//	AosValueRslt valueCounterIdRslt;
//	if (!AosValueSel::getValueStatic(valueCounterIdRslt, counterIdChild, rdata))
//	{
//		rdata->setError() << "Missing counter docid";
//		OmnAlarm << rdata->getErrmsg() << enderr;
//		return false;
//	}
//
	//OmnString counter_id = valueCounterIdRslt.getStr();

	// gets counter name from smart doc. It's necessary.
	AosXmlTagPtr cNamesChild = sdoc->getFirstChild(AOSTAG_COUNTERNAMES);
	if (!cNamesChild)
	{
		rdata->setError() << "Missing counter name nodes";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// counterName should be in the form :
	// xxx|$$|xxx|$$|xxx
	vector<OmnString> counterNames;
	bool rslt = AosActUpdateCounter::getCounterNames(rdata, sdoc, counterNames);
	aos_assert_rr(rslt, rdata, false);

	if(counterNames.size() == 0)
	{
		rdata->setError() << "Failed to get counter name";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// gets counter type from smart doc. It's optional.
	AosXmlTagPtr cTypeChild = sdoc->getFirstChild(AOSTAG_COUNTERTYPE);
	AosValueRslt valueCtypeRslt;
	OmnString ctype;
	if (cTypeChild && AosValueSel::getValueStatic(valueCtypeRslt, cTypeChild, rdata))
	{
		ctype = valueCtypeRslt.getStr();
	}
	AosTimeGran::E time_gran = AosTimeGran::toEnum(ctype);
	
	//// gets counter value from smart doc. It's optional. The default value is "1".
	//AosXmlTagPtr cValueChild = sdoc->getFirstChild(AOSTAG_COUNTERVALUE);
	//AosValueRslt valueVRslt;
	//int64_t value = 0;
	//if (cValueChild && AosValueSel::getValue(valueVRslt, cValueChild, rdata))
	//{
	//	value = valueVRslt.getI64();
	//}
	//value = -value;
	
	// gets counter value from smart doc. It's optional. The default value is "1".
	AosXmlTagPtr cValueChild = sdoc->getFirstChild(AOSTAG_COUNTERVALUE);
	if (!cValueChild)
	{
		rdata->setError() << "Missing counter name nodes";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	vector<int64_t> values;
	rslt = AosActUpdateCounter::getCounterValues(rdata, sdoc,
			        AOSTAG_COUNTERVALUE, values);
	aos_assert_rr(rslt, rdata, false);

	aos_assert_r(values.size() != 0, false);
	vector<int64_t> counterValues;
	for (u32 i = 0; i< values.size(); i++)
	{
		counterValues.push_back(-values[i]);
	}

	AosXmlTagPtr statTypeChild = sdoc->getFirstChild(AOSTAG_COUNTERSTATTYPE);
	AosValueRslt statTypeRslt;
	OmnString statType;
	if (statTypeChild && AosValueSel::getValueStatic(statTypeRslt, statTypeChild, rdata))
	{
		statType = statTypeRslt.getStr();
	}
	
	// gets counter time from smart doc. It's optional. The default value is system clock.
	AosXmlTagPtr cTimeChild = sdoc->getFirstChild(AOSTAG_COUNTERTIME);
	AosValueRslt valueTimeRslt;
	OmnString timeStr;
	if (cTimeChild && AosValueSel::getValueStatic(valueTimeRslt, cTimeChild, rdata))
	{
		timeStr = valueTimeRslt.getStr();
	}

	//gets counter time format from smart doc. It's optional.
	AosXmlTagPtr cTimeFormatChild = sdoc->getFirstChild(AOSTAG_COUNTERTIMEFORMAT);
	AosValueRslt valueTimeFormatRslt;
	OmnString timeFormatStr;
	if (cTimeChild && AosValueSel::getValueStatic(valueTimeFormatRslt, cTimeFormatChild, rdata))
	{
	    timeFormatStr = valueTimeFormatRslt.getStr();
	}
	if (timeStr != "" && timeFormatStr == "") timeFormatStr = "%Y/%m/%d %H:%M:%S";

	//gets counter Operation from smart doc. It's optional. Default values: add
	AosXmlTagPtr cOperationChild = sdoc->getFirstChild(AOSTAG_COUNTEROPERATION);
	AosValueRslt valueOperationRslt;
	OmnString operationStr;
	if (cOperationChild && AosValueSel::getValueStatic(valueOperationRslt, cOperationChild, rdata))
	{
		operationStr = valueOperationRslt.getStr();
	}
	if (operationStr == "") operationStr = "add";

	rslt = false;
	u32 vSize = counterValues.size();
	u32 nSize = counterNames.size();
	aos_assert_r(vSize, false);
	aos_assert_r(nSize, false);
	if(vSize == nSize)
	{
		for (u32 i = 0; i < nSize; i++)
		{
			rslt = AosProcCounter(counterNames[i], counterValues[i], time_gran, 
					timeStr, timeFormatStr, statType, operationStr, rdata);
			aos_assert_r(rslt, false);
		}
		return true;
	}

	if(nSize > vSize)
	{
		for (u32 i = 0; i < nSize; i++)
		{
			rslt = AosProcCounter(counterNames[i], counterValues[0], time_gran, 
					timeStr, timeFormatStr, statType, operationStr, rdata);
			aos_assert_r(rslt, false);
		}
	}
	else
	{
		for (u32 i = 0; i < vSize; i++)
		{
OmnScreen << "name :" << counterNames[0] << " value:" << counterValues[i]<< endl;
			rslt = AosProcCounter(counterNames[0], counterValues[i], time_gran, 
					timeStr, timeFormatStr, statType, operationStr, rdata);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}


AosActionObjPtr
AosActDeleteCounter::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActDeleteCounter(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


