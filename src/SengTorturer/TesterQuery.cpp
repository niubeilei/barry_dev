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
// The torturer created many docs. This tester query the server based on
// the docs the torturer has created. 
//
// There are two forms of queries. One is expressed in strings and the
// other XML. This tester will exercise both. 
//
// Queries are expressed in multiple terms. Each term defines one atomic
// condition. In the current implementation, the following terms are 
// supported:
// 		TermAnd
// 		TermArith
// 		TermTag
// 		TermWord
// 		TermLog
// 		TermLogin
//
// The system currently does not support mTermOr. 
//
// There are several types of querues:
// 		1. Query docs by objid
// 		2. Query docs by docid
// 		3. Query docs by cloud id
// 		4. Query docs by conditions
//
// Query docs by objid:
// 	It randomly picks a doc created by the tester, and queries the server.
// 	The server should be able to return the doc. 
//
// Query docs by docid:
//  (Similar to the above).
//
// Query docs by cloud id:
//  (Similar to the above).
//
// Query docs by conditions:
//  Below are the variables:
//  	1. The number of terms
//  	2. The direction
//  	3. Order
//  	4. Paging
//
// Modification History:
// 10/26/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/TesterQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "SengTorturer/TesterCreateObj.h"
#include "SengTorturer/TesterLogin.h"
#include "SengTorturer/SengTesterXmlDoc.h"
#include "SengTorturer/SengTesterMgr.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorturer/SengTesterThrd.h"
#include "SengTorturer/SengTesterFileMgr.h"
#include "SengTorturer/StUtil.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"

# if 0
AosQueryTester::AosQueryTester(const OmnString &weight_tagname):
AosSengTester(weight_tagname)
{
	AosSengTesterPtr thisptr(this, false);
	registerTester(eQueryTester, thisptr);
}


AosQueryTester::~AosQueryTester()
{
}


bool 
AosQueryTester::test()
{
	// This is the overall control of the testing. It randomly determines what
	// to do for this call.
	mLocalDocid = thread->getNewDocId();

	Operation opr = pickOperation();
	switch (opr)
	{
	case eQueryDocByDocid:
		 return queryDocByDocid();

	case eQueryDocByObjid:
		 return queryDocByObjid();

	case eQueryDocByCloudid:
		 return queryDocByCloudid();

	case eQueryDocsByQueries:
		 return queryDocsByQueries();

	case eQueryDocsByKeywords:
		 return queryDocsByKeywords();

	case eQueryDocsByTags:
		 return queryDocsByTags();

	case eQueryDocsByContainers:
		 return queryDocsByContainers();

	default:
		 OmnAlarm << "Unrecognized operation: " << opr << enderr;
		 return false;
	}
	
	OmnShouldNeverComeHere;
	return false;
}


AosQueryTester::Operation
AosQueryTester::pickOperation()
{
	int vv = rand() % mTotalWeight;
	int total = 0;
	for (int i=0; i<eMaxOperation; i++)
	{
		total += mWeights[i];
		if (vv < total) return i;
	}

	return eMaxOperation-1;
}


bool
AosQueryTester::queryDocsByKeywords()
{
	// A doc contains a number of words, called Keywords. This function 
	// queries the picked doc based on keywords. Multiple keywords can 
	// be specified. In the current implementations, multiple keywords
	// are all ANDed. 
	//
	// Queries are typed based on the number of keywords and whether 
	// the selected keywors exist in the picked doc or not. For each
	// type of queries, it records the query time, calculates the average, 
	// min, and max time. 
	//
	// If errors were found, it should record the original query but 
	// do not raise alarm.
	
	aos_assert_r(collectWords(), false);

	// 1. Determine whether to query keywords from the doc or
	//    query keywords that are not in the doc.
	// 2. Randomly pick one or more keywords from the doc
	// 3. Determine page size
	// 4. Determine order
	// 5. Query the server
	// 6. Verify the response
	
	// 1. Determine whether to query keywords from the doc or
	//    query keywords that are not in the doc.
	OmnToBeDone;

	// 2. Randomly pick one or more keywords from the doc
	OmnToBeDone;

	// 3. Determine page size
	OmnToBeDone;
	
	// 4. Determine Order
	OmnToBeDone;
	
	// 5. Query the Server
	OmnToBeDone;

	// 6. Verify the response
	OmnToBeDone;

	return false;
}


bool
AosQueryThrd::queryDocsByTags()
{
	// Similar to query keywords.
	/*
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

	AosXmlTagPtr queried_doc = AosSengAdminSelf->queryDocByTags(
			mSiteid, word, doc->getAttrInt(AOSTAG_DOCID,AOS_INVWID));
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
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosQueryThrd::queryDocByDocid()
{
	// 1. Pick a doc
	// 2. Determine whether to do a valid query or invalid query
	// 3. Query the server
	// 4. Verify the response
	/*
	OmnString docid = doc->getAttrStr(AOSTAG_DOCID);
	AosXmlTagPtr queried_doc = AosSengAdminSelf->queryDocByDocid(
			mSiteid, doc->getAttrStr(AOSTAG_DOCID), 0);
	OmnCreateTc << (queried_doc) << endtc;
	if(!queried_doc)
	{
		cout << "docid:" << doc->getAttrInt(AOSTAG_DOCID,AOS_INVWID) << endl;
		cout << "doc:" << doc->getData()<< endl;
		OmnMark;
	}
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosQueryThrd::queryDocByObjid(const AosXmlTagPtr &doc)
{
	// 1. Pick a doc
	// 2. Determine whether to do a valid query or invalid query
	// 3. Query the server
	// 4. Verify the response
	/*
	OmnString objid = doc->getAttrStr(AOSTAG_DOCID);
	AosXmlTagPtr queried_doc = AosSengAdminSelf->queryDocByObjid(
			mSiteid, doc->getAttrStr(AOSTAG_OBJID), 0);
	OmnCreateTc << (queried_doc) << endtc;
	if(!queried_doc)
	{
		cout << "objid:" <<  doc->getAttrStr(AOSTAG_OBJID) << endl;
		cout << "doc:" << doc->getData()<< endl;
		OmnMark;
	}
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosQueryThrd::queryDocsByContainers(const AosXmlTagPtr &doc)
{
	// A container may contain zero or more docs. Each container
	// has the following index lists:
	// 	1. Listed by Objids
	// 	2. Listed by Creation Time		(not implemented yet)
	// 	3. Listed by Modification Time	(not implemented yet)
	// 	4. Listed by Creator			(not implemented yet)
	// 	5. Listed by Modifier			(not implemented yet)
	//
	// In addition, a container may create customized indexes, 
	// specified by the attribute AOSTAG_CUST_LISTS, which is 
	// a list of attribute names (this feature was not implemented
	// yet as of this writing (10/31/2010). If an attribute is defined in 
	// it, a special list is created that is sorted based on 
	// the attribute name.
	//
	// It is possible to query multiple containers. If querying multiple
	// containers, the query is ANDed, which means that a doc is queried
	// if and only if the doc exists in all the containers.
	//
	// 1. Determine whether to do a valid query or an invalid
	//    query. 
	// 2. Pick a container
	// 3. Query the server
	// 4. Verify the response
	//
	// The thread maintains all the contains it has created so far. 
	// If it is a valid query, it can pick a container from the thread. 
	// The thread also records all the members of a given container. 
	// We will use this to verify the response. 
	/*
	OmnString container = doc->getContainer1();
	AosXmlTagPtr queried_doc = AosSengAdminSelf->queryDocByContainer(
			mSiteid, container, doc->getAttrU64(AOSTAG_DOCID,0));
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
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosQueryThrd::queryDocsByQueries()
{
	// This is a comprehensive query testing. In general, a query
	// may contain a number of conditions. Each condition is defined
	// as a term. Multiple terms can be ANDed (OR is not supported
	// yet). A doc is queried if the doc satifies all the terms contained
	// in a query.
	//
	// 1. Determine the number of terms
	// 2. Determine each term
	// 3. Determine page size
	// 4. Determine order field
	// 5. Determine query direction
	// 6. Determine the number of 'goto'
	// 7. Query the server
	// 8. Verify the results.
	//
	// Once a query is determined, this function should determine all
	// the docs that satisfy the query. This is used to determine whether
	// the query results are correct. 
	//
	// To improve the performance, the test thread keeps a number of 
	// index tables for selected attributes, keywords, and tags. If
	// all the attributes, keywords, and tags in a query are indexed
	// by the thread, it will verify the results. Otherwise, it will 
	// not verify the results. 
	//
	// To verify results, we can call the thread's member function 
	// to get all the matching docs, sorted based on a specified 
	// attribute. This makes the response verification very easy.
	// It can also be used to verify the 'goto' operation.
	
	aos_assert_r(pickNumTerms(), false);
	aos_assert_r(createTerms(), false);
	aos_assert_r(pickPagesize(), false);
	aos_assert_r(pickOrder(), false);
	aos_assert_r(pickReverse(), false);

	// Query the server
	int tries = pickQueryTries();
	for (int i=0; i<tries; i++)
	{
		aos_assert_r(pickStartIdx(), false);
		aos_assert_r(queryServer(), false);
		aos_assert_r(verifyResponse(), false);
	}

	return true;

	/*
	// 3. Determine
	doc->resetAttrNameValueLoop();
	int attrNum = 0;
	bool finished = false;
	OmnString attrName;
	OmnString attrValue;
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
			mSiteid, finalName, finalOpr, finalValue,
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
	*/
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
#endif

