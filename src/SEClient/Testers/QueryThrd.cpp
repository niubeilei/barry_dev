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
// This class runs as a separate thread. It randomly sleeps for a 
// a period of time. When it wakes up, it creates a query and query
// the system.
//
// Modification History:
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEClient/Testers/QueryThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/DocServer.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEUtil/DocTags.h"
#include "SEClient/SEClient.h"
#include "SEClient/Testers/SearchEngTester.h"
#include "SEClient/Testers/TestXmlDoc.h"
#include "SEClient/Testers/CreateDocThrd.h"
#include "SEClient/Testers/DeleteDocThrd.h"
#include "SEClient/Testers/ModifyDocThrd.h"
#include "SEClient/Testers/QueryThrd.h"
#include "SEClient/Testers/DocThrd.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/Opr.h"
#include "Util/OmnNew.h"
#include "Util/ValList.h"
#include "Util/StrParser.h"
#include "Util/File.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "WordProc/WordFilter.h"
#include "WordProc/WordNormal.h"
#include "WordParser/WordParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosQueryThrd::AosQueryThrd(
		AosSearchEngTesterPtr &torturer,
		const int total,
		const int weight, 
		const OmnIpAddr &raddr, 
		const int rport, 
		const OmnString &attrtname, 
		const OmnString &docwdtname, 
		const OmnString &tagtname)
:
mTorturer(torturer),
mTotal(total),
mWeight(weight),
mNumQueriesCreated(0),
mAttrTname(attrtname),
mDocwdTname(docwdtname),
mTagTname(tagtname)
{
	int a = 0;
	a++;
	mAttrNames = new OmnString[eMaxAttrNum];
	mAttrVals = new OmnString[eMaxAttrNum];

	mForceDocid = -1;//43426;
	mForceWord = "";
	mForceType = eQueryAnyType;
	mForceName = "";
	mForceValue = "";
	mForceOpr = eAosOpr_Invalid;

	mWordParser = OmnNew AosWordParser();

mForceDocid = 81926;
mForceWord = "Comments";
mForceType = eQueryKeyword;
//mForceDocid = 22776;
//mForceWord = "images";
//mForceType = eQueryContainer;
//	mForceName = "zky_ctmepo";
//	mForceValue = "1281589643";
//	mForceOpr = eAosOpr_eq;

//	mForceDocid = 32886;
//	mForceType = eQueryAttribute;
//	mForceName = "zky_mtmepo";
//	mForceValue = "1276044378";


//	mForceWord = "_zt21_35108_zky_oidhm__43250";

//		mForceDocid = 9131;
//	//	mForceWord = "dayuan";
//	mForceDocid = 19107;
//	mForceWord = "images";
//	mForceType = eQueryContainer;


	mConn = OmnNew AosSEClient(raddr, rport);
	mSiteid = "100";

	if (mWeight <= 0) mWeight = eDftWeight;
}

bool
AosQueryThrd::start()
{

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "QueryThrd", 0, true, true, __FILE__, __LINE__);
	return mThread->start();
}


AosQueryThrd::~AosQueryThrd()
{
}


bool
AosQueryThrd::stop()
{
	if(mThread.isNull())
	{
		return true;
	}
	mThread->stop();
	return true;
}


bool	
AosQueryThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	static int counter = 1;
    while (state == OmnThrdStatus::eActive)
    {
//		queryDocs();

		basicQuery();
		OmnScreen << "query " << counter++ << "finished" << endl;
	}
	return true;
}


bool
AosQueryThrd::queryDocs()
{
	if (mNumQueriesCreated >= mTotal) return true;
	mNumQueriesCreated++;
	if (mNumQueriesCreated%100 == 0)
		OmnScreen << "Process query: " << mNumQueriesCreated << endl;	
	if (mNumQueriesCreated == 2192)
	{
		OmnMark;
	}

	// Construct the query:
	// 	<request>
	// 		<item name="operation">serverCmd</item>
	// 		<item name="siteid">xxx</item>
	// 		<item name="username">xxx</item>
	// 		<item name="appname">xxx</item>
	// 		<cmd opr="retlist"
	// 			 psize="xxx"
	// 			 startidx="xxx"
	// 			 ctnr="xxx"
	// 			 order="xxx"
	// 			 dir="xxx"
	// 			 <conds>
	// 			 	<term type="xxx" .../>
	// 			 	<term type="xxx" .../>
	// 			 	...
	// 			 </conds>
	// 			 <fnames xml="true">
	// 			 	<fname .../>
	// 			 	<fname .../>
	// 			 	...
	// 			 </fnames>
	// 		</cmd>
	// 	</request>
	OmnString conds;

	/*
	OmnString req;
	if (mCrtResp)
	{
		AosXmlTagPtr doc = mCrtResp->getFirstChild();
		aos_assert_r(doc, false);
		doc = doc->getFirstChild("Contents");
		aos_assert_r(doc, false);
		int total = doc->getAttrInt("total", -1);
		int start = doc->getAttrInt("start", -1);
		int num   = doc->getAttrInt("num", -1);
		aos_assert_r(total >= 0, false);
		aos_assert_r(start >= 0, false);
		aos_assert_r(num   >= 0, false);
		aos_assert_r(start + num <= total, false);

		if (start + num < total)
		{
			OmnString queryid = doc->getAttrStr("queryid");
			aos_assert_r(queryid != "", false);
			aos_assert_r(num == mPsize, false);
			req = "<request>";
			req << "<item name=\"operation\">serverCmd</item>"
				<< "<item name=\"" << AOSTAG_SITEID << "\">" << mSiteid << "</item>"
				<< "<item name=\"" << AOSTAG_USERNAME << "\">" << mUname << "</item>"
				<< "<item name=\"" << AOSTAG_APPNAME << "\">" << mAppname << "</item>"
				<< "<cmd opr=\"retlist\" "
				<< "qtype=\"sql\" queryid=\"";
			req << queryid << "\" "
				<< "psize=\"" << mPsize << "\" "
				<< "startidx=\"" << mStartIdx << "\"/></request>";
		}
	}

	if (req == "")
	{
		mSiteid = AosTestXmlDoc::getRandomSiteid();
		mUname = mTorturer->getUsername();
		mAppname = mTorturer->getAppname();
		mContainer = mTorturer->getCtnrname();
		mControllingTable = 0;
		aos_assert_r(createFnames(), false);
		aos_assert_r(createConds(), false);
		aos_assert_r(createOrder(), false);
		aos_assert_r(createLimit(), false);

		// OmnScreen << "Reset startidx" << endl;
		mStartIdx = 0;
		req = "<request>";
		req << "<item name=\"operation\">serverCmd</item>"
			<< "<item name=\"" << AOSTAG_SITEID << "\">" << mSiteid << "</item>"
			<< "<item name=\"" << AOSTAG_USERNAME << "\">" << mUname << "</item>"
			<< "<item name=\"" << AOSTAG_APPNAME << "\">" << mAppname << "</item>"
			<< "<cmd opr=\"retlist\" "
			<< "qtype=\"sql\" "
			<< "psize=\"" << mPsize << "\" "
			<< "startidx=\"" << mStartIdx << "\" ";
		if (mOrder != "") req << "order=\"" << mOrder << "\"";
		if (mDir != "") req << "dir=\"" << mDir << "\"";
		if (mCtnrs != "") req << "ctnrs=\"" << mCtnrs << "\"";
		req << ">" << mCondStr << mFnames << "</cmd></request>";
		mOrigReq = req;
	}

	int vv = rand() % 100;
	if (vv < 10)
	{
		// Need to verify queries. Before sending the query, 
		// need to inform the server not to update the server
		mTorturer->stopUpdating();
	}

	OmnString errmsg;
	OmnString resp;
	// OmnScreen << "To send req: " << req << endl;
	aos_assert_r(mConn->procRequest(mSiteid, mAppname, mUname, req, resp, errmsg), false);
	// OmnScreen << "Got response: " << resp << endl;

	// if (vv < 10) aos_assert_r(verifyResp(resp), false);
	if (!verifyResp(resp))
	{
		OmnAlarm << "Failed the query!" << enderr;
		OmnScreen << "Request: " << mOrigReq << endl;
		OmnScreen << "Response: " << resp << endl;
		OmnScreen << "Stmt: " << mStmt << endl;
		exit(0);
	}
	*/

	return true;
}


bool
AosQueryThrd::verifyResp(const OmnString &resp)
{
	// The response should be in the form:
	// 	<Contents total="xxx" start="xxx" num="xxx" time="xxx">
	// 		<record .../>
	// 		<record .../>
	// 		...
	// 	</Contents>
return true;
/*
	AosXmlParser parser;
	mCrtResp = parser.parse(resp, "");
	aos_assert_r(mCrtResp, false);
	AosXmlTagPtr doc = mCrtResp->getFirstChild();
	aos_assert_r(doc, false);
	doc = doc->getFirstChild("Contents");
	aos_assert_r(doc, false);
	AosXmlTagPtr retrieved_doc = doc->getFirstChild();
	// OmnScreen << "Verifying: " << mNumQueriesCreated << ":" << tt << endl;

	int total = doc->getAttrInt("total", -1);
	int start = doc->getAttrInt("start", -1);
	OmnString tt = doc->getAttrStr("time", "");
	aos_assert_r(tt != "", false);
	u32 len; 
	double value;
	aos_assert_r(tt.parseDouble(0, len, value), false);
	aos_assert_r(value >= 0.0 && value < 1000.0, false);
	int numRecords = doc->getAttrInt("num", -1);
	aos_assert_r(total >= 0, false);
	aos_assert_r(start >= 0, false);
	aos_assert_r(numRecords >= 0, false);
	aos_assert_r(start + numRecords <= total, false);

	OmnString queryid = doc->getAttrStr("queryid");
	aos_assert_r(queryid != "", false);

	OmnString docid;
	mNoMoreDocs = false;
	while (retrieved_doc && (docid = getNextDocid()) != "")
	{
		numRecords--;
		aos_assert_r(retrieved_doc, false);
		OmnString did = retrieved_doc->getAttrStr(AOSTAG_DOCID, "");

		if (docid != did)
		{
			OmnAlarm << "docid mismatch. Stored docid: " 
				<< docid << ". Retrieved from Search Engine: " << did << enderr;
			return false;
		}

		// Retrieve the stored doc
		AosXmlTagPtr origdoc = mTorturer->readDoc(mSiteid, docid);
		aos_assert_r(origdoc, false);
		AosXmlTagPtr stored_doc = origdoc->getFirstChild();

		if (mNumFields == 1 && mFtypes[0] == 'x')
		{
			// It is selecting the doc itself
			if (!stored_doc->isDocSame(retrieved_doc))
			{
				OmnAlarm << "Doc mismatch." << enderr;
				OmnScreen << "Original doc: " << stored_doc->getData() << endl;
				OmnScreen << "Retrieved doc: " << retrieved_doc->getData() << endl;
				return false;
			}
		}
		else
		{
			bool exist;
			for (int i=0; i<mNumFields; i++)
			{
				OmnString value1 = stored_doc->xpathQuery(mOnames[i], exist, "");
				OmnString value2;
				switch (mFtypes[i])
				{
				case '1':
					 value2 = retrieved_doc->getAttrStr(mCnames[i]);
					 break;

				case '2':
				case '3':
					 // It is subtag:
					 // 	<name>xxx</name>
					 {
						 OmnString path = mCnames[i];
						 path << "/_#text";
						 value2 = retrieved_doc->xpathQuery(path, exist, "");
					 }
					 break;

				default:
					 OmnAlarm << "Incorrect!" << enderr;
					 return false;
				}

				if (value1 != value2)
				{
					// Currently we cannot check zky_cnts
					OmnAlarm << "Failed the query! mOname: " 
						<< mOnames[i] << ". value1=" << value1 
						<< "------. Cname: " << mCnames[i] 
						<< ", value2=" << value2 << "-----" << enderr;
					return false;
				}
			}
		}

		retrieved_doc = doc->getNextChild();
	}

	aos_assert_r(numRecords == 0, false);
	aos_assert_r(!retrieved_doc, false);
	return true;
	*/
}


OmnString
AosQueryThrd::getNextDocid()
{
	// If mOrder is defined, it starts from mOrder. Otherwise, 
	// it starts from the first condition. 
	//
	// For each condition, it checks whether the current pointer
	// points to a valid doc. If yes, it checks the next condition. 
	// If the last condition points to a valid docid, the docid
	// is valid. The last condition advances its pointer and 
	// returns. 
	//
	// Otherwise, the previous condition advances its pointer
	// and repeats the above process until either a valid
	// docid is found or returns "" (meaning that no more
	// docids can be found).
	
	aos_assert_r(mNumConds > 0, false);
	OmnString docid;
	while ((docid = nextDocid()) != "")
	{
		bool isGood = true;
		for (int i=1; i<mNumConds; i++)
		{
			if (!isDocGood(i, docid)) 
			{
				isGood = false;
				break;
			}
		}

		if (isGood && checkFnames(docid)) return docid;
	}

	return "";
}


bool
AosQueryThrd::isDocGood(const int idx, const OmnString &docid)
{
	// Currently we will construct a query and query the database. 
	// This can be an efficiency issue. We may improve it in 
	// the future.
	OmnString stmt = "select count(*) from ";
	switch (mConds[idx].type)
	{
	case eLE:
	case eLT:
	case eEQ:
	case eNE:
	case eGT:
	case eGE:
		 stmt << mAttrTname << " where aname='"
			 << mConds[idx].aname << "' and avalue" << mConds[idx].opr 
			 << "'" << mConds[idx].avalue << "' and docid='"
			 << docid << "'";
		 break;

	case eWordAnd:
	case eWordOr:
		 OmnNotImplementedYet;
		 /*
		 stmt << mDocwdTname << " where word='" 
			 << mConds[idx].avalue << "' and docid='" << docid << "'";
		 */
		 break;

	case eTag:
		 stmt << mTagTname << " where tag='" 
			 << mConds[idx].avalue << "' and docid='" << docid << "'";
		 break;

	case eInRange:
		 stmt << mAttrTname << " where aname='"
			 << mConds[idx].aname << "' and avalue" << mConds[idx].opr
			 << "'" << mConds[idx].avalue << "' and avalue" << mConds[idx].rightOpr
			 << "'" << mConds[idx].value2 
			 << "' and docid='" << docid << "'";
		 break;

	default:
		 OmnAlarm << "Invalid condition type: " << mConds[idx].type << enderr;
		 return false;
	}

	stmt << " and siteid='" << mSiteid << "'";

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	OmnDbRecordPtr record;
	OmnRslt rslt = store->query(stmt, record);
	aos_assert_r(record, false);
	int num = record->getInt(0, -1, rslt);
	return (num > 0);
}


bool
AosQueryThrd::checkFnames(const OmnString &docid)
{
	// Need to check whether it contains all the attributes defined
	// by mOnames
	OmnString stmt;
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	OmnDbRecordPtr record;
	OmnRslt rslt;
	int num;

	for (int i=0; i<mNumFields; i++)
	{
		if (mFtypes[i] == 'x')
		{
			aos_assert_r(mNumFields == 1, false);
			return true;
		}
		stmt = "select count(*) from ";
		stmt << mAttrTname << " where aname='" << mOnames[i] << "'"
			<< " and docid='" << docid << "'";
		rslt = store->query(stmt, record);
		aos_assert_r(record, false);
		num = record->getInt(0, -1, rslt);
		if (num <= 0) return false;
	}
	return true;
}


OmnString
AosQueryThrd::nextDocid()
{
	if (mNoMoreDocs) return "";

	if (!mControllingTable || 
		 mControllingTable->entries() == 0 ||
		!mControllingTable->hasMore())
	{
		mStmt = "select docid from ";
		bool isDesc = false;

		// There is no order field. Use the first condition
		// as the controlling condition. 
		switch (mConds[0].type)
		{
		case eEQ:
		case eNE:
		case eLT:
		case eLE:
		case eGT:
		case eGE:
			 mStmt << mAttrTname << " where aname='"
			 	<< mConds[0].aname << "' and avalue" << mConds[0].opr
				<< "'" << mConds[0].avalue << "'";
			 isDesc = mConds[0].isDesc;
			 break;

		case eInRange:
			 mStmt << mAttrTname << " where aname='" 
				 << mConds[0].aname << "' and avalue"
				 << mConds[0].opr << "'" << mConds[0].avalue
				 << "' and avalue" << mConds[0].rightOpr 
				 << "'" << mConds[0].value2 << "'";
			 isDesc = mConds[0].isDesc;
			 break;

		case eWordAnd:
		case eWordOr:
			 OmnNotImplementedYet;
			 /*
			 mStmt = "select docid from ";
			 mStmt << mDocwdTname << " where word='"
				 << mConds[0].avalue << "'";
			 isDesc = mConds[0].isDesc;
			 */
			 break;

		case eTag:
			 mStmt = "select docid from ";
			 mStmt << mTagTname << " where tag='"
				 << mConds[0].avalue << "'";
			 isDesc = mConds[0].isDesc;
			 break;

		default:
			 OmnAlarm << "Unrecognized condition type: " 
				 << mConds[0].type << enderr;
			 return "";
		}

		/*
		if (mOrder != "")
		{
			// mOrder is an attribute name. It means the result
			// should be ordered by the values of the attribute, 
			// eithe ascending or desencding. 
			mStmt << " order by " << mOrder;
			isDesc = mDescending;
		}
		*/

		mStmt << " and siteid='" << mSiteid 
			<< "' order by avalue limit " << mStartIdx << ", " << mPsize;
		// if (isDesc) mStmt << " desc";
		// OmnScreen << mStmt << endl;
		// OmnScreen << "Update startidx: " << mStartIdx << ":" << mPsize << endl;
		mStartIdx += mPsize;

		OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
		aos_assert_r(store->query(mStmt, mControllingTable), "");
		aos_assert_r(mControllingTable, "");
		if (mControllingTable->entries() == 0) 
		{
			mNoMoreDocs = true;
			return "";
		}
		mControllingTable->reset();
	}

	OmnDbRecordPtr record = mControllingTable->next();
	aos_assert_r(record, "");
	OmnRslt rslt;
	return record->getStr(0, "", rslt);
}


bool
AosQueryThrd::signal(const int threadLogicId)
{
	return true;	
}


bool    
AosQueryThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosQueryThrd::createConds()
{
	//	<conds>
	//		<cond type="xxx">
	//		...
	//	</conds>
	//
	//	Currently we only implement:
	//	<conds>
	//		<cond type="AND">
	//			<term type="le|lt|eq|ne|gt|ge|range|word|tag"
	//				lhs="xxx"
	//				rhs="xxx"/>
	//			...
	//		</cond>
	//	</conds>
	mNumConds = 0;
	int num = 0;
	int vv = rand();
	vv = vv % 100;
	if (vv <= 80) num = 1;
	else if (vv <= 95) num = 2;
	else if (vv <= 97) num = 3;
	else if (vv <= 98) num = 4;
	else if (vv <= 99) num = 5;
	else num = 6;
	// num = 1;

	aos_assert_r(num < eMaxConds, false);

	mCondStr = "<conds><cond type=\"AND\">";
	for (int i=0; i<num; i++)
	{
		vv = rand() % 100;
		// if      (vv < 10) createInRange();
		// else if (vv < 20) createWord();
		// else if (vv < 30) createTag();
		if (vv < 40)      createArith("eq", "=", eEQ);
		else if (vv < 50) createArith("lt", "<", eLT);
		else if (vv < 60) createArith("le", "<=", eLE);
		else if (vv < 70) createArith("ne", "!=", eNE);
		else if (vv < 80) createArith("gt", ">", eGT);
		else              createArith("ge", ">=", eGE);
	}
	mCondStr << "</cond></conds>";
	return true;
}


bool
AosQueryThrd::createArith(
		const OmnString &opr, 
		const OmnString &opr1, 
		const Opr type)
{
	// It creates a query condition of the following form:
	// 		<term type="xxx">
	// 			<lhs><![CDATA[xxx..]]></lhs>
	// 			<rhs><![CDATA[xxx..]]></rhs>
	// 		</term>
	OmnString name = AosTestXmlDoc::getAttrname(mSiteid);
	mConds[mNumConds].aname = name;
	mConds[mNumConds].avalue = AosTestXmlDoc::getAttrvalue("", mSiteid, "");
	mConds[mNumConds].type = type;
	mConds[mNumConds].opr = opr1;

	mCondStr << "<term type=\"" << opr << "\">"
		<< "<lhs><![CDATA[" << mConds[mNumConds].aname << "]]></lhs>"
		<< "<rhs><![CDATA[" << mConds[mNumConds].avalue << "]]></rhs>"
		<< "</term>";
	mNumConds++;
	return true;
}


bool
AosQueryThrd::createInRange()
{
	// It creates a query condition of the following form:
	// 		<term type="range" aname="xxx" [(min, max)]/>
	OmnString v1 = AosTestXmlDoc::getAttrvalue("", mSiteid, "");
	OmnString v2 = AosTestXmlDoc::getAttrvalue("", mSiteid, "");
	mConds[mNumConds].aname = AosTestXmlDoc::getAttrname(mSiteid);
	mConds[mNumConds].type = eInRange;

	int vv = rand() % 100;
	OmnString leftBkt, rightBkt, leftOpr, rightOpr;
	if (vv < 25) 
	{
		leftBkt = "["; 
		rightBkt = "]";
		mConds[mNumConds].opr = "<=";
		mConds[mNumConds].rightOpr = ">=";
	}
	else if (vv < 50) 
	{
		leftBkt = "("; 
		rightBkt = "]";
		mConds[mNumConds].opr = "<";
		mConds[mNumConds].rightOpr = ">=";
	}
	else if (vv < 75) 
	{
		leftBkt = "("; 
		rightBkt = ")";
		mConds[mNumConds].opr = "<";
		mConds[mNumConds].rightOpr = ">";
	}
	else              
	{
		leftBkt = "["; 
		rightBkt = ")";
		mConds[mNumConds].opr = "<=";
		mConds[mNumConds].rightOpr = ">";
	}

	mCondStr << "<term type=\"range\" left=\"" << leftBkt << "\""
		<< " right=\"" << rightBkt << "\">"
		<< "<aname><![CDATA[" << mConds[mNumConds].aname << "]]></aname>";
	mConds[mNumConds].type = eInRange;

	if (v1 <= v2)
	{
		mConds[mNumConds].avalue = v1;
		mConds[mNumConds].value2 = v2;
	}
	else
	{
		mConds[mNumConds].avalue = v2;
		mConds[mNumConds].value2 = v1;
		mCondStr << leftBkt << v2 << "," << v1 << rightBkt << "\"/>";
	}
	mCondStr << "<min><![CDATA[" << mConds[mNumConds].avalue << "]]></min>"
			<< "<max><![CDATA[" << mConds[mNumConds].value2 << "]]></max></term>";
	mNumConds++;
	return true;
}


bool
AosQueryThrd::createOrder()
{
	// It is in the form: 
	// 	order by <fname>
	// Currently we only support one ordering field.
	mOrder = "";
	int value = rand() % 100;
	if (value < 20) return true;
	mOrder = AosTestXmlDoc::getAttrname(mSiteid);

	value = rand() % 100;
	mDescending = (value > 35)?"asce":"desc";
	return true;
}


bool 
AosQueryThrd::createLimit()
{
	// OmnScreen << "select startidx" << endl;
	mStartIdx = 0;
	mPsize = 30;
	int value = rand() % 100;
	// if (value < 40) return true;
	if (value < 140) return true;

	if      (value < 50) mStartIdx = rand() % 10;
	else if (value < 60) mStartIdx = rand() % 50;
	else if (value < 70) mStartIdx = rand() % 100;
	else if (value < 80) mStartIdx = rand() % 300;
	else if (value < 90) mStartIdx = rand() % 1000;
	else if (value < 95) mStartIdx = rand() % 5000;
	else 			     mStartIdx = rand() % 50000;

	value = rand() % 100;
	if      (value < 10) mPsize = rand() % 10;
	else if (value < 20) mPsize = rand() % 20;
	else if (value < 30) mPsize = rand() % 40;
	else if (value < 40) mPsize = rand() % 80;
	else if (value < 50) mPsize = rand() % 100;
	else if (value < 60) mPsize = rand() % 200;
	else if (value < 70) mPsize = rand() % 400;
	else if (value < 80) mPsize = rand() % 800;
	else if (value < 90) mPsize = rand() % 1000;
	else 			 	 mPsize = rand() % 2000;

	return true;
}


bool
AosQueryThrd::createWord()
{
	// <term type="word">xxx</term>
	OmnString ww;
	if ((rand() % 100) < 70) 
	{
		ww = "wordand";
		mConds[mNumConds].type = eWordAnd;
	}
	else
	{
		ww = "wordor";
		mConds[mNumConds].type = eWordOr;
	}
	mConds[mNumConds].avalue = AosTestXmlDoc::getDocWord();

	mCondStr << "<term type=\"" << ww << "\"><![CDATA[" << mConds[mNumConds].avalue
		<< "]]></term>";
	mNumConds++;
	return true;
}


bool
AosQueryThrd::createTag()
{
	// <term type="tag">xxx</term>
	mConds[mNumConds].type = eTag;
	mConds[mNumConds].avalue = AosTestXmlDoc::getDocWord();

	mCondStr << "<term type=\"tag\"><![CDATA[" << mConds[mNumConds].avalue
		<< "]]></term>";
	mNumConds++;
	return true;
}


bool
AosQueryThrd::createFnames()
{
	// 	<fnames>
	// 		<field type="xxx">
	// 			<oname><![CDATA[xxx]]></oname>
	// 			<cname><![CDATA[xxx]]></cname>
	//		</field>
	// 		...
	// 	</fnames>
	//
	//  where 'type' can be 1, 2, 3, or 'x' (retrieving the xmlobj)
	
	int vv = rand() % 100;
	if (vv < 20)
	{
		mFnames = "<fnames><fname type=\"x\"/></fnames>";
		mFtypes[0] = 'x';
		mNumFields = 1;
		return true;
	}

	// Determine the number of fields
	if (vv < 40) mNumFields = 1;
	else if (vv < 60) mNumFields = 2;
	else if (vv < 80) mNumFields = 3;
	else if (vv < 90) mNumFields = 4;
	else if (vv < 95) mNumFields = 5;
	else mNumFields = 6;

	mFnames = "<fnames>";
	for (int i=0; i<mNumFields; i++)
	{
		mOnames[i] = AosTestXmlDoc::getAttrname(mSiteid);
		mOnames[i].replace(':', '0');
		mCnames[i] = AosTestXmlDoc::getCommonWords();
		mCnames[i].replace(':', '1');
		mFtypes[i] = getFtype();
		mFnames << "<fname type=\"" << mFtypes[i] << "\">"
			<< "<oname><![CDATA[" << mOnames[i] << "]]></oname>"
			<< "<cname><![CDATA[" << mCnames[i] << "]]></cname>"
			<< "</fname>";
	}
	mFnames << "</fnames>";
	return true;
}


char
AosQueryThrd::getFtype()
{
	// Field types are 1, 2, or 3.
	int vv = rand() % 100;
	if (vv < 40) return '1';
	if (vv < 70) return '2';
	return '3';
}


OmnString
AosQueryThrd::getCname(const OmnString &oname)
{
	return oname;
}


AosXmlTagPtr
AosQueryThrd::pickDoc()
{
	AosXmlTagPtr doc = 0;
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	bool rslt = false;
	while(!rslt)
	{
		int docnum = OmnRandom::nextInt(1,eMaxDocNum);
		if(mForceDocid >=0)
		{
			docnum = mForceDocid;
		}

		//		rslt = AosDocServer::getSelf()->getDoc(docnum,docbuff);
		rslt = AosSengAdmin::getSelf()->readDoc(docnum,docbuff);
		if(!rslt)continue;

		AosXmlTagPtr doc = parser.parse(docbuff, "");
		if (!doc)
		{
			rslt = false;
			continue;
		}
		return doc;
	}
	return doc;
}

AosQueryThrd::QueryType
AosQueryThrd::getQueryType()
{
	if(mForceType != eQueryAnyType)
		return mForceType;

	int total = eQueryKeywordRate + eQueryTagRate + eQueryDocidRate + eQueryObjidRate + eQueryAttributeRate + eQueryContainerRate;
	if(total <= 0)
	{
		return (AosQueryThrd::QueryType)OmnRandom::nextInt(eQueryKeyword,eQueryContainer);
	}

	int rd = OmnRandom::nextInt(0,total-1);
	if(rd < eQueryKeywordRate)return eQueryKeyword;
	rd -= eQueryKeywordRate;

	if(rd < eQueryTagRate)return eQueryTag;
	rd -= eQueryTagRate;

	if(rd < eQueryDocidRate)return eQueryDocid;
	rd -= eQueryDocidRate;

	if(rd < eQueryObjidRate)return eQueryObjid;
	rd -= eQueryObjidRate;

	if(rd < eQueryAttributeRate)return eQueryAttribute;
	rd -= eQueryAttributeRate;

	if(rd < eQueryContainerRate)return eQueryContainer;
	rd -= eQueryContainerRate;

	return eQueryContainer;
}

bool
AosQueryThrd::basicQuery()
{
	// This function randomly selects a doc. It then collects all the
	// keywords and attributes from the doc and does the following 
	// queries:
	// 1. Keyword query
	// 2. Tag query
	// 3. Container query
	// 4. Attribute query
	// 5. Query doc by docid
	// 6. Query doc by objid
//cout << "pick doc"	<< endl;
	AosXmlTagPtr doc = pickDoc();
	aos_assert_r(doc, false);

	OmnStrHash<int, 0x3ff> hash;
	bool rslt = collectWords(doc, hash);
	aos_assert_r(rslt, false);

	int nn = OmnRandom::nextInt(1,eNumQueriesPerDoc);
	for (int i=0; i<nn; i++)
	{
//cout << "query"	<< endl;
		// Determine the type of query: 
		QueryType type = getQueryType();
		switch (type)
		{
		case eQueryKeyword:
//cout << "key"	<< endl;
			 aos_assert_r(queryKeyword(doc, hash), false);
//cout << "key  end "	<< endl;
			 break;

		case eQueryTag:
//cout << "tag"	<< endl;
			 aos_assert_r(queryTag(doc), false);
//cout << "tag end"	<< endl;
			 break;

		case eQueryDocid:
//cout << "docid"	<< endl;
			 aos_assert_r(queryDocid(doc), false);
//cout << "docid end"	<< endl;
			 break;

		case eQueryObjid:
//cout << "objid"	<< endl;
			 aos_assert_r(queryObjid(doc), false);
//cout << "objid end"	<< endl;
			 break;

		case eQueryAttribute:
//cout << "attr"	<< endl;
			 aos_assert_r(queryAttribute(doc), false);
//cout << "attr end"	<< endl;
			 break;

		case eQueryContainer:
//cout << "container"	<< endl;
			 aos_assert_r(queryContainer(doc), false);
//cout << "container end"	<< endl;
			 break;

		default:
			 OmnAlarm << "Unrecognized query type: " << type << enderr;
			 break;
		}
		OmnSleep(1);
	}

	return true;
}


bool
AosQueryThrd::collectWords(const AosXmlTagPtr &xml,
						   OmnStrHash<int, 0x3ff> &hash)
{
	// It collects all the words from 'xml' into 'hash'. 
	char *buff1;
	xml->resetGetWords();
	u64 wordid;

	char wordbuff1[AosXmlTag::eMaxWordLen+1];
	char wordbuff2[AosXmlTag::eMaxWordLen+1];
	int wordlen = 0;
	while (xml->nextWord(mWordParser, wordbuff1, AosXmlTag::eMaxWordLen, wordlen))
	{
		if (wordlen >= AOSSE_MAX_WORDLEN || wordlen <= 1) continue;
		wordid = AosWordFilter::getSelf()->filter(wordbuff1, wordlen);
		if (wordid != AOS_INVWID) continue;
		
		buff1 = (char *)AosWordNormal::getSelf()->normalize(wordbuff1, wordlen,
			wordbuff2, AosXmlTag::eMaxWordLen);
		
		if (buff1) hash.add(buff1, wordlen, 1);
	}
	return true;
}


bool
AosQueryThrd::queryKeyword(const AosXmlTagPtr &doc,
						   OmnStrHash<int, 0x3ff> &hash)
{
	// This function randomly pick a word from 'hash', and then
	// queries the server. If the server fails returning the
	// doc, it is an error. Otherwise, it passes the test.
	OmnString word = hash.getRandKey("");
	if(mForceWord != "")
	{
		word = mForceWord;
	}

	aos_assert_r(word != "", false);
	AosXmlTagPtr queried_doc = AosSengAdmin::getSelf()->queryDocByWords(mSiteid,
																		word,
																		doc->getAttrInt(AOSTAG_DOCID,AOS_INVWID));
	OmnCreateTc << (queried_doc) << endtc;
	if(!queried_doc)
	{
		cout << "key word:" << word << endl;
		cout << "docid:" << doc->getAttrInt(AOSTAG_DOCID,AOS_INVWID) << endl;
		cout << "doc:" << doc->getData()<< endl;
		OmnSleep(1);
		OmnMark;
	}
	return true;
}


bool
AosQueryThrd::queryTag(const AosXmlTagPtr &doc)
{
	// This function randomly pick a tag from the doc. It then 
	// queries the server. If the server fails returning the
	// doc, it is an error. Otherwise, it passes the test.
	OmnString tags = doc->getAttrStr(AOSTAG_TAG);
	if (tags != "") tags << ",";
	tags << doc->getAttrStr(AOSTAG_TAGC);
	if (tags == "") return true;

	AosStrSplit split;
	OmnString *tagarray;
	tagarray = OmnNew OmnString[eMaxTags];

//	bool finished;
//	int nn = split.splitStr(tags.data(), ",", tagarray, eMaxTags, finished);

	// use strparser
	OmnStrParser parser(tags,", ",false,false);
	int tagIdx = 0;
	OmnString subword;
	while(tagIdx < eMaxTags)
	{
		subword = parser.nextWord();
		if(subword == "") break;

		tagarray[tagIdx] = subword;
		tagIdx ++;
	}
	if(tagIdx <=1)
	{
		// no tags in it
		return true;
	}

	OmnString word = tagarray[OmnRandom::nextInt(0,tagIdx-2)];
	if(mForceWord != "")
	{
		word = mForceWord;
	}
	// Randomly pick one from the array

	AosXmlTagPtr queried_doc = AosSengAdminSelf->queryDocByTags(mSiteid,
															   word,
															   doc->getAttrInt(AOSTAG_DOCID,AOS_INVWID));
	OmnCreateTc << (queried_doc) << endtc;
	if(!queried_doc)
	{
		cout << "tag name:" << word << endl;
		cout << "docid:" << doc->getAttrInt(AOSTAG_DOCID,AOS_INVWID) << endl;
		cout << "doc:" << doc->getData()<< endl;
		OmnSleep(1);
		OmnMark;
	}
	OmnDelete []tagarray;
	return true;
}


bool
AosQueryThrd::queryDocid(const AosXmlTagPtr &doc)
{
	OmnString docid = doc->getAttrStr(AOSTAG_DOCID);
	AosXmlTagPtr queried_doc = AosSengAdminSelf->queryDocByDocid(mSiteid,
															   doc->getAttrStr(AOSTAG_DOCID),
															   0);
	OmnCreateTc << (queried_doc) << endtc;
	if(!queried_doc)
	{
		cout << "docid:" << doc->getAttrInt(AOSTAG_DOCID,AOS_INVWID) << endl;
		cout << "doc:" << doc->getData()<< endl;
		OmnMark;
	}
	return true;
}


bool
AosQueryThrd::queryObjid(const AosXmlTagPtr &doc)
{
	OmnString objid = doc->getAttrStr(AOSTAG_DOCID);
	AosXmlTagPtr queried_doc = AosSengAdminSelf->queryDocByObjid(mSiteid,
															   doc->getAttrStr(AOSTAG_OBJID),
															   0);
	OmnCreateTc << (queried_doc) << endtc;
	if(!queried_doc)
	{
		cout << "objid:" <<  doc->getAttrStr(AOSTAG_OBJID) << endl;
		cout << "doc:" << doc->getData()<< endl;
		OmnMark;
	}
	return true;
}


bool
AosQueryThrd::queryContainer(const AosXmlTagPtr &doc)
{
	OmnString container = doc->getContainer1();
	AosXmlTagPtr queried_doc = AosSengAdminSelf->queryDocByContainer(mSiteid,
															   container,
															   doc->getAttrU64(AOSTAG_DOCID,0));
	OmnCreateTc << (queried_doc) << endtc;
	if(!queried_doc)
	{
		cout << "container name:" << container << endl;
		cout << "docid:" << doc->getAttrInt(AOSTAG_DOCID,AOS_INVWID) << endl;
		cout << "doc:" << doc->getData()<< endl;
		OmnSleep(1);
		OmnMark;
	}

	return true;
}


bool
AosQueryThrd::queryAttribute(const AosXmlTagPtr &doc)
{
	doc->resetAttrNameValueLoop();
	int attrNum = 0;
	bool finished = false;
	OmnString attrName;
	OmnString attrValue;
//	OmnVList<OmnString> attrNames;
//	OmnVList<OmnString> attrValues;
//	OmnString *attrNames = OmnNew OmnString[eMaxAttrNum];
//	OmnString *attrValues = OmnNew OmnString[eMaxAttrNum];
	while(attrNum < eMaxAttrNum-1)
	{
		bool hasAttr = doc->nextAttr("",
									 attrName,
									 attrValue,
									 finished,
									 false);
		if(!hasAttr)break;
		mAttrNames[attrNum] = attrName;
		mAttrVals[attrNum] = attrValue;
		attrNum ++;
		if(finished)break;
	}
	if(attrNum <= 0)
	{
		return true;
	}

	int attrid = OmnRandom::nextInt(0,attrNum - 1);
	// This function randomly picks an attribute, queries the database
	// to determine whether it can get the doc.
	OmnString finalName;
	OmnString finalValue;
	AosOpr finalOpr = eAosOpr_eq;

	finalName = mAttrNames[attrid];
	if(!decideAttrQueryParam(mAttrVals[attrid],finalOpr,finalValue))
	{
		return true;
	}
//	finalValue = mAttrVals[attrid];

	if(mForceName != "")
	{
		finalName = mForceName;
	}
	if(mForceValue != "")
	{
		finalValue = mForceValue;
	}
	if(mForceOpr != eAosOpr_Invalid)
	{
		finalOpr = mForceOpr;
	}

	AosXmlTagPtr queried_doc = AosSengAdminSelf->queryDocByAttrs(
																mSiteid,
																finalName,
																finalOpr,
																finalValue,
																doc->getAttrU64(AOSTAG_DOCID,0));
	OmnCreateTc << (queried_doc) << endtc;
	if(!queried_doc)
	{
		cout << "attrname:" << finalName << endl;
		cout << "attrvalue:" << finalValue << endl;
		cout << "opr:" << AosOpr_toStr(finalOpr) << endl;
		cout << "docid:" << doc->getAttrInt(AOSTAG_DOCID,AOS_INVWID) << endl;
		cout << "doc:" << doc->getData()<< endl;
		OmnSleep(1);
		OmnMark;
	}

	return true;
}

bool
AosQueryThrd::decideAttrQueryParam(const OmnString &origValue,
									 AosOpr &finalOpr,
									 OmnString &finalValue)
{
	// generate value
	int r1 = OmnRandom::nextInt(1,100);
	if(r1 <= eAttrQueryEqualValueRate)
	{
		// use the origValue
		finalValue = origValue;
	}
	else
	{
		// generate value
		int r2 = OmnRandom::nextInt(1,100);
		if(r2 <= eAttrQueryStringTotalChangeRate)
		{
			// totally generate a new string
			int len = OmnRandom::nextInt(1,eAttrQueryStringMaxLen);
			finalValue = OmnRandom::getRandomStr("",0,len);
		}
		else
		{
			// generate string based on the orig one
			int lenremain = OmnRandom::nextInt(1,origValue.length());
			if(lenremain > eAttrQueryStringMaxLen)lenremain = eAttrQueryStringMaxLen;

			int lennew = OmnRandom::nextInt(0,eAttrQueryStringMaxLen-lenremain);
			finalValue = OmnRandom::getRandomStr(origValue,lenremain,lennew);
		}
	}

	int cmp = strcmp(origValue.data(),finalValue.data());
	if(cmp == 0)
	{
		// equal
		int r3 = OmnRandom::nextInt(1,100);
		if(r3 <= eAttrQueryEqualValueEQOprRate)
		{
			finalOpr = eAosOpr_eq;
		}
		else if(r3 <= eAttrQueryEqualValueEQOprRate + eAttrQueryEqualValueGEOprRate)
		{
			finalOpr = eAosOpr_ge;
		}
		else
		{
			finalOpr = eAosOpr_le;
		}
	}

	if(cmp > 0)// origValue > finalValue
	{
		int r3 = OmnRandom::nextInt(1,100);
		if(r3 <= eAttrQueryGTValueGTOprRate)
		{
			finalOpr = eAosOpr_gt;
		}
		else
		{
			finalOpr = eAosOpr_ge;
		}
	}

	if(cmp < 0)// origValue < finalValue
	{
		int r3 = OmnRandom::nextInt(1,100);
		if(r3 <= eAttrQueryLTValueLTOprRate)
		{
			finalOpr = eAosOpr_lt;
		}
		else
		{
			finalOpr = eAosOpr_le;
		}
	}
	return true;
}
