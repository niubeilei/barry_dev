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
#include "SengTorturer/TesterModifyObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
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


AosModifyObjTester::AosModifyObjTester(const bool regflag)
:
AosSengTester(AosSengTester_ModifyObj, AosTesterId::eModifyObj, regflag),
mUseKnownCtnrWeight(50),
mUseDuplicatedCtnrWeight(50),
mUsePubCtnrWeight(50),
mPrivateDocWeight(50),
mUseUniqueObjidWeight(50),
mCreateDocWeight(50),
mWithCidWeight(50),
mCanModify(false)
{
}


AosModifyObjTester::AosModifyObjTester()
:
AosSengTester(AosSengTester_ModifyObj, "mod_obj", AosTesterId::eModifyObj),
mUseKnownCtnrWeight(50),
mUseDuplicatedCtnrWeight(50),
mUsePubCtnrWeight(50),
mPrivateDocWeight(50),
mUseUniqueObjidWeight(50),
mCreateDocWeight(50),
mWithCidWeight(50),
mCanModify(false)
{
}


AosModifyObjTester::~AosModifyObjTester()
{
}


bool 
AosModifyObjTester::test()
{
	AosRundataPtr rdata = mThread->getRundata();
	aos_assert_r(pickDoc(), false);
	if (!mRawDoc)
	{
		OmnScreen << "Nothing to modify!" << endl;
		return true;
	}
	aos_assert_r(modifyXml(), false);
	aos_assert_r(modifyDoc(rdata), false);
	aos_assert_r(checkModify(), false);
	aos_assert_r(modifyDocToMemory(), false);
	return true;
}


bool
AosModifyObjTester::pickDoc()
{
	mRawDoc = 0;
	mLocalDocid = 0;
	mServerDocid = 0;	
	mServerDoc = 0;
	int i = 1;
	int total = mThread->getStDocSize();
	while (i++<total)
	{
		AosStDocPtr doc = mThread->pickDoc();
		if (!doc) continue;
		mObjid = doc->objid;
		mLocalDocid = doc->locdid;
		mServerDocid = doc->svrdid;
		AosStContainerPtr container = mThread->getContainer(mLocalDocid);
		AosStUser *user = mThread->getUser(mLocalDocid);
		const AosStAccessRecord *ar = mThread->getLocalAccessRecord(mLocalDocid);
		if (!container && !user && !ar)
		{
			//Not allow delete container and user
			break;
		}
	}

	if (i >= total) return true;
	mRawDoc = mThread->readLocalDocFromFile(mLocalDocid);
	mServerDoc = AosSengAdmin::getSelf()->retrieveDocByDocid(
						mThread->getSiteid(), 
						mThread->getCrtSsid(),
						mThread->getUrlDocid(),
						mServerDocid, 
						false
						);
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mRawDoc, false);
	aos_assert_r(mLocalDocid == mServerDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);
	aos_assert_r(mServerDoc->isDocSame(mRawDoc), false);
	mOld = mServerDoc;
	return true;
}

/*
bool
AosModifyObjTester::pickDoc()
{
	AosStDocPtr doc = mThread->pickDoc();
	if (doc)
	{
		mObjid = doc->objid;
		mLocalDocid = doc->locdid;
		mServerDocid = doc->svrdid;
OmnScreen <<"mLocalDocid: "<< mLocalDocid<< endl;
	}
	mRawDoc = mThread->readDoc(mLocalDocid);
	if (!mRawDoc)
		OmnMark;
	mServerDoc = AosSengAdmin::getSelf()->retrieveDocByDocid(
			mThread->getSiteid(), mThread->getCrtSsid(),mServerDocid, mThread->getUrlDocid());
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mRawDoc, false);
	aos_assert_r(mLocalDocid == mServerDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);
	aos_assert_r(mServerDoc->isDocSame(mRawDoc), false);
	return true;
}
*/

bool
AosModifyObjTester::modifyXml()
{
	if (!mRawDoc) return true;
	AosXmlTagPtr dd = mRawDoc;
	aos_assert_r(mRawDoc, false);
	aos_assert_r(dd, false);
	
	OmnString docstr = dd->toString();
	AosXmlParser parser1;
	AosXmlTagPtr doc = parser1.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r (doc, false);
	mName = "",	mOldname = "", 	mNewname = "", mTagname = "";
	mTag.initSub();
	int i = rand()%100;
	switch (i/10)
	{
	case 0:
		 mTag.modifyAttr(doc, mName, mOldname, mNewname);
		 break;

	case 1:
		 mTag.addAttr(doc, mName, mNewname);
		 break;

	case 2:
		 mTag.deleteAttr(doc, mName);
		 break;

	case 3:
		 mTag.removeTag(doc, mTagname);
		 break;

	case 4:
		 mTag.addTag(doc, mTagname);
		 break;

	case 5:
		 mTag.addTextWords(doc, mName);
		 break;

	case 6:
		 mTag.removeTextWords(doc, mName);
		 break;

	case 7:
		 mTag.addTexts(doc, mTagname, mName);
		 break;

	case 8: 
		 mTag.modifyTexts(doc, mNewname, mOldname);
		 break;

	case 9:
		 mTag.removeTexts(doc);
		 break;
	}

	OmnString data = doc->toString();
	AosXmlParser parser;
	mRawDoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return true;
}

bool
AosModifyObjTester::modifyDoc(const AosRundataPtr &rdata)
{
	if (!mRawDoc) return true;
	aos_assert_r(mRawDoc, false);
	bool rslt = mThread->modifyObj(mRawDoc, false);
	mCanModify = canModify();
	if (!mCanModify)
	{
		aos_assert_r(!rslt, false);
		return true;
	}
	//retrieve by docid
	aos_assert_r(rslt, false);
	mServerDoc = AosSengAdmin::getSelf()->retrieveDocByDocid(
			mThread->getSiteid(), mThread->getCrtSsid(),
			mThread->getUrlDocid(),mServerDocid, false);
	aos_assert_r(mServerDoc, false);
	return true;
}

bool
AosModifyObjTester::canModify()
{
	if (mThread->getCrtSsid() == "") return false;
	//check the access
	if (mThread->isRoot()) return true;
	OmnString cid = mThread->getCrtCid();
	return (mThread->canDoOpr(mServerDoc->getAttrU64(AOSTEST_DOCID, 0), AosSecOpr::eWrite));
}

bool
AosModifyObjTester::checkModify()
{
	if (!mRawDoc) return true;
	if (!mCanModify) return true;
	//retrieve by objid
	AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByObjid(
							mThread->getSiteid(), 
							mThread->getCrtSsid(), 
							mThread->getUrlDocid(),
							mObjid, 
							true 
							);

	aos_assert_r(mServerDoc, false);
	aos_assert_r(doc, false);
	aos_assert_r(doc->getAttrStr(AOSTAG_OBJID) == 
			mServerDoc->getAttrStr(AOSTAG_OBJID), false);
	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID) ==
			mServerDoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID), false);
	aos_assert_r(doc->getAttrU64(AOSTEST_DOCID, AOS_INVDID) ==
			mServerDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);

	aos_assert_r(mServerDoc->isDocSame(mRawDoc), false);
	
//	OmnString local_docid = mServerDoc->getAttrStr(AOSTEST_DOCID);
//	aos_assert_r(local_docid!="", false);
//	aos_assert_r(mThread->modifyDocToFile(atoll(local_docid.data()), mServerDoc), false);
	aos_assert_r(check(), false);
	return true;
}

bool
AosModifyObjTester::check()
{
	int total = mTag.getTotalNumLevel();	
	AosXmlTagPtr doc, xml;

	//if (mTag.getSubLevel(0) == 0 && mTag.getSubTagName(0) == "") doc = mServerDoc->getFirstChild();
	if (mTag.mSubLevel[0] == 0 && mTag.mSubTagName[0] == "")doc = mServerDoc->getFirstChild();	
	else doc = mServerDoc;

	aos_assert_r(doc, false);

	for (int i = 1; i<total; i++)
	{
		//OmnString str = mTag.getSubTagName(i);
		OmnString str = mTag.mSubTagName[i];
		if (str == "")break;

		//<tag1>
		//	<tag2 x="1">
		//	</tag2>
		//	<tag2 y= "22">
		//	</tag2>
		//</tag1>
		//Do not check the same node
		xml = doc->getFirstChild(str);
		doc = doc->getNextChild(str);
		if (xml && doc) 
		{
			return true;
		}
		doc = xml;

		aos_assert_r(doc, false);
	}
	
	OmnString opr = mTag.getCrtOpr();
//OmnScreen << "opr :" << opr << endl;
	if (opr == "addAttr")
	{
		if (mName == "") return true;
		OmnString value = doc->getAttrStr(mName);
		aos_assert_r (value == mNewname, false);
		return true;
	}

	if (opr == "modifyAttr")
	{
		if (mName == "") return true;
//OmnScreen << "modifyAttr :" << mName << ",total :"  << total << endl;
		bool rslt = true;
		OmnString value = doc->getAttrStr(mName);
		if (total !=1)
		{
			aos_assert_r (value == mNewname, false);
			return true;
		}
		if (total == 1) rslt = canAttrModify(mName);
		if (rslt) 
		{
//OmnScreen << doc->getAttrStr(mName) << ":" << mNewname <<endl;
			if (mName == AOSTAG_PARENTC)
			{
				aos_assert_r (value != mOldname, false);
			}
			else
			{
				aos_assert_r (value == mNewname, false);
			}
			return true;
		}
		else 
		{
//OmnScreen << doc->getAttrStr(mName) << ":" << mOldname <<endl;
			if (mName == AOSTAG_COUNTERRD) 
			{
				aos_assert_r(value != mOldname, false);
				return true;
			}
			//if (mName == AOSTAG_MT_EPOCH) 
			//{
			//	aos_assert_r(doc->getAttrStr(mName)!="", false);
			//	return true;
			//}
			aos_assert_r(value == mOldname, false);
			return true;
		}
	}

	if (opr =="deleteAttr")
	{
		if (mName == "") return true;
//OmnScreen << "deleteAttr :" << mName << ",total :"  << total << endl;
		bool rslt = true;
		OmnString value = doc->getAttrStr(mName);
		if (total !=1)
		{
			aos_assert_r (value == "", false);
			return true;
		}
		if (total == 1) rslt = canAttrDelete(mName);
//OmnScreen << doc->getAttrStr(mName) << ":" << mName <<endl;
		if (rslt) 
		{
			if  (mName == AOSTAG_PARENTC || mName == AOSTAG_MODUSER)  
			{
				aos_assert_r(value!="", false);
			}
			else
			{
				aos_assert_r (value == "", false);
			}
		}
		else 
		{
			aos_assert_r(value!="", false);
		}
		return true;
	}
	
	if (opr == "addTag")
	{
		if (mTagname == "") return true;
		AosXmlTagPtr xx= doc->getFirstChild(mTagname);
		aos_assert_r(xx, false);
		return true;
	}

	if (opr == "removeTag")
	{
		if (mTagname == "") return true;
		AosXmlTagPtr xx = doc->getFirstChild(mTagname);
		aos_assert_r(!xx, false);
	}
	
	if (opr == "addTextWords")
	{
		if (mName == "") return true;
		OmnString value = doc->getNodeText();
		aos_assert_r (value == mName, false);
	}
	
	if (opr == "removeTextWords")
	{
		if (mName == "") return true;
		OmnString value = doc->getNodeText();
		aos_assert_r(value == mName, false);
	}
	
	if (opr == "modifyTexts")
	{
		if (mNewname== "") return true;
		OmnString value = doc->getNodeText();
		char c = mNewname.data()[0];
		if (c == ' ') return true;
		aos_assert_r( value == mNewname, false);
		//aos_assert_r(doc->getNodeText() == mNewname, false);
		return true;
	}

	if (opr == "addTexts")
	{
		if (mName =="") return true;
		OmnString value = doc->getNodeText(mTagname);
		aos_assert_r(value == mName, false);
		return true;
	}

	if (opr == "removeTexts")
	{
		OmnString value = doc->getNodeText();
		aos_assert_r(value == "", false);
		return true;
	}
	return true;
}

bool
AosModifyObjTester::modifyDocToMemory()
{
	if (!mRawDoc) return true;
	if (!mCanModify)return true;
	aos_assert_r(mLocalDocid, false);
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mThread->modifyDocToMemory(mLocalDocid, mServerDoc), false);
	return true;
}

bool
AosModifyObjTester::canAttrModify(const OmnString &aname)
{
		// Following attributes cannot be modified
		// AOSTAG_CTIME
		// AOSTAG_CT_EPOCH
		// AOSTAG_MTIME
		// AOSTAG_MT_EPOCH
		// AOSTAG_SITEID
		// AOSTAG_OTYPE
		// AOSTAG_SUBTYPE
		// AOSTAG_COUNTERCM
		// AOSTAG_COUNTERDW
		// AOSTAG_COUNTERLK
		// AOSTAG_COUNTERRC
		// AOSTAG_COUNTERUP
		// AOSTAG_COUNTERRD
		// AOSTAG_VOTEDOWN
		// AOSTAG_VOTEUP
		// AOSTAG_VOTETOTAL
		// AOSTAG_CREATOR
		if (strcmp(aname.data(), AOSTAG_CTIME) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_CT_EPOCH) == 0) return false;
		//if (strcmp(aname, AOSTAG_MT_EPOCH) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_SITEID) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERCM) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERDW) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERLK) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERRC) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERUP) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERRD) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_VOTEDOWN) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_VOTEUP) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_VOTETOTAL) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNERTUP) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_CREATOR) == 0) return false;
		return true;
}

bool
AosModifyObjTester::canAttrDelete(const OmnString &aname)
{
		// Following attributes cannot be modified
		// AOSTAG_CTIME
		// AOSTAG_CT_EPOCH
		// AOSTAG_MTIME
		// AOSTAG_MT_EPOCH
		// AOSTAG_SITEID
		// AOSTAG_OTYPE
		// AOSTAG_SUBTYPE
		// AOSTAG_COUNTERCM
		// AOSTAG_COUNTERDW
		// AOSTAG_COUNTERLK
		// AOSTAG_COUNTERRC
		// AOSTAG_COUNTERUP
		// AOSTAG_COUNTERRD
		// AOSTAG_VOTEDOWN
		// AOSTAG_VOTEUP
		// AOSTAG_VOTETOTAL
		// AOSTAG_CREATOR
		if (strcmp(aname.data(), AOSTAG_MT_EPOCH) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_CTIME) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_CT_EPOCH) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_SITEID) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_OTYPE) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_MODUSER) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_SUBTYPE) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERCM) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERDW) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERLK) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERRC) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERUP) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_COUNTERRD) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_VOTEDOWN) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_VOTEUP) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_VOTETOTAL) == 0) return false;
		if (strcmp(aname.data(), AOSTAG_CREATOR) == 0) return false;
		return true;
}
