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
// 01/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/TesterUserOprArd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SengTorturer/SengTesterThrd.h"
#include "SengTorturer/SengTesterMgr.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StUserOprArd.h"
#include "SengTorUtil/StDoc.h"
#include "Tester/TestPkg.h"


AosUserOprArdTester::AosUserOprArdTester(const bool regflag)
:
AosSengTester(AosSengTester_UserOprArd, AosTesterId::eUserOprArd, regflag)
{
}


AosUserOprArdTester::~AosUserOprArdTester()
{
}


bool 
AosUserOprArdTester::test()
{
	AosRundataPtr rdata = mThread->getRundata(); 

	switch (OmnRandom::percent(30, 30, 30))
	{
	case 1:
		 return createUserOprArd();

	case 2: 
		 return removeUserOprArd();

	case 3: 
		 return modifyUserOprArd();

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosUserOprArdTester::createUserOprArd()
{
	// The format is:
	// 	<opr_ard ...>
	// 		<AOSTAG_OPERATIONS>
	// 			<oprcode AOSTAG_ACCESS=AOSVALUE_GRANT|AOSVALUE_DENY/>
	// 			<oprcode AOSTAG_ACCESS=AOSVALUE_GRANT|AOSVALUE_DENY/>
	// 			...
	// 		</AOSTAG_OPERATIONS>
	// 	</opr_ard>
	
	// 1. Pick a container
	AosStDocPtr st_doc = mThread->pickDoc();
	if (!st_doc)
	{
		// There are no containers yet. Do nothing.
		return true;
	}

	// Retrieve the ownder's domain doc
	AosXmlTagPtr acct_doc = mThread->getOwnerAccountDoc(st_doc->objid);
	aos_assert_r(acct_doc, false);
	
	// Calculate the objid of the Domain Operation ARD
	OmnString did = acct_doc->getAttrStr(AOSTAG_DOCID);
	aos_assert_r(did != "", false);
	OmnString oprard_objid = AosObjid::composeUserOprArdObjid(did);
	aos_assert_r(oprard_objid != "", false);

	// Retrieve the operation ard
	AosXmlTagPtr oprard_doc = mThread->retrieveDocByObjid(oprard_objid);
	if (oprard_doc)
	{
		// It already exists. Do nothing.
		return true;
	}

	// Time to create the oprard doc.
	oprard_doc = AosStUserOprArd::createUserOprArd(oprard_objid, mThread);
	aos_assert_r(oprard_doc, false);

	// Check whether the requester has the permission to create 
	bool can_create = mThread->canDoOpr(oprard_doc, AosSecOpr::eCreate);

	// Create the doc
	AosXmlTagPtr doc_created = mThread->createDocOnServer(oprard_doc);
	if (can_create)
	{
		aos_assert_r(doc_created, false);
		aos_assert_r(doc_created->getAttrStr(AOSTAG_OBJID) == oprard_objid, false);
	}
	else
	{
		aos_assert_r(!doc_created, false);
	}

	return true;
}


bool
AosUserOprArdTester::removeUserOprArd()
{
	// 1. Pick a doc
	AosStDocPtr st_doc = mThread->pickDoc();
	if (!st_doc)
	{
		// There are no containers yet. Do nothing.
		return true;
	}

	// Retrieve the ownder's domain doc
	AosXmlTagPtr acct_doc = mThread->getOwnerAccountDoc(st_doc->objid);
	aos_assert_r(acct_doc, false);
	
	// Calculate the objid of the Domain Operation ARD
	OmnString did = acct_doc->getAttrStr(AOSTAG_DOCID);
	aos_assert_r(did != "", false);
	OmnString oprard_objid = AosObjid::composeUserOprArdObjid(did);
	aos_assert_r(oprard_objid != "", false);

	// Retrieve the operation ard
	AosXmlTagPtr oprard_doc = mThread->retrieveDocByObjid(oprard_objid);
	if (!oprard_doc)
	{
		// It does not have the oprard. Do nothing.
		return true;
	}

	// Check whether the requester has the permission to create 
	bool can_remove = mThread->canDoOpr(oprard_doc, AosSecOpr::eDelete);

	// Remove the doc from the server
	bool rslt = mThread->removeDocFromServer(oprard_doc);
	if (can_remove)
	{
		aos_assert_r(rslt, false);
	}
	else
	{
		aos_assert_r(!rslt, false);
	}

	return true;
}


bool
AosUserOprArdTester::modifyUserOprArd()
{
	// The format is:
	// 	<opr_ard ...>
	// 		<AOSTAG_OPERATIONS>
	// 			<oprcode AOSTAG_OVERRIDE="true|false">
	// 			 	<access type="xxx">xxx</access>
	// 			  	<access type="xxx">xxx</access>
	// 			   	...
	// 			</oprcode>
	// 			...
	// 		</AOSTAG_OPERATIONS>
	// 	</opr_ard>
	
	// 1. Pick a doc
	AosStDocPtr st_doc = mThread->pickDoc();
	if (!st_doc)
	{
		// There are no containers yet. Do nothing.
		return true;
	}

	// Retrieve the ownder's domain doc
	AosXmlTagPtr acct_doc = mThread->getOwnerAccountDoc(st_doc->objid);
	aos_assert_r(acct_doc, false);
	
	// Calculate the objid of the Domain Operation ARD
	OmnString did = acct_doc->getAttrStr(AOSTAG_DOCID);
	aos_assert_r(did != "", false);
	OmnString oprard_objid = AosObjid::composeUserOprArdObjid(did);
	aos_assert_r(oprard_objid != "", false);

	// Retrieve the operation ard
	AosXmlTagPtr oprard_doc = mThread->retrieveDocByObjid(oprard_objid);
	if (!oprard_doc)
	{
		// It does not have the oprard. Do nothing.
		return true;
	}

	// Check whether the requester has the permission to create 
	bool can_modify = mThread->canDoOpr(oprard_doc, AosSecOpr::eWrite);

	// Modify the doc
	AosXmlTagPtr modified = AosStUserOprArd::modifyUserOprArd(oprard_doc, mThread);
	aos_assert_r(modified, false);

	bool rslt = mThread->modifyObj(modified, true);
	if (can_modify)
	{
		aos_assert_r(rslt, false);
	}
	else
	{
		aos_assert_r(!rslt, false);
	}

	return true;
}

