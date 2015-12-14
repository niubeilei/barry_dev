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
// Logs are handled based on containers. A container may define one or more 
// logs, which are identified by log-types, which are strings. This term
// is used to retrieve logs. 
//
// Each log entry is an XML, whose docid is Logid. 
//
// Modification History:
// 06/22/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermLog.h"

#include "Alarm/Alarm.h"
#include "IILSelector/IILSelector.h"
#include "QueryCond/QueryCond.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEUtil/IILName.h"


AosTermLog::AosTermLog(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_LOGS, AosQueryType::eLog, regflag)
{
}


AosTermLog::AosTermLog(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_LOGS, AosQueryType::eLog, false),
mDataRetrieved(false)
{
	// The def should be:
	// 	<term type=AOSTERMTYPE_LOG 
	// 		  reverse="true|false" 
	// 		  order="true|false">
	// 		<selector type=AOSIILSELTYPE_LOG
	// 			container="xxx"
	// 			logname="xxx"
	// 			number="xxx"/>
	// 		<cond .../>
	// 	</term>
	//
	mReverse = (def->getAttrStr("reverse", "") == "true");
	mOrder = (def->getAttrStr("order", "") == "true");
	//mCond = AosQueryCond::parseCondStatic(def, rdata);
	AosXmlTagPtr selector = def->getFirstChild("selector");
	aos_assert(selector);
	mCtnrObjid = selector->getAttrStr("container", "");
	mLogName = selector->getAttrStr("logname", "");
	mNumber = selector->getAttrInt64("number", 20);
	mIsGood = AosIILSelector::resolveIIL(def, mIILName, rdata);
	if (!mIsGood) return;
}


AosTermLog::AosTermLog(
		const OmnString &ctnr_objid,
		const OmnString &logname,
		const bool reverse, 
		const bool order, 
		const AosOpr opr,
		const i64 &logtime,
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_LOGS, AosQueryType::eLog, false),
mDataRetrieved(false),
mCtnrObjid(ctnr_objid),
mLogName(logname)
{
	mReverse = reverse;
	mOrder = order;
	mCond = AosQueryCond::createCond(opr, logtime);
	mIsGood = true;
}


AosTermLog::~AosTermLog()
{
}


void
AosTermLog::toString(OmnString &str)
{
	OmnString od = (mReverse) ? "true" : "false";
	str << "<tag type=\"lg\" container=\"" << mIILName
	<< "\" reverse=\"" << od
	<< "\" order=\"" << mOrder
	<< "\"/>";
}


bool 
AosTermLog::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context,false);
	if (mDataRetrieved) return true;
	mDataRetrieved = true;
	if (mCtnrObjid == "") 
	{
		rdata->setError() << "No container objid specified!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	query_context->setReverse(mReverse);
	query_context->setOpr(eAosOpr_an);
	bool rslt = AosQueryColumn(mIILName, query_rslt, query_bitmap, query_context, rdata);
	return rslt;
}


AosXmlTagPtr 
AosTermLog::getDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	// Chen Ding, 2013/06/17
	OmnNotImplementedYet;
	return 0;
	// return  AosSeLogClientObj::getLogClient()->retrieveLog(docid, rdata);
}


bool
AosTermLog::convertToXml(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const OmnString fields[5],
		const i64 &nn,
		const AosRundataPtr &rdata)
{
	/* LoginLog is not fixed yet. Waiting for Joshi
	// It is to retrieve login logs. The query should be in the form:
	//      "<container>|$|lg|$|logtime|$|reverse|$|opr
	// Note that to logs, reverse means just the opposite.
	// It should container the query to the following format:
	//  <term type="lg" reverse="true" container="xxx"/>
	AOSMONITORLOG_LINE(rdata);
	bool rev = (nn >= 4 && fields[3] == "true")?false:true;
	i64 logtime = (nn >= 3 && fields[2] != "*")?atoll(fields[2]):0;
	AosOpr opr = (nn >= 5 && fields[4] != "")?
	AosOpr_toEnum(fields[4]):eAosOpr_an;
	mSpecialTerm = OmnNew AosTermLoginLog(mSiteid, fields[0], rev, opr, logtime, rdata);
	appendJoins(AOSTAG_USERID, errmsg);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool 
AosTermLog::convertQuery(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosQueryTermObjPtr
AosTermLog::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermLog(def, rdata);
}

