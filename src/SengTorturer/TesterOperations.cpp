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
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/TesterOperations.h"

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
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"


AosOperationsTester::AosOperationsTester(const bool regflag)
:
AosSengTester(AosSengTester_Operations, AosTesterId::eOperations, regflag)
{
}


AosOperationsTester::AosOperationsTester()
:
AosSengTester(AosSengTester_Operations, AosTesterId::eOperations, false)
{
}


AosOperationsTester::~AosOperationsTester()
{
}


bool 
AosOperationsTester::configTester(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr def = config->getFirstChild("operations_tester");
	if (!def) return true;

	mWtDefineOperations = def->getAttrInt("define_operations", eDftDefineOperations);
	mWtAddOperations = def->getAttrInt("add_operations", eDftAddOperations);
	mWtRemoveOperations = def->getAttrInt("remove_operations", eDftRemoveOperations);
	mWtCheckOperations = def->getAttrInt("check_operations", eDftCheckOperations);

	return true;
}


bool 
AosOperationsTester::test()
{
	// This is the virtual function called from AosSengTester. It does just one
	// test. The caller will loop over the tries.
	AosRundataPtr rdata = mThread->getRundata(); 

	bool rslt = false;
	switch (OmnRandom::percent(30, 30, 30))
	{
	case 0:
		 rslt = addOperations();
		 break;

	case 1: 
		 rslt = removeOperations();
		 break;

	case 2: 
		 rslt = modifyOperations();
		 break;

	default:
		 OmnShouldNeverComeHere;
		 break;
	}

	aos_assert_r(rslt, false);
	return true;
}


bool
AosOperationsTester::addOperations()
{
	// Operations are defined in containers:
	// 	<container ...>
	// 		<AOSTAG_OPR_DEFS>
	// 			<sec_opr_id>
	// 				<cond .../>
	// 				<cond .../>
	// 				...
	// 			</sec_opr_id>
	// 			...
	// 		</AOSTAG_OPR_DEFS>
	// 		...
	// 	</container>
	
	// 1. Pick a container
	AosStContainerPtr st_ctnr = mThread->pickContainer();
	if (!st_ctnr)
	{
		// There are no containers yet. Do nothing.
		return true;
	}

	OmnString objid = st_ctnr->getObjid();
	AosXmlTagPtr ctnr_doc = mThread->retrieveDocByObjid(objid);
	aos_assert_r(ctnr_doc, false);
	AosXmlTagPtr oprtag = ctnr_doc->getFirstChild(AOSTAG_OPR_DEFS);
	if (!oprtag)
	{
		oprtag = ctnr_doc->addNode1(AOSTAG_OPR_DEFS);
		aos_assert_r(oprtag, false);
	}

	OmnString opr_id = AosSecOpr::toString(AosStUtil::getSecOpr());
	AosXmlTagPtr secoprtag = oprtag->getFirstChild(opr_id);
	if (!secoprtag)
	{
		secoprtag = oprtag->addNode1(opr_id);
	}

	bool rslt = st_ctnr->addOperation(ctnr_doc, mThread);
	aos_assert_r(rslt, false);

	rslt = mThread->saveDoc(ctnr_doc);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosOperationsTester::removeOperations()
{
	// 1. Pick a container
	AosStContainerPtr st_ctnr = mThread->pickContainer();
	if (!st_ctnr)
	{
		// There are no containers yet. Do nothing.
		return true;
	}

	// 2. Retrieve the XML container
	OmnString objid = st_ctnr->getObjid();
	AosXmlTagPtr ctnr_doc = mThread->retrieveDocByObjid(objid);
	aos_assert_r(ctnr_doc, false);

	// 3. Check whether it has operation definition.
	AosXmlTagPtr oprtag = ctnr_doc->getFirstChild(AOSTAG_OPR_DEFS);
	if (!oprtag)
	{
		// It has no operation definition. Do nothing.
		return true;
	}

	// 4. Pick an opr_id to remove
	OmnString opr_id = AosSecOpr::toString(AosStUtil::getSecOpr());
	AosXmlTagPtr secoprtag = oprtag->getFirstChild(opr_id);
	if (!secoprtag)
	{
		// Not there. Do nothing.
		return true;
	}

	bool rslt = st_ctnr->removeOperation(ctnr_doc, mThread);
	aos_assert_r(rslt, false);

	rslt = mThread->saveDoc(ctnr_doc);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosOperationsTester::modifyOperations()
{
	// 1. Pick a container
	AosStContainerPtr st_ctnr = mThread->pickContainer();
	if (!st_ctnr)
	{
		// There are no containers yet. Do nothing.
		return true;
	}

	// 2. Retrieve the XML container
	OmnString objid = st_ctnr->getObjid();
	AosXmlTagPtr ctnr_doc = mThread->retrieveDocByObjid(objid);
	aos_assert_r(ctnr_doc, false);

	bool rslt = st_ctnr->modifyOperation(ctnr_doc, mThread);
	aos_assert_r(rslt, false);

	rslt = mThread->saveDoc(ctnr_doc);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosOperationsTester::checkOperation()
{
	// Operations are defined based on the following (SecUtil/SecOpr.h)
	//	eRead,
	//	eWrite,
	//	eCreate,
	//	eDelete,
	//	eCopy,
	//	eList,
	//	eAddMem,
	//	eDelMem,
	//	eSvrCmd,
	//	eSendMsg,
	//	eAddObjid,
	//	eVote,
	//	eFeel,
	//	eReadAcd,
	//	eModifyAcd,
	//	eCreateAcd,
	//	eCreateUrl,
	//	eRemoveUrl,
	//	eRegisterHook,
	//	eUploadFile,
	//	eCreateAcct,
	//	eModifyAcct,
	//	eDeleteAcct,
	//	eCreateStatemachine,
	//	eCreateDomainOprArd,
	//	eReservedObjid,
	AosStContainerPtr st_ctnr = mThread->pickContainer();
	if (!st_ctnr) return true;

	OmnString cmd, operation;
	bool rslt = st_ctnr->pickOperation(cmd, operation, mThread);
	OmnNotImplementedYet;
	return rslt;
}

