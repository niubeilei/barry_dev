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
#include "Query/QueryReq.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "LogUtil/XmlLog.h"
#include "Porting/TimeOfDay.h"
#include "Query/QueryReq.h"
#include "Query/Ptrs.h"
#include "QueryClient/QueryClient.h"
#include "Query/TermOr.h"
#include "Query/TermAnd.h"
#include "Query/TermLog.h"
#include "Query/TermKeywords.h"
#include "Query/TermAllowance.h"
#include "Query/TermContainer.h"
#include "Query/TermWhoReadme.h"
#include "Query/TermUserCustom.h"
#include "Query/TermFriend.h"
#include "Query/TermTag.h"
#include "Query/TermArith.h"
#include "Query/TermCounterSingle.h"
#include "Query/TermComment.h"
#include "Query/TermMonitorLog.h"
#include "QueryUtil/QrUtil.h"
#include "QueryUtil/QueryConfig.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryEngineObj.h"
#include "SEInterfaces/QueryProcCallback.h"
#include "SEInterfaces/QueryCacherObj.h"
#include "SEInterfaces/QueryProcObj.h"
//#include "SEInterfaces/StatQueryAnalyzerObj.h"
//#include "StatQueryAnalyzer/StatQueryUnit.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Thread/Mutex.h"
#include "Util/Opr.h"
#include "Util/StrSplit.h"
#include "Util/StrParser.h"
#include "Util/OmnNew.h"
#include "Query/Update.h"
#include "WordParser/WordNorm.h"
#include "VersionServer/VersionServer.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/XmlRc.h"
#include "QueryClient/BatchQuery.h"
#include "QueryClient/BatchQueryReader.h"


AosWordNormPtr 	AosQueryReq::smWordNorm = OmnNew AosWordNorm("","");
OmnMutexPtr	  	AosQueryReq::smIDLock = OmnNew OmnMutex();
i64			  	AosQueryReq::smIDCur = 0;

static i64 sgNumQueries = 0;


AosQueryReq::AosQueryReq()
:
mIsGood(false),
mIsNew(true),
mReverseOrder(false),
mPsize(eDftPsize),
mCrtIdx(0),
mStartIdx(0),
mNumDocs(0),
mTotal(0),
mLock(OmnNew OmnMutex()),
mLock1(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mGetTotal(false),
mBatchQueryFlag(false),
mIsSmartQuery(false),
mIsStatQuery(false)		// Ketty 2014/01/22
{
	//OmnScreen << "create query req:[" << this << "]" << endl; 
}


AosQueryReq::AosQueryReq(
		const i64 &qid,
		const AosRundataPtr &rdata)
:
mIsGood(false),
mIsNew(true),
mQueryId(qid),
mReverseOrder(false),
mPsize(eDftPsize),
mCrtIdx(0),
mStartIdx(0),
mNumDocs(0),
mTotal(0),
mLock(OmnNew OmnMutex()),
mLock1(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mGetTotal(false),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mBatchQueryFlag(false),
mIsSmartQuery(false),
mIsStatQuery(false)		// Ketty 2014/01/22
{
	// Chen Ding, 2013/01/03
	//mQueryCacher = AosCreateQueryCacher(rdata);

	//OmnScreen << "create query req:[" << this << "]" << endl; 
}


AosQueryReq::AosQueryReq(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
mIsGood(true),
mIsNew(true),
mReverseOrder(false),
mPsize(eDftPsize),
mCrtIdx(0),
mStartIdx(0),
mNumDocs(0),
mTotal(0),
mLock(OmnNew OmnMutex()),
mLock1(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mGetTotal(false),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mBatchQueryFlag(false),
mIsSmartQuery(false),
mIsStatQuery(false)		// Ketty 2014/01/22
{
	// Chen Ding, 2013/01/03
	//mQueryCacher = AosCreateQueryCacher(rdata);

	mOrigReq = def;
	mQueryId = def->getAttrInt64(AOSTAG_QUERYID, 0);
	if(!mQueryId)
	{
		mQueryId = getNewQueryID();
	}
	setStartIdx(def->getAttrInt64("start_idx",0));
	mNumDocs = 0;

	mGetTotal = def->getAttrBool("get_total");
	setPsize(def->getAttrStr("psize"));

	// Chen Ding, 08/20/2011
	// We no longer support this.
	// mQueryReq->setOrderName(def->getAttrStr("order2"));
	// mSort = def->getAttrBool("sortflag");
	// mIncrease = def->getAttrBool("increase");
	mRemoveMeta = def->getAttrBool("rmmetadata");
	mSubopr = def->getAttrStr("subopr");

	// Chen Ding, 08/20/2011
	// mQueryReq->setOrder(def->getAttrStr("order"), def->getAttrStr("reverse"), rdata);
	setOrder(def->getAttrStr("orderctnr"), 
			def->getAttrStr("order"), def->getAttrStr("reverse"), rdata);

	mQueryStartTime = OmnGetTimestamp();
	mCrtIdx = -1;

	bool rslt = parseSqlPublic(rdata, def);
	if (!rslt)
	{
		mIsGood = false;
		return;
	}
	rdata->setOk();

	//OmnScreen << "create query req:[" << this << "]" << endl; 
}


AosQueryReq::~AosQueryReq()
{
	//OmnScreen << "delete query req:[" << this << "]" << endl; 
}
	

AosQueryReqObjPtr
AosQueryReq::createQuery(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosQueryReqObjPtr query;
	try
	{
		query = OmnNew AosQueryReq(def, rdata);
	}
	catch(...)
	{
		OmnAlarm << "failed to create query" << enderr;
		return 0;
	}
	return query;
}


bool	
AosQueryReq::procPublic(
		const AosXmlTagPtr &cmd,
		OmnString &contents,
		const AosRundataPtr &rdata)

{
	// mQueryStartTime = OmnGetTimestamp();
	OmnString errmsg;
	mConfig = cmd;
	mBatchQueryFlag = mConfig->getAttrBool("zky_batch_query_flag", false);
	
	bool rslt = runQuery(rdata, cmd, contents);
	rdata->setOk();
	return rslt;
}


bool
AosQueryReq::procSQL(
		const AosRundataPtr &rdata,
		OmnString &contents)
{
	bool finished = false;
	OmnString records;
	mNumDocs = 0;
	// mQueryStartTime = OmnGetTimestamp();
	if (mSubopr == "version") 
	{
		rdata->setError() << "version not supported anymore!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (mSubopr == "ver_deleted")
	{
		rdata->setError() << "ver_deleted not supported anymore!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (mSubopr == "delobjs")
	{
		rdata->setError() << "delobjs not supported anymore!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = analyzeQuery(rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "invalid_query");
		return false;
	}
	// Chen Ding, 2013/08/18
	if (mIsSmartQuery)
	{
		smartQueryPostProcess(contents, rdata);
		return true;
	}
	
	if (mIsStatQuery)
	{
		// Ketty, 2014/01/20
		return false;
		//AosQueryReqObjPtr thisptr(this, false);
		//return AosStatQueryAnalyzerObj::generateRsltStatic(rdata,
		//		thisptr, contents);
	}

	rslt = generateRslt(records, finished, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "invalid_query");
		return false;
	}

	return finishUpQuery(contents, records, finished, rdata);
}


bool
AosQueryReq::finishUpQuery(
		OmnString &contents,
		const OmnString &records, 
		bool &finished,
		const AosRundataPtr &rdata)
{
	if(mCrtIdx < 0 && mNumDocs > 0)mCrtIdx = 0;

	i64 preIdx = mCrtIdx;
	mCrtIdx += mNumDocs;

	// AOSMONITORLOG_LINE(rdata);

	if (finished)
	{
	 	// The query is finished. Need to inform the query manager
	 	// to remove this query.
		// AOSMONITORLOG_LINE(rdata);
	 	AosQueryReqObjPtr thisptr(this, false);
		AosQueryClient::getSelf()->queryFinished(thisptr, rdata);
 		if (mTotal > mCrtIdx)
		{
			// AOSMONITORLOG_LINE(rdata);
			//mTotal = mCrtIdx;
		}
 	}

	u64 time2 = OmnGetTimestamp();
	
	contents = "<Contents total=\"";
	contents << mTotal << "\" start_idx=\""
		<< preIdx << "\" crt_idx=\""
		<< preIdx << "\" keywords=\""
		<< rdata->getArg1("keywords") << "\" queryid=\""
		<< mQueryId << "\" num=\""
		<< mNumDocs << "\" time=\""
		<< (time2 - mQueryStartTime)/1000.0 << "ms\"";
	contents << ">" << records << "</Contents>";

	//OmnScreen << "Query: " << sgNumQueries++ << ":" 
	//	<< time2 - mQueryStartTime<< endl;
	rdata->setOk();
	return true;
}

bool	
AosQueryReq::runQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd,
		OmnString &contents, 
		const AosQueryProcCallbackPtr &callback)
{
	return runQuery(rdata, cmd, contents);
}


bool	
AosQueryReq::runQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd,
		OmnString &contents)
{
	// This function runs the query 'stmt'. 'query' is in the form:
	// 	select <attrnames> from <container> where <cond> AND <cond> ...
	if (mIsNew)
	{
		aos_assert_r(parseSqlPublic(rdata, cmd), false);
		mLock->lock();
		mIsNew = false;
	}
	else
	{
		mOrTerm->setPagesize(mPsize);
		mOrTerm->setStartIdx(mStartIdx);
		mLock->lock();
	}

	if (!mIsGood)
	{
		mLock->unlock();
		OmnString errmsg = "Query is incorrect: ";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return false;
	}

	// AOSMONITORLOG_LINE(rdata);
	mBatchData.clear();
	if (procSQL(rdata, contents))
	{
		rdata->setOk();
	}
	mLock->unlock();
	// AOSMONITORLOG_FINISH(rdata);
	return true;
}


bool	
AosQueryReq::parseSqlPublic(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd)
{
	// 'cmd' should be in the fomr:
	// 	<cmd startidx="xxx"
	// 		 psize="xxx"
	// 		 order="fieldname:order"
	// 		 queryid="xxx"
	// 		 query="xxx"
	// 		 sitereverse="true"	// means it is reversed based on siteid tag
	// 		 siteorder="true"	// means it is reversed based on siteid tag
	// 		 >
	// 		<conds>
	// 			<cond type="AND">
	// 				<term type="xxx" order="xxx"/>
	// 				<term type="xxx" order="xxx"/>
	//				...
	//			</cond>
	//		</conds>
	//		<fnames>
	//			<fname oname="xxx" cname="xxx" type="1|2|3|x"/>
	//			<fname oname="xxx" cname="xxx" type="1|2|3|x"/>
	//			...
	//		</fnames>
	//		<tables>
	//			<table name="xxx" />
	//			<table name="xxx" />
	//			...
	//		</tables>
	//	</cmd>
	//
	// There are four field types:
	// 	'1':	The field is an attribute and it is treated as an attribute
	// 			in the response.
	//  '2':	The field is an attribute but it is converted as a subtag
	//          in the response. The subtag name is 'cname'.
	//  '3':	The field is the object itself. It is added as the CDATA
	//          of a subtag. The subtag name is 'cname'.
	//  'x':	The field is the object itself and it is appended as a
	//          subtag. The subtag name is 'cname'.
	//
	// It is in the form:
	// 	select <attrnames> from containers where <cond> AND <cond> limit start, psize
	//
	// Query conditions can be specified in one of the two forms. One is 
	// through the 'query' attribute (for backward compatibility), and the
	// other is through the '<conds>' subtag (recommended).
	//
	// AOSMONITORLOG_ENTER(rdata);
	mLock->lock();
	mIsGood = false;
	OmnString errmsg = "Syntax error!";
	AosXmlTagPtr condsRoot;

	// Chen Ding, 2013/01/29
	if (cmd->getAttrInt64(AOSTAG_QUERYVERSION, 0) >= 2)
	{
		return parseSqlPublicVer2(rdata, cmd);
	}
	
	// Ketty 2014/02/13
	bool rslt = parseQueryTables(rdata, cmd);
	if (!rslt)
	{
		// must before parse conds. need check mIsStatQuery conf.
		errmsg = "Failed parsing the tables!";
		mLock->unlock();
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return false;
	}
	
	rslt = parseGroupByFields(rdata, cmd);
	if (!rslt)
	{
		// must before parse conds. need check mIsStatQuery conf.
		errmsg = "Failed parsing the groupby fields!";
		mLock->unlock();
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return false;
	}

	AosXmlTagPtr conds = cmd->getFirstChild("conds");
	// AOSMONITORLOG_LINE(rdata);

	rslt = parseConds(rdata, conds);
	if (!rslt)
	{
		// Chen Ding, 09/20/2011
		// Backward compatibility
		if (!mOrTerm)
		{
			OmnString tname = cmd->getAttrStr("tname");
			if (tname != "")
			{
				if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
				AosQueryTermObjPtr cond = OmnNew AosTermAnd();
				mOrTerm->addTerm(cond, rdata);

				AosQueryTermObjPtr term = OmnNew AosTermArith(
						"", "", false, false, eAosOpr_an, tname, rdata);
				cond->addTerm(term, rdata);
			}
		}

		if (!mOrTerm)
		{
			OmnAlarm << "Failed creating query!" << enderr;
			mLock->unlock();
			return false;
		}
	}

	AosXmlTagPtr update = cmd->getFirstChild("update");
	if (update)
	{
		// AOSMONITORLOG_LINE(rdata);
		try
		{
			mUpdate = OmnNew AosUpdate(update);
		}
		catch(AosUpdate::UpdateException &e)
		{
			mUpdate = NULL;
		}
	}

	AosXmlTagPtr fnames = cmd->getFirstChild("fnames");
	rslt = false;

	if (fnames) 
	{
		// AOSMONITORLOG_LINE(rdata);
		// Chen Ding, 2013/07/27
		// rslt = AosQrUtil::parseFnames(rdata, fnames, mFieldDef, errcode, errmsg);
		rslt = AosQrUtil::parseFnames(rdata, fnames, mFieldDef);
	}
	else 
	{
		// if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		// {
		// 	mLock->unlock();
		// 	AosSetError(rdata, AosErrmsgId::eMissingFieldname);
		// 	OmnAlarm << rdata->getErrmsg() << cmd->toString() << enderr;
		// 	return false;
		// }

		// Backward compatibility
		OmnString ff = cmd->getAttrStr("fnames");
		if (ff != "")
		{
			// AOSMONITORLOG_LINE(rdata);
			rslt = AosQrUtil::parseFnames(rdata, ff, mFieldDef, "|$$|", "|$|");
		}
		else
		{
			rslt = true;
		}
	}

	if (mFieldDef.empty())
	{
		rslt = false;
	}

	if (!rslt)
	{
		errmsg = "Failed parsing the fields!";
		mLock->unlock();
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	
	rslt = true;
	AosXmlTagPtr joinxml = cmd->getFirstChild("joins");
	if (joinxml)
	{
		rslt = setJoins(joinxml, errmsg);
	}

	if (!rslt)
	{
		errmsg = "Failed parsing the Joins!";
		mLock->unlock();
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return false;
	}
	
	if (!mOrTerm)
	{
		errmsg = "Missing conditions!";
		mLock->unlock();
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return false;
	}

	mIsGood = true;
	mIsNew = false;
	rdata->setOk();
	mLock->unlock();
	return true;
}


bool
AosQueryReq::parseQueryTables(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd)
{
	aos_assert_r(cmd, false);
	AosXmlTagPtr tables_conf = cmd->getFirstChild("tables");	
	if(!tables_conf)	return true;
	
	AosXmlTagPtr table_def = tables_conf->getFirstChild();
	OmnString table_name;
	while(table_def)
	{
		table_name = table_def->getAttrStr("name", "");	
		aos_assert_r(table_name != "", false);
		mTables.push_back(table_name);
		
		table_def = tables_conf->getNextChild();
	}
	
	// temp
	if(mTables.size() != 0)		checkIsStatQuery(rdata);
	return true;
}
	

bool
AosQueryReq::parseGroupByFields(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd)
{
	// Ketty 2014/04/10
	aos_assert_r(cmd, false);
	
	AosXmlTagPtr grpby_conf = cmd->getFirstChild("groupby_fields");
	if(!grpby_conf)	return true;
	
	AosXmlTagPtr grpby_def = grpby_conf->getFirstChild();
	OmnString field_name;
	while(grpby_def)
	{
		field_name = grpby_def->getAttrStr("name", "");	
		aos_assert_r(field_name != "", false);
		mGroupByFields.push_back(field_name);
		
		grpby_def = grpby_conf->getNextChild();
	}
	
	return true;
}


bool	
AosQueryReq::parseSqlPublicVer2(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd)
{
	// 'cmd' should be in the fomr:
	// 	<cmd opr="retlist" page_idx="xxx" logic_id="xxx" zky_queryver="2">
	// 		<query
	// 		 	psize="xxx"
	// 		 	queryid="xxx"
	// 			<conds>
	// 				<cond type="AND">
	// 					<term type="xxx" order="xxx"/>
	// 					<term type="xxx" order="xxx"/>
	//					...
	//				</cond>
	// 				<cond type="AND">
	// 					<term type="xxx" order="xxx"/>
	// 					<term type="xxx" order="xxx"/>
	//					...
	//				</cond>
	//			</conds>
	//			<update>
	//				...
	//			</update>
	//			<fnames>
	//				<fname oname="xxx" cname="xxx" type="1|2|3|x"/>
	//				<fname oname="xxx" cname="xxx" type="1|2|3|x"/>
	//				...
	//			</fnames>
	//			<joins>
	//				...
	//			</joins>
	//		</query>
	//	</cmd>
	//
	// There are four field types:
	// 	'1':	The field is an attribute and it is treated as an attribute
	// 			in the response.
	//  '2':	The field is an attribute but it is converted as a subtag
	//          in the response. The subtag name is 'cname'.
	//  '3':	The field is the object itself. It is added as the CDATA
	//          of a subtag. The subtag name is 'cname'.
	//  'x':	The field is the object itself and it is appended as a
	//          subtag. The subtag name is 'cname'.
	//
	mLock->lock();
	mIsGood = false;
	AosXmlTagPtr condsRoot;

	AosXmlTagPtr query = cmd->getFirstChild("query");
	if (!query)
	{
		mLock->unlock();
		AosSetErrorU(rdata, "miss_query") << ": " << cmd->toString() << enderr;
		return false;
	}

	// Parse conditions
	AosXmlTagPtr conds = query->getFirstChild("conds");
	if (!conds)
	{
		mLock->unlock();
		AosSetErrorU(rdata, "miss_conds") << ": " << cmd->toString() << enderr;
		return false;
	}
	if (!parseConds(rdata, conds)) 
	{
		mLock->unlock();
		return false;
	}

	// Parse updates
	AosXmlTagPtr update = query->getFirstChild("update");
	if (update)
	{
		try
		{
			mUpdate = OmnNew AosUpdate(update);
		}

		catch(AosUpdate::UpdateException &e)
		{
			mUpdate = NULL;
		}
	}

	// Parse field names
	AosXmlTagPtr fnames = query->getFirstChild("fnames");
	if (!fnames) 
	{
		mLock->unlock();
		AosSetErrorU(rdata, "miss_fnames") << ": " << query->toString() << enderr;
		return false;
	}

	// Chen Ding, 2013/07/27
	// AosXmlRc errcode;
	// OmnString errmsg;
	// if (!AosQrUtil::parseFnames(rdata, fnames, mFieldDef, errcode, errmsg) || mFieldDef.empty())
	if (!AosQrUtil::parseFnames(rdata, fnames, mFieldDef) || mFieldDef.empty())
	{
		mLock->unlock();
		AosSetErrorU(rdata, "miss_fnames") << ": " << query->toString() << enderr;
		return false;
	}

	// Parse Joins
	AosXmlTagPtr joinxml = query->getFirstChild("joins");
	if (joinxml)
	{
		OmnString errmsg;
		if (!setJoins(joinxml, errmsg)) 
		{
			mLock->unlock();
			return false;
		}
	}

	if (!mOrTerm)
	{
		mLock->unlock();
		AosSetErrorU(rdata, "internal_error") << ": " << cmd->toString() << enderr;
		return false;
	}

	mIsGood = true;
	mIsNew = false;
	rdata->setOk();
	mLock->unlock();
	return true;
}


bool
AosQueryReq::setOrder(
		const OmnString &container,
		const OmnString &fname, 
		const OmnString &reverse,
		const AosRundataPtr &rdata)
{
	// 'order_fname' identifies the name of the field that controls the order.
	// 'reverse' specifies whether the results are in the reverse order ("true")
	mLock->lock();
	mOrderContainer = container;
	mOrderFname = fname;
	mReverseOrder = (reverse == "true");
	mLock->unlock();
	return true;
}


bool
AosQueryReq::parseConds(
		const AosRundataPtr rdata,
		const AosXmlTagPtr &conds)
{
	// 'conds' should be in the form:
	// 	<conds>
	// 		<cond type="AND">
	// 			<term type="xxx" .../>
	// 			<term type="xxx" .../>
	// 			...
	// 			<term type="xxx" .../>
	// 		</cond>
	// 		<cond type="AND">
	// 			<term type="xxx" .../>
	// 			<term type="xxx" .../>
	// 			...
	// 			<term type="xxx" .../>
	// 		</cond>
	// 		<stat>
	// 		  <statterm type="counterQuery||counterMulti">
	// 			<zky_ctnr>xxxx</zky_ctnr>
	// 			<zky_ctype>xxxx</zky_ctype>
	// 			<zky_starttime>xxxx</zky_starttime>
	// 			<zky_endtime>xxxx</zky_endtime>
	// 			<zky_cname>
	// 				<termname zky_value_type="attr"  zky_xpath="city">
	// 					<zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 				</termname>
	// 				<termname zky_value_type="const">xxx</termname>
	// 				....
	// 			</zky_cname>
	// 		  </statterm>
	// 		</stat>
	// 	</conds>
	//
	// If 'mSiteid' is not empty, it is limited to the site. Need
	// to add the condition:
	// 	<term type="tag">mSiteid</term>

	// Containers are converted into
	// searching for tags. Each container is treated as a tag.
	// If a query contains a container, it means that the searched
	// should be in that container. If a doc is in a container, 
	// its docid should be added to the IIL:
	// Containers are formatted as:
	// 		[true:]<container>, [true:]<container>
	// where '[true:]' is optional. If not present, it is the normal
	// order. Otherwise, it is the reversed order.
	
	AosQueryTermObjPtr cond;

	// 1. Handle all containers. Containers are handled by 'AosTermTag'
	// with the IIL name: ??? + ":" + container name
	OmnString word;

	if (!conds) return false;

	mIsNeedSmartQuery  = conds->getAttrBool("isNeedSmartQuery", false);
	// All other conditions are defined by 'conds'. 
	AosXmlTagPtr termtmp;
	i64 guard = 20;
	AosXmlTagPtr andterm = conds->getFirstChild();
	while (guard-- && andterm)
	{
		OmnString type = andterm->getAttrStr("type");
		if (andterm->getTagname() == AOSTAG_QUERY_STAT)
        {
            // This is a statistics tag
			if (!cond) cond = OmnNew AosTermAnd();
            processStatistics(cond, andterm, rdata);
            andterm = conds->getNextChild();
            continue;
        }

		/*
		if (type == "qry_usercustom")
		{
			if (!cond) cond = OmnNew AosTermAnd();
			AosXmlTagPtr term = andterm->getFirstChild();
			cond->addTerm(term, rdata);
			andterm = conds->getNextChild();
			continue;
		}
		*/

		if (type != "AND")
		{
			rdata->setError() << "Expecting the AND term but failed!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (!cond) cond = OmnNew AosTermAnd();
		AosXmlTagPtr term = andterm->getFirstChild();
		termtmp = term;

		while (term)
		{
			if (rdata->getCid()!= "")
			{
				AosXmlTagPtr objcond = term;
				bool exist;
				OmnString objidlhs = objcond->xpathQuery("lhs/_$text", exist, "");
				if (objidlhs == AOSTAG_OBJID)
				{
					OmnString objidrhs = objcond->xpathQuery("rhs/_$text", exist, "");
					OmnString modobjid  = objidrhs;
					OmnString errmsg;
					bool rslt = AosObjid::procObjid(modobjid, rdata->getCid(), errmsg);
					if (!rslt)
					{
						AosSetError(rdata, errmsg);
					}
					else if (modobjid != objidrhs)
					{
						objcond->setNodeText("rhs", modobjid, true);
					}
				}
			}
			cond->addTerm(term, rdata);
			term = andterm->getNextChild();
			
		}
		andterm = conds->getNextChild();
	}

	// Check containers. It is possible to 
	if (cond)
	{
		if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
		mOrTerm->addTerm(cond, rdata);
	}

	if (mOrderFname != "" && mOrTerm) 
	{
		mOrTerm->setOrder(mOrderContainer, mOrderFname, mReverseOrder, rdata);
	}

	// Backward Compatibility, Chen Ding, 09/20/2011
	aos_assert_r(mOrTerm, false);
	mOrTerm->setPagesize(mPsize);
	mOrTerm->setStartIdx(mStartIdx);

	// Ketty 2014/02/21
	if (!mIsStatQuery && !AosQueryConfig::usingSmartQuery())
	{
		mOrTerm->loadData(rdata);
		mTotal = mOrTerm->getTotal(rdata);
		mOrTerm->reset(rdata);
	}

	OmnString str;
	if (mOrTerm) mOrTerm->toString(str);
	mIsGood = true;
	rdata->setOk();
	return true;
}


bool
AosQueryReq::setJoins(
		const AosXmlTagPtr &joins,
		OmnString &errmsg)
{
	//Joins are specified as:
	//<joins>
	//	<join join_attridx="1|2|3" join_type="docid|objid|cid" join_attr="xxx"/>
	//	......
	//</joins>
	mJoinDefs.clear();
	if (!joins)
	{
		return true;
	}

	AosXmlTagPtr join = joins->getFirstChild();
	if (!join)
	{
		return true;
	}
	i64 joins_num = joins->getNumSubtags();
	i64 guard = eMaxJoins;
	i64 i = 0;
	while(join && guard-- && joins_num)
	{
		AosQrUtil::JoinDef joindef;
		joindef.index = join->getAttrInt64("join_attridx", 0);
		if (joindef.index < 0 || joindef.index >i)
		{
			errmsg = "Joins are not correct!";
			return false;
		}
		// If there is another ':', it will be used to determine
		// the join attribute type: 
		// 0:   docid
		// 1:   objid 
		// 2:   cid
		OmnString jointype = join->getAttrStr("join_type", "docid");
		const char *data = jointype.data();

		AosQrUtil::JoinType jtype = AosQrUtil::eInvalidJoin;
		switch (data[0])
		{
		case 'c': 
			 if (strcmp(data, "cloudid") == 0) jtype = AosQrUtil::eCloudidJoin;
			 break;

		case 'd': 
			 if (strcmp(data, "docid") == 0) jtype = AosQrUtil::eDocidJoin; 
			 break;

		case 'o': 
			 if (strcmp(data, "objid") == 0) jtype = AosQrUtil::eObjidJoin;
			 break;

		default:
			 break;
		}

		if (!AosQrUtil::isValidJoin(jtype))
		{
			errmsg = "Invalid field type: ";
			errmsg << jointype;
			OmnAlarm << errmsg << enderr;
			return false;
		}

		OmnString attr = join->getAttrStr("join_attr");
		if (attr == "")
		{
			joindef.index = 0;
			joindef.type = AosQrUtil::eDocidJoin;
			joindef.name = "";
		}
		else
		{
			joindef.name = attr;
			joindef.type = jtype;
		}

		mJoinDefs.push_back(joindef);
		join = joins->getNextChild();
		i++;
	}
	return true;
}


/*
bool
AosQueryReq::setJoins(const OmnString &joins, OmnString &errmsg)
{
	// Joins are specified as:
	// 		joined_aname[:idx]:type,joined_aname[:idx]:type,...
	// If 'idx' is specified, the joined xml should be retrieved from 
	// 'idx-th' XML. If not specified, idx = 0. This allows chained joins.
	// jointype : 0 is docid, 1 is objid, 2 is cloudid. 
	AosStrSplit split;
	bool finished;
	if (joins == "")
	{
		mNumJoins = 0;
		return true;
	}

	mNumJoins = split.splitStr(joins.data(), ",", mJoins, eMaxJoins, finished);
	for (i64 i=0; i<mNumJoins; i++)
	{
		i64 pos = mJoins[i].indexOf(0, ':');
		if (pos > 0)
		{
			mJoinIdx[i] = atoi(&mJoins[i].data()[pos+1]);
			// Ice Yu, 12/22/2010, Zky1992
			//if (mJoinIdx[i] < 0 || mJoinIdx[i] >= i)
			if (mJoinIdx[i] < 0 || mJoinIdx[i] > i)
			{
				errmsg = "Joins are not correct!";
				return false;
			}

			// Ice Yu, 12/22/2010, Zky1992
			// If there is another ':', it will be used to determine
			// the join attribute type: 
			// 0:	docid
			// 1:	objid
			// 2:	cid
			i64 pos2 = mJoins[i].indexOf(pos+1, ':');
			mJoinType[i] = atoi(&mJoins[i].data()[pos2+1]);
			if (mJoinType[i] < 0 || mJoinType[i] >= 3)
			{
				errmsg = "Joins type not correct";
				return false;
			}
			mJoins[i].setLength(pos);
		}
		else
		{
			mJoinIdx[i] = 0;
			mJoinType[i] = 0;
		}
	}
	return true;
}


bool
AosQueryReq::appendJoins(const OmnString &joins, OmnString &errmsg)
{
	// Joins are specified as:
	// 		joined_aname[:idx],joined_aname[:idx],...
	// If 'idx' is specified, the joined xml should be retrieved from 
	// 'idx-th' XML. If not specified, idx = 0. This allows chained joins.
	// jointype : 0 is docid, 1 is objid, 2 is cloudid. 
	if (joins == "") return true;

	OmnStrParser1 split(joins, ", ");
	OmnString word;
	while ((word = split.nextWord()) != "" && mNumJoins < eMaxJoins)
	{
		i64 pos = word.indexOf(0, ':');
		if (pos > 0)
		{
			i64 join_idx = atoi(&word.data()[pos+1]);
			// Ice Yu, 12/22/2010, Zky1992
			// if (join_idx < 0 || join_idx >= mNumJoins)
			if (join_idx < 0 || join_idx > mNumJoins)
			{
				errmsg = "Joins are not correct!";
				return false;
			}
			mJoinIdx[mNumJoins] = join_idx;
			mJoins[mNumJoins] = word;

			// Ice Yu, 12/22/2010, Zky1992
			i64 pos2 = mJoins[mNumJoins].indexOf(pos+1, ':');
			mJoinType[mNumJoins] = atoi(&mJoins[mNumJoins].data()[pos2+1]);
			if (mJoinType[mNumJoins] < 0 || mJoinType[mNumJoins] >= 3)
			{
				errmsg = "Joins type not correct";
				return false;
			}
			mJoins[mNumJoins].setLength(pos);
		}
		else
		{
			mJoinIdx[mNumJoins] = 0;
			mJoins[mNumJoins] = word;
			mJoinType[mNumJoins] = 0;	// Ice Yu, 12/22/2010, Zky1292
		}

		mNumJoins++;
	}
	return true;
}
*/


bool
AosQueryReq::getJoins(const AosXmlTagPtr &xml, const AosRundataPtr &rdata)
{
	// A doc has been retrieved by a query. This function retrieves all 
	// the joined docs based on the join definitions:
	// 1. Initially, mXmls[0] = xml;
	// 2. It loops over all the joins. Each join is defined as:
	// 		name
	// 		type
	// 		index
	//    It uses 'name' is the name of the attribute through which 
	//    a docid identifier (docid, objid, or cloudid, depending on 'type')
	//    can be retrieved. It uses that id to retrieve a doc. 
	//    If the doc is retrieved, it appends the doc into mXmls.
	//    This means that each join may create a doc. All the docs retrieved
	//    by joins are stored in mXmls. This is to be used to create the query results.
	// 	  
	aos_assert_r(xml, false);
	mXmls.clear(); 
	mXmls.push_back(xml);
	AosXmlTagPtr doc;
	i64 numJoins = mJoinDefs.size();
	i64 numXmls = mXmls.size();
	for (i64 i=0; i<numJoins; i++)
	{
		doc = 0;
		i64 xml_idx = mJoinDefs[i].index;
		if (xml_idx >= 0 && xml_idx < numXmls && mXmls[xml_idx])
		{
			// Ice Yu, 12/22/2010, Zky1992
			u64 docid;
			OmnString objid, cloudid;
			switch(mJoinDefs[i].type)
			{
			case AosQrUtil::eDocidJoin:
				 // Use 'mJoins[i].name' as the name of the attribute to retrieve docid.
				 docid = mXmls[xml_idx]->getAttrU64(mJoinDefs[i].name, AOS_INVDID);
				 if (docid != AOS_INVDID)
				 {
					doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
				 }
				 break;

			case AosQrUtil::eObjidJoin: 
				 // Use 'mJoinDefs[i].name' as the name of the attribute to retrieve objid
				 objid = mXmls[xml_idx]->getAttrStr(mJoinDefs[i].name, "");
				 if (objid != "")
				 {
					doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
				 }
				 break;

			case AosQrUtil::eCloudidJoin:
				 // Use 'mJoinDefs[i].name' as the name of the attribute to retrieve cloudid
				 cloudid = mXmls[xml_idx]->getAttrStr(mJoinDefs[i].name, "");
				 if (cloudid != "")
				 {
					doc = AosDocClientObj::getDocClient()->getDocByCloudid(cloudid, rdata); 
				 }
				 break;
				 
			default:
				 OmnAlarm << "Unrecognized join type: " << mJoinDefs[i].type << enderr;
				 break;
			}
			if (doc)
			{
				doc = doc->clone(AosMemoryCheckerArgsBegin);
				if (mRemoveMeta)
				{
					doc->removeMetadata();	
				}
				mXmls.push_back(doc);
			}
		}
	}
	return true;
}


bool
AosQueryReq::processStatistics(
		const AosQueryTermObjPtr &cond,
		const AosXmlTagPtr &stat_tag, 
		const AosRundataPtr &rdata)
{
	// This function processes the <stat> tag:
	// 	<conds>
	// 		<cond ...>
	// 			<term .../>
	// 			<term .../>
	//			...
	// 			<term .../>
	// 		</cond>
	// 		<cond .../>
	// 		<stat> 
	// 		 <statterm type="couterSingle||counterMulti||counterSub">
	// 			<zky_ctnr>xxxx</zky_ctnr>
	// 	        <zky_ctype>xxxx</zky_ctype>
	// 	        <zky_starttime>xxxx</zky_starttime>
	// 	        <zky_endtime>xxxx</zky_endtime>
	// 	        <zky_cname>
	// 	           <termname zky_value_type="attr"  zky_xpath="city">
	// 	                <zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 	           </termname>
	// 	  	       <termname zky_value_type="const">xxx</termname>
	// 	             ....
	// 	        </zky_cname>
	// 	      </statterm>
	// 		</stat>
	// 	</conds>
	
	aos_assert_rr(stat_tag, rdata, false);
	AosXmlTagPtr statTerm = stat_tag->getFirstChild();
	if(!statTerm)return true;

	AosQueryType::E type = AosQueryType::toEnum(statTerm->getAttrStr("type", ""));
	if (!AosQueryType::isValidCounterType(type))
	{
		rdata->setError() << "Unrecognized statistics type: " << statTerm->getAttrStr("type", "");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (type == AosQueryType::eCounterMulti)
	{
		if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
		bool rslt = mOrTerm->addStatTerm(statTerm, rdata);
		aos_assert_rr(rslt, rdata, false);
		rdata->setOk();
		return true;
	}

	AosQueryTermObjPtr term = cond->addTerm(statTerm, rdata);
	aos_assert_r(term, false);

	if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
	mOrTerm->setDocRetriever(term);
	cond->setNeedOrder(false);
	return true;
}


i64 
AosQueryReq::getNewQueryID()
{
	smIDLock->lock();
	i64 new_id = ++smIDCur;
	smIDLock->unlock();
	return new_id;
}


bool
AosQueryReq::willRunSmartQuery(const AosRundataPtr &rdata)
{
	aos_assert_r(mOrTerm, false);
	if (!AosQueryConfig::usingSmartQuery())
	{
		return false;
	}
	return mOrTerm->isCompoundQuery();
}


bool
AosQueryReq::analyzeQuery(const AosRundataPtr &rdata)
{
	aos_assert_r(mOrTerm, false);
	mOrTerm->setPagesize(mPsize);
	
	if(mIsStatQuery)
	{
		return runStatQuery(rdata); 
	}

	if (!AosQueryConfig::usingSmartQuery() || !mIsNeedSmartQuery)
	{
		return normalQuery(rdata);
	}

	// 1. If mOrTerm has more than one term, need to consider using
	//    tbe bitmap query.
	i64 num = 0;
	AosQueryTermObjPtr *terms = mOrTerm->getTerms(num);
	aos_assert_r(num >= 1, false);
	aos_assert_r(terms, false);

// CHENDING TEMP
//	if (num > 1)
//	{
		return smartQuery(rdata);
//	}

	AosQueryTermObjPtr andTerm = terms[0];
	aos_assert_r(andTerm, false);

	AosQueryTermObjPtr *tt = andTerm->getTerms(num);
	aos_assert_r(num >= 1, false);
	aos_assert_r(tt, false);

	if (num == 1) 
	{
		return normalQuery(rdata);
	}

	for (i64 i=0; i<num; i++)
	{
		switch (tt[i]->getType())
		{
		case AosQueryType::eByDocid:
			 // In the current implementations, this term cannot be
			 // used with any other condition. 
			 OmnAlarm << "Please do not use this" << enderr;
			 return normalQuery(rdata);

		default:
			 break;
		}

		if (!tt[i]->canUseBitmapQuery())
		{
			// It can't use bitmap query. 
			return normalQuery(rdata);
		}
	}

	return smartQuery(rdata);
}

bool
AosQueryReq::checkIsStatQuery(const AosRundataPtr &rdata)
{
	// Ketty temp.
	//mIsStatQuery = true;
	//return true;
	
	for(u32 i=0; i<mTables.size(); i++)
	{
		AosXmlTagPtr doc = AosGetDocByObjid(mTables[i], rdata);
		aos_assert_r(doc, false);
		AosXmlTagPtr tag = doc->getFirstChild("statistics");
		if (tag) {
			AosXmlTagPtr stat= tag->getFirstChild("statistic");
			if (stat){
				mIsStatQuery = true;
				return stat->getAttrStr("stat_objid", "")!="";
			}
		}
	}
	mIsStatQuery = false;
	return false;
}

bool
AosQueryReq::normalQuery(const AosRundataPtr &rdata) 
{
	bool rslt = mOrTerm->runQuery(rdata);
	return rslt;
}


bool
AosQueryReq::smartQuery(const AosRundataPtr &rdata)
{
	bool rslt = false;
	mLock1->lock();
	mIsSmartQuery = true;
	AosQueryReqObjPtr thisptr(this, false);
	AosQueryProcCallbackPtr thisptr2(this, false);
	rslt = AosQueryEngineObj::getQueryEngine()->runQueryStatic(rdata, thisptr, thisptr2);
	aos_assert_rl(rslt,mLock,false);
	mCondVar->wait(mLock1);
	mLock1->unlock();
	return rslt;
}

bool
AosQueryReq::smartQueryPostProcess(OmnString &contents,const AosRundataPtr &rdata) 
{
	bool rslt = false;
	OmnString records;
	bool finished = false;
	rslt = generateRsltSmart(records, finished, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "invalid_query");
		return false;
	}

	return finishUpQuerySmart(contents, records, finished, rdata);
	
}


bool
AosQueryReq::generateRsltSmart(
		OmnString &records, 
		bool &finished,
		const AosRundataPtr &rdata) 
{
	u64 docid; 
	OmnString value;
	OmnString errmsg;
	AosXmlTagPtr xml;
	i64 num_docs = 0;
	bool rslt = true;
	if(!mOrigQueryRslt)
	{
		rslt = false;
	}
	else
	{
		mOrigQueryRslt->reset();
	}
	while (rslt)
	{
		rslt = mOrigQueryRslt->nextDocid(docid, finished, rdata);
		if(!rslt || (docid == AOS_INVDID && finished))
		{
			finished = true;
			break;
		}
		
		xml = AosQueryTerm::getDocStatic(docid, rdata);
		
		if (!xml)
		{
			OmnAlarm << "Failed to retrieve the doc: " << docid << enderr;
			mTotal --;
			continue;
		}

		xml = xml->clone(AosMemoryCheckerArgsBegin);
		if (mRemoveMeta)
		{
			xml->removeMetadata();
		}

		if (mUpdate)
		{
			mUpdate->update(xml);
		}

		getJoins(xml, rdata);

		if (AosQrUtil::createRecord(rdata, records, mXmls, mFieldDef))
		{
			mNumDocs++;
		}
		num_docs++;
		if (num_docs >= mPsize) break;
	}

	return true;
}


bool
AosQueryReq::runStatQuery(const AosRundataPtr &rdata)
{
	return false;
	//AosQueryReqObjPtr thisptr(this, false);
	//return AosStatQueryAnalyzerObj::runQueryStatic(rdata, thisptr);
}

bool
AosQueryReq::finishUpQuerySmart(
		OmnString &contents,
		const OmnString &records, 
		bool &finished,
		const AosRundataPtr &rdata)
{
	if(mCrtIdx < 0 && mNumDocs > 0)mCrtIdx = 0;

	i64 preIdx = mCrtIdx;
	mCrtIdx += mNumDocs;

	if (finished)
	{
	 	// The query is finished. Need to inform the query manager
	 	// to remove this query.
		// AOSMONITORLOG_LINE(rdata);
	 	AosQueryReqObjPtr thisptr(this, false);
		AosQueryClient::getSelf()->queryFinished(thisptr, rdata);
 		if (mTotal > mCrtIdx)
		{
			// AOSMONITORLOG_LINE(rdata);
			//mTotal = mCrtIdx;
		}
 	}

	u64 time2 = OmnGetTimestamp();
	
	contents = "<Contents total=\"";
	contents << mTotal << "\" start_idx=\""
		<< preIdx << "\" crt_idx=\""
		<< preIdx << "\" keywords=\""
		<< rdata->getArg1("keywords") << "\" queryid=\""
		<< mQueryId << "\" num=\""
		<< mNumDocs << "\" time=\""
		<< (time2 - mQueryStartTime)/1000.0 << "ms\"";
	contents << ">" << records << "</Contents>";

	//OmnScreen << "Query: " << sgNumQueries++ << ":" 
	//	<< time2 - mQueryStartTime<< endl;
	rdata->setOk();
	return true;
}



bool
AosQueryReq::generateRslt(
		OmnString &records, 
		bool &finished,
		const AosRundataPtr &rdata) 
{
	// AOSMONITORLOG_ENTER(rdata);
	if (mBatchQueryFlag)
	{
		return procQueryRawRslts(rdata);
	}

	if (mStartIdx != mCrtIdx) 
	{
	 	if (mStartIdx < 0)
		{
			mStartIdx = (mOrderFname != "" && mReverseOrder) ? mTotal - 1 : 0;
		}
		if (mStartIdx < 0)
		{
			mStartIdx = 0;
		}

	 	mCrtIdx = mStartIdx;
	}
	else
	{
		if (mStartIdx < 0)
		{
			mStartIdx = 0;
			mCrtIdx = 0;
		}
	}

	if (mStartIdx < 0) mStartIdx = 0;

	// AOSMONITORLOG_LINE(rdata);

	mOrTerm->moveTo(mStartIdx, rdata);
	mTotal = mOrTerm->getTotal(rdata);
	u64 docid; 
	OmnString value;
	OmnString errmsg;
	AosXmlTagPtr xml;
	i64 num_docs = 0;
	bool rslt = true;
	while (rslt)
	{
		if(mOrTerm->withValue())
		{
			rslt = mOrTerm->nextDocid(0, docid, value, finished, rdata);
		}
		else
		{
			rslt = mOrTerm->nextDocid(0, docid, finished, rdata);
		}
		if(!rslt) break;
		if(docid == AOS_INVDID && finished) break;

		if(mOrTerm->withValue())
		{
			xml = mOrTerm->getDoc(docid, value, rdata);
		}
		else
		{
			xml = mOrTerm->getDoc(docid, rdata);
		}
		
		if (!xml)
		{
			OmnAlarm << "Failed to retrieve the doc: " << docid << enderr;
			mTotal --;
			continue;
		}

		xml = xml->clone(AosMemoryCheckerArgsBegin);
		if (mRemoveMeta)
		{
			xml->removeMetadata();
		}

		if (mUpdate)
		{
			mUpdate->update(xml);
		}

		getJoins(xml, rdata);

		if (AosQrUtil::createRecord(rdata, records, mXmls, mFieldDef))
		{
			mNumDocs++;
		}
		num_docs++;
		if (num_docs >= mPsize) break;
	}

	return true;
}


// Chen Ding, 2013/01/27
AosXmlLogPtr
AosQueryReq::getLog()
{
	if (!mLog) mLog = OmnNew AosXmlLog();
	return mLog;
}


void
AosQueryReq::appendLog(const OmnString &str)
{
	if (!mLog) mLog = OmnNew AosXmlLog();
	(*mLog) << str << endl;
}

bool	
AosQueryReq::finished()const
{
	if(!mOrTerm) return true;
	return mOrTerm->queryFinished();
}


OmnString 
AosQueryReq::toString() const
{
	return "QueryReq: not implemented yet!";
}


AosXmlTagPtr
AosQueryReq::getOrigReq() const
{
	return mOrigReq;
}


bool
AosQueryReq::generateResults(
		const AosRundataPtr &rdata, 
		const AosQueryRsltObjPtr &query_rslt,
		OmnString &records)
{
	// This function converts the results in 'query_rslt' into 
	// a string. The function is called when a query proc finishes
	// the current page. 
	records = "";
	if (!query_rslt || query_rslt->isEmpty())
	{
		return true;
	}

	query_rslt->reset();
	u64 docid;
	OmnString value;
	bool finished;
	OmnString errmsg;
	while (query_rslt->nextDocidValue(docid, value, finished, rdata))
	{
		if (finished) break;

		aos_assert_rr(docid != AOS_INVDID, rdata, false);
			
		if (records == "") records = "<Contents>";
		records << "<entry key=\"" << value << "\" value=\"" << docid << "\"/>";
		/* Chen Ding, 2013/08/18
		AosXmlTagPtr xml = mOrTerm->getDoc(docid, rdata);
		if (!xml)
		{
			OmnAlarm << "Failed to retrieve the doc: " << docid << enderr;
			continue;
		}

		if (mUpdate)
		{
			mUpdate->update(xml);
		}

		getJoins(xml, rdata);

		// Chen Ding, 2013/07/27
		AosQrUtil::createRecord(records, mXmls, errmsg, mFieldDef, rdata);
		*/
	}

	records << "</Contents>";
	
	return true;
}


bool 
AosQueryReq::setQueryProc(const AosQueryProcObjPtr &query_proc)
{
	mQueryProc = query_proc;
	return true;
}


AosQueryProcObjPtr 
AosQueryReq::getQueryProc() const
{
	return mQueryProc;
}


// Chen Ding, 2014/01/31
bool 
AosQueryReq::getTimeCond(
		const AosRundataPtr &rdata, 
		const OmnString &time_fieldname, 
		int &num_matched, 
		int *start_days, 
		int *end_days)
{
	aos_assert_rr(mOrTerm, rdata, false);
	return mOrTerm->getTimeCond(rdata, time_fieldname, num_matched, start_days, end_days);
}
	

// Ketty 2014/02/14
vector<AosQrUtil::FieldDef> &
AosQueryReq::getSelectedFields()
{
	return mFieldDef;
}


//void
//AosQueryReq::setStatQueryUnit(const AosStatQueryUnitPtr &qry_unit)
//{
//	mStatQueryUnit = qry_unit;
//}


//AosStatQueryUnitPtr 
//AosQueryReq::getStatQueryUnit()
//{
//	return mStatQueryUnit;
//}


bool 
AosQueryReq::queryFinished(
	const AosRundataPtr &rdata,
	const AosQueryRsltObjPtr &results,
	const AosBitmapObjPtr &bitmap,
	const AosQueryProcObjPtr &proc)
{
	mLock1->lock();
	mOrigQueryRslt = results;
	mCondVar->signal();
	mLock1->unlock();
	return true;
}

bool 
AosQueryReq::queryFailed(
					const AosRundataPtr &rdata,
					const OmnString &errmsg)
{
	mLock1->lock();
	mOrigQueryRslt = 0;
	mCondVar->signal();
	mLock1->unlock();
	return true;
}

