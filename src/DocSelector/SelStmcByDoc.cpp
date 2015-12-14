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
// Modification History:
// 01/03/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocSelector/SelStmcByDoc.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/String.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"

AosDocSelStmcByDoc::AosDocSelStmcByDoc(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_STMC_BY_DOC, AosDocSelectorType::eStmcByDoc, reg)
{
}


AosDocSelStmcByDoc::~AosDocSelStmcByDoc()
{
}

AosXmlTagPtr 
AosDocSelStmcByDoc::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// This selector selects the statemachine instance doc for a given doc.
	// All statemachines are defined on a doc. If a statemachine manages multiple
	// docs or not any doc at all, the user should create an 'artificial' doc
	// for the statemachine instance. 
	//
	// Case 1: Doc Based Statemachines
	// In this case, each statemaching instance is associated with one and only one
	// doc. The statemachine instance doc's objid is composed as:
	// 		AosObjid::composeStmcInstDocObjid(objid)
	// where 'objid' is the objid of the doc bing associated with the statemachine. 
	// This selector selects the doc, and then converts the objid into the 
	// statemachine instance doc's objid.
	//
	// Case 2: Container Based Statemachines
	// In this case, the statemachine is for an entire contaner, not individual
	// docs. As an example, we may want to use a statemachine to control that
	// in order to create a new doc in a container, the container will do some
	// actions, go to the State "Family Being Created", it then requires 
	// another doc shall be created in another containe, goes to the State
	// "Family Member Created". It is possible to create multiple family
	// members. After that, it should go to the State "Enroll Family". 
	//
	// Case 3: Artificial Doc
	// If a statemachine does not involve docs at all or involve multiple docs,
	// an artificial doc, called Statemachine Instance Shell, is created. 
	// Every operation relating to that statemachine should refer to the 
	// objid of the shell. This shell is a doc.
	//
	// All the above cases associate a statemachine instance with a unique doc, 
	// regardless of whether it is an individual doc, a container or an artificial
	// doc. 
	//
	// The mission of this doc selector is to identify the doc, convert the doc's
	// objid into the statemachine instance doc's objid, and then retrieves the
	// statemachine instance doc. 
	//
	// Note that the instance may or may not have been created. 
	//
	// It assumes the following format:
	// 	<doc_sel ...>
	// 		<AOSTAG_VALUE_SEL .../>		// It selects the doc's objid
	// 	</doc_sel>
	if (!sdoc)
	{
		AosSetErrorUser(rdata, "missing_sdoc") << enderr;
		return 0;
	}

	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, AOSTAG_VALUE_SEL, rdata);
	aos_assert_rr(rslt, rdata, 0);

	OmnString objid = value.getStr();
	if (!rslt || objid == "")
	{
		AosSetError(rdata, "missing_objid") << ": " << sdoc->getAttrStr(AOSTAG_OBJID);
		return 0;
	}

	u64 docid = AosGetDocidByObjid(objid, rdata);
	if (!docid)
	{
		AosSetErrorUser(rdata, "failed_ret_docid_by_objid") << objid << enderr;
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	OmnString inst_objid = AosObjid::composeStmcInstDocObjid(docid);
	aos_assert_rr(inst_objid != "", rdata, 0);
	AosXmlTagPtr doc = AosGetDocByObjid(inst_objid, rdata);
	AOSLOG_LEAVE(rdata);
	return doc;
}


OmnString 
AosDocSelStmcByDoc::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}

