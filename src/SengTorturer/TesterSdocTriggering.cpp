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
//
// Modification History:
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/TesterSdocTriggering.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "SengTorturer/TesterLogin.h"
#include "SengTorturer/SengTesterXmlDoc.h"
#include "SengTorturer/SengTesterMgr.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorturer/SengTesterThrd.h"
#include "SengTorturer/SengTesterFileMgr.h"
#include "SengTorturer/StUtil.h"
#include "SengTorturer/XmlTagRand.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"
#include "Util/UtUtil.h"
#include "Rundata/Rundata.h"

u64 	AosSdocTriggeringTester::mNum = 0;
bool	AosSdocTriggeringTester::mSaveSdocFlag = false;
AosSdocTriggeringTester::AosSdocTriggeringTester(
		const bool regflag)
:
AosSengTester(AosSengTester_SdocTriggering, AosTesterId::eSdocTriggering, regflag)
{
	mLock = OmnNew OmnMutex();
}


AosSdocTriggeringTester::AosSdocTriggeringTester()
:
AosSengTester(AosSengTester_SdocTriggering, "sdoc_triggering", AosTesterId::eSdocTriggering)
{
	mLock = OmnNew OmnMutex();
}


AosSdocTriggeringTester::~AosSdocTriggeringTester()
{
}


bool 
AosSdocTriggeringTester::test()
{
	clear();
	AosRundataPtr rdata  = mThread->getRundata();
	OmnScreen << "ThreadId:" << mThreadId<< endl;
	if (!mThread->checkSaveSdocFlag())
	{
		for (int i=1; i<=6; i++)
		{
			OmnString fname;
			fname <<i;
			generateSmartdoc(fname);
		}
		mSaveSdocFlag = true;
	}
	//testModifyAttr(rdata);
	mType = rand()%eEVENT_MAX;
	//mType = eEVENT_CTNR_ADDMEMBER;
	switch(mType)
	{
		case eEVENT_CTNR_ADDMEMBER:
			 OmnScreen << "TestCreate!" << endl;
			 mLock->lock();
			 modifyConfingDoc(rdata);
			 testCreate(rdata);
			 mLock->unlock();
			 break;
		case eEVENT_CTNR_DELMEMBER:
			 OmnScreen << "TestDelete!"<< endl;
			 mLock->lock();
			 modifyConfingDoc(rdata);
			 testDelete(rdata);
			 mLock->unlock();
			 break;
		case eEVENT_CTNR_MODMEMBER:
			 OmnScreen << "TestModify!" <<endl;
			 mLock->lock();
			 modifyConfingDoc(rdata);
			 testModify(rdata);
			 mLock->unlock();
			 break;
		case eEVENT_CTNR_READMEMBER:
			 OmnScreen << "TestRead!" << endl;
			 mLock->lock();
			 modifyConfingDoc(rdata);
			 testRead(rdata);
			 mLock->unlock();
			 break;
		default:
			 OmnAlarm << "Unrecognized event: " << mType << enderr;
	}
	return true;
}

bool
AosSdocTriggeringTester::testRead(const AosRundataPtr &rdata)
{
	OmnString objid = pickDoc();
	if (mFlag) return true;
	AosXmlTagPtr doc = mThread->retrieveDocByObjid(objid);
	aos_assert_r(doc, false);
	OmnString pctr_objid = doc->getAttrStr(AOSTAG_PARENTC, "");
	aos_assert_r(pctr_objid != "", false);
	const char *data = pctr_objid.data();
	u64 ee = data[0] - '0';
	if (ee >= 10) return true;
	aos_assert_r(retrieveCtnrDoc(pctr_objid), false);
	aos_assert_r(ModifyCtnrEvent(rdata), false);
	AosXmlTagPtr docxml = mThread->retrieveDocByObjid(objid);
	aos_assert_r(mOldCtnr, false);
	bool rslt = mThread->modifyObj(mOldCtnr, rdata);
	aos_assert_r(rslt, false);
	checkResults();
	return true;
}

bool
AosSdocTriggeringTester::testModify(const AosRundataPtr &rdata)
{
	OmnString objid = pickDoc();
	if (mFlag) return true;
	AosXmlTagPtr doc = mThread->retrieveDocByObjid(objid);
	aos_assert_r(doc ,false);
	aos_assert_r(doc->getAttrStr(AOSTAG_OBJID, "") == objid, false);
	OmnString pctr_objid = doc->getAttrStr(AOSTAG_PARENTC, "");
	aos_assert_r(retrieveCtnrDoc(pctr_objid), false);
	aos_assert_r(ModifyCtnrEvent(rdata), false);

	int len = (rand() % 10) + 5;
	char data[40];
	AosRandomLetterStr(len, data);
	OmnString attr = data;
	aos_assert_r(attr != "", false);
	doc->setAttr(attr, rand());

	bool rslt = mThread->modifyObj(doc, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(doc, false);

	aos_assert_r(mOldCtnr, false);
	bool rslt1 = mThread->modifyObj(mOldCtnr, rdata);
	aos_assert_r(rslt1, false);

	checkResults();
//	u64 local_docid = doc->getAttrU64(AOSTEST_DOCID, 0);
//	aos_assert_r(local_docid, false);
//	aos_assert_r(mThread->modifyDocToList(local_docid, doc), false);
	return true;
}

bool
AosSdocTriggeringTester::testCreate(const AosRundataPtr &rdata)
{
	aos_assert_r(pickContainer(), false);
	aos_assert_r(ModifyCtnrEvent(rdata), false);

	OmnString ctnr_objid = mCtnr->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(ctnr_objid != "", false);
	OmnString prefix, cc;
	AosObjid::decomposeObjid(ctnr_objid, prefix, cc);
	OmnString parent_is_public = "false";
	if (cc == "") parent_is_public ="true";

	OmnString objid = mThread->pickNewObjid();
	u64 local_docid = mThread->getNewDocId();
	AosXmlTagPtr doc = mThread->createDoc();
	aos_assert_r(doc, false);
	doc->setAttr(AOSTAG_OBJID, objid);
	doc->setAttr(AOSTAG_CTNR_PUBLIC, parent_is_public);
	doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
	doc->setAttr(AOSTEST_DOCID, local_docid);
	doc->setAttr(AOSTAG_PARENTC, ctnr_objid);
	AosXmlTagPtr resp;
	resp = AosSengAdmin::getSelf()->createDoc(
			    doc, 
				mThread->getCrtSsid(), 
				mThread->getUrlDocid(), 
				"", 
				"false", 
				"false",
				mThread->getRundata());
	aos_assert_r(resp, false);
	aos_assert_r(resp->getAttrStr(AOSTAG_OBJID, "") == objid, false);
	AosXmlTagPtr serverdoc = mThread->retrieveDocByObjid(objid);
	aos_assert_r(serverdoc, false);
	aos_assert_r(local_docid == serverdoc->getAttrU64(AOSTEST_DOCID, 0), false);
OmnScreen << "to add doc: " << local_docid << endl;
	aos_assert_r(mThread->addDoc1(local_docid, serverdoc), false);

	aos_assert_r(mOldCtnr, false);
	bool rslt = mThread->modifyObj(mOldCtnr, rdata);
	aos_assert_r(rslt, false);

	checkResults();
	return false;
	
}

bool
AosSdocTriggeringTester::testDelete(const AosRundataPtr &rdata)
{
	OmnString objid = pickDoc();
	if (mFlag) return true;
	AosXmlTagPtr doc = mThread->retrieveDocByObjid(objid);
	aos_assert_r(doc ,false);
	OmnString pctr_objid = doc->getAttrStr(AOSTAG_PARENTC, "");
	aos_assert_r(retrieveCtnrDoc(pctr_objid), false);
	aos_assert_r(ModifyCtnrEvent(rdata), false);
	u64 serverdocid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(serverdocid, false);
	aos_assert_r(objid == doc->getAttrStr(AOSTAG_OBJID, ""), false);
	bool rslt = mThread->removeDocFromServer(doc);
	aos_assert_r(rslt, false);

	aos_assert_r(mOldCtnr, false);
	bool rslt1 = mThread->modifyObj(mOldCtnr, rdata);
	aos_assert_r(rslt1, false);
	checkResults();
	
	aos_assert_r(doc, false);
	u64 local_docid = doc->getAttrU64(AOSTEST_DOCID, 0); 
	aos_assert_r(local_docid, false);
	aos_assert_r(mThread->removeDocToFile(local_docid), false);                
	aos_assert_r(mThread->isDeleteToFile(local_docid),false);
	aos_assert_r(mThread->removeDoc(local_docid, objid, serverdocid), false);
	//aos_assert_r(!mThread->pickDoc(local_docid), false);
	aos_assert_r(!mThread->getDocByObjid(objid), false);
	OmnString did;
	did << serverdocid;
	aos_assert_r(mThread->getServeridByLocalid(did) ==0, false);
	return true;
}

bool
AosSdocTriggeringTester::generateSmartdoc(const OmnString &fname)
{
	OmnString filename;
	filename <<"Smartdoc";
	filename<< "/" << fname << ".txt"; 
	OmnFilePtr file = OmnNew OmnFile(filename.data(), OmnFile::eReadOnly);
	aos_assert_r(file->isGood(), false);
	OmnString contents;
	file->readToString(contents);
	//file->closeFile();
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(contents, "" AosMemoryCheckerArgs);
	if (doc->isRootTag()) doc = doc->getFirstChild();
	OmnString ee;
	ee << mThreadId << doc->getAttrStr(AOSTAG_OBJID, "");
	doc->setAttr(AOSTAG_OBJID, ee);
	modifysdoc_modifyattr(doc);
	AosXmlTagPtr resp = AosSengAdmin::getSelf()->createDoc(
			    			doc, 
							mThread->getCrtSsid(), 
							mThread->getUrlDocid(), 
							"", 
							"false", 
							"false",
							mThread->getRundata());
	aos_assert_r(resp, false);
//	if (doc->isRoot()) doc = doc->getFirstChild(); 
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(objid != "", false);
	aos_assert_r(objid == resp->getAttrStr(AOSTAG_OBJID, ""), false);
	//if (objid == "sdoc_modifyattr1" || objid =="sdoc_modifyattr2" || objid=="sdoc_modifyattr3" || objid =="sdoc_modifyattr4")	
	OmnString a;
	a << mThreadId << "sdoc_modifyattr1";
	OmnString b;
	b << mThreadId << "sdoc_modifyattr2";
	OmnString c;
	c << mThreadId << "sdoc_modifyattr3";
	OmnString d;
	d << mThreadId << "sdoc_modifyattr4";
	if (objid == a|| objid == b || objid == c||objid == d)
	{
		mSdocObjids.push_back(objid);
	}
	// save Objid
	return true;
}

bool
AosSdocTriggeringTester::modifysdoc_modifyattr(AosXmlTagPtr &doc)
{
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(objid!="", false);
	OmnString a;
	a << mThreadId << "sdoc_modifyattr1";
	OmnString b;
	b << mThreadId << "sdoc_modifyattr2";
	OmnString c;
	c << mThreadId << "sdoc_modifyattr3";
	OmnString d;
	d << mThreadId << "sdoc_modifyattr4";
	if (objid == a|| objid == b || objid == c||objid == d)
	{
		AosXmlTagPtr xml = doc->getFirstChild("actions");
		aos_assert_r(xml, false);
		xml = xml->getFirstChild("action");
		aos_assert_r(xml, false);
		AosXmlTagPtr docselector = xml->getFirstChild("zky_docselector");
		aos_assert_r(docselector, false);
		OmnString dd ;
		dd << mThreadId << docselector->getAttrStr("zky_doc_objid", "");
		docselector ->setAttr("zky_doc_objid", dd); 

		AosXmlTagPtr valuedef = xml->getFirstChild("zky_valuedef");
		aos_assert_r(valuedef, false);
		docselector = valuedef->getFirstChild("zky_docselector");
		aos_assert_r(docselector, false);
		OmnString ddd ;
		ddd << mThreadId << docselector->getAttrStr("zky_doc_objid", "");
		docselector ->setAttr("zky_doc_objid", ddd);
	}
	return true;	
}

bool
AosSdocTriggeringTester::ModifyCtnrEvent(const AosRundataPtr &rdata)
{
	OmnString text;
	OmnString text1;
	mSobjid = "";
	int i = rand()%2;
	if(i == 0)
	{
		text = pickSdocObjid();
		text1 = pickSdocObjid();
		mSobjid << text << "," << text1;
	}
	else
	{
		text = pickSdocObjid();
		mSobjid = text;

	}
	if (text == text1)
		OmnMark;
	aos_assert_r(text != "", false);
	aos_assert_r(mCtnr, false);
	mOldCtnr = mCtnr->clone(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr eventtag = mCtnr->getFirstChild("Event");
	if (!eventtag)
	{
		mCtnr->addNode1("Event");
	}
	
	OmnString eventstr = toEnum(mType);
	if (!eventtag) eventtag =  mCtnr->getFirstChild("Event");
	aos_assert_r(eventtag, false);
	AosXmlTagPtr subtag = eventtag->getFirstChild(eventstr);
	if (!subtag)
	{
		eventtag->addNode1(eventstr);
		AosXmlTagPtr tag = eventtag->getFirstChild(eventstr); 
		aos_assert_r(tag, false);
		tag->setNodeText("OBJID", text, true);
		if (text1 != "")
		{
			OmnString value = "<OBJID><![CDATA[";
			value << text1 << "]]></OBJID>";
			tag->addNode1("OBJID", value); 
		}
	}
	else
	{
		subtag->setNodeText("OBJID", text, true);
		if (text1 !="")
		{
			OmnString value = "<OBJID><![CDATA[";
			value << text1 << "]]></OBJID>";
			subtag->addNode1("OBJID", value); 
		}
	}
	bool rslt = mThread->modifyObj(mCtnr, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosSdocTriggeringTester::checkResults()
{

	OmnStrParser1 parser(mSobjid, ",");
	OmnString word;                        
	while((word = parser.nextWord())!= "")
	{
		AosXmlTagPtr sdoc = mThread->retrieveDocByObjid(word);	
		aos_assert_r(sdoc, false);
		OmnString objid;
		objid << mThreadId << "modify_objid";
		AosXmlTagPtr checkdoc = mThread->retrieveDocByObjid(objid);
		aos_assert_r(checkdoc, false);
		OmnString objid1;
		objid1<<mThreadId <<"config_objid";
		AosXmlTagPtr configdoc = mThread->retrieveDocByObjid(objid1);
		aos_assert_r(configdoc, false);

		OmnString attrname = sdoc->getAttrStr("zky_xpath"); 
		aos_assert_r(attrname !="", false);

		OmnString okvalue = configdoc->getAttrStr(attrname);
		OmnString value = checkdoc->getAttrStr(attrname);
		aos_assert_r(okvalue == value, false);
	}
	return true;
}

bool 
AosSdocTriggeringTester::pickContainer()
{
	AosStContainerPtr ctnr = mThread->pickContainer();
	OmnString objid = ctnr->mObjid;
	aos_assert_r(objid!="", false);
	mCtnr = mThread->retrieveDocByObjid(objid);
	aos_assert_r(mCtnr,false);
	return true;
}

bool
AosSdocTriggeringTester::retrieveCtnrDoc(const OmnString &objid)
{
	aos_assert_r(objid != "", false);
	OmnStrParser1 parser(objid, ", ");
	OmnString word;
	word = parser.nextWord();
	aos_assert_r(word != "", false);
	mCtnr = mThread->retrieveDocByObjid(word);
	aos_assert_r(mCtnr,false);
	return true;
}

OmnString
AosSdocTriggeringTester::pickDoc()
{
	int i;
	mFlag = false;
	OmnString objid;
	u64 serverdocid;
	int total = mThread->getStDocSize();
	for(i = 1; i<total; i++)
	{
		AosStDocPtr doc = mThread->pickDoc();
		if (doc)
		{
			objid = doc->objid;
			u64 localDocid = doc->locdid;
			serverdocid = doc->svrdid;

			AosStContainerPtr container = mThread->getContainer(localDocid);
			AosStUser *user = mThread->getUser(localDocid);
			AosStAccessRecord *ar = mThread->getLocalAccessRecord(localDocid);
			if ((!container) && (!user) && (!ar))
			{	
				//Not allow delete container and user
				break;
			}
		}
	}
	
	if (i >= total) 
	{	
		mFlag = true;
		return "";
	}
	AosXmlTagPtr doc1 = mThread->retrieveDoc(serverdocid);
	aos_assert_r(doc1, "");
	AosXmlTagPtr doc = mThread->retrieveDocByObjid(objid);
	aos_assert_r(doc, "");
	return objid;
}

OmnString 
AosSdocTriggeringTester::pickSdocObjid()
{
	if (mSdocObjids.size() == 0) return "";
	int i = rand()%(mSdocObjids.size());
	return mSdocObjids[i];
}

u64
AosSdocTriggeringTester::getNum()
{
	return mNum++;
}

bool
AosSdocTriggeringTester::modifyConfingDoc(const AosRundataPtr &rdata)
{
	//"config_objid" value Random Variable;
	OmnString objid1;
	objid1<<mThreadId <<"config_objid";
	AosXmlTagPtr configdoc = mThread->retrieveDocByObjid(objid1);
	aos_assert_r(configdoc, false);
	OmnString value;
	u64 num = getNum();
	aos_assert_r(num <100000, false);
	value<<"a" << num;
	configdoc->setAttr("zky_a", value); 

	value = "";
	num = getNum();
	aos_assert_r(num <100000, false);
	value<<"b" << num;
	configdoc->setAttr("zky_b", value); 

	value = "";
	num = getNum();
	aos_assert_r(num <100000, false);
	value<<"c" << num;
	configdoc->setAttr("zky_c", value); 

	value = "";
	num = getNum();
	aos_assert_r(num <100000, false);
	value<<"d" << num;
	configdoc->setAttr("zky_d", value);

	bool rslt = mThread->modifyObj(configdoc, rdata);
	aos_assert_r(rslt, false);
	return true;
}
bool
AosSdocTriggeringTester::clear()
{
	mCtnr = 0;
	mOldCtnr = 0;
	mType = 100000;
	mSobjid = "";
	mFlag = false;
	return true;
}

bool
AosSdocTriggeringTester::testModifyAttr(const AosRundataPtr &rdata)
{
	//modify "modift_objid" doc ;
	//modify pctrsctnr doc;
	OmnString objid;
	objid << mThreadId << "modify_objid";
	AosXmlTagPtr doc = mThread->retrieveDocByObjid(objid);
	aos_assert_r(doc, false);
	OmnString pctr_objid = doc->getAttrStr(AOSTAG_PARENTC, "");
	aos_assert_r(pctr_objid != "", false);
	aos_assert_r(retrieveCtnrDoc(pctr_objid), false);
	mType = 4;
	aos_assert_r(ModifyCtnrEvent(rdata), false);
	clear();
	return true;
}
