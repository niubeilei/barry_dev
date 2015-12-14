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
// 05/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ValueSelObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/StrSplit.h"



static AosValueSelObjPtr 	sgValueSel[AosValueSelType::eMax+1];
static OmnMutex 			sgLock;

#include "ValueSel/AllValueSel.h"

AosValueSelObj::AosValueSelObj(
		const OmnString &name, 
		const AosValueSelType::E type, 
		const bool reg)
:
mName(name),
mType(type),
mIsTemplate(false)
{
}


AosValueSelObj::~AosValueSelObj()
{
}


bool
AosValueSelObj::registerValueSel(AosValueSelObjPtr &actopr)
{
	AosValueSelType::E type = actopr->mType;
	if (type <= AosValueSelType::eInvalid || type >= AosValueSelType::eMax)
	{
		OmnAlarm << "Invalid action id: " << type << enderr;
		return false;
	}
	
	OmnString errmsg;
	bool rslt = AosValueSelType::addName(actopr->mName, type, errmsg);
	if (!rslt)
	{
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (!sgValueSel[type])
	{
		sgValueSel[type] = actopr;
	}
	return true;
}


AosValueSelObjPtr
AosValueSelObj::getValueSelStatic(
		const AosXmlTagPtr &item, 
		const AosRundataPtr &rdata) 
{
	/*
	// Chen Ding, 2013/07/14
	OmnString id = item->getAttrStr(AOSTAG_TYPE);
	if (id == "")
	{
		// Backward compatibility
		id = item->getAttrStr(AOSTAG_VALUE_TYPE);
	}*/
	// Ken Lee, 2013/07/26
	OmnString id = item->getAttrStr(AOSTAG_VALUE_TYPE);
	if (id == "")
	{
		id = item->getAttrStr(AOSTAG_TYPE);
	}

	if (id == "")
	{
		// Check whether it is a DLL
		OmnNotImplementedYet;
		return 0;
	}
	aos_assert_rr(id != "", rdata, 0);
	
	AosValueSelType::E type = AosValueSelType::toEnum(id);
	aos_assert_rr(AosValueSelType::isValid(type), rdata, 0);
	
	return sgValueSel[type];	
}


AosValueSelObjPtr
AosValueSelObj::getValueSelStatic(
		const AosValueSelType::E type, 
		const AosRundataPtr &rdata) 
{
	aos_assert_rr(AosValueSelType::isValid(type), rdata, 0);
	return sgValueSel[type];	
}

	
bool
AosValueSelObj::getValueStatic(
			AosValueRslt &valueRslt,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata) 
{
	aos_assert_rr(sdoc, rdata, false);
	AosValueSelObjPtr value_sel = getValueSelStatic(sdoc, rdata);
	if (value_sel)
	{
		return value_sel->run(valueRslt, sdoc, rdata);
	}

	AosXmlTagPtr jimo_doc;
	AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), sdoc, jimo_doc);
	if (jimo)
	{
		bool rslt = jimo->run(rdata, sdoc);
		valueRslt = rdata->getValueRslt();
		return rslt;
	}

	AosSetErrorUser(rdata, "invalid_value_selector") << sdoc->toString() << enderr;
	return false;
}


/*
bool
AosValueSelObj::getValueForCounterVarStatic(
		const OmnString &xpath, 
		OmnString &value, 
		const AosRundataPtr &rdata)
{
	// This function is used to retrieve value for counters. 
	// 'xpath' is in the form:
	// 		did:xpath
	// 	
	// where 'did' indicates how to get the doc, and 'xpath' 
	// specifies the xpath in that doc.
	// In the current implementations:
	// 	01: Received Doc
	// 	02: Retrieved Doc
	// 	03: Created Doc
	aos_assert_r(xpath != "" && rdata, false);
	AosStrSplit split;
	bool finished;
	OmnString pair[3];
	split.splitStr(xpath.data(), ":", pair, 3, finished);
	aos_assert_r(pair[0] != "" && pair[1] != "" && pair[2] != "", false);
	AosXmlTagPtr docXml;
	int docType = atoi(pair[1].data());
	OmnString path = pair[2];
	switch(docType)
	{
	case 1 : docXml = rdata->getReceivedDoc(); break;
	case 2 : docXml = rdata->getRetrievedDoc(); break;
	case 3 : docXml = rdata->getCreatedDoc(); break;
	default :
			 AosSetErrorU(rdata, "unrecognized_doc_type") << pair[0] << enderr;
	}
	aos_assert_r(docXml, false);
	bool exist;
	value = docXml->xpathQuery(path, exist, "");
	return exist;
}
*/


bool 
AosValueSelObj::retrieveValuesStatic(
		vector<OmnString> &m_values,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This function retrieves values based on 'sdoc', which is in the form:
	// 	<sdoc ...>
	// 		<valuesel .../>
	// 		<valuesel .../>
	// 		...
	// 	</sdoc>
	m_values.clear();
	aos_assert_rr(sdoc, rdata, false);
	AosXmlTagPtr def = sdoc->getFirstChild();
	OmnString vv;
	while (def)
	{
		AosValueRslt value;
		bool rslt = getValueStatic(value, def, rdata);
		aos_assert_rr(rslt, rdata, false);
		vv = value.getStr();
		if (!rslt)
		{
			OmnAlarm << "value is not string" << enderr;
		}
		else
		{
			m_values.push_back(vv);
		}
		def = sdoc->getNextChild();
	}
	return true;
}


AosValueSelObjPtr
AosValueSelObj::getValueSelectorStatic(const AosValueSelType::E type)
{
	aos_assert_r(AosValueSelType::isValid(type), 0);
	return sgValueSel[type];
}


bool
AosValueSelObj::composeValuesStatic(
			AosValueRslt &valueRslt,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata)
{
	// It assumes 'sdoc' is in the form:
	// 	<values>
	// 		<value AOSTAG_VALUE_TYPE= "attr/,,,".../>
	// 		<value .../>
	// 		...
	// 	</values>
	OmnString value;
	aos_assert_r(sdoc, false);
	AosValueRslt vv;
	AosXmlTagPtr item = sdoc->getFirstChild();	
	OmnString result;
	bool rslt;
	while(item)
	{
		aos_assert_r(getValueStatic(vv, item, rdata), false);
		result = vv.getStr();
		if (!rslt)
		{
			OmnAlarm << "value is not a string" << enderr;
		}
		else
		{
			value << result;
		}
		item = sdoc->getNextChild();
	}
	valueRslt.setStr(value);
	return true;
}


bool 
AosValueSelObj::getValueStatic(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(sdoc, rdata, false);
	AosXmlTagPtr tag = sdoc->getFirstChild(tagname);
	if (!tag)
	{
		AosSetErrorUser(rdata, "missing_value_selector")
			<< AOSDICTERM("tagname", rdata.getPtr()) << ": " << tagname << enderr;
		return false;
	}

	return getValueStatic(valueRslt, tag, rdata);
}

