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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermIIL.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "Query/TermArith.h"
#include "Rundata/Rundata.h"
#include "SEUtil/SeTypes.h"
#include "Util/ValueRslt.h"


AosTermIIL::AosTermIIL(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_IIL, AosQueryType::eIIL, regflag)
{
}


AosTermIIL::AosTermIIL(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_IIL, AosQueryType::eIIL, false)
{
	// 'def' is in the form:
	// 	<term type="lt|le|eq|ne|gt|ge|an" odrattr="true|false" odrctnr="xxx">
	// 		<lhs><![CDATA[xxx]]></lhs>
	// 		<rhs><![CDATA[xxx]]></rhs>
	//	</term>
	//
	// The new form will be:
	// 	<term type="arith" order="true|false" reverse="true|false">
	// 		<selector type=AOSIILSELTYPE_ATTR 
	// 			container="xxx" 
	// 			AOSTAG_ANAME="xxx"/>
	// 		<cond zky_opr="xxx">xxx</cond>
	// 	</term>
	//
	bool exist;
	mOpr = AosOpr_toEnum(def->xpathQuery("cond/@zky_opr", exist, ""));
	mValue = def->xpathQuery("cond/_$text", exist, "");
	mIsGood = AosIILSelector::resolveIIL(def, mIILName, rdata);
}


AosTermIIL::AosTermIIL(const AosQueryCondInfo &cond, const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_IIL, AosQueryType::eIIL, false)
{
	mIILName = AosIILName::composeContainerAttrIILName(cond.mContainer, cond.mAname);
	mValue = cond.mValue;
	mOpr = cond.mOpr;
	mReverse = cond.mReverse;
	mOrder = cond.mOrder;
}


AosTermIIL::AosTermIIL(
		const OmnString &ctnr_objid,
		const OmnString &aname, 
		const OmnString &value, 
		const AosOpr opr,
		const bool reverse,
		const bool order,
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_IIL, AosQueryType::eIIL, false)
{
	mIILName = AosIILName::composeContainerAttrIILName(ctnr_objid, aname);
	mValue = value;
	mOpr = opr;
	mReverse = reverse;
	mOrder = order;
}


AosTermIIL::AosTermIIL(
		const u64 &iilid, 
		const OmnString &value, 
		const AosOpr opr,
		const bool reverse,
		const bool order,
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_IIL, AosQueryType::eIIL, false)
{
	mIILId = iilid; 
	mValue = value;
	mOpr = opr;
	mReverse = reverse;
	mOrder = order;
}


// Chen Ding, 2013/02/07
AosTermIIL::AosTermIIL(
		const OmnString &iilname, 
		const AosValueRslt &value1, 
		const AosValueRslt &value2, 
		const AosOpr opr,
		const bool reverse,
		const bool order,
		const bool flag,
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_IIL, AosQueryType::eIIL, false)
{
	mIILId = 0; 
	mIILName = iilname;
	mValue = value1.getStr();
	mValue2 = value2.getStr();
	mOpr = opr;
	mReverse = reverse;
	mOrder = order;
}


void
AosTermIIL::toString(OmnString &str)
{
	// 	<arith iilname="xxx" reverse="xxx" opr="xxx" value="xxx"/>
	str << "<iil iilname=\"" << mIILName
		<< "\" reverse=\"" << mReverse
		<< "\" order=\"" << mOrder
		<< "\" opr=\"" << AosOpr_toStr(mOpr)
		<< "\" value=\"" << mValue << "\"/>";
}


AosQueryTermObjPtr
AosTermIIL::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermIIL(def, rdata);
}

