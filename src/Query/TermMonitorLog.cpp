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
// 01/03/2011 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "Query/TermMonitorLog.h"

#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEModules/LogMgr.h"
#include "SEUtil/Docid.h"
#include "XmlUtil/XmlTag.h"




AosTermMonitorLog::AosTermMonitorLog(const bool regflag)
:
AosQueryTerm(AOSTERMTYPE_MONITOR_LOG, AosQueryType::eMonitorLog, regflag)
{
}


AosTermMonitorLog::AosTermMonitorLog(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_MONITOR_LOG, AosQueryType::eMonitorLog, false)
{
	// The def should be:
	// 	<term type=AOSQUERYTYPE_MONITORLOG siteid="xxx" reverse="true" docid="xxx"/>
	mProblemDocid = def->getAttrStr("docid", "");
	mReverse = (def->getAttrStr("reverse", "") == "true");
	mIsGood = true;
	mCrtLogDoc = 0;
	mPreLogLoc = 0;
}


AosTermMonitorLog::AosTermMonitorLog(
		const OmnString &problem_docid, 
		const bool reverse) 
:
AosQueryTerm(AOSTERMTYPE_MONITOR_LOG, AosQueryType::eMonitorLog, false)
{
	if(problem_docid != "")
	{
		mProblemDocid = problem_docid.data();
	}
	mReverse = reverse;
	mIsGood = true;
	mCrtLogDoc = 0;
	mPreLogLoc = 0;
}


AosTermMonitorLog::~AosTermMonitorLog()
{
}


void
AosTermMonitorLog::toString(OmnString &str)
{
	OmnString od = (mReverse)?"true":"false";
	str << "<tag type=\"" << AOSQUERYTYPE_MONITORLOG << "\" problem_docid=\"" << mProblemDocid
	<< "\" reverse=\"" << od
	<< "\"/>";
}


bool 	
AosTermMonitorLog::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	finished = true;
	return false;
	/*
	finished = false;

	if (mNoMoreDocs)
	{
		finished = true;
		docid = AOS_INVDID;
		return true;
	}
	
	u32 seqno;
	u64 offset;
	AosXmlTagPtr doc;
	if(!mPreLogLoc)
	{
		//get the preMonitorTransId from the problem Doc identified by mProblemDocid
		u64 did = AosDocid::convertToU64(mProblemDocid); 
		//AosXmlTagPtr problemDoc = AosDocClientObj::getDocClient()->getDocByDocId(mProblemDocid, "", rdata);
		AosXmlTagPtr problemDoc = AosDocClientObj::getDocClient()->getDocByDocid(did, rdata);
		
		if (!problemDoc)
		{
			finished = true;
			docid = AOS_INVDID;
			rdata->setError() << "Problem doc not found: " << mProblemDocid;
			return false;
		}

		if (problemDoc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_PROBLEMREPORT)
		{
			finished = true;
			docid = AOS_INVDID;
			rdata->setError() << "Problem Doc not correct!";
			return false;
		}

		OmnString preMonitorTid = problemDoc->getAttrStr(AOSTAG_PREMONITORTID, "");
		if (preMonitorTid == "")
		{
			finished = true;
			docid = AOS_INVDID;
			rdata->setError() << "Missing problem ID!";
			return false;
		}
		//get log docid. (seqno + offset).
		OmnStrParser1 parser(preMonitorTid, "_");
		mBrowserId = atoll(parser.nextWord());
		seqno = atoll(parser.nextWord());
		offset = atoll(parser.nextWord());
	}
	else
	{
		seqno = (u32)mPreLogLoc;
		offset = mPreLogLoc >> 32;
	}

	bool init = true;
	u32 cBId = 0;
	while(cBId != mBrowserId)
	{
		if(init)
		{
			init = false;
		}else
		{
			seqno = (u32)mPreLogLoc;
			offset = mPreLogLoc >> 32;
		}
		
		doc = AosRundata::readMonitorLog(seqno, offset, mPreLogLoc, cBId);
		if(!doc && !mPreLogLoc)
		{
			mNoMoreDocs = true;
			finished = true;
			return true;
		}
	}

	mCrtLogDoc = doc;
	docid = (offset << 32) + seqno;
	return true;
	*/
}


i64		
AosTermMonitorLog::getTotal(const AosRundataPtr &rdata)
{
	i64 total = 0;
	bool finished = false;
	u64 docid;
	OmnString errmsg;

	AosXmlTagPtr 	cld = mCrtLogDoc;
	u64				pll = mPreLogLoc;
	mPreLogLoc = 0;
	mCrtLogDoc = 0;

	while(!finished && total < eMaxLogs)
	{
		bool rslt = nextDocid(0, docid, finished, rdata);
		aos_assert_r(rslt, 0);
		total++;
	}

	mCrtLogDoc = cld;
	mPreLogLoc = pll;

	return total;
}


bool	
AosTermMonitorLog::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	return false;
}


void	
AosTermMonitorLog::reset(const AosRundataPtr &rdata)
{
	/*
	mCrtIdx = -10;
	mCrtIILIdx = -10;
	mNoMoreDocs = false;
	mCrtLogDoc = 0;
	mPreLogLoc = 0;
	*/
	OmnNotImplementedYet;
}


bool 	
AosTermMonitorLog::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	/*
	bool finished = false;
	u64 docid;
	mCrtIdx = -10;
	mCrtIILIdx = -10;
	mPreLogLoc = 0;
	mCrtLogDoc = 0;
	
	for (i64 i=0; i<startidx; i++)
	{
		aos_assert_r(nextDocid(docid, finished, rdata), false);
		if (finished) return false;
	}
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosTermMonitorLog::setOrder(
		const OmnString &container, 
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &)
{
	return false;
}


AosXmlTagPtr
AosTermMonitorLog::getDoc(const u64 &docid)
{
	//retrieve log entry by docid, this docid is logSeqno.
	if(!mCrtLogDoc || !docid)
	{
		mNoMoreDocs = true;
		mCrtLogDoc = 0;
		mPreLogLoc = 0;
		return 0;
	}
	return mCrtLogDoc;
}


bool
AosTermMonitorLog::convertToXml(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const OmnString fields[5],
		const i64 &nn,
		const AosRundataPtr &rdata)
{
	// "AOSQUERYTYPE_MONITORLOG|$|=|$|problem_docid|$|1"
	// bool rev = (nn >= 3 && fields[2] == "true")?true:false;
	// bool rev = true;
	// mSpecialTerm = OmnNew AosTermMonitorLog(mSiteid, fields[2], rev);
	OmnAlarm << "Monitor Log needs to rework: " << cond_str << enderr;
	return false;
}


AosQueryTermObjPtr
AosTermMonitorLog::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermMonitorLog(def, rdata);
}


bool	
AosTermMonitorLog::collectInfo(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}







