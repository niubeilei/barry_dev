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
#include "EventCond/CondArith.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "IILClient/IILClient.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocServer.h"
#include "SEUtilServer/UserDocMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


#include <string>
#include <vector>
using namespace std;

AosCondArith::AosCondArith()
:
AosEventCond(AosEventCondType::eEq, true)
{
}


AosCondArith::~AosCondArith()
{
}

	
bool	
AosCondArith::evalCond(
		const AosXmlTagPtr &def,
		bool rslt,
		const AosRundataPtr &rdata)
{
	// It evaluates a condition:
	// 	<cond type="001" opr="xxx">
	// 		<lhs .../>
	// 		<rhs .../>
	// 	</cond>
	//
	//  XMLVar: 
	//  	<lhs type="const">abc</type>
	//  <lhs type="obj" attrname="xxx"/>
	//  <cond type="001" docname="xxx" aname="xxx" opr="xxx">value</cond>
	//  docname identifies the doc whose attribute is evaluated
	//  aname is the name of the attribute whose value is retrieved
	//  opr is the arithmetical operator. Allowed values are: 
	//  value is the value to be compared
	
	OmnString errmsg;
	aos_assert_r(def, false);
	aos_assert_r(rdata, false);

//	AosXmlTagPtr sdoc = smartdoc->getDoc();
//	aos_assert_r(sdoc, false);

	AosXmlTagPtr xml = def;
	OmnString opr = xml->getAttrStr("opr");
	if(opr == "")
	{
		errmsg << "opr is empty!~";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString type = xml->getAttrStr("type");
	if(type == "")
	{
		errmsg << "type is empty!~";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	//OmnString value1 = AosValueSel::getValue(xml->getNextChild(), rdata);
	//OmnString value2 = AosValueSel::getValue(xml->getNextChild(), rdata);
	OmnString value1, value2;
	AosValueRslt valueRslt;
	if (AosValueSel::composeValuesStatic(valueRslt, xml->getNextChild(), rdata))
	{
		if (!valueRslt.isValid())
		{
			rdata->setError() << "Value is invalid";
			return false;
		}

		if (valueRslt.isXmlDoc())
		{
			rdata->setError() << "Value is an XML doc";
			return false;
		}

		//value1 = valueRslt.getValueStr();
		valueRslt.getValueStr(value1);
	}

	AosValueRslt valueRslt1;
	if (AosValueSel::composeValuesStatic(valueRslt1, xml->getNextChild(), rdata))
	{
		if (!valueRslt1.isValid())
		{
			rdata->setError() << "Value is invalid";
			return false;
		}

		if (valueRslt1.isXmlDoc())
		{
			rdata->setError() << "Value is an XML doc";
			return false;
		}

		//value2 = valueRslt1.getValueStr();
		valueRslt1.getValueStr(value2);
	}

	const char* type_c = type.data();
	switch(type_c[0])
	{
		case 'i':
		if(type == "int")
			    return cmpCond(opr, atoi(value1.data()), atoi(value2.data()));

		case 's':
		if(type == "string")
			    return  cmpCond(opr, value1, value2);

		default:
		OmnAlarm << "nonlicet type~" << enderr;
		break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosCondArith::cmpCond(
		const OmnString &opr,
		const int &v1,
		const int &v2)
{
	const char* opr_c = opr.data();
	switch(opr_c[0])
	{
		case 'e':
			if(opr == "eq") return v1 == v2;

		case 'n':
			if(opr == "ne") return v1 != v2;

		case 'l':
			if(opr == "le") return v1 <= v2;
			if(opr == "lt") return v1 < v2;

		case 'g':
			if(opr == "ge") return v1 >= v2;
			if(opr == "gt") return v1 > v2;

		default:
			OmnAlarm << "nonlicet opr~~~" << enderr;
			break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosCondArith::cmpCond(
		const OmnString &opr,
		const OmnString &v1,
		const OmnString &v2)
{
	int i = strcmp(v1.data(), v2.data());
	const char* opr_c = opr.data();
	switch(opr_c[0])
	{
	case 'e':
		 if(opr == "eq") return i == 0;

	case 'n':
		 if(opr == "ne") return i != 0;

	case 'l':
		 if(opr == "le") return i <= 0;
		 if(opr == "lt") return i < 0;

	case 'g':
		 if(opr == "ge") return i >= 0;
		 if(opr == "gt") return i > 0;

	default:
		 OmnAlarm << "nonlicet opr~~~" << enderr;
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}

