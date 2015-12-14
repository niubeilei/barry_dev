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
// This class simulates Access Operations. In the security model, accesses
// to docs may be translated into User Defined Operations, such as:
// 		Creating Invoice
// 		Approve Request
// This class is used to convert doc accesses into User Defined Operations.
//
// Modification History:
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/StOprTransDocAttr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocSelector/SelRdataDoc.h"
#include "Random/CommonValues.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StDoc.h"
#include "ValueSel/ValueSelAttr.h"
#include "ValueSel/ValueSelNum2StrMap.h"
#include "ValueSel/ValueSelCompose.h"
#include "XmlUtil/SeXmlParser.h"



AosStOprTransDocAttr::AosStOprTransDocAttr(const bool regflag)
:
AosStOprTranslator(eDocAttr, regflag)
{
}


AosStOprTransDocAttr::~AosStOprTransDocAttr()
{
}


OmnString
AosStOprTransDocAttr::setRandContents(const AosRundataPtr &rdata)
{
	// This function translates an SecOpr to a user defined operation
	// based on the values of a selected attribute of the accessed
	// doc that is to be modified:
	// 		Before Value		After Value			Operation
	// 		-------------------------------------------------
	// 		val11				val12				opr1
	// 		val21				val22				opr2
	// 		...
	// 		-------------------------------------------------
	// The XML format is:
	// 	<cond ...>
	// 		<AOSTAG_CONDITION_THEN>
	// 			<valuesel type="map">
	// 				<AOSTAG_VALUE .../>		The one that selects the value to be mapped
	// 				<AOSTAG_MAP .../>		The map
	// 			</valuesel>
	// 		</AOSTAG_CONDITION_THEN>
	// 	</cond>
	
	// 1. Determine the value selector
	OmnString valuesel = determineValueSel(rdata);
	aos_assert_r(valuesel != "", "");

	// 2. Determine the map
	OmnString mapvalues = determineMapValues(rdata);
	aos_assert_r(mapvalues != "", "");

	// 3. Generate the map selector
	OmnString mapsel = "";//AosValueSelNum2StrMap::getXmlStr(
			//AOSTAG_MAP, AosValueSel::eDftLevel, valuesel, mapvalues, rdata);
	aos_assert_r(mapsel != "", "");

	// 4. Construct the THEN part
	OmnString then_str = "<";
	then_str << AOSTAG_CONDITION_THEN << ">"
		<< valuesel << mapsel << "</" << AOSTAG_CONDITION_THEN << ">";

	// 5. Generate the condition
	OmnString condstr = "";//AosCondRandUtil::pickCondition("cond",
			//AosRandomRule::eDftLevel, mCond, then_str, "", 0, rdata);
	mCondSdoc = AosXmlParser::parse(condstr AosMemoryCheckerArgs);
	aos_assert_r(mCondSdoc, "");
	return condstr;
}


OmnString
AosStOprTransDocAttr::determineMapValues(const AosRundataPtr &rdata)
{
	// This function creates the map tag for the condition.
	//
	// 1. Create the values. These values are used to create the map.
	OmnString docstr;
	int nn = OmnRandom::intByRange(
			2, 5, 10, 
			6, 20, 50,
			21, 50, 30,
			51, 100, 10);
	for (int i=0; i<nn; i++)
	{
		if (i != 0) docstr << ",";
		docstr << AosCommonValues::pickAttrName()
			<< "," << AosCommonValues::pickAttrValue();
	}
	return docstr;
}

	
OmnString
AosStOprTransDocAttr::determineValueSel(const AosRundataPtr &rdata)
{
	// This function generates a value selector that is the composing
	// of the values before and after the modifications of a specific
	// attribute, cancatenated by "_". 
	//
	// The XML format is:
	// 	<AOSTAG_VALUE .../>		The one that selects the value to be mapped
	//
	// <AOSTAG_VALUE ...> is a value selector that composes the values
	// before and after the modifications. 
	
	// 1. Determine the attribute name.
	OmnString aname = AosCommonValues::pickAttrName();
	aos_assert_r(aname != "", "");

	// 2. Determine the firts doc selector
	OmnString docselector = "";//AosDocSelRdataDoc::getXmlStr(
			//"docselect", "", "", rdata);
	aos_assert_r(docselector != "", "");

	// 4. Determine the value selectors that select the values before and
	// after the modifications.
	OmnString val_selectors = "";//AosValueSelAttr::getXmlStr("valuesel", 
			//AosRandomRule::eDftLevel, "", aname, docselector, "", rdata);

	// 5. Determine the second doc selector
	docselector = "";//AosDocSelRdataDoc::getXmlStr(
			//"docselect", "", "", rdata);
	aos_assert_r(docselector != "", "");
	val_selectors << "";//AosValueSelAttr::getXmlStr("valuesel", 
			//AosRandomRule::eDftLevel, "", aname, docselector, "", rdata);

	OmnString sep = OmnRandom::pickWordSeparator();

	// 6. Determeine the value selector
	OmnString value_sel = "";//AosValueSelCompose::getXmlStr(
			//AOSTAG_VALUE, AosRandomRule::eDftLevel, val_selectors, sep, rdata);
	return value_sel;
}


bool 
AosStOprTransDocAttr::pickCommand(
		OmnString &cmd, 
		OmnString &operation, 
		const AosSengTestThrdPtr &thread)
{
	OmnNotImplementedYet;
	return false;
}



