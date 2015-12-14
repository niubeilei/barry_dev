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
#include "Actions/ActUpdateCounter.h"

#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterClt/CounterClt.h"
#include "CounterUtil/CounterUtil.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "ValueSel/ValueSel.h"
#include "Util/OmnNew.h"
#include "Util/DataTypes.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <vector>

using namespace std;

AosActUpdateCounter::AosActUpdateCounter(const bool flag)
:
AosSdocAction(AOSACTTYPE_UPDATECOUNTER, AosActionType::eUpdateCounter, flag)
{
}

AosActUpdateCounter::~AosActUpdateCounter()
{
}

bool
AosActUpdateCounter::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function updates a counter based on the smartdoc configuration.
	// sdoc:Action Doc
	// Action Doc format
	// 	<action type="updatecounter"> 
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
	// 			<counter_name zky_value_type="const">xxxxx</counter_name>
	// 			or
	// 			<counter_name zky_value_type="attr" zky_xpath="xxx" zky_names_flag = "true">
	// 				<zky_docselector zky_type="workingdoc" zky_doc_xpath="entries"></zky_docselector>
	// 			</counter_name>
	// 		</counter_names>
	// 		<counter_value zky_value_type="attr" zky_xpath="value" zky_values_flag="true" zky_data_type="int64|double" factor="100">
	// 			 <zky_docselector zky_docselector_type="receiveddoc" | zky_doc_xpath="entries"></zky_docselector>
	// 		</counter_value>
	// 		<counter_stattype zky_value_type="const"><![CDATA[max|sum|min|avg]]></counter_stattype
	// 		<counter_time zky_value_type="attr" zky_xpath="time">
	// 			 <zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 		</counter_time>
	// 		<counter_type zky_value_type="const">yer|mnt|day</counter_type>
	// 		<counter_timeformat zky_value_type="const">%Y-%m-%d</counter_timeformat>
	// 		<counter_operation zky_value_type="const">add|set</counter_operation>
	// 	</action>
	
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		rdata->setError() << "Missing smartdoc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	// gets docid from smart doc. It's necessary.
	//AosXmlTagPtr CounterIdChild = sdoc->getFirstChild(AOSTAG_COUNTERID);
	//if (!CounterIdChild)
	//{
	//	rdata->setError() << "Missing counter container docid node";
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	return false;
	//}
	//AosValueRslt valueCounterIdRslt;
	//if (!AosValueSel::getValueStatic(valueCounterIdRslt, CounterIdChild, rdata))
	//{
	//	rdata->setError() << "Missing counter container docid";
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	return false;
	//}
	//
	//OmnString counterId = valueCounterIdRslt.getStr();
	// counterName should be in the form :
	// xxx|$$|xxx|$$|xxx
	// gets counter name from smart doc. It's necessary.
	vector<OmnString> counterNames;
	bool rslt = getCounterNames(rdata, sdoc, counterNames);
	aos_assert_r(rslt, false);

	// gets counter value from smart doc. It's optional. The default value is "1".
	vector<int64_t> counterValues;
	rslt = getCounterValues(rdata, sdoc, 
			AOSTAG_COUNTERVALUE, counterValues);
	aos_assert_r(rslt, false);

	// gets counter type from smart doc. It's optional.
	AosXmlTagPtr cTypeChild = sdoc->getFirstChild(AOSTAG_COUNTERTYPE);
	AosValueRslt valueCtypeRslt;
	OmnString ctype;
	if (cTypeChild && AosValueSel::getValueStatic(valueCtypeRslt, cTypeChild, rdata))
	{
		ctype = valueCtypeRslt.getStr();
	}
	AosTimeGran::E time_gran = AosTimeGran::toEnum(ctype);

	// gets counter stattype from smart doc. It's optional.
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

	//gets counter time format from smart doc. It's optional. Default values:%Y/%m/%d %H:%M:%S 
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

	u32 vSize = counterValues.size();
	u32 nSize = counterNames.size();
	aos_assert_r(vSize, false);
	aos_assert_r(nSize, false);
	rslt = false;
	if(vSize == nSize)
	{
		for (u32 i = 0; i < nSize; i++)
		{
OmnScreen << "name :" << counterNames[i] << " value:" << counterValues[i]<< endl;
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
OmnScreen << "name :" << counterNames[i] << " value:" << counterValues[0]<< endl;
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


bool
AosActUpdateCounter::getCounterValues(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		vector<int64_t> &counterValues)
{
	// This function retrieves the counter's value. It is defined by 'sdoc':
	// 	<sdoc ...>
	// 		<tagname factor="xxx" .../>
	// 		...
	// 	</sdoc>
	// where 'tagname' is a subtag that defines a value selector, 'factor'
	// is optional. If the value type is double and if 'factor' is not 0, 
	// the actual value is multiplied by 'factor'.
	aos_assert_r(tagname != "", false);
	// gets counter value from smart doc. It's optional. The default value is "1".
	//AosXmlTagPtr cValueChild = sdoc->getFirstChild(AOSTAG_COUNTERVALUE);
	AosXmlTagPtr cValueChild = sdoc->getFirstChild(tagname);
	AosValueRslt valueVRslt;
	int64_t value;
	if (!cValueChild)
	{
		value = 1;
		counterValues.push_back(value);
		return true;
	}

	// Check whether it is to retrieve just one value (AOSTAG_VALUES_FLAG="false")
	// or an array of values. 
	if (!cValueChild->getAttrBool(AOSTAG_VALUES_FLAG, false))
	{
		if (AosValueSel::getValueStatic(valueVRslt, cValueChild, rdata))
		{
			if (valueVRslt.getType() == AosDataType::eDouble)
			{
				// The value is supposed to be double, but the system 
				// can only store integral values. It needs to be 
				// converted into integers by multipling a factor,
				// which can be retrieved from 'cValueChild'.
				double factor = cValueChild->getAttrDouble("factor", 1);
				double vv = valueVRslt.getDouble();
				vv *= factor;
				vv = vv + 0.5;
				value = (int64_t)vv;
			}
			else
			{
				value = valueVRslt.getI64();
			}
		}
		else
		{
			value = 0;
		}
		counterValues.push_back(value);
		return true;
	}

	//The 'entires' should be in the format:
	//<entries>
	//<entry xpath="xx"/>
	//<entry xpath="xx"/>
	//<entry xpath="xx"/>
	//<entry xpath="xx"/>
	//</entries>
	counterValues.clear();
	AosXmlTagPtr selector = cValueChild->xpathGetChild(AOSTAG_DOCSELECTOR);
	if (!selector)
	{
		AosSetError(rdata, AosErrmsgId::eSmartDocIncorrect);
		OmnAlarm << rdata->getErrmsg() << ". Def: " << sdoc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr entries = AosRunDocSelector(rdata, selector);
	if (entries && entries->isRootTag()) entries = entries->getFirstChild();
	if (!entries)
	{
		rdata->setError() << "Missing source doc!";
		return false;
	}

	OmnString xpath = cValueChild->getAttrStr(AOSTAG_XPATH);
	aos_assert_r(xpath != "", false);
	OmnString datatype = cValueChild->getAttrStr(AOSTAG_DATA_TYPE);
	AosXmlTagPtr entry = entries->getFirstChild(); 
	while (entry)
	{
		if (datatype == AOSDATATYPE_DOUBLE)
		{
			double factor = cValueChild->getAttrDouble("factor", 1);
			bool exist;
			OmnString v1 = entry->xpathQuery(xpath, exist, "1"); 
			double vv = atof(v1.data());
			vv *= factor;
			vv = vv + 0.5;
			value = (int64_t)vv;
		}
		else
		{
			bool exist;
			OmnString v1 = entry->xpathQuery(xpath, exist, "1");
			value = atoll(v1.data());
			//value = entry->getAttrInt64(xpath, 1);
		}

		counterValues.push_back(value);
		entry = entries->getNextChild();
	}
	aos_assert_r(counterValues.size() != 0, false);
	return true;
}


bool
AosActUpdateCounter::getCounterNames(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &sdoc,
				vector<OmnString> &counterNames)
{
	// gets counter name from smart doc. It's necessary.
	AosXmlTagPtr cNamesChild = sdoc->getFirstChild(AOSTAG_COUNTERNAMES);
	if (!cNamesChild)
	{
		rdata->setError() << "Missing counter name nodes";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr cNameTag;
	AosXmlTagPtr cNameChild = cNamesChild->getFirstChild(AOSTAG_COUNTERNAME);
	OmnString counterName;
	while(cNameChild)
	{
		if (cNameChild->getAttrBool(AOSTAG_NAMES_FLAG, false))
		{
			cNameTag = cNameChild;
		}
		else
		{
			AosValueRslt valueCnameRslt;
			if (AosValueSel::getValueStatic(valueCnameRslt, cNameChild, rdata))
			{
				OmnString vv = valueCnameRslt.getStr();
				if (vv != "")
				{
					if (counterName == "") 
					{
						counterName = vv;
					}
					else
					{
						counterName << AOS_COUNTER_SEGMENT_SEP << vv;
					}
				}
			}
		}
		cNameChild = cNamesChild->getNextChild(AOSTAG_COUNTERNAME);
	}

	if (counterName == "")
	{
		rdata->setError() << "Failed to get counter name";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString cname = counterName;
	counterNames.push_back(cname);

	//The 'entires' should be in the format:
	//<entries>
	//<entry xpath="xx"/>
	//<entry xpath="xx"/>
	//<entry xpath="xx"/>
	//<entry xpath="xx"/>
	//</entries>
	
	if (!cNameTag) return true;
	counterNames.clear();

	aos_assert_r(cNameTag, false);
	AosXmlTagPtr selector = cNameTag->xpathGetChild(AOSTAG_DOCSELECTOR);
	if (!selector)
	{
		AosSetError(rdata, AosErrmsgId::eSmartDocIncorrect);
		OmnAlarm << rdata->getErrmsg() << ". Def: " << sdoc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr entries = AosRunDocSelector(rdata, selector);
	if (entries && entries->isRootTag()) entries = entries->getFirstChild();
	if (!entries)
	{
		rdata->setError() << "Missing entries doc!";
		return false;
	}

	OmnString xpath = cNameTag->getAttrStr(AOSTAG_XPATH);
	aos_assert_r(xpath != "", false);
	AosXmlTagPtr entry = entries->getFirstChild(); 
	while (entry)
	{
		bool exist;
		OmnString name = entry->xpathQuery(xpath, exist, "");
		OmnString countername = counterName;
		if (countername == "") 
		{
			countername = name;
		}
		else
		{
			countername <<  AOS_COUNTER_SEGMENT_SEP << name;
		}
		counterNames.push_back(countername);
		entry = entries->getNextChild();
	}

	aos_assert_r(counterNames.size() != 0, false);
	return true;
}


AosActionObjPtr
AosActUpdateCounter::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActUpdateCounter(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


