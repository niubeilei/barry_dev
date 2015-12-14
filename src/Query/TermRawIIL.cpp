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
// 06/07/2012 Created by Felicia Peng
////////////////////////////////////////////////////////////////////////////
#include "Query/TermRawIIL.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "QueryCond/CondNames.h"
#include "QueryUtil/QrUtil.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"


AosTermRawIIL::AosTermRawIIL(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_RAWIIL, AosQueryType::eRawIIL, regflag)
{
}

AosTermRawIIL::~AosTermRawIIL()
{
}

AosTermRawIIL::AosTermRawIIL(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_RAWIIL, AosQueryType::eRawIIL, false)
{
	// 'def' is in the form:
	// 	<term type="lt|le|eq|ne|gt|ge|an" odrattr="true|false" odrctnr="xxx">
	// 		<lhs><![CDATA[xxx]]></lhs>
	// 		<rhs><![CDATA[xxx]]></rhs>
	//	</term>
	//
	// The new form will be:
	// 	<term type="arith" 
	// 		order="true|false" 
	// 		reverse="true|false"
	// 		AOSTAG_VALUE_DFT_PROC="xxx"
	// 		AOSTAG_CTNR_DFT_PROC="xxx">
	// 		<selector type=AOSIILSELTYPE_ATTR 
	// 			container="xxx" 
	// 			AOSTAG_ANAME="xxx"/>
	// 		<cond .../>
	// 		<AOSTAG_VALUE_DFT_VALUE>default_value</AOSTAG_VALUE_DFT_VALUE>
	// 	</term>
	//
	
	// Chen Ding, 08/23/2011
	// Backward compatibility
	bool ignore_ctnr = false;
	bool ignore_value = false;
	if (!def->getFirstChild(AOSTAG_SELECTOR))
	{
		mFieldname = def->xpathQuery("lhs/_#text");
		mIILName = AosIILName::composeCtnrMemberListing("", mFieldname);
		OmnString opr = def->getAttrStr("type");
		mValue = def->xpathQuery("rhs/_#text");
		if (opr == "eq")
		{
			mOpr = eAosOpr_eq;
		}
		mIsGood = true;
		mQueryRsltOpr = def->getAttrStr("operator", "index");
	}
	else
	{
		mFieldname = def->xpathQuery("selector/@aname");
		mQueryRsltOpr = def->xpathQuery("selector/@operator", "index");

		mIsGood = AosIILSelector::resolveIIL(def, mIILId, rdata);

		if (!mIsGood || !mIILId)
		{
			mIsGood = AosIILSelector::resolveIIL(def, mIILName, rdata);
		}
		if (!mIsGood) return;

		bool exist;
		mOpr = AosOpr_toEnum(def->xpathQuery("cond/@zky_opr", exist, ""));
		mValue = def->xpathQuery("cond/_$text", exist, "");
		OmnString tmpvalue = mValue;
		AosXmlTagPtr varlag = def->xpathGetChild("cond/parm");
		if(mValue == "" && (varlag || (tmpvalue.length() > 8 &&strncmp(tmpvalue.data(), "clipvar/", 8) == 0)))
		{
			mValue = def->xpathQuery("cond/@zky_initvalue", exist, "");
		}
		mCtnrObjid = def->xpathQuery("selector/@container", "");

		OmnString xpath = AOSTAG_CTNR_DFT_VALUE;
		xpath << "/_#text";
		OmnString dftproc = def->getAttrStr(AOSTAG_CTNR_DFT_PROC);
		OmnString dftvalue = def->xpathQuery(xpath);
		AosQrUtil::procQueryVars(mCtnrObjid, dftproc, dftvalue, ignore_ctnr, rdata);

		xpath = AOSTAG_VALUE_DFT_VALUE;
		xpath << "/_#text";
		dftproc = def->getAttrStr(AOSTAG_VALUE_DFT_PROC);
		dftvalue = def->xpathQuery(xpath);
		AosQrUtil::procQueryVars(mValue, dftproc, dftvalue, ignore_value, rdata);
	}

	mIsGood = !ignore_value;
}


AosTermRawIIL::AosTermRawIIL(
		const OmnString &aname, 
		const OmnString &rhs,
		const bool order,
		const bool reverse, 
		const AosOpr opr,
		const OmnString &ctnr_objid,
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_RAWIIL, AosQueryType::eRawIIL, false)
{
	mOrder = order;
	mReverse = reverse;
	mValue = rhs;
	mOpr = opr;
	mCtnrObjid = ctnr_objid;
	bool ignore;
	AosQrUtil::procQueryVars(mCtnrObjid, "", "", ignore, rdata);
	AosQrUtil::procQueryVars(mValue, "", "", ignore, rdata);

	if (!AosOpr_valid(mOpr))
	{
		rdata->setError() << "Operator is invalid: " << mOpr;
		OmnAlarm << rdata->getErrmsg() << enderr;
		mIsGood = false;
		return;
	}

	// Note that 'mValue' should be a quoted string. Need to remove the quotes.
	// Quotes can be either single quote or double quote. 
	if (!mValue.removeQuotes(false))
	{
		rdata->setError() << "The value does not seem to be a quoted string or " 
			<< "the quotes are incorrect: ";
		OmnAlarm << rdata->getErrmsg() << enderr;
		mIsGood = false;
		return;
	}

	//if (mOpr == eAosOpr_Objid)
	//{
	//	// It is to query based on objid
	//	if (mCtnrObjid != "") 
	//	{
	//		mIILName = AosIILName::composeCtnrMemberListing(mCtnrObjid, mFieldname);
	//	}
	//	else
	//	{
	//		mIILName = AosIILName::composeObjidListingName(rdata->getSiteid());
	//	}
	//}
	//else
	//{
		mIILName = AosIILName::composeCtnrMemberListing(mCtnrObjid, aname);
	//}

	if (mValue == "*")
	{
		mOpr = eAosOpr_an;
	}

	mIsGood = (mValue != "" && mValue != AOS_NULL_STRING);
}


void
AosTermRawIIL::toString(OmnString &str)
{
	// 	<arith iilname="xxx" reverse="xxx" opr="xxx" value="xxx"/>
	/*OmnString od = (mReverse)?"true":"false";
	str << "<arith iilname=\"" << mIILName
		<< "\" reverse=\"" << od
		<< "\" order=\"" << mOrder
		<< "\" odrattr=\"" << mCtnrObjid
		<< "\" opr=\"" << AosOpr_toStr(mOpr)
		<< "\" value=\"" << mValue << "\"/>";
	*/
}


AosQueryTermObjPtr
AosTermRawIIL::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	return OmnNew AosTermRawIIL(def, rdata);
}


AosXmlTagPtr
AosTermRawIIL::getDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}


AosXmlTagPtr
AosTermRawIIL::getDoc(const u64 &docid, const OmnString &key, const AosRundataPtr &rdata)
{
	// It will create an XML doc:
	// 		<record AOSTAG_KEY="xxx" AOSTAG_VALUE="xxx"/>
	OmnString docstr;
	docstr << "<record>"
		   << "<" << AOSTAG_KEY << "><![CDATA[" << key << "]]></" << AOSTAG_KEY << ">"
		   << "<" << AOSTAG_VALUE << "><![CDATA[" << docid << "]]></" << AOSTAG_VALUE << ">"
		   << "</record>";

	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);

//OmnScreen << "key = " << key << "   value=" << docid << endl;
	return doc;
}


bool 	
AosTermRawIIL::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	docid = 0;
	finished = true;
	return false;
}

bool
AosTermRawIIL::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(query_rslt, rdata, false);
	query_rslt->setWithValues(true);
	query_rslt->setOperator(AosDataColOpr::toEnum(mQueryRsltOpr));

	if(query_context)
	{
		query_context->setOpr(mOpr);
		query_context->setStrValue(mValue);
		query_context->setReverse(mReverse);
	}
	
	if (mIILId)
	{
		return AosQueryColumn(mIILId, query_rslt, query_bitmap, query_context, rdata);
	}

	return AosQueryColumn(mIILName, query_rslt, query_bitmap, query_context, rdata);
}

