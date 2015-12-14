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
// This is a Term Template. When creating a new term, one should copy
// this file and modify it. 
//
// Modification History:
// 08/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermVersionDocs.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "Query/TermAnd.h"
#include "Query/IILSelTypes.h"
#include "QueryCond/QueryCond.h"
#include "Rundata/Rundata.h"
#include "SeLogSvr/SeLogEntryVersion.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/IILClientObj.h"


AosTermVersionDocs::AosTermVersionDocs(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_VERSION_DOCS, AosQueryType::eVersionDocs, regflag)
{
}


AosTermVersionDocs::AosTermVersionDocs(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_VERSION_DOCS, AosQueryType::eVersionDocs, false)
{
	if (!parse(def, rdata))
	{
		OmnExcept e(__FILE__, __LINE__, rdata->getErrmsg());
		throw e;
	}
}


AosTermVersionDocs::~AosTermVersionDocs()
{
}


bool
AosTermVersionDocs::parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// It assumes 'def':
	// 	<term type= AOSTERMTYPE_VERSION_DOCS
	// 		  reverse="true|false" 
	// 		  order="true|false"
	// 		  objid="xxx">
	// 		<selector 
	// 			type="ver"
	// 			container="xxx"/>
	// 	</term>
	//
	aos_assert_rr(def, rdata, false);

	mReverse = (def->getAttrStr("reverse", "") == "true");
	mOrder = (def->getAttrStr("order", "") == "true");
	mObjid = def->getAttrStr("objid", "");
	aos_assert_r(mObjid != "", false);
	mIsGood = initIILName(mObjid, rdata);
	if (!mIsGood) return false;
	return true;
}


bool
AosTermVersionDocs::initIILName(const OmnString &objid, const AosRundataPtr &rdata)
{
	OmnString iilname = AosIILName::composeDeletedObjidIILName();
	aos_assert_rr(iilname != "", rdata, false);
	aos_assert_r(objid != "", false);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (!doc)
	{
		u64 logid;
		bool isunique;
		//AosIILClient::getSelf()->getDocid(iilname, 
		AosIILClientObj::getIILClient()->getDocid(iilname, 
				objid, eAosOpr_eq, true, logid, isunique, rdata);
		if (logid == 0)
		{
			AosSetError(rdata, AosErrmsgId::eMissingDoc);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		// Chen Ding, 2013/06/17
		OmnNotImplementedYet;
		return false;
		/*
		doc = AosSeLogClientObj::getLogClient()->retrieveLog(logid, rdata);
		if (!doc)
		{
			AosSetError(rdata, AosErrmsgId::eFailedRetrieveDoc);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		*/
	}
	OmnString container = doc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(container != "", false);
	mIILName = AosIILName::composeVersionIILName(container);
	aos_assert_r(mIILName != "", false);
	mCtnrDocid = AosDocClientObj::getDocClient()->getDocidByObjid(container, rdata);
	if (mCtnrDocid == 0)
	{
		u64 logid;
		bool isunique;
		//AosIILClient::getSelf()->getDocid(iilname, 
		AosIILClientObj::getIILClient()->getDocid(iilname, 
				objid, eAosOpr_eq, true, logid, isunique, rdata);
		if (logid == 0)
		{
			AosSetError(rdata, AosErrmsgId::eMissingDoc);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		// Chen Ding, 2013/06/17
		OmnNotImplementedYet;
		return false;
		/*
		doc = AosSeLogClientObj::getLogClient()->retrieveLog(logid, rdata);
		if (!doc)
		{
			AosSetError(rdata, AosErrmsgId::eFailedRetrieveDoc);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		mCtnrDocid = doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(mCtnrDocid>0, false);
		*/
	}
	return true;
}

void
AosTermVersionDocs::toString(OmnString &str)
{
	// This function converts the term into an XML.
	// 	<term type="ret_vers" order="true|false" reverse="true|false">
	// 		<iilselector type="versions" objid="xxx" docid="xxx"/>
	// 		<cond type="xxx" .../>
	// 	</term>
}


AosQueryTermObjPtr
AosTermVersionDocs::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermVersionDocs(def, rdata);
}


bool 	
AosTermVersionDocs::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	// get the number of this obj.
	if (!query_context || !query_rslt)
	{
		OmnNotImplementedYet;
		return false;
	}
	// bool rslt = AosIILClient::getSelf()->getDocid(iilname, mObjid, count, rdata);
	// aos_assert_r(rslt, false);
	// if (count == 0) return true;
	// query_rslt->setPageSize(count);

	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(mObjid);
	
	return AosQueryColumn(mIILName, query_rslt, 0, query_context, rdata);
}


bool 	
AosTermVersionDocs::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	docid = 0;
	finished = true;

	aos_assert_r(parent_term, false);
	if (!mDataLoaded)
	{
		aos_assert_r(parent_term->getType() == AosQueryType::eAnd, false);
		AosQueryRsltObjPtr query_rslt = parent_term->getQueryData();
		if (!query_rslt)
		{
			OmnAlarm << "Query rslt is null!" << enderr;
			docid = 0;
			finished = true;
			return false;
		}
		loadData(query_rslt, rdata);
		if (!mRetrievedVers || !mCrtRecord)
		{
			docid = 0;
			finished = true;
			return true;
		}

		mCrtIdx = 1;
		docid = 1;
		finished = false;
		return true;
	}

	if (!mRetrievedVers || !mCrtRecord)
	{
		OmnAlarm << "Internal error!" << enderr;
		docid = 0;
		finished = true;
		return false;
	}

	mCrtRecord = mRetrievedVers->getNextChild();
	if (!mCrtRecord)
	{
		docid = 0;
		finished = true;
		return true;
	}

	mCrtIdx++;
	docid = mCrtIdx;
	finished = false;
	return true;
}


bool 	
AosTermVersionDocs::loadData(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the data based on 'query_rslt'.
	if (mDataLoaded) return true;
	
	mRetrievedVers = 0;
	mDataLoaded = true;
	u64 logid;
	bool finished = false;
	OmnString req = "<Contents fields=\"";
	req << AOSTAG_OBJID << "|$|" << AOSTAG_OBJID << "|$|1" << "|$$|" 
		<< AOSTAG_DOCID << "|$|" << AOSTAG_DOCID << "|$|1" << "|$$|"
		<< AOSTAG_LOGID << "|$|" << AOSTAG_LOGID << "|$|1" << "|$$|"
		<< AOSTAG_VERSION << "|$|" << AOSTAG_VERSION << "|$|1" << "\">";
	i64 num_logs = 0;
	while (!finished)
	{
		bool rslt = query_rslt->nextDocid(logid, finished, rdata);
		if (!rslt || logid == 0 || finished) break;
		if (num_logs++ != 0) req << ",";
		req << logid;
	}
	req << "</Contents>";

	if (num_logs == 0)
	{
		return true;
	}
	
	AosXmlParser parser;
	AosXmlTagPtr request = parser.parse(req, "" AosMemoryCheckerArgs);
	aos_assert_r(request, false);

	// Chen Ding, 2013/06/17
	OmnNotImplementedYet;
	return false;
	/*
	bool rslt = AosSeLogClientObj::getLogClient()->retrieveLogs(mRetrievedVers, 
			request, mCtnrDocid, rdata);
	if (!rslt || !mRetrievedVers)
	{
		rdata->setError() << "Failed to retrieve versions!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// mRetrievedVers should be in the form:
	// 	<Contents>
	// 		<record .../>
	// 		<record .../>
	// 		...
	// 	</Contents>
	mCrtRecord = mRetrievedVers->getFirstChild();
	return true;
	*/
}

	
AosXmlTagPtr
AosTermVersionDocs::getDoc(const u64 &logid, const AosRundataPtr &rdata)
{
	aos_assert_r(mCrtRecord, 0);
	return mCrtRecord;
}

