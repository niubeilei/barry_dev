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
#if 0
#include "QueryTorturer/TesterQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"

#define PERCENT(n)   (rand()%100 < n)

AosTesterQuery::AosTesterQuery()
:
mNumDocs(0),
mRoundNum(0)
{
}


AosTesterQuery::~AosTesterQuery()
{
}


bool		
AosTesterQuery::basicTest()
{rand();
	int num_round = rand()%eMaxRoundNum;

	for(mRoundNum = 0;mRoundNum < num_round;mRoundNum ++)
//	for(int i = 0;i < num_round;i ++)
	{
		//1. add/del/modify doc
		procDocs();
		
		//2. query doc
		//procQuery();
	}
	
	return false;
}

bool		
AosTesterQuery::init()
{
	mHitName = OmnNew OmnString[eNumWords];
	mStrName = OmnNew OmnString[eNumWords];
	mU64Name = OmnNew OmnString[eNumWords];

//	mHitDocs = OmnNew set<u64>[eNumWords];
//	mStrDocs = OmnNew set<u64>[eNumWords];
//	mU64Docs = OmnNew set<u64>[eNumWords];
	mHitDocs = OmnNew map<u64,AosTesterQueryUnitPtr>[eNumWords];
	mStrDocs = OmnNew map<u64,AosTesterQueryUnitPtr>[eNumWords];
	mU64Docs = OmnNew map<u64,AosTesterQueryUnitPtr>[eNumWords];
	
	for(int i = 0; i < eNumWords;i++)
	{
		mHitName[i] = "hit_";
		mHitName[i] << i;
		
		mStrName[i] = "str_";
		mStrName[i] << i;

		mU64Name[i] = "u64_";
		mU64Name[i] << i << "__d";
	}
	
	return true;
}
	

bool		
AosTesterQuery::procDocs()
{
	int num_oprs = rand()%eMaxOprNum;
	
	cout << "proc doc:" << num_oprs << endl;
	for(int i = 0;i < num_oprs;i++)
	{
		procDoc();
	}
//	OmnSleep(1);
	return true;
}


bool		
AosTesterQuery::procDoc()
{
	int opr = rand()%100;
	if(opr < 80 && mNumDocs < eMaxDocs)
	{
		addDoc();
	}
	else if(opr < 90 && mNumDocs > 0)
	{
//		removeDoc();
	}
	else if(mNumDocs > 0)
	{
//		modifyDoc();
	}
	return true;
}
	
bool		
AosTesterQuery::procQuery()
{
	cout << "proc query" << endl;

	// get keywords
	int num_str		 = rand()%eNumWords;
	int num_u64 	 = rand()%eNumWords;
	int num_keywords = rand()%eNumWords;
	

num_u64 = 0;
num_keywords = 0;

	int u64_seq[eNumWords];
	int str_seq[eNumWords];
	int keyword_seq[eNumWords];
	AosOpr 		str_opr[eNumWords];
	AosOpr 		u64_opr[eNumWords];
	OmnString	str_value[eNumWords];
	u64			u64_value[eNumWords];
	
	memset(str_seq	  ,0,num_str*sizeof(int));
	memset(u64_seq	  ,0,num_u64*sizeof(int));
	memset(keyword_seq,0,num_keywords*sizeof(int));
	
	while(num_str + num_u64 + num_keywords == 0 || 
		  num_str + num_u64 + num_keywords > 2*eNumWords)
	{
		num_str = rand()%eNumWords;
		num_u64 = rand()%eNumWords;
		num_keywords = rand()%eNumWords;
num_u64 = 0;
num_keywords = 0;
	}
	
	//select str, u64, keywords
	for(int i = 0;i < num_str;i++)
	{
		str_seq[i] = rand()%eNumWords;
		
		for(int j = 0;j < i;j++)
		{
			if(str_seq[i] == str_seq[j])
			{
				i--;
				break;
			}
		}
	}

	for(int i = 0;i < num_str;i++)
	{
		str_opr[i] = OmnRandom::nextOpr();
		if(str_opr[i] == eAosOpr_Objid ||
		   str_opr[i] == eAosOpr_prefix ||
		   str_opr[i] == eAosOpr_range ||
		   str_opr[i] == eAosOpr_date ||
		   str_opr[i] == eAosOpr_epoch||
		   str_opr[i] == eAosOpr_Invalid)
		{
			i--;
			continue;
		}
			
		str_value[i] = OmnRandom::nextLetterDigitStr(1,50);
	}	

	for(int i = 0;i < num_u64;i++)
	{
		u64_seq[i] = rand()%eNumWords;
		
		for(int j = 0;j < i;j++)
		{
			if(u64_seq[i] == u64_seq[j])
			{
				i--;
				break;
			}
		}
	}
	for(int i = 0;i < num_u64;i++)
	{
		u64_opr[i] = OmnRandom::nextOpr();
		if(u64_opr[i] == eAosOpr_Objid ||
		   u64_opr[i] == eAosOpr_prefix ||
		   u64_opr[i] == eAosOpr_like ||
		   u64_opr[i] == eAosOpr_range ||
		   u64_opr[i] == eAosOpr_date ||
		   u64_opr[i] == eAosOpr_epoch ||
		   u64_opr[i] == eAosOpr_Invalid)
		{
			i--;
			continue;
		}
		u64_value[i] = OmnRandom::nextU64();
	}	

	for(int i = 0;i < num_keywords;i++)
	{
		keyword_seq[i] = rand()%eNumWords;
		
		for(int j = 0;j < i;j++)
		{
			if(keyword_seq[i] == keyword_seq[j])
			{
				i--;
				break;
			}
		}
	}
	
	// query 

	int num_all = num_keywords + num_str + num_u64;

	AosSengAdmin::QueryType *types = OmnNew AosSengAdmin::QueryType[num_all];
	OmnString *attrs = OmnNew OmnString[num_all];
	AosOpr *oprs = OmnNew AosOpr[num_all];
	OmnString *values = OmnNew OmnString[num_all];
	bool *orders = new bool[num_all];
	bool *reverses= new bool[num_all];

	int num_cur_all = 0;
	int num_cur_str = 0;
	int num_cur_u64 = 0;
	int num_cur_hit = 0;
	
	while(num_cur_all < num_all)
	{
		int type = rand()%3;
		// type == 0 str, type == 1 u64, type == 2 hit
		if(type == 0)// str
		{
			if(num_cur_str >= num_str)
			{
				continue;
			}
			types[num_cur_all]    = AosSengAdmin::eQueryStr;
			attrs[num_cur_all]    = mStrName[str_seq[num_cur_str]];
			oprs[num_cur_all]     = str_opr[num_cur_str];
			values[num_cur_all]   = str_value[num_cur_str];
			orders[num_cur_all]   = false;
			reverses[num_cur_all] = false;
			num_cur_str ++;
			num_cur_all ++;
		}
		else if(type == 1)// type == 1 , u64
		{
			if(num_cur_u64 >= num_u64)
			{
				continue;
			}
			types[num_cur_all]    = AosSengAdmin::eQueryU64;
			attrs[num_cur_all]    = mU64Name[u64_seq[num_cur_u64]];
			oprs[num_cur_all]     = u64_opr[num_cur_u64];
			values[num_cur_all]   = "";
			values[num_cur_all] << u64_value[num_cur_u64];
			orders[num_cur_all]   = false;
			reverses[num_cur_all] = false;
			num_cur_u64 ++;
			num_cur_all ++;
		}
		else// type == 2 , hit
		{
			if(num_cur_hit >= num_keywords)
			{
				continue;
			}
			types[num_cur_all]    = AosSengAdmin::eQueryHit;
			attrs[num_cur_all]    = mHitName[keyword_seq[num_cur_hit]];
			oprs[num_cur_all]     = eAosOpr_an;
			values[num_cur_all]   = "";
			orders[num_cur_all]   = false;
			reverses[num_cur_all] = false;
			num_cur_hit ++;
			num_cur_all ++;
		}
	}
	
	bool reverse = true;
	orders[0] = true;
	if(rand()%2 == 1)reverse = false;
	reverses[0] = reverse;

	// expect result
	set<u64> expect [10];
//	set<u64> expect2;
//	set<u64> expect3;
//	set<u64> expect4;

	map<u64,AosTesterQueryUnitPtr> *ordermap;
	if(types[0] == AosSengAdmin::eQueryHit)
	{
		ordermap = &(mHitDocs[keyword_seq[0]]);
	}
	else if(types[0] == AosSengAdmin::eQueryStr)
	{
		ordermap = &(mStrDocs[str_seq[0]]);
	}
	else//QueryU64
	{
		ordermap = &(mU64Docs[u64_seq[0]]);
	}
	
//	map<u64,AosTesterQueryUnitPtr> &curhit = mHitDocs[keyword_seq[0]];

	map<u64,AosTesterQueryUnitPtr>::iterator itr;
	map<u64,AosTesterQueryUnitPtr>::iterator itr2;
	for(itr = ordermap->begin();itr!= ordermap->end();itr++)
	{
		bool exist = true;
		for(int i = 0;i < num_keywords;i++)
		{
			if(mHitDocs[keyword_seq[i]].find(itr->second->mDocid)== mHitDocs[keyword_seq[i]].end())
			{
				exist = false;
				break;
			}
		}
		if(!exist)continue;
		for(int i = 0;i < num_str;i++)
		{
			int seq = str_seq[i];
			itr2 = mStrDocs[seq].find(itr->second->mDocid);
			if(itr2 == mStrDocs[seq].end())
			{
				exist = false;
				break;
			}
			if(!valueMatch(itr2->second->mStrValue.data(), str_opr[i],str_value[i].data()))
			{
				exist = false;
				break;
			}
		}
		if(!exist)continue;
		for(int i = 0;i < num_u64;i++)
		{
			int seq = u64_seq[i];
			itr2 = mU64Docs[seq].find(itr->second->mDocid);
			if(itr2 == mU64Docs[seq].end())
			{
				exist = false;
				break;
			}
			if(!valueMatch(itr2->second->mU64Value, u64_opr[i],u64_value[i]))
			{
				exist = false;
				break;
			}
		}
		if(exist)
		{
cout << endl;
cout << "check doc " << 		itr->second->mDocid ;
cout << " ok";		
			for(int i=0;i < 10;i++)
			{	
				expect[i].insert(itr->second->mDocid);
//				expect2.insert(itr->second->mDocid);
//				expect3.insert(itr->second->mDocid);
//				expect4.insert(itr->second->mDocid);
			}
		}
	}	
if(values[0] == "QhY55V2xQ4JaJ")
{
	OmnMark;
}		
	bool rslt = query(
		num_all,
		types,
		attrs,
		oprs,
		values,
		orders,
		reverses,
		ordermap,
		expect[0],
		false);
		
	if(!rslt)
	{
		OmnSleep(5);
		rslt = query(num_all,
						types,
						attrs,
						oprs,
						values,
						orders,
						reverses,
						ordermap,
						expect[1],
						false);

	}

	if(!rslt)
	{
		OmnSleep(5);
		rslt = query(num_all,
						types,
						attrs,
						oprs,
						values,
						orders,
						reverses,
						ordermap,
						expect[2],
						true);
	}

	if(!rslt)
	{
		// for error checking
		OmnSleep(30);
		rslt = query(num_all,
						types,
						attrs,
						oprs,
						values,
						orders,
						reverses,
						ordermap,
						expect[3],
						true);
	}

	if(!rslt)
	{
		// for error checking
		OmnSleep(30);
		rslt = query(num_all,
						types,
						attrs,
						oprs,
						values,
						orders,
						reverses,
						ordermap,
						expect[4],
						true);
	}

	if(!rslt)
	{
		// for error checking
		OmnSleep(30);
		rslt = query(num_all,
						types,
						attrs,
						oprs,
						values,
						orders,
						reverses,
						ordermap,
						expect[5],
						true);
	}

	if(!rslt)
	{
		// for error checking
		OmnSleep(30);
		rslt = query(num_all,
						types,
						attrs,
						oprs,
						values,
						orders,
						reverses,
						ordermap,
						expect[6],
						true);
	}

	if(types)
	{
		delete []types;
		types = 0;
	}
	if(attrs)
	{
		delete []attrs;
		attrs = 0;
	}
	if(oprs)
	{
		delete []oprs;
		oprs = 0;
	}
	if(values)
	{
		delete []values;
		values = 0;
	}
	if(orders)
	{
		delete []orders;
		orders = 0;
	}
	if(reverses)
	{
		delete []reverses;
		reverses = 0;
	}
	return true;
}

bool		
AosTesterQuery::addDoc()
{
	//  <node aaa__d="123" bbb="abc"> <text>ccc dddd </text> </node>
	bool 		addHit[eNumWords];
	bool 		addU64[eNumWords];
	bool 		addStr[eNumWords];
	OmnString 	strValue[eNumWords];
	u64 		u64Value[eNumWords];
	
	OmnString doc_str = "<node zky_otype=\"test\" ";
	for(int i = 0;i < eNumWords;i ++)
	{
		if(PERCENT(eU64Rate))
		{
			u64Value[i] = OmnRandom::nextU64();
			doc_str << mU64Name[i] << "=\"" << u64Value[i] << "\" ";
			addU64[i] = true;
		}
		else
		{
			addU64[i] = false;
		}
	}
	
	for(int i = 0;i < eNumWords;i ++)
	{
		if(PERCENT(eStrRate)) 
		{
			strValue[i] = OmnRandom::nextLetterDigitStr(1,50);
			doc_str << mStrName[i] << "=\"" << strValue[i] << "\" ";
			addStr[i] = true;
		}
		else
		{
			addStr[i] = false;
		}
	}
	
	doc_str << " > <text> ";

	for(int i = 0;i < eNumWords;i ++)
	{
		if(PERCENT(eHitRate))
		{
			doc_str << mHitName[i] << " ";
			addHit[i] = true;
		}
		else
		{
			addHit[i] = false;
		}
	}
	
	doc_str << "</text></node>";
	
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(doc_str, "" AosMemoryCheckerArgs);
cout << doc_str.data() << endl;
	aos_assert_r(root,false);
	AosXmlTagPtr resp = AosSengAdmin::getSelf()->createDoc(
//		mSiteId,
		root,
		mSsid,
		mUrldocid,
		"",
		"false",
		"",
		mRundata);
	if(!resp)
	{
		OmnSleep(100);
	}
	aos_assert_r(resp,false);
	u64 docid = resp->getAttrU64(AOSTAG_DOCID, 0);
	cout << "docid:" << docid << endl;
	mAllDocs.insert(docid);
	
	mNumDocs++;
	AosTesterQueryUnitPtr cur_unit;
	for(int i = 0;i < eNumWords;i ++)
	{
		if(addHit[i])
		{
			cur_unit = OmnNew AosTesterQueryUnit();
			cur_unit->mDocid = docid;
			mHitDocs[i].insert(pair<u64,AosTesterQueryUnitPtr>(docid,cur_unit));
		}
		if(addU64[i])
		{
			cur_unit = OmnNew AosTesterQueryUnit();
			cur_unit->mDocid = docid;
			cur_unit->mU64Value = u64Value[i];
			mU64Docs[i].insert(pair<u64,AosTesterQueryUnitPtr>(docid,cur_unit));
		}
		if(addStr[i])
		{
			cur_unit = OmnNew AosTesterQueryUnit();
			cur_unit->mDocid = docid;
			cur_unit->mStrValue = strValue[i];
			mStrDocs[i].insert(pair<u64,AosTesterQueryUnitPtr>(docid,cur_unit));
		}
	}
	
	cout << "hit count:";
	for(int i = 0;i < eNumWords;i ++)
	{
		cout << mHitDocs[i].size() << "   ";
	}
	cout << endl;
	return true;
}

bool		
AosTesterQuery::removeDoc()
{
	int doc_idx = rand()%mNumDocs;
	set<u64>::iterator itr = mAllDocs.begin();
	for(int i = 0;i < doc_idx;i++)itr++;
	u64 docid = *itr;

	cout << "docid:" << docid << endl;

	mAllDocs.erase(docid);
	mNumDocs--;
	
	for(int i = 0;i < eNumWords;i ++)
	{
		mStrDocs[i].erase(docid);
		mU64Docs[i].erase(docid);
		mHitDocs[i].erase(docid);
	}

/*	
bool
AosSengAdmin::sendDeleteReq(
			const OmnString &siteid,
		    const OmnString &ssid,
		    const OmnString &docid)
*/
	OmnString docid_str;
	docid_str << docid;
	AosSengAdmin::getSelf()->sendDeleteReq(
		mSiteId,
		mSsid,
		mUrldocid,
		docid_str);
	
	return true;
}

bool		
AosTesterQuery::modifyDoc()
{
	int doc_idx = rand()%mNumDocs;
	set<u64>::iterator itr = mAllDocs.begin();
	for(int i = 0;i < doc_idx;i++)itr++;
	u64 docid = *itr;

	//  <node aaa__d="123" bbb="abc"> <text>ccc dddd </text> </node>
	bool addHit[eNumWords];
	bool addU64[eNumWords];
	bool addStr[eNumWords];
	
	
	OmnString doc_str = "<node zky_otype=\"test\" zky_docid=\"";
	doc_str << docid << "\" zky_objid=\"doc_"<<docid << "\" ";
	
	for(int i = 0;i < eNumWords;i ++)
	{
		if(PERCENT(eU64Rate))
		{
			doc_str << mU64Name[i] << "=\"" << 123 << "\" ";
			addU64[i] = true;
		}
		else
		{
			addU64[i] = false;
		}
	}
	
	for(int i = 0;i < eNumWords;i ++)
	{
		if(PERCENT(eStrRate))
		{
			doc_str << mStrName[i] << "=\"" << "abc" << "\" ";
			addStr[i] = true;
		}
		else
		{
			addU64[i] = false;
		}
	}
	
	doc_str << " > <text> ";

	for(int i = 0;i < eNumWords;i ++)
	{
		if(PERCENT(eHitRate))
		{
			doc_str << mHitName[i] << " ";
			addHit[i] = true;
		}
		else
		{
			addU64[i] = false;
		}
	}
	
	doc_str << "</text></node>";
	
cout << "modify : " << doc_str.data() << endl;
//	AosXmlParser parser;
//	AosXmlTagPtr root = parser.parse(doc_str, "" AosMemoryCheckerArgs);
//	aos_assert_r(root,false);
	
	// update
	//bool sendModifyReq(
	//	const OmnString &siteid, 
	//	const OmnString &ssid, 
	//	const OmnString &docstr, 
	//	const AosRundataPtr &rdata,
	//	const bool check_error = false);

	AosSengAdmin::getSelf()->sendModifyReq(
		mSiteId,
		mSsid,
		mUrldocid,
		doc_str,
		mRundata);
	
	AosTesterQueryUnitPtr cur_unit;

	for(int i = 0;i < eNumWords;i ++)
	{
		mStrDocs[i].erase(docid);
		mU64Docs[i].erase(docid);
		mHitDocs[i].erase(docid);
		if(addStr[i])
		{
			cur_unit = OmnNew AosTesterQueryUnit();
			cur_unit->mDocid = docid;
			mStrDocs[i].insert(pair<u64,AosTesterQueryUnitPtr>(docid,cur_unit));
		}
		if(addU64[i])
		{
			cur_unit = OmnNew AosTesterQueryUnit();
			cur_unit->mDocid = docid;
			mU64Docs[i].insert(pair<u64,AosTesterQueryUnitPtr>(docid,cur_unit));
		}
		if(addHit[i])
		{
			cur_unit = OmnNew AosTesterQueryUnit();
			cur_unit->mDocid = docid;
			mHitDocs[i].insert(pair<u64,AosTesterQueryUnitPtr>(docid,cur_unit));
		}
	}
	
	return true;
}


bool		
AosTesterQuery::query(int num_all,
				AosSengAdmin::QueryType *types,
				OmnString *attrs,
				AosOpr *oprs,
				OmnString *values,
				bool *orders,
				bool *reverses,
				map<u64,AosTesterQueryUnitPtr> *ordermap,
				set<u64> &expect,
				bool error_check)
{
	int startidx = 0;
	u64 this_docid = 0;
	u64 last_docid = 0;
	u64 this_u64 = 0;
	u64 last_u64 = 0;
	OmnString this_str;
	OmnString last_str;
	int queryid = 0;

	bool finished = false;
	AosXmlTagPtr resp;

	map<u64,AosTesterQueryUnitPtr>::iterator itr3;

	while(!finished)
	{		
		resp = AosSengAdmin::getSelf()->queryDocsByAttrs(
			mSiteId,
			num_all,
			types,
			attrs,
			oprs,
			values,
			orders,
			reverses,
			50,
			false,
			startidx,
			queryid);
		aos_assert_r(resp,false);
		
		
		// while loop get all the docids
		AosXmlTagPtr xContents = resp->getFirstChild("Contents");
		queryid = xContents->getAttrU64("queryid",0);
		startidx = xContents->getAttrU64("crt_idx",0);
		AosXmlTagPtr xRecord = xContents->getFirstChild("record");
		if(!xRecord)
		{
			finished = true;
		}
		while(xRecord)
		{
			this_docid = xRecord->getAttrU64("zky_docid",0);
			cout << this_docid << endl;
			
			// check the docid
			// 2. check whether it should be in the result
			
			if(mAllDocs.find(this_docid) != mAllDocs.end())
			{
				if(expect.find(this_docid) == expect.end())
				{
					// in the real result but not in the expect
					if(error_check)
					{
						OmnAlarm << "docid " << this_docid << "not in the expect" << enderr;
					}
					else
					{
						cout << "docid " << this_docid << "not in the expect" << endl;
					}
						
					return false;
				}
				expect.erase(this_docid);
				cout << "expect size " << expect.size() << endl;
			}
			else
			{
				// this doc is not belong to this thread
				// just let it go
				xRecord = xContents->getNextChild("record");
				continue;
			}

			// 2. check the order
			
			// case 1: the order cond is str
			if(types[0] == AosSengAdmin::eQueryStr)
			{
				itr3 = ordermap->find(this_docid);
				if(itr3 == ordermap->end())
				{
					// not in the order list
					if(error_check)
					{
						OmnAlarm << "This docid is not in the order cond list" 
							<< this_docid << enderr;
					}
					else
					{
						cout << "This docid is not in the order cond list" 
							<< this_docid << endl;
					}
					
					return false;
				}
				this_str = itr3->second->mStrValue;
				AosOpr opr = eAosOpr_ge;
				if(last_docid && !valueMatch(this_str.data(),opr,last_str))
				{
					if(error_check)
					{
						OmnAlarm << "The order of the docid is wrong \n docid " 
							<< this_docid << ":" << this_str
							<< " should be large then " 
							<< last_docid << ":" << last_str 
							<< enderr;						
					}
					else
					{
						OmnCout << "The order of the docid is wrong \n docid " 
							<< this_docid << ":" << this_str.data()
							<< " should be large then " 
							<< last_docid << ":" << last_str.data() 
							<< endl;						
					}
					return false;
				}
				last_str = this_str;
			}
			
			// case 2: the order cond is u64
			if(types[0] == AosSengAdmin::eQueryU64)
			{
				itr3 = ordermap->find(this_docid);
				if(itr3 == ordermap->end())
				{
					// not in the order list
					if(error_check)
					{
						OmnAlarm << "This docid is not in the order cond list" 
							<< this_docid << enderr;
					}
					else
					{
						cout << "This docid is not in the order cond list" 
							<< this_docid << endl;
					}
					return false;
				}
				this_u64 = itr3->second->mU64Value;
				if(last_docid && (this_u64 < last_u64))
				{
					if(error_check)
					{
						OmnAlarm << "The order of the docid is wrong \n docid " 
							<< this_docid << ":" << this_str
							<< " should be large then " 
							<< last_docid << ":" << last_str 
							<< enderr;						
					}
					else
					{
						cout << "The order of the docid is wrong \n docid " 
							<< this_docid << ":" << this_str.data()
							<< " should be large then " 
							<< last_docid << ":" << last_str.data()
							<< endl;						
					}
					return false;
				}
				last_u64 = this_u64;
			}
			// case 3: the order cond is hit
			if(types[0] == AosSengAdmin::eQueryHit)
			{
				if(last_docid && last_docid > this_docid)
				{
					if(error_check)
					{
						OmnAlarm << "The order of the docid is wrong \n docid " 
							<< this_docid << "should be large then " << last_docid << enderr;				
					}
					else
					{
						cout << "The order of the docid is wrong \n docid " 
							<< this_docid << "should be large then " << last_docid << endl;				
					}
					return false;
				}
			}
			last_docid = this_docid;
			xRecord = xContents->getNextChild("record");
		}
		//finished = 
	}	
	
	// check if there is any docid remained in the expect
	if(expect.size() != 0)
	{
		if(error_check)
		{
			OmnAlarm << "some docids are not found" << enderr;
		}
		set<u64>::iterator expect_itr;
		for(expect_itr = expect.begin();expect_itr!= expect.end();expect_itr++)
		{
			cout << *expect_itr << endl;
		}
		if(error_check)
		{
			OmnAlarm << "some docids are not found" << enderr;
		}
		else
		{
			cout << "some docids are not found" << endl;
		}
		
		return false;
	}
	return true;
}
#endif
