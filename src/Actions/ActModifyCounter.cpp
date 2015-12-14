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
#include "Actions/ActModifyCounter.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "ValueSel/ValueSel.h"
#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Actions/ActUpdateCounter.h"
#include "CounterClt/CounterClt.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

using namespace std;

AosActModifyCounter::AosActModifyCounter(const bool flag)
:
AosSdocAction(AOSACTTYPE_MODIFYCOUNTER, AosActionType::eModifyCounter, flag)
{
}


AosActModifyCounter::~AosActModifyCounter()
{
}

bool
AosActModifyCounter::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function modifies a counter based on the smartdoc configuration.
	// sdoc:Action Doc
	// Action Doc format
	// 	<action zky_type = modifycounter>
	// 		<counter_names>
	// 			<counter_name zky_value_type="attr" zky_xpath="city">
	// 				 <zky_docselector zky_docselector_type="workingdoc"></zky_docselector>
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
	//			</counter_name>
	// 		</counter_names>
	// 		<counter_newvalue zky_value_type="attr" zky_xpath="value" zky_values_flag="true" zky_data_type="int64|double" factor="100">
	// 		     <zky_docselector zky_docselector_type="workingdoc"  zky_doc_xpath="entries"></zky_docselector>
	// 		</counter_newvalue>
	// 		<counter_oldvalue zky_value_type="const" zky_data_type="int64" zky_values_flag="true" zky_data_type="int64|double" factor="100">
	// 			<zky_docselector zky_type="docid">
	// 				<zky_valuedef zky_value_type="attr" zky_xpath="zky_docid" zky_data_type="u64">
	// 					<zky_docselector zky_type="workingdoc"/>
	// 				</zky_valuedef>
	// 			</zky_docselector>
	// 		</counter_oldvalue>
	//		<counter_stattype zky_value_type="const"><![CDATA[max|sum|min|avg]]></counter_stattype>
	//	     <counter_operation zky_value_type="const">add|set</counter_operation>
	//		<counter_time zky_value_type="attr" zky_xpath="time">
	//		     <zky_docselector zky_docselector_type="workingdoc"></zky_docselector>
	//	     </counter_time>
	//	     <counter_type zky_value_type="const">yer|mnt|day</counter_type>
	//	     <counter_timeformat zky_value_type="const">%Y-%m-%d</counter_timeformat>
	// 	</action>
	
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		rdata->setError() << "Missing smartdoc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// gets docid from smart doc. It's necessary.cname
	//AosXmlTagPtr CounterIdChild = sdoc->getFirstChild(AOSTAG_COUNTERID);
    //if (!CounterIdChild)
    //{
      //  rdata->setError() << "Missing counter container docid node";
      //  OmnAlarm << rdata->getErrmsg() << enderr;
     //   return false;
    //}
    //AosValueRslt valueCounterIdRslt;
    //if (!AosValueSel::getValueStatic(valueCounterIdRslt, CounterIdChild, rdata))
    //{
     //   rdata->setError() << "Missing counter container docid";
     //   OmnAlarm << rdata->getErrmsg() << enderr;
    //    return false;
    //}

	//OmnString counterId  = valueCounterIdRslt.getStr();

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
	AosActUpdateCounter::getCounterNames(rdata, sdoc, counterNames);

	if (counterNames.size() == 0)
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

	// gets counter stattype from smart doc. It's optional.
    AosXmlTagPtr statTypeChild = sdoc->getFirstChild(AOSTAG_COUNTERSTATTYPE);
    AosValueRslt statTypeRslt;
    OmnString statType;
    if (statTypeChild && AosValueSel::getValueStatic(statTypeRslt, statTypeChild, rdata))
    {
        statType = statTypeRslt.getStr();
    }

	//// gets the old value of counter from smart doc. It's necessary.
	//AosXmlTagPtr cValueOldChild = sdoc->getFirstChild(AOSTAG_COUNTEROLDVALUE);
	//AosValueRslt valueOldRslt;
    //if (!cValueOldChild || !AosValueSel::getValue(valueOldRslt, cValueOldChild, rdata))
    //{
	//	rdata->setError() << "Missing new the counter value" ;
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	return false;
    //}

	//// gets the new value of counter from smart doc. It's necessary.
	//AosXmlTagPtr cValueNewChild = sdoc->getFirstChild(AOSTAG_COUNTERNEWVALUE);
	//AosValueRslt valueNewRslt;
    //if (!cValueNewChild || !AosValueSel::getValue(valueNewRslt, cValueNewChild, rdata))
    //{
	///	rdata->setError() << "Missing new the counter value" ;
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	return false;
    //}
	
	//AosValueRslt diffValue = valueNewRslt - valueOldRslt;
	//int64_t value = diffValue.getI64();
	
	//gets the old value of counter from smart doc. It's necessary.
	AosXmlTagPtr cValueOldChild = sdoc->getFirstChild(AOSTAG_COUNTEROLDVALUE);
	if (!cValueOldChild)
	{
		 rdata->setError() << "Missing new the counter value" ;
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return false;
	}

	// gets the old value of counter from smart doc. It's necessary.
	vector<int64_t> oldvalues;
	bool rslt = AosActUpdateCounter::getCounterValues(rdata, sdoc,
			AOSTAG_COUNTEROLDVALUE, oldvalues);
	aos_assert_r(rslt, false);

	//gets the new value of counter from smart doc. It's necessary.
	AosXmlTagPtr cValueNewChild = sdoc->getFirstChild(AOSTAG_COUNTERNEWVALUE);
    if (!cValueNewChild)
    {
		rdata->setError() << "Missing new the counter value" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// gets the new value of counter from smart doc. It's necessary.
	vector<int64_t> newvalues;
	rslt = AosActUpdateCounter::getCounterValues(rdata, sdoc,
			AOSTAG_COUNTERNEWVALUE, newvalues);
	aos_assert_r(rslt, false);

	vector<int64_t> value;
	for(u32 i = 0; i<newvalues.size(); i++)
	{
		int64_t oldvv =	(i < oldvalues.size())?oldvalues[i]:0;
		int64_t vv = newvalues[i] - oldvv; 
		value.push_back(vv);
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
	u32 vSize = value.size();
	u32 nSize = counterNames.size();
	aos_assert_r(vSize, false);
	aos_assert_r(nSize, false);
	if(vSize == nSize)
	{
		for (u32 i = 0; i < nSize; i++)
		{
OmnScreen << "name :" << counterNames[i] << " value:" << value[i]<< endl;
			rslt = AosProcCounter(counterNames[i], value[i], time_gran, 
					timeStr, timeFormatStr, statType, operationStr, rdata);
			aos_assert_r(rslt, false);
		}
		return true;
	}

	if(nSize > vSize)                      
	{                                       
		for (u32 i = 0; i < nSize; i++)
		{
OmnScreen << "name :" << counterNames[i] << " value:" << value[0]<< endl;
			rslt = AosProcCounter(counterNames[i], value[0], time_gran, 
					timeStr, timeFormatStr, statType, operationStr, rdata);
			aos_assert_r(rslt, false);
		}
	}                                                                       
	else
	{                                       
		for (u32 i = 0; i < vSize; i++)
		{
OmnScreen << "name :" << counterNames[0] << " value:" << value[i]<< endl;
				rslt = AosProcCounter(counterNames[0], value[i], time_gran, 
						timeStr, timeFormatStr, statType, operationStr, rdata);
			aos_assert_r(rslt, false); 
		}
	}                                                                  
	return true;
}


AosActionObjPtr
AosActModifyCounter::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActModifyCounter(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


