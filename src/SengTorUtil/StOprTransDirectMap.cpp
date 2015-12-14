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
// It does not care which doc being accessed. This is
// normally used to convert the accesses to a container into
// user defined operations. Example:
// 		Container: "Customers"
// 		Add Member: Operation is "Creating Customer"
//
// This means that eDirectMap directly maps an SecOpr to a user
// defined operation:
// 		sec_opr1	user_operation1
// 		sec_opr2	user_operation2
// 		...
// The map is defined by 'mUserOperation'.
//
// Modification History:
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/StOprTransDirectMap.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
//#include "Conds/Torturer/CondRandUtil.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StDoc.h"
#include "ValueSel/ValueSelConst.h"
#include "XmlUtil/SeXmlParser.h"


AosStOprTransDirectMap::AosStOprTransDirectMap(const bool regflag)
:
AosStOprTranslator(eDirectMap, regflag)
{
}


AosStOprTransDirectMap::~AosStOprTransDirectMap()
{
}


OmnString
AosStOprTransDirectMap::setRandContents(const AosRundataPtr &rdata)
{
	// This function creates a eDirectMap entry, which maps sec_opr to a 
	// user defined operation. 
	// 	<condition ...>
	// 		<AOSTAG_CONDITION_THEN>
	// 			<value_sel ... />
	// 		</AOSTAG_CONDITION_THEN>
	// 		<AOSTAG_CONDITION_ELSE>
	// 			<value_sel .../>
	// 		</AOSTAG_CONDITION_ELSE>
	// 	</condition>
	// bool rslt = generateRandCond(rdata);
	// aos_assert_r(rslt, false);
	
	// In the current implementations, it simply generates a 
	// word, and creates a const value selector.
	OmnString oprname = OmnRandom::word30();
	OmnString then_str = "";//AosValueSelConst::getXmlStr(
			//AOSTAG_CONDITION_THEN, AosValueSel::eDftLevel, oprname, rdata);
	aos_assert_r(then_str != "", "");
	// AosXmlTagPtr child = AosXmlParser::parse(sel_str AosMemoryCheckerArgs);
	// aos_assert_r(child, false);
	// mCondSdoc->addNode(child);

	OmnString condstr = "";//AosCondRandUtil::pickCondition("cond", 
//			AosRandomRule::eDftLevel, mCond, then_str, "", 0, rdata);
	aos_assert_r(mCond, "");
	mCondSdoc = AosXmlParser::parse(condstr AosMemoryCheckerArgs);
	aos_assert_r(mCondSdoc, "");
	return condstr;
}


AosStOprTranslatorPtr
AosStOprTransDirectMap::clone() const
{
	return OmnNew AosStOprTransDirectMap(false);
}


bool 
AosStOprTransDirectMap::pickCommand(
		OmnString &cmd, 
		OmnString &operation, 
		const AosSengTestThrdPtr &thread)
{
	OmnNotImplementedYet;
	return false;
}

