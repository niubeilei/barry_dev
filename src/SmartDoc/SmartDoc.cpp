////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// A smartdoc has a group of conditions and a group of actions. 
// It first evaluates its conditions. If evalutes false, it does nothing.
// Otherwise, it runs its actions. 
//
// Note that smartdoc may be overridden. The above is the generic form
// of smartdocs.
//
// Smartdocs SHALL not contain any member data that are used for 
// specific processings. For instance, if we want to collect the 
// number of reads of a specific doc, the number of reads SHALL not
// be stored in this class. It should be store in either the XML
// sdoc or in IIL.
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SmartDoc.h"

#include "API/AosApi.h"
#include "aosDLL/DllMgr.h"
#include "SEInterfaces/DocClientObj.h"
#include "Security/Session.h"
#include "SeLogClient/SeLogClient.h"
#include "SEInterfaces/ActionObj.h"
#include "SmartDoc/SdocTypes.h"
#include "SmartDoc/AllSmartDocs.h"
#include "Alarm/Alarm.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SmartDoc/SMDMgr.h"
#include "alarm_c/alarm.h"
#include "QueryClient/QueryClient.h"
#include "Rundata/Rundata.h"
#include "EventCond/EventCond.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SdocRunLoopAction.h"
#include "Thread/ThreadPool.h"

#include "SmartDoc/SdocBatchGetIILDocids.h"	// Jackie 2013/10/09


static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("smartdoc", __FILE__, __LINE__);

static OmnSPtr<AosSmartDoc> 	sgSmartdocs[AosSdocId::eMax];
AosStr2U32_t 					sgIdMap;
static OmnMutex					sgLock;

static AosSdocJoin				sgSdocJoin(true);
static AosSdocTotal				sgSdocTotal(true);
static AosSdocCreateOrder		sgSdocCreateOrder(true);
static AosSdocCounter			sgSdocCounter(true);
static AosSdocCountDown			sgSdocCountDown(true);
static AosSdocCreateDoc			sgSdocCreateDoc(true);
static AosSdocDataCollect 		sgSdocDataCollect(true);
static AosSdocDataSync			sgSdocDataSync(true);
static AosSdocAllowance			sgSdocAllowance(true);
//static AosSdocThumbNail			sgSdocThumbNail(true);
//static AosSdocComposerTN		sgSdocComposerTN(true);
static AosSdocVote				sgSdocVote(true);
static AosSdocSetVote			sgSdocSetVote(true);
//static AosSdocVerCode			sgSdocVerCode(true);
static AosSdocIsDeleted			sgSdocIsDeleted(true);
static AosSdocCreateContainer 	sgSdocCreateContainer(true);
static AosSdocConvertData		sgSdocConvertData(true);
static AosSdocReserveRes		sgSdocReserveRes(true);
static AosSdocWriteDoc	        sgSdocWriteDoc(true);
static AosSdocCreateDocs	    sgSdocCreateDocs(true);
static AosSdocAlipay			sgSdocAlipay(true);
static AosSdocRunAction			sgSdocRunAction(true);
static AosSdocActionsOnDocs		sgSdocActionsOnDocs(true);
static AosSdocProcServer		sgSdocProcServer(true);
static AosSdocImport			sgSdocImport(true);
//static AosSdocExport			sgSdocExport(true);
//static AosSdocCopyDocsFromCtnr	sgSdocCopyDocsFromCtnr(true);
static AosSdocCheckUnique		sgSdocCheckUnique(true);
static AosSdocSchedule			sgSdocSchedule(true);
static AosSdocCopyDocs          sgSdocCopyDocs(true);
//static AosSdocSendMail			sgSdocSendMail(true);
static AosSdoc00002				sgSdoc00002(true);
//static AosSdocRelation2106      sgSdocRelation2106(true);
static AosSdocRelation2107      sgSdocRelation2107(true);
//static AosSdocRunLoopAction     sgSdocRunLoopAction(true);
//static AosSdocWarehouse		    sgSdocWarehouse(true);
//static AosSdocInlineSearch		sgSdocInlineSearch(true);
static AosSdocModifyCompDoc		sgSdocModifyCompDoc(true);
static AosSdocDelCompDocMembers	sgSdocDelCompDocMembers(true);
static AosSdocGetMediaData		sgSdocGetMediaData(true);
static AosSdocCondActions		sgSdocCondActions(true);
static AosSdocCountByDay        sgSdocCountByDay(true);
//static AosSdocFenHong           sgSdocFenHong(true);
static AosSdocTXTImport			sgSdocTXTImport(true);
static AosSdocStoreQuery		sgSdocStoreQuery(true);
//static AosSdocDocument			sgSdocDocument(true);
//static AosSdocOutputDataFileFormat		sgSdocOutputDataFileFormat(true);
//static AosSdocOutputDataXmlFormat		sgSdocOutputDataXmlFormat(true);
//static AosSdocOutputDataRecordFormat		sgSdocOutputDataRecordFormat(true);
static AosSdocBatchGetIILDocids		sgAosSdocBatchGetIILDocids(true);
static AosSdocCompareTwoDocs		sgAosSdocCompareTwoDocs(true);
//modify by crystal
//static AosSdocImportDocument		sgSdocImportDocument(true);



AosSmartDoc::AosSmartDoc(
		const OmnString &name, 
		const AosSdocId::E type, 
		const bool regflag)
:
mSdocType(type)
{
	if (regflag) 
	{
		OmnSPtr<AosSmartDoc> thisptr(this, false);
		OmnString errmsg;
		if (!registerSdoc(thisptr, name, type, errmsg))
		{
			OmnThrowException(errmsg);
		}
	}
}


AosSmartDoc::AosSmartDoc(const OmnString &objid, const AosRundataPtr &rdata)
:
mSdocType(AosSdocId::eShell)
{
	// This constructor creates a new smartdoc, retrieve the
	// smartdoc from the database, parses the smartdoc into
	// conditions and actions. If errors are found, it will
	// throw exception.
	if (!loadSmartdoc(objid, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


void
AosSmartDoc::sConfig()
{
	for(u32 i=0; i<AosSdocId::eMax; i++)
	{
		if(sgSmartdocs[i])	sgSmartdocs[i]->config();
	}
}


bool
AosSmartDoc::loadSmartdoc(const OmnString &objid, const AosRundataPtr &rdata)
{
	// This function retrieves the smartdoc from database, 
	// parses the smartdoc into conditions and actions. 
	// Conditions are linked as a linked list. Its head is stored
	// in 'mCondHead'. Similarly, actions are linked as a linked
	// list. Its head is stored in 'mActionHead'.
	//

	// Smartdoc is an XML in the following format:
	// 	<smartdoc ...>
	// 		<AOSTAG_CONDITIONS>
	// 			<condition .../>
	// 			<condition .../>
	// 			...
	// 		</AOSTAG_CONDITIONS>
	// 		<actions cond_exec="true">
	// 			<action .../>
	// 			<action .../>
	// 			...
	// 		</actions>
	// 		<actions cond_exec="false">
	// 			<action .../>
	// 			<action .../>
	// 			...
	// 		</actions>
	//
	// 	</smartdoc>
	
	// 1. Retrieve the XML smartdoc. 
	mSdoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (!mSdoc)
	{
		AosSetError(rdata, "Smartdoc not found:") << objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 2. Parse the conditions, if any
	AosXmlTagPtr conds = mSdoc->getFirstChild(AOSTAG_CONDITIONS);
	if (conds)
	{
		mCondHead = conds->getFirstChild();
		AosXmlTagPtr current = mCondHead;
		AosXmlTagPtr next;

		int guard = eMaxConds;
		while (current && guard--)
		{
			next = conds->getNextChild();
			if (!next) break;
			
			current->setNext(next);
			next->setPrev(current);
			current = next;
		}
	}

	// Parse the actions
	AosXmlTagPtr actions = mSdoc->getFirstChild(AOSTAG_ACTIONS);
	if (actions)
	{
		mActionHead = actions->getFirstChild();
		AosXmlTagPtr current = mActionHead;
		AosXmlTagPtr next;

		int guard = eMaxActions;
		while (current && guard--)
		{
			next = actions->getNextChild();
			if (!next) break;
			current->setNext(next);
			next->setPrev(current);
			current = next;
		}
	}

	return true;
}


AosSmartDoc::~AosSmartDoc()
{
}


bool
AosSmartDoc::registerSdoc(
		const OmnSPtr<AosSmartDoc> &sdoc, 
		const OmnString &name, 
		const AosSdocId::E type, 
		OmnString &errmsg)
{
	sgLock.lock();

	if (!AosSdocId::isValid(type))
	{
		sgLock.unlock();
		errmsg = "Incorrect smartdoc: ";
		errmsg << type;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	bool rslt = AosSdocId::addName(name, type, errmsg);
	if (!rslt)
	{
		OmnAlarm << "Failed adding the smartdoc name: " << name 
			<< ". Error Message: " << errmsg << enderr;
		return false;
	}

	if (sgSmartdocs[type])
	{
		sgLock.unlock();
		errmsg = "Smart doc already registered: ";
		errmsg << type;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgSmartdocs[type] = sdoc.getPtr();
	sgLock.unlock();
	return true;
}


bool
AosSmartDoc::parseActions(
		const AosXmlTagPtr &sdoc, 
		std::vector<AosActionObjPtr> &the_actions,
		const AosRundataPtr &rdata)
{
	// 	<actions ...>
	// 		<action type="xxx">
	// 			<attr namebd="xxx" attrname=" " />
	// 		<action ...>
	// 		...
	// 		<action ...>
	// 	</actions>
	aos_assert_r(sdoc, false);

	the_actions.clear();

	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
	if (!actions) return true;

	// Retrieve all the actions
	AosXmlTagPtr action = actions->getFirstChild(AOSTAG_ACTION);
	while (action)
	{
		// AosActionObjPtr sdocAction = AosSdocAction::getAction(action, rdata);
		// AosActionObjPtr sdocAction = AosSdocAction::createAction(action, rdata);
		AosActionObjPtr sdocAction = AosSdocAction::getAction(action, rdata);
		// get action type
		if(!sdocAction)
		{
			AosSetError(rdata, "Failed to create the action:") << action->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		
		the_actions.push_back(sdocAction);
		action = actions->getNextChild();
	}
	return true;
}


bool
AosSmartDoc::runSmartdocs(
		const OmnString &sdoc_objids,
		const AosRundataPtr &rdata)
{
	// This function runs smart docs. The smart doc IDs are in 'sdoc_objids', 
	// separated by commas. 
	OmnStrParser1 parser(sdoc_objids, ", ");
	OmnString sdoc_objid;
	while ((sdoc_objid = parser.nextWord()) != "")
	{
		runSmartdoc(sdoc_objid, rdata);
	}

	return true;
}


bool
AosSmartDoc::runSmartdoc(
		const OmnString &sdoc_objid,
		const AosRundataPtr &rdata)
{
	// This function runs smart doc 'sdoc_objid'. 
	AosXmlTagPtr sdoc = AosGetDocByObjid(sdoc_objid, rdata);
	if (!sdoc)
	{
		AosSetError(rdata, "Failed retrieving smartdoc:") << sdoc_objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Ken Lee, 2013/09/13
	sdoc = sdoc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(sdoc, false);

	OmnString type = sdoc->getAttrStr(AOSTAG_SMARTDOC_TYPE);
	AosSmartDocObjPtr smartdoc = getSmartDoc(type);
	if (!smartdoc)
	{
		// Chen Ding, 2013/05/30
		OmnString otype = sdoc->getAttrStr(AOSTAG_OTYPE);
		if (otype == AOSOTYPE_JIMO)
		{
		    return AosRunJimo(rdata.getPtr(), rdata->getWorkingDoc(), sdoc);
		}
		if (AosActionObj::runAction(sdoc, rdata)) return true;

		AosSetError(rdata, "Smartdoc not defined: ") << type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rdata->setSdoc(sdoc);
	OmnString status = sdoc->getAttrStr(AOSTAG_INTERRUPT);
	rdata->setSdocReturnStaus(status);
	bool rslt = smartdoc->run(sdoc, rdata);
	aos_assert_r(rslt, false);

	// Ken Lee, 2013/05/08
	smartdoc->setSdoc(0);

	return true;
}


bool
AosSmartDoc::runSmartdoc(
		const u64 &sdoc_docid,
		const AosRundataPtr &rdata)
{
	// This function runs smart doc 'sdoc_objid'. 
	AosXmlTagPtr sdoc = AosGetDocByDocid(sdoc_docid, rdata);
	if (!sdoc)
	{
		AosSetError(rdata, "Failed retrieving smartdoc:") << sdoc_docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return runSmartdoc(sdoc, rdata);
	/*
	OmnString type = sdoc->getAttrStr(AOSTAG_SMARTDOC_TYPE);
	AosSmartDocObjPtr smartdoc = getSmartDoc(type);
	if (!smartdoc)
	{
		AosSetError(rdata, "Smartdoc not defined: ") << type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rdata->setSdoc(sdoc);
	// rdata->setCrtSmartdoc(smartdoc);
	bool rslt = smartdoc->run(sdoc, rdata);
	AOSSYSLOG_CREATE(rslt, type, rdata); 

	// Ken Lee, 2013/05/08
	smartdoc->setSdoc(0);

	return rslt;
	*/
}


AosSmartDocObjPtr
AosSmartDoc::getSmartDoc(const OmnString &sdoc_objid)
{
	if (sdoc_objid == "") return 0;

	sgLock.lock();
	AosSdocId::E id = AosSdocId::toEnum(sdoc_objid);
	if (!AosSdocId::isValid(id))
	{
		sgLock.unlock();
		return 0;
	}
	
	AosSmartDocObjPtr sdoc = sgSmartdocs[id];
	sgLock.unlock();
	aos_assert_r(sdoc, 0);
	return sdoc;
}


AosXmlTagPtr
AosSmartDoc::createDoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
 	// This function uses AosSdocCreateDoc to create a doc based on 'sdoc'
 	// and 'tagname'. 
	return AosSdocCreateDoc::createDoc(sdoc, rdata);
}


AosXmlTagPtr
AosSmartDoc::createDoc(
 		const AosXmlTagPtr &sdoc,
 		const OmnString &tagname,
 		const AosRundataPtr &rdata)
{
 	// This function uses AosSdocCreateDoc to create a doc based on 'sdoc'
 	// and 'tagname'. 
	return AosSdocCreateDoc::createDoc(sdoc, tagname, rdata);
}


bool
AosSmartDoc::procSmartdocs(
		const OmnString &sdoc_objids, 
		const AosRundataPtr &rdata)
{
	OmnStrParser1 parser(sdoc_objids, ", ");
	OmnString sdoc_objid;
	while ((sdoc_objid = parser.nextWord()) != "")
	{
		runSmartdoc(sdoc_objid, rdata);
		//TSK0070, Linda, 2011/05/12  Container Member Verifications
		if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
		{
			if (!rdata->isOk()) return false; 
			return true;
		}
	}
	return true;
}


bool
AosSmartDoc::runSmartdoc(
		const AosXmlTagPtr &thesdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(thesdoc, false);

	AosXmlTagPtr sdoc = thesdoc->clone(AosMemoryCheckerArgsBegin);	
	aos_assert_r(sdoc, false);
		
	OmnString type = sdoc->getAttrStr(AOSTAG_SMARTDOC_TYPE);
	AosSmartDocObjPtr smartdoc = getSmartDoc(type);
	if (!smartdoc)
	{
		// Chen Ding, 2013/05/30
		OmnString otype = sdoc->getAttrStr(AOSTAG_OTYPE);
		if (otype == AOSOTYPE_JIMO)
		{
			return AosRunJimo(rdata.getPtr(), rdata->getWorkingDoc(), sdoc);
		}

		if (AosActionObj::runAction(sdoc, rdata)) return true;

		AosSetError(rdata, "Smartdoc not defined: ") << type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Chen Ding, 2013/05/24
	smartdoc->setSdoc(sdoc);
	OmnString status = sdoc->getAttrStr(AOSTAG_INTERRUPT);
	rdata->setSdocReturnStaus(status);
	bool rslt = smartdoc->run(sdoc, rdata);
	aos_assert_r(rslt, false);

	// Ken Lee, 2013/05/08
	smartdoc->setSdoc(0);

	return true;
}


#if 0
bool
AosSmartDoc::runSmartdoc(
		const u64 &sdocid, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(sdocid, rdata, false);
	AosXmlTagPtr sdoc = AosGetDocByDocid(sdocid, rdata);
	if (!sdoc)
	{
		AosSetErrorUser(rdata, "Failed retrieving smartdoc:") << sdocid << enderr;
		return false;
	}

	return runSmartdoc(sdoc, rdata);
	/*
	sdoc = sdoc->clone(AosMemoryCheckerArgsBegin);	
	aos_assert_r(sdoc, false);
	
	OmnString type = sdoc->getAttrStr(AOSTAG_SMARTDOC_TYPE);
	AosSmartDocObjPtr smartdoc = AosSmartDoc::getSmartDoc(type);
	if (!smartdoc)
	{
		// Chen Ding, 2013/05/30
		OmnString otype = sdoc->getAttrStr(AOSTAG_OTYPE);
		if (otype == AOSOTYPE_JIMO) return AosRunJimo(rdata, rdata->getWorkingDoc(), sdoc); 

		// Chen Ding, 2013/05/03
		// Check whether it is a DLL.
		if (AosActionObj::runAction(sdoc, rdata))
		{
			return true;		
		}

		AosSetError(rdata, "invalid_sdoc_type") << ": " << type;
		return false;
	}

	smartdoc->setSdoc(sdoc);
		
	OmnString status = sdoc->getAttrStr(AOSTAG_INTERRUPT);
	rdata->setSdocReturnStaus(status);

	// rdata->setCrtSmartdoc(smartdoc);
	bool rslt = smartdoc->run(sdoc, rdata);
	AOSSYSLOG_CREATE(rslt, type, rdata); 

	// Ken Lee, 2013/05/08
	smartdoc->setSdoc(0);

	//TSK0070, Linda, 2011/05/12  Container Member Verifications
	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
	{
		if (!rdata->isOk()) return false; 
	}
	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_BREAK)
	{
		if (!rdata->isOk()) return true;
	}
	return true;
	*/
}
#endif


AosSmartDocObjPtr 	
AosSmartDoc::clone()
{
	OmnShouldNeverComeHere;
	return 0;
}


bool  
AosSmartDoc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

	
bool
AosSmartDoc::addThreadShellProc(
		const OmnThrdShellProcPtr &runner,
		const AosRundataPtr &rdata)
{
	aos_assert_r(runner, false);
	aos_assert_r(sgThreadPool, false);
	return sgThreadPool->proc(runner);
}


