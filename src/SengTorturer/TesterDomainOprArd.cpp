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
#include "SengTorturer/TesterDomainOprArd.h"

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
#include "SengTorUtil/Ptrs.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDomainOprArd.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"


AosDomainOprArdTester::AosDomainOprArdTester(const bool regflag)
:
AosSengTester(AosSengTester_DomainOprArd, AosTesterId::eDomainOprArd, regflag)
{
}


AosDomainOprArdTester::~AosDomainOprArdTester()
{
}


bool 
AosDomainOprArdTester::test()
{
	AosRundataPtr rdata = mThread->getRundata(); 

	switch (OmnRandom::percent(30, 30, 30))
	{
	case 1:
		 return createDomainOprArd();

	case 2: 
		 return removeDomainOprArd();

	case 3: 
		 return modifyDomainOprArd();

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDomainOprArdTester::createDomainOprArd()
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
	
	// 1. Pick a container
	AosStContainerPtr st_ctnr = mThread->pickContainer();
	if (!st_ctnr)
	{
		// There are no containers yet. Do nothing.
		return true;
	}

	if (st_ctnr->getDomainOprArd())
	{
		// It already has it. Do nothing.
		return true;
	}

	// Retrieve the ownder's domain doc
	AosXmlTagPtr domain_doc = mThread->getOwnerDomainDoc(st_ctnr->mObjid);
	aos_assert_r(domain_doc, false);
	
	// Calculate the objid of the Domain Operation ARD
	OmnString did = domain_doc->getAttrStr(AOSTAG_DOCID);
	aos_assert_r(did != "", false);
	OmnString oprard_objid = AosObjid::composeUserDomainOprArdObjid(did);
	aos_assert_r(oprard_objid != "", false);

	// The doc should not exist.
	AosXmlTagPtr oprard_doc = mThread->retrieveDocByObjid(oprard_objid);
	aos_assert_r(oprard_doc, false);

	// Time to create the oprard doc.
	oprard_doc = AosStDomainOprArd::createDomainOprArd(oprard_objid, mThread);
	aos_assert_r(oprard_doc, false);

	// Check whether the requester has the permission to create 
	bool can_create = mThread->canDoOpr(st_ctnr, AosSecOpr::eCreateDomainOprArd);

	// Create the doc
	AosXmlTagPtr doc_created = mThread->createDocOnServer(oprard_doc);
	if (can_create)
	{
		aos_assert_r(doc_created, false);
		aos_assert_r(doc_created->getAttrStr(AOSTAG_OBJID) == oprard_objid, false);
		st_ctnr->setDomainOprArd(doc_created);
	}
	else
	{
		aos_assert_r(!doc_created, false);
	}

	return true;
}


bool
AosDomainOprArdTester::removeDomainOprArd()
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
	
	// 1. Pick a container
	AosStContainerPtr st_ctnr = mThread->pickContainer();
	if (!st_ctnr)
	{
		// There are no containers yet. Do nothing.
		return true;
	}

	if (!st_ctnr->getDomainOprArd())
	{
		// It does not have it. Do nothing.
		return true;
	}

	// Retrieve the doc
	AosXmlTagPtr domain_doc = st_ctnr->getDomainOprArd();
	aos_assert_r(domain_doc, false);
	OmnString oprard_objid = domain_doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(oprard_objid != "", false);
	AosXmlTagPtr oprard_doc = mThread->retrieveDocByObjid(oprard_objid);
	aos_assert_r(oprard_doc, false);

	// Check whether the requester has the permission to create 
	bool can_remove = mThread->canDoOpr(st_ctnr, AosSecOpr::eDelete);

	// Remove the doc from the server
	bool rslt = mThread->removeDocFromServer(oprard_doc);
	if (can_remove)
	{
		aos_assert_r(rslt, false);
		st_ctnr->setDomainOprArd(0);
	}
	else
	{
		aos_assert_r(!rslt, false);
	}

	return true;
}


bool
AosDomainOprArdTester::modifyDomainOprArd()
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
	
	// 1. Pick a container
	AosStContainerPtr st_ctnr = mThread->pickContainer();
	if (!st_ctnr)
	{
		// There are no containers yet. Do nothing.
		return true;
	}

	if (!st_ctnr->getDomainOprArd())
	{
		// It does not have it. Do nothing.
		return true;
	}

	// Retrieve the doc
	AosXmlTagPtr domain_doc = st_ctnr->getDomainOprArd();
	aos_assert_r(domain_doc, false);
	OmnString oprard_objid = domain_doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(oprard_objid != "", false);
	AosXmlTagPtr oprard_doc = mThread->retrieveDocByObjid(oprard_objid);
	aos_assert_r(oprard_doc, false);

	// Check whether the requester has the permission to create 
	bool can_modify = mThread->canDoOpr(st_ctnr, AosSecOpr::eWrite);

	// Modify the doc
	AosXmlTagPtr modified = AosStDomainOprArd::modifyDomainOprArd(oprard_doc, mThread);
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

