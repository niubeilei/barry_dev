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
#include "Query/TermArith.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "QueryCond/CondNames.h"
#include "QueryUtil/QrUtil.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/StrSplit.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosTermArith::AosTermArith(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_ARITH, AosQueryType::eArith, regflag)
{
}


AosTermArith::AosTermArith(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_ARITH, AosQueryType::eArith, false)
{
	// 'def' is in the form:
	// 	<term type="lt|le|eq|ne|gt|ge|an|r1" odrattr="true|false" odrctnr="xxx">
	// 		<lhs><![CDATA[xxx]]></lhs>
	// 		<rhs><![CDATA[xxx]]></rhs>
	// 		<rhs2><![CDATA[xxx]]></rhs2>
	//	</term>
	//
	// The new form will be:
	// 	<term type="arith" 
	// 		order="true|false" 
	// 		reverse="true|false"
	// 		AOSTAG_VALUE_DFT_PROC="xxx"
	// 		iilname="xxx"
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
	}
	else
	{
		//Ice 2011/09/21
		mFieldname = def->xpathQuery("selector/@aname");

		mIILName = def->getAttrStr("iilname");
		if (mIILName == "")
		{
			mIsGood = AosIILSelector::resolveIIL(def, mIILName, rdata);
		}
		if (!mIsGood) return;

		bool exist;
		mOpr = AosOpr_toEnum(def->xpathQuery("cond/@zky_opr", exist, ""));
		mValue = def->xpathQuery("cond/_$text", exist, "");

		// Chen Ding, 2013/12/31
		vector<OmnString> values;
		int nn = AosSplitStr(mValue, "$$", values, 2);
		if (nn == 2)
		{
			vector<OmnString> pair;
			nn = AosSplitStr(values[0], "$", pair, 2);
			OmnString new_value;
			if (nn == 2)
			{
				const char *data = pair[1].data();
				char *ptr = (char *)strstr(data, "epoch_hour(");
				if (ptr && data[pair[1].length()-1] == ')')
				{
					OmnString ss(&ptr[11]);
					ss.setLength(ss.length()-1);
					i64 uu = AosTimeUtil::str2EpochTime(ss);
					i64 epoch_hour = uu/3600;
					new_value << pair[0] << "$" << epoch_hour;
				}
				else
				{
					new_value << pair[0] << "$" << pair[1];
				}
			}
			else
			{
				new_value << pair[0];
			}

			nn = AosSplitStr(values[1], "$", pair, 2);
			if (nn == 2)
			{
				const char *data = pair[1].data();
				char *ptr = (char *)strstr(data, "epoch_hour(");
				if (ptr && data[pair[1].length()-1] == ')')
				{
					OmnString ss(&ptr[11]);
					ss.setLength(ss.length()-1);
					i64 uu = AosTimeUtil::str2EpochTime(ss);
					i32 epoch_hour = uu/3600;
					new_value << "$$" << pair[0] << "$" << epoch_hour;
				}
				else
				{
					new_value << "$$" << pair[0] << "$" << pair[1];
				}
			}
			else
			{
				new_value << "$$" << pair[0];
			}

			mValue = new_value;
		}

		OmnString tmpvalue = mValue;
		AosXmlTagPtr varlag = def->xpathGetChild("cond/parm");
		if(mValue == "" && (varlag || (tmpvalue.length() > 8 &&strncmp(tmpvalue.data(), "clipvar/", 8) == 0)))
		{
			mValue = def->xpathQuery("cond/@zky_initvalue", exist, "");
		}

		// Linda, 2012/12/19 
		// IIL Query Range(iilKey>= mValue && iilKey <= mValue2)
		// Chen Ding, 2013/09/05
		// if (mOpr == eAosOpr_range_ge_le && mValue != "")
		if (mValue != "")
		{
			if (AosOpr_isRangeOpr(mOpr))
			{
				bool finished = false;
				vector<OmnString> str;
				OmnString sep = "$$";
				i64 n = AosStrSplit::splitStrBySubstr(
						mValue.data(), sep.data(), str, 2, finished);
				if (n == 2 && finished && str.size() == 2)
				{
					mValue = str[0];
					mValue2 = str[1];
				}
			}
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
		
		/*
		// Chen Ding, 09/15/2011
		if (mValue.length() == 24 && mValue == "logininfo/zky_cloudid__a")
		{
			mValue = rdata->getCid();
		}
		else if (mValue.length() > 8 &&
			strncmp(mValue.data(), "clipvar/", 8) == 0)
		{
			// This means the data is configured to listen to the 
			// clipvar. Will ignore it.
			mValue = "";
		}
		*/
	}

	// Chen Ding, 09/08/2011
	// if (mOpr != eAosOpr_an && (mValue == "" || mValue == AOS_NULL_STRING)) 
	// {
	// 	mIsGood = false;
	// }
	mIsGood = !ignore_value;
}


AosTermArith::AosTermArith(
		const OmnString &aname, 
		const OmnString &rhs,
		const bool order,
		const bool reverse, 
		const AosOpr opr,
		const OmnString &ctnr_objid,
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_ARITH, AosQueryType::eArith, false)
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
	//		mIILName = AosIILName::composeCtnrMemberListing( mCtnrObjid, mFieldname);
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

AosTermArith::AosTermArith(
		const OmnString &iilname,
		const AosOpr opr,
		const OmnString &rhs,
		const bool order,
		const bool reverse, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_ARITH, AosQueryType::eArith, false)
{
	// Ketty 2014/02/14
	mOrder = order;
	mReverse = reverse;
	mValue = rhs;
	mOpr = opr;
	mIILName = iilname;
	bool ignore;
	//AosQrUtil::procQueryVars(mCtnrObjid, "", "", ignore, rdata);
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

	if (mValue == "*")
	{
		mOpr = eAosOpr_an;
	}

	mIsGood = (mValue != "" && mValue != AOS_NULL_STRING);
}


void
AosTermArith::toString(OmnString &str)
{
	// 	<arith iilname="xxx" reverse="xxx" opr="xxx" value="xxx"/>
	OmnString od = (mReverse)?"true":"false";
	str << "<arith iilname=\"" << mIILName
		<< "\" reverse=\"" << od
		<< "\" order=\"" << mOrder
		<< "\" odrattr=\"" << mCtnrObjid
		<< "\" opr=\"" << AosOpr_toStr(mOpr)
		<< "\" value=\"" << mValue; 
		if (mValue2 != "") str << "\" value2=\"" << mValue2; 
		str << "\"/>";
}

bool
AosTermArith::convertToXml(
		const OmnString &cond_str, 
		const AosXmlTagPtr &and_tag, 
		const OmnString fields[5],
		const i64 &nn,
		const AosRundataPtr &rdata)
{
	// It converts the string format term to the XML form: 
	//  <term type="arith" order="true|false" reverse="true|false">
	// 	   	<selector type="attr" container="xxx" aname="xxx"/>
	// 	   	<cond .../>
	// 	</term>

	OmnString aname = fields[0];
	OmnString opr = fields[1];
	OmnString value = fields[2];

	// 1. Add the term tag
	AosXmlTagPtr term_tag = and_tag->addNode1("term");
	aos_assert_r(term_tag, false);
	term_tag->setAttr(AOSTAG_TYPE, AOSTERMTYPE_ARITH);
	term_tag->setAttr("reverse", "false");
	term_tag->setAttr("order", "false");
	term_tag->setAttr("zky_ctobjid", "giccreators_querynew_term_h");

	//2.Add the selector tag
	AosXmlTagPtr selector_tag = term_tag->addNode1("selector");
	aos_assert_r(selector_tag, false);
	selector_tag->setAttr(AOSTAG_TYPE, AOSIILSELTYPE_ATTR);
	selector_tag->setAttr("aname", aname);

	// 3. Add the condition
	AosXmlTagPtr cond_tag = term_tag->addNode1("cond");
	aos_assert_r(cond_tag, false);
	cond_tag->setAttr(AOSTAG_TYPE, AOSCONDTYPE_ARITH);
	cond_tag->setAttr(AOSTAG_OPR, opr);
	cond_tag->setNodeText(value, true);
	return true;
}


bool
AosTermArith::convertQuery(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	// Convert the following:
	// 	<term type="lt|le|eq|ne|gt|ge|an" 
	// 		odrattr="true"
	// 		odrctnr="xxx">
	// 	 	<lhs><![CDATA[xxx]]></lhs>
	// 	  	<rhs><![CDATA[xxx]]></rhs>
	// 	</term>
	//
	// To the new form:
	//  <term type="arith" order="true|false" reverse="true|false">
	// 	   	<selector type="attr" container="xxx" aname="xxx"/>
	// 	   	<cond .../>
	// 	</term>

	OmnString opr = term->getAttrStr(AOSTAG_TYPE);
	term->setAttr(AOSTAG_TYPE, AOSTERMTYPE_ARITH);
	//Johzi, 2011/08/26
	term->setAttr("zky_ctobjid", "giccreators_querynew_term_h");

	bool odrattr = term->getAttrBool("odrattr", false);
	OmnString ctnr_objid = term->getAttrStr("odrctnr");
	term->removeAttr("odrattr");
	term->removeAttr("odrctnr");
	OmnString lhs = term->xpathQuery("lhs/_$text");
	OmnString rhs = term->xpathQuery("rhs/_$text");
	aos_assert_rr(lhs != "", rdata, false);

	aos_assert_r(term->removeNode("lhs", false, false), false);
	AosXmlTagPtr selector_tag = term->addNode1("selector");
	aos_assert_r(selector_tag, false);
	selector_tag->setAttr(AOSTAG_TYPE, AOSIILSELTYPE_ATTR);
	if (odrattr && ctnr_objid != "")
	{
		selector_tag->setAttr(AOSTAG_CONTAINER_NOZK, ctnr_objid);
	}

	selector_tag->setAttr("aname", lhs);

	// Create the condition
	AosXmlTagPtr cond_tag = term->addNode1("cond");
	aos_assert_r(cond_tag, false);
	cond_tag->setAttr(AOSTAG_TYPE, AOSCONDTYPE_ARITH);
	cond_tag->setAttr(AOSTAG_OPR, opr);

	if (rhs != "")
	{
		cond_tag->setNodeText(rhs, true);
	}
	else
	{
		// Need to check whether it is in the form:
		// 	<rhs type="var">
		// 		<parm .../>
		// 	</rhs>
		OmnString type = term->xpathQuery("rhs/type");
		if (type == "var")
		{
			AosXmlTagPtr rhstag = term->getFirstChild("rhs");
			aos_assert_r(rhstag, false);
			AosXmlTagPtr parmtag = rhstag->getFirstChild();
			aos_assert_r(parmtag, false);
			AosXmlTagPtr newparmtag = cond_tag->addNode(parmtag);
			aos_assert_r(newparmtag, false);
		}
	}
	aos_assert_r(term->removeNode("rhs", false, false), false);
	
	return true;
}


bool 
AosTermArith::convertEpochCond(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosQueryTermObjPtr
AosTermArith::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermArith(def, rdata);
}

bool
AosTermArith::isStatValueTerm()
{
	return false;
}

bool
AosTermArith::isTimeTerm()
{
	return mFieldname == "_time";
}


