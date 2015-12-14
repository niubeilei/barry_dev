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
#include "SengTorturer/TesterAccessCtrl.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include <stdlib.h>
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEModules/SeRundata.h"
#include "SmartDoc/SmartDoc.h"
#include "SengTorturer/TesterReadObj.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SengTorturer/TesterLogin.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"

#define WeightTag 90

const int sgMaxAccess = 7;
const int sgMaxOperation = 9;
static OmnString sgAccess[sgMaxAccess];
static OmnString sgOperation[sgMaxOperation];

AosAccessCtrlTester::AosAccessCtrlTester(const bool reg)
:
AosSengTester(AosSengTester_AccessControl, AosTesterId::eAccessCtrl, reg),
eLoginWhenNeeded(50),
eLoginAsOwner(10),
eLoginAsRoot(50),
eAddAccWeight(25),
eRemoveAccWeight(50),
eModifyAccWeight(75),
eNormalAccWeight(30),
ePicOwnDoc(50)
{
	/*
	sgAccess[0] = AOSACTP_PRIVATE;
	sgAccess[1] = AOSACTP_PUBLIC;
	sgAccess[2] = AOSACTP_LOGIN_PUBLIC;
	sgAccess[3] = "group";
	sgAccess[4] = "role";
	sgAccess[5] = "tpublic";
	sgAccess[6] = "token";

	sgOperation[0] = AOSTAG_READ_ACSTYPE; 		//read
	sgOperation[1] = AOSTAG_DELMEM_ACSTYPE; 	//delete member
	sgOperation[2] = AOSTAG_ADDMEM_ACSTYPE; 	//add member
	sgOperation[3] = AOSTAG_DELETE_ACSTYPE; 	//delete
	sgOperation[4] = AOSTAG_COPY_ACSTYPE;  		//copy
	sgOperation[5] = AOSTAG_CREATE_ACSTYPE; 	//create
	sgOperation[6] = AOSTAG_WRITE_ACSTYPE; 		//write
	sgOperation[7] = AOSTAG_SENDMSG_ACSTYPE; 	// send message
	sgOperation[8] = AOSTAG_VOTE_ACCESSES; 		//vote
	*/
}


AosAccessCtrlTester::AosAccessCtrlTester()
:
AosSengTester(AosSengTester_AccessControl, "access", AosTesterId::eAccessCtrl),
eLoginWhenNeeded(50),
eLoginAsOwner(10),
eLoginAsRoot(50),
eAddAccWeight(25),
eRemoveAccWeight(50),
eModifyAccWeight(75),
eNormalAccWeight(30),
ePicOwnDoc(50)
{
}

AosAccessCtrlTester::~AosAccessCtrlTester()
{
}


bool 
AosAccessCtrlTester::test()
{
	aos_assert_r(mThread->randomLogin(), false);
	int tries = 10;
	for (int i=0; i<tries; i++)
	{
		aos_assert_r(doOneTest(), false);
	}
	return true;
}


bool
AosAccessCtrlTester::doOneTest()
{
	AosRundataPtr rdata = mThread->getRundata();
	//rdata->setLogging(true);
	const int num_oprs = 15;
	int i = rand() % num_oprs;
	switch (i)
	{
	case 0:
		 aos_assert_r(testRead(rdata), false);
		 return true;

	case 1:
		 aos_assert_r(testModify(rdata), false);
		 return true;

	case 2:
		 aos_assert_r(testDelete(rdata), false);
		 return true;

	case 3:
		 aos_assert_r(testModifyAccesses(rdata), false);
		 return true;

	case 4:
		 // aos_assert_r(testCopy(rdata), false);
		 return true;

	case 5:
		 // aos_assert_r(testListing(rdata), false);
		 return true;

	case 6:
		 // aos_assert_r(testAddMem(rdata), false);
		 return true;

	case 7:
		 // aos_assert_r(testDelMem(rdata), false);
		 return true;

	case 8:
		 // aos_assert_r(testAddObjid(rdata), false);
		 return true;

	case 9:
		 // aos_assert_r(testVote(rdata), false);
		 return true;

	case 10:
		 // aos_assert_r(testFeel(rdata), false);
		 return true;

	case 11:
		 // aos_assert_r(testReadAcd(rdata), false);
		 return true;

	case 12:
		 // aos_assert_r(testModifyAcd(rdata), false);
		 return true;

	case 13:
		 // aos_assert_r(testCreateUrl(rdata), false);
		 return true;

	case 14:
		 // aos_assert_r(testModifyAttr(rdata), false);
		 return true;

	default:
		 aos_assert_r(testRead(rdata), false);
		 return true;
	}
	return true;
}


bool
AosAccessCtrlTester::testModifyAccesses(const AosRundataPtr &rdata)
{
	// This function randomly picks a doc and then
	// modifies its accesses. Modifying accesses itself
	// is access controlled. 
	
	// 1. Pick a doc
	const AosStDocPtr doc = pickDoc();
	if (!doc) return true;

	// 2. Retrieve the doc's access record
	bool is_parent;
	AosStAccessRecord* ard = mThread->getAcd(doc, is_parent);
	aos_assert_r(ard, false);
	
	// 3. If the record is owned by the current user, it should
	// be able to modify it.
	if (doc->creator == mThread->getCrtCid())
	{
		// The doc is owned by the current user. 
		aos_assert_r(modifyAccess(doc, ard), false);
		return true;
	}

	if (mThread->getCrtCid() == "")
	{
		// Nobody logged in yet. It should not be able
		// to modify the accesses
		aos_assert_r(!modifyAccess(doc, ard), false);

		// Determine whether to login
		if ((rand() % 100) < eLoginWhenNeeded)
		{
			aos_assert_r(mThread->randomLogin(), false);
		}
		else
		{
			// Do not login. 
			return true;
		}
	}

	// Check whether it should login as the owner.
	if (OmnRandom::percent(eLoginAsOwner))
	{
		aos_assert_r(mThread->login(doc->creator), false);
		aos_assert_r(mThread->getCrtCid() == doc->creator, false);
		aos_assert_r(modifyAccess(doc, ard), false);
		return true;
	}
		
	// Determine whether to login as root. 
	if (OmnRandom::percent(eLoginAsRoot))
	{
		aos_assert_r(mThread->loginAsRoot(), false);
		aos_assert_r(modifyAccess(doc, ard), false);
		return true;
	}

	// Somebody logged in. Need to check whether the user
	// can modify the doc. 
	if (ard->canDoOpr(AosSecOpr::eModifyAcd, doc->creator, mThread->getCrtCid()))
	{
	 	aos_assert_r(modifyAccess(doc, ard), false);
	 	return true;
	}
		
	// It should not be able to modify the access record
	aos_assert_r(!modifyAccess(doc, ard), false);
	return true;
}


bool
AosAccessCtrlTester::modifyAccess(const AosStDocPtr &doc, AosStAccessRecord *ard)
{
	// This function randomly modifies the access record. 
	// It may or may not be able to do so depending on the
	// access controls. 
	
	// 1. Retrieve the access record
	AosXmlTagPtr xmlrcd = mThread->retrieveAccessRcdOrParent(doc->svrdid);
	aos_assert_r(xmlrcd, false);

	// 2. Create the local ard
	AosStAccessRecord local_ard;
	local_ard.mOwnLocdid = doc->locdid;
	local_ard.mOwnSvrdid = xmlrcd->getAttrU64(AOSTAG_OWNER_DOCID, 0);
	local_ard.mSvrdid = xmlrcd->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(mThread->syncAccesses(&local_ard, xmlrcd), false);
	aos_assert_r(ard->isSame(&local_ard), false);

	// 3. Determine which operation to modify
	int opr = (rand() % ((AosSecOpr::eMax) - AosSecOpr::eRead)) + AosSecOpr::eRead;
	aos_assert_r(AosSecOpr::isValid((AosSecOpr::E)opr), false);

	// 4. Determine the access type
	int acctype = (rand() % (AosAccessType::eMax - AosAccessType::ePrivate)) + 
			AosAccessType::ePrivate;
	aos_assert_r(AosAccessType::isValid((AosAccessType::E)acctype), false);

	// 5. Determine whether to add, remove, or modify. Note that 
	// it creates a temporary access record and tries to modify
	// the tempoary access record. If failed, the original access
	// record is untouched. Otherwise, it modifies the original
	// access record.
	bool rslt = false;
	AosStAccessRecord tmp_ard = *ard;
	switch (OmnRandom::percent(eAddAccWeight, eRemoveAccWeight, eModifyAccWeight, eNormalAccWeight))
	{
	case 0:
		 rslt = addAccess(doc, (AosSecOpr::E)opr, (AosAccessType::E)acctype, &tmp_ard, xmlrcd);
		aos_assert_r(rslt, false);
		break;
	
	case 1:
		 rslt = removeAccess(doc, (AosSecOpr::E)opr, (AosAccessType::E)acctype, &tmp_ard, xmlrcd);
		 aos_assert_r(rslt, false);
		 break;

	case 2:
		 rslt = modifyAccess(doc, (AosSecOpr::E)opr, (AosAccessType::E)acctype, &tmp_ard, xmlrcd);
		 aos_assert_r(rslt, false);
		 break;

	default:
		 rslt = normalizeAccess(doc, (AosSecOpr::E)opr, (AosAccessType::E)acctype, &tmp_ard, xmlrcd);
		 aos_assert_r(rslt, false);
	}

	if (rslt)
	{
		*ard = tmp_ard;
		aos_assert_r(mThread->modifyAccessRecord(ard), false);
		return true;
	}

	return false;
}


bool
AosAccessCtrlTester::checkModifyAccessRecord(
			const u64 &local_did,
			AosStAccessRecord *ard, 
			const AosXmlTagPtr &xmlrcd)
{
	AosStAccessRecord local_ard;
	local_ard.mOwnLocdid = local_did;
	local_ard.mOwnSvrdid = xmlrcd->getAttrU64(AOSTAG_OWNER_DOCID, 0);
	local_ard.mSvrdid = xmlrcd->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(mThread->syncAccesses(&local_ard, xmlrcd), false);
	aos_assert_r(ard->isSame(&local_ard), false);
	return true;
}


bool
AosAccessCtrlTester::addAccess(
		const AosStDocPtr &doc,
		const AosSecOpr::E opr,
		const AosAccessType::E acctype,
		AosStAccessRecord *ard, 
		const AosXmlTagPtr &xmlrcd)
{
	// This function adds an access type to the operator 'opr'
	// Access types are:
	// 		Private
	// 		Public
	// 		Login Public
	// 		Group
	// 		Role
	// 		TimedPublic
	// If there is a private, only the following can be added:
	// 		Group, Role
	// There are some logic relations among access types. For instance,
	// if there is the public access, there is no point to add private. 
	// This function simply adds one access without any checking. If there are
	// too may accesses, it does nothing.
	if (ard->isAccFull(opr)) return true;
	aos_assert_r(ard->addAccess(opr, acctype), false);
	return syncAccessWithServer(ard, xmlrcd);
}


bool
AosAccessCtrlTester::modifyAccess(
		const AosStDocPtr &doc,
		const AosSecOpr::E opr,
		const AosAccessType::E acctype,
		AosStAccessRecord *ard, 
		const AosXmlTagPtr &xmlrcd)
{
	// This function modifies an access type 
	ard->modifyAccess(opr, acctype);
	return syncAccessWithServer(ard, xmlrcd);
}


bool
AosAccessCtrlTester::removeAccess(
		const AosStDocPtr &doc,
		const AosSecOpr::E opr,
		const AosAccessType::E acctype,
		AosStAccessRecord *ard, 
		const AosXmlTagPtr &xmlrcd)
{
	// This function modifies an access type 
	ard->removeAccess(opr, acctype);
	return syncAccessWithServer(ard, xmlrcd);
}


bool
AosAccessCtrlTester::normalizeAccess(
		const AosStDocPtr &doc,
		const AosSecOpr::E opr,
		const AosAccessType::E acctype,
		AosStAccessRecord *ard, 
		const AosXmlTagPtr &xmlrcd)
{
	// This function modifies an access type 
	ard->normalizeAccess();
	return syncAccessWithServer(ard, xmlrcd);
}


bool
AosAccessCtrlTester::syncAccessWithServer(
		AosStAccessRecord *ard, 
		const AosXmlTagPtr &xmlrcd)
{
	// This function modifies 'xmlrcd' based on 'ard'. It then
	// modifies the xmlrcd on the server. This function does not
	// assume that the caller has the permission to do so. 
	// If it does not, the function returns false. Otherwise, 
	// it returns true.
	aos_assert_r(ard->setXml(xmlrcd), false);
	aos_assert_r(checkModifyAccessRecord(ard->mOwnLocdid, ard, xmlrcd), false);
	return mThread->modifyARToServer(xmlrcd);
}


bool
AosAccessCtrlTester::testRead(const AosRundataPtr &rdata)
{
	// Pick a doc
	AosStDocPtr doc;
	while(1)
	{
		doc = pickDoc();
		if (doc)
		{
			if (!(doc->parent_locdid <= 0))break;
		}
		if (!doc) return true;
	}

	// Retrieve its access record
	bool is_parent;
	AosStAccessRecord *ard = mThread->getAcd(doc, is_parent);
	aos_assert_r(ard, false);

	AosXmlTagPtr xml = mThread->retrieveDoc(doc->svrdid);
	if (mThread->isRoot() || ard->canDoOpr(
			AosSecOpr::eRead, doc->creator, mThread->getCrtCid())) 
	{
		// It can read the doc.
		aos_assert_r(xml, false);
		ard->canDoOpr(AosSecOpr::eRead, doc->creator, mThread->getCrtCid());
		aos_assert_r(xml->getAttrU64(AOSTAG_DOCID, 0) == doc->svrdid, false);
		aos_assert_r(xml->getAttrStr(AOSTAG_OBJID) == doc->objid, false);
		return true;
	}
	aos_assert_r(!xml, false);
	return true;
}


AosStDocPtr
AosAccessCtrlTester::pickDoc()
{
	int vv = rand() % 100;
	bool own_doc = (vv < ePicOwnDoc);
	AosStDocPtr doc = mThread->pickDoc(own_doc);
	if (!doc) 
	{
		doc = mThread->pickDoc();
	}
	return doc;
}


bool
AosAccessCtrlTester::testModify(const AosRundataPtr &rdata)
{
	// This function randomly picks a doc. It then either adds or modifies
	// one of its attribute, and then tries to modify the doc on the server. 
	// If the access allows, the modification should be successful. Otherwise, 
	// it should fail.
	// Pick a doc
	const AosStDocPtr doc = pickDoc();
	if (!doc) return true;

	// Retrieve its access record
	bool is_parent;
	AosStAccessRecord *ard = mThread->getAcd(doc, is_parent);
	aos_assert_r(ard, false);
	
	aos_assert_r(doc->locdid, false);
	AosXmlTagPtr xmlrcd = mThread->readLocalDocFromFile(doc->locdid);
	aos_assert_r(xmlrcd, false);
	
	int len = (rand() % 10) + 5;
	OmnString attr = OmnRandom::word(len);
	xmlrcd->setAttr(attr, rand()); 
	
	//rdata->setLogging(true);
	//rdata->clearLogs();
	bool rslt = mThread->modifyObj(xmlrcd, rdata); 

	if (mThread->isRoot() || ard->canDoOpr(AosSecOpr::eWrite, 
			doc->creator, mThread->getCrtCid()))
	{
		aos_assert_r(rslt, false);
		ard->canDoOpr(AosSecOpr::eWrite, doc->creator, mThread->getCrtCid());
		return true;
	}
	aos_assert_r(!rslt, false);
	return true;
}


bool
AosAccessCtrlTester::testDelete(const AosRundataPtr &rdata)
{
	// Pick a doc
	const AosStDocPtr doc = pickDoc();
	if (!doc) return true;

	// Retrieve its access record
	bool is_parent;
	AosStAccessRecord *ard = mThread->getAcd(doc, is_parent);
	aos_assert_r(ard, false);
	
	bool rslt = mThread->removeDocFromServer(doc); 

	if (mThread->isRoot() || 
		ard->canDoOpr(AosSecOpr::eDelete, doc->creator, mThread->getCrtCid()))
	{
		aos_assert_r(rslt, false);
		return true;
	}

	aos_assert_r(!rslt, false);
	return true;
}
