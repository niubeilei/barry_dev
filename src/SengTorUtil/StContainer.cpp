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
#include "SengTorUtil/StContainer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StDomainOprArd.h"
#include "SengTorUtil/StUserOprArd.h"
#include "SengTorUtil/StAccess.h"
#include "SengTorUtil/StOperationArd.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/SengTesterThrd.h"


OmnString 
AosStContainer1::getOperation(
		const AosStDocPtr &local_doc,
		const AosSecOpr::E opr_id, 
		const AosSengTestThrdPtr &thread)
{
	// This function uses mTranslators to translate the access into 
	// a user defined operation, if possible. Entries in mTranslators
	// are keyed by 'opr_id'. 
	aos_assert_r(AosSecOpr::isValid(opr_id), "");
	vector<AosStOprTranslatorPtr> translators = mTranslators[opr_id];
	if (translators.size() == 0) return "";

	for (u32 i=0; i<translators.size(); i++)
	{
		AosStOprTranslatorPtr translator = translators[i];
		OmnString opr = translator->determineOperation(local_doc, opr_id, thread);
		if (opr != "") return opr;
	};
	return "";
}


bool
AosStContainer1::canDoOpr(
		const AosStDocPtr &local_doc,
		const AosSecOpr::E opr, 
		const AosSengTestThrdPtr &thread)
{
	OmnString operation = getOperation(local_doc, opr, thread);
	if (operation != "")
	{
		// The access converts to a user defined operation.
		bool granted, denied, rollback;
		bool rslt = canDoOpr(local_doc, operation, granted, denied, rollback, thread);
		aos_assert_r(rslt, false);
		if (granted) return true;
		if (denied) return false;
		if (!rollback) return false;
	}

	// It rolls back to the normal access controls.
	return canDoOprNormal(local_doc, opr, thread);
}


bool
AosStContainer1::canDoOpr(
		const AosStDocPtr &local_doc,
		const OmnString &operation,
		bool &granted,
		bool &denied, 
		bool &rollback, 
		const AosSengTestThrdPtr &thread)
{
	// This function does the following:
	// a. If there is a domain operation access record, use it.
	// b. If not granted, it checks whether it allows personal 
	//    operation access record.
	// c. If yes, it uses the personal operation access record.
	// d. If not granted, it checks whether it should rollback.
	// e. If yes, it uses the normal access controls. 
	aos_assert_r(local_doc, false);
	AosStContainerPtr accessed_domain = thread->getContainer(local_doc->locdid);
	aos_assert_r(accessed_domain, false);
	AosXmlTagPtr opr_ard = accessed_domain->getOperationArd();
	if (opr_ard)
	{
		// There is a user domain operation ard. Use it to check.
		bool rslt = AosStDomainOprArd::checkAccess(
				opr_ard, local_doc, operation, granted, denied, thread);
		aos_assert_r(rslt, false);
		if (granted || denied) return true;
	}

	// Need to check whether the requester has an operation ard.
	opr_ard = thread->getUserOprArd();
	if (opr_ard)
	{
		bool rslt = AosStUserOprArd::checkAccess(
				opr_ard, local_doc, operation, granted, denied, thread);
		aos_assert_r(rslt, false);
		if (granted || denied) return true;
	}

	denied = false;
	granted = false;
	return true;
}


bool 	
AosStContainer1::canDoOprNormal(
		const AosStDocPtr &local_doc, 
		const AosSecOpr::E opr, 
		const AosSengTestThrdPtr &thread)
{
	// This function checks whether the requester 'requester_cid' can perform
	// the operation 'opr' on the doc 'local_did'. It is done by retrieving 
	// its local access record. 
	// If the local access record is not there yet, it tries to get its 
	// parent access record.
	//
	// 1. If 'local_did' does not find a local doc, it is an error.
	// 2. If the doc does not have its local access record, retrieve its 
	//    access record from the server. 
	// 3. If it does not have an access record on the server, retrieve the 
	// 	  doc's parent doc. If not found, it is an error. 
	// 4. Retrieve its parent's access record from the server. 
	// 5. If the server access record (either itself or its parent), its
	//    local access record is created. 
	// 6. If no access record is found, use the default. 
	// 7. Otherwise, use the access record to check the access.
	aos_assert_r(local_doc, false);
	OmnString owner_cid = local_doc->creator;

	AosStAccessRecord *arcd = thread->getLocalAccessRecord(local_doc->locdid);
	if (!arcd)
	{
		// Retrieve the access record from the server
		AosXmlTagPtr svr_ard = thread->retrieveAccessRcd(local_doc->svrdid, false, false);
		if (!svr_ard)
		{
			// Did not retrieve its access record from the server. 
			// Retrieve its parent's access record. 
			aos_assert_r(local_doc->parent_locdid > 0, false);
			u64 parent_locdid = local_doc->parent_locdid;
			AosStDocPtr parent_doc = thread->getDocByLocalDid(parent_locdid);
			if (!parent_doc)
			{
				OmnAlarm << "Failed retrieving the parent: " << parent_locdid << enderr;
				return false;
			}

			arcd = thread->getLocalAccessRecord(parent_locdid);
			if (!arcd)
			{
				svr_ard = thread->retrieveAccessRcd(parent_doc->svrdid, false, false);
				if (svr_ard)
				{
					arcd = thread->createAccessRecord(svr_ard, parent_locdid);
				}
			}
		}
		else
		{
			arcd = thread->createAccessRecord(svr_ard, local_doc->locdid);
		}
	}

	if (arcd)
	{
		return arcd->canDoOpr(opr, owner_cid, thread->getCrtCid());
	}

	// No access record yet. 
	return AosStAccessRecord::canDoOprDefault(opr, owner_cid, thread->getCrtCid());
}


AosXmlTagPtr
AosStContainer1::getOperationArd()
{
	// A container may define an Operation Access Record (at most one).
	return mDomainOprArd;	
}


bool 
AosStContainer1::addOperation(
		const AosXmlTagPtr &ctnr_xml, 
		AosSengTestThrdPtr &thread)
{
	// A container maintains an Operation Translator Map that are 
	// responsible for translating accesses into operations.
	// This function adds a new translation into the container.
	AosStContainerPtr st_ctnr(this, false);
	return AosStOprTranslator::addOperation(ctnr_xml, st_ctnr, thread);
}


bool 
AosStContainer1::removeOperation(
		const AosXmlTagPtr &xml_ctnr, 
		AosSengTestThrdPtr &thread)
{
	// This function randomly removes an operation translator from both
	// 'xml_ctnr' and 'st_ctnr'. Note that the caller should save 
	// 'xml_ctnr' on the server.
	aos_assert_r(xml_ctnr, false);
	AosXmlTagPtr oprtag = xml_ctnr->getFirstChild(AOSTAG_OPR_DEFS);
	if (!oprtag)
	{
		// No AOSTAG_OPR_DEFS yet, the map mTranslators should be empty.
		for (int i=0; i<AosSecOpr::eMax; i++)
		{
			aos_assert_r(mTranslators[i].size() == 0, false);
		}
		return true;
	}

	// Get a random SecOpr
	AosSecOpr::E opr_id= AosStUtil::getSecOpr();
	OmnString opr_str = AosSecOpr::toString(opr_id);
	AosXmlTagPtr secoprtag = oprtag->getFirstChild(opr_str);
	vector<AosStOprTranslatorPtr> translators = mTranslators[opr_id];
	if (!secoprtag)
	{
		// The subtag for 'opr_id' is not there yet. Check it.
		aos_assert_r(translators.size() == 0, false);
		return true;
	}
	int num_tags = secoprtag->getNumSubtags();
	if ((u32)num_tags != translators.size())
	{
		// u32 ss = translators.size();
//		OmnAlarm << "internal error: " << num_tags << ":" << ss << enderr;
//		return false;
return true;
	}

	if (num_tags == 0)
	{
		// The subtag for 'opr_id' has no children.
		return true;
	}

	// Pick an operation to remove
	int nn = rand() % translators.size();
	AosStOprTranslatorPtr translator = translators[nn];
	AosStContainerPtr st_ctnr(this, false);
	return translator->removeOperation(opr_str, st_ctnr, xml_ctnr, thread);
}


/*
bool 
AosStContainer1::checkOperation(const AosXmlTagPtr &xml_ctnr, AosSengTestThrdPtr &thread)
{
	aos_assert_r(xml_ctnr, false);
	AosXmlTagPtr oprtag = xml_ctnr->getFirstChild(AOSTAG_OPR_DEFS);
	if (!oprtag)
	{
		// No AOSTAG_OPR_DEFS yet, the map mTranslators should be empty.
		aos_assert_r(mTranslators.size() == 0, false);
		return true;
	}

	// Get a random SecOpr
	AosSecOpr::E opr_id= AosStUtil::getSecOpr();
	OmnString opr_str = AosSecOpr::toString(opr_id);
	AosXmlTagPtr secoprtag = oprtag->getFirstChild(opr_str);
	OprMapItr_t itr = mTranslators.find(opr_str);
	if (!secoprtag)
	{
		// The subtag for 'opr_id' is not there yet. Check it.
		aos_assert_r(itr == mTranslators.end(), false);
		return true;
	}

	// Check all the operations
	AosStOprTranslatorPtr translator = itr->second;
	AosStContainerPtr thisptr(this, false);
	while (translator)
	{
		aos_assert_r(translator->checkOperation(thisptr, xml_ctnr, thread), false);
		translator = translator->getNext();
	}
	return true;
}
*/


bool 
AosStContainer1::addTranslator(
		const OmnString &opr_str, 
		const AosStOprTranslatorPtr &translator)
{
	AosSecOpr::E opr_id = AosSecOpr::toEnum(opr_str);
	aos_assert_r(AosSecOpr::isValid(opr_id), false);
	vector<AosStOprTranslatorPtr> tt = mTranslators[opr_id];
	tt.push_back(translator);
	return true;
}


bool 
AosStContainer1::removeTranslator(
		const OmnString &opr_str,
		const AosStOprTranslatorPtr &translator)
{
	// This function removes the translator 'translator'.
	AosSecOpr::E opr_id = AosSecOpr::toEnum(opr_str);
	aos_assert_r(AosSecOpr::isValid(opr_id), false);
	vector<AosStOprTranslatorPtr> tts = mTranslators[opr_id];
	aos_assert_r(tts.size(), false);

	vector<AosStOprTranslatorPtr>::iterator itr = tts.begin();
	for (; itr!=tts.end(); itr++)
	{
		AosStOprTranslatorPtr crt = *itr;
		if (crt == translator)
		{
			tts.erase(itr);
			return true;
		}
	};

	OmnAlarm << "Failed to find the translator to remove" << enderr;
	return false;
}


bool
AosStContainer1::pickOperation(
		OmnString &cmd, 
		OmnString &operation, 
		const AosSengTestThrdPtr &thread)
{
	// This function checks whether the container defined operations.
	// If not, it sets 'cmd' and 'operation' to empty strings and
	// returns true.
	//
	// Otherwise, it randomly determines whether to pick a valid
	// operation or invalid operation. 
	AosStOprTranslatorPtr translator = pickTranslator();
	if (!translator)
	{
		operation = "";
		cmd = pickCommand(thread);
		return true;
	}
	return translator->pickCommand(cmd, operation, thread);
}


AosStOprTranslatorPtr
AosStContainer1::pickTranslator()
{
	// There are translators. Pick one translator.
	int nn = OmnRandom::nextInt1(0, AosSecOpr::eMax-1);
	vector<AosStOprTranslatorPtr> translators = mTranslators[nn];
	OmnNotImplementedYet;
	return 0;
}


OmnString
AosStContainer1::pickCommand(const AosSengTestThrdPtr &thread)
{
	OmnNotImplementedYet;
	return "";
}

