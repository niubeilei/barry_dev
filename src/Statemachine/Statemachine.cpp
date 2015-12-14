////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Statemachine/Statemachine.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Conds/Condition.h"
#include "Debug/Except.h"
#include "EventMgr/EventTriggers.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/LangDictObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEUtil/Objid.h"
#include "Statemachine/StmcUtil.h"
#include "Thread/Mutex.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include <vector>
using namespace std;

AosDocClientObjPtr AosStatemachine::smDocClient;
AosSeLogClientObjPtr AosStatemachine::smLogClient;

const OmnString sgEntryType_AccessDenied = "Access Denied";
// const OmnString sgEntryType_OperationNotFound = "Operation Not Found";
const OmnString sgEntryType_MissingStmcType = "Missing Statemachine Type";
const OmnString sgEntryType_StopStatemachine = "Stop Statemachine";
const OmnString sgEntryType_StopStatemachineDenied = "Stop Statemachine Denied";
const OmnString sgEntryType_StatemachineFinished = "Statemachine Finished";
const OmnString sgEntryType_StateTransit = "Statemachine Transit";
const OmnString sgEntryType_MissingFromState = "Missing From-State";
const OmnString sgEntryType_MissingToState = "Missing To-State";
const OmnString sgEntryType_AccessGranted = "Access Granted";
const OmnString sgEntryType_FailedAction = "Action Execution Failed";
const OmnString sgEntryType_ActionExecuted = "Action Executed";
const OmnString sgEntryType_InternalError = "Internal Error";
const OmnString sgEntryType_StatemachineStarted = "Statemachine Started";
const OmnString sgEventType_StateNotFound = "State Not Found";
const OmnString sgEntryType_InitStateIdEmpty = "Init State Id Empty";
const OmnString sgEntryType_InitStateSelectorEmpty = "Init State Selector Empty";
const OmnString sgEntryType_FailedCreateInstDoc = "Failed Create Instance Doc";
const OmnString sgEntryType_InternalLog = "Internal Log";
const OmnString sgEntryType_MissInstDocCreator = "Missing Instance Doc Creator";
const OmnString sgEntryType_MissingContainer = "Missing Container";
const OmnString sgEntryType_FailedCreateContainer = "Failed Create Container";
const OmnString sgEntryType_MissingInitStateSelector = "Missing Init State Selector";
const OmnString sgEntryType_ProcOpreation = "Process Operation";

AosStatemachine::AosStatemachine()
{
}


AosStatemachine::~AosStatemachine()
{
}


bool
AosStatemachine::procEvent(
		const OmnString &trigger,
		const AosXmlTagPtr &olddoc,
		const AosXmlTagPtr &newdoc,
		bool &continue_proc,
		const AosRundataPtr &rdata)
{
OmnNotImplementedYet;
return false;
/*
	// In our doc processing model, there are a number of triggers, 
	// which are defined in 'EventMgr/EventTriggers.h':
	// 		create
	// 		delete
	// 		modify
	// 		read
	// 		pre_create
	// 		pre_delete
	// 		pre_modify
	// 		pre_read
	// 		system
	// 		timer
	// All the 'pre_' triggers should have been processed through AosSecurityMgr.
	// This function processes 'create, delete, modify, read'. 
	// In other word, this function is called whenever a doc is created, modified, 
	// read, or deleted. It calls this function to trigger statemachine processing.
	// This function is called from 
	// 		AosEvent::checkEvent(olddoc, newdoc, trigger, rdata)
	// Note that a statemachine may block the normal event processing. In that 
	// case, 'continue_proc' is set to true. Otherwise, it is set to false.
	// In this case, AosEvent will continue the normal event processing.
	
	continue_proc = true;

	// 1. Check whether it is operation-based.
	if (!AosEventTrigger::isPostProcTriggers(trigger)) return true;

	// Set the docs to rdata. These may be needed in processing smart docs.
	// rdata->setDocByVar(olddoc, AOSARG_OLD_DOC);
	// rdata->setDocByVar(newdoc, AOSARG_NEW_DOC);

	vector<AosXmlTagPtr> &stmc_docs = rdata->getStatemachines();
	vector<AosXmlTagPtr> &inst_docs = rdata->getStmcInstDocs();
	aos_assert_rr(stmc_docs.size() == inst_docs.size(), rdata, false);
	bool flag = true;
	for (u32 i=0; i<inst_docs.size(); i++)
	{
		procEvent(stmc_docs[i], inst_docs[i], flag, rdata);
		if (!flag) continue_proc = false;
	}
	return true;
	*/
}


bool
AosStatemachine::procEvent(
		const AosXmlTagPtr &stmc_doc,
		const AosXmlTagPtr &inst_doc,
		bool &continue_proc,
		const AosRundataPtr &rdata)
{
OmnNotImplementedYet;
return false;
/*	// This function processes an operation on the statemachine 'inst_doc'.
	// It is called from AosEvent, when an operation was performed on a 
	// doc. Note that the pre-doc operations are filtered.
	//
	// This function assumes an access operation has been determined. There are 
	// two types of access operations, one is the System Defined Access
	// Operations, which are defined in SecOpr.h, and the other User Defined
	// Access Operations.
	aos_assert_rr(stmc_doc, rdata, false);
	aos_assert_rr(inst_doc, rdata, false);

	AosXmlTagPtr crt_state = getCrtState(stmc_doc, inst_doc, rdata);
	aos_assert_rr(crt_state, rdata, false);

	OmnString access_opr = rdata->getAccessOperation();
	if (access_opr == "")
	{
		AosSetErrorU(rdata, "missing_access_operation") 
			<< rdata->getSummaryInfo() << enderr;
		return false;
	}

	// Each state uses the subtag <AOSTAG_OPERATIONS> to define how 
	// operations are processed. This is the most important part of
	// statemachines. Operations are determined by AosSecurityMgr. 
	// There are two types of operations: User Defined Operations and
	// System Defined Operations. All are called Access Operations.
	// If the operation 'access_opr' maps to one of the subtags 
	// below, it will evaluate all the conditions. If a condition
	// evaluates true, its associated actions are run.
	// 	<state ...>
	// 		<AOSTAG_OPERATIONS>
	// 			<opr_id ...>
	// 				<cond ...>
	// 					<action .../>
	// 					<action .../>
	// 					...
	// 				</cond>
	// 				...
	// 			</opr_id>
	// 			...
	// 		</AOSTAG_OPERATIONS>
	// 	</state>
	rdata->setArg1(AOSARG_TRANSIT_STATE, "");
	continue_proc = true;
	AosXmlTagPtr operations = crt_state->getFirstChild(AOSTAG_OPERATIONS);
	if (!operations)
	{
		// There are not operation based actions. Do nothing.
		return true;
	}

	AosXmlTagPtr operation = operations->getFirstChild(access_opr);
	if (!operation)
	{
		// No operation found. 
		return true;
	}

	// Operation found. Try all the conditions.
	rdata->setCrtStatemachine(stmc_doc, inst_doc);
	AosXmlTagPtr cond = operation->getFirstChild();
	if (!cond) return true;

	// Some conditions are found. 
	OmnString term = AOSTERM("proc_operation", rdata);
	logOperation(AOSTERM("proc_operation", rdata), __FILE__, __LINE__, 
			stmc_doc, inst_doc, sgEntryType_ProcOpreation, crt_state, access_opr, rdata);
	int guard = eMaxConds;
	rdata->setArg1(AOSARG_CONTINUE_PROC, "");
	while (guard-- && cond)
	{
		// Make sure it is a condition
		if (cond->getAttrStr(AOSTAG_TYPE) == AOSOTYPE_CONDITION)
		{
			if (AosCondition::evalCondStatic(cond, rdata))
			{
				// The condition evaluates to true. Run the actions. 
				// 	<cond ...>
				// 		<action .../>
				// 		<action .../>
				// 		...
				// 	</cond>
				runActions(cond, AOSVALUE_ACTSRC_OPERATION, crt_state,
						stmc_doc, inst_doc, rdata);
			}
		}
		cond = operation->getNextChild();
	}

	// Check whether it wants to block the normal event processing
	if (rdata->getArg1(AOSARG_CONTINUE_PROC) == "false")
	{
		continue_proc = false;
	}
	else
	{
		// Check whether the state forces the blocking
		if (crt_state->getAttrBool(AOSTAG_BLOCK_NORMAL_EVENT_PROC, false))
		{
			continue_proc = false;
		}
	}

	if (guard == 0)
	{
		// This is a serious problem
		AosSetErrorU(rdata, "internal_error") << enderr;
		return false;
	}

	// Check whether it needs to transit state
	OmnString next_state_id = rdata->getArg1(AOSARG_TRANSIT_STATE);
	guard = eMaxTransitStates;
	AosXmlTagPtr crtstate = crt_state;
	while (guard-- && next_state_id != "")
	{
		// It needs to transit to the next state
		AosXmlTagPtr next_state = getState(next_state_id, stmc_doc, rdata);
		if (!next_state)
		{
			AosSetErrorU(rdata, "invalid_state_id") 
				<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
			break;
		}
		rdata->setArg1(AOSARG_TRANSIT_STATE, "");
		transitState(crtstate, next_state, stmc_doc, inst_doc, rdata);
		next_state_id = rdata->getArg1(AOSARG_TRANSIT_STATE);
		crtstate = next_state;
	}

	if (guard == 0)
	{
		// This is a serious problem
		AosSetErrorU(rdata, "internal_error") << enderr;
		return false;
	}
	return true;
	*/
}


bool
AosStatemachine::startStatemachineBySdoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// This is called from the action StartStatemachine. The smartdoc 'sdoc' 
	// should have a doc selector that selects the statemachine to start:
	// 	<sdoc ...>
	// 		<AOSTAG_STATEMACHINE_SELECTOR .../>		// The statemachine selector
	// 		...
	// 	</sdoc>
	if (!sdoc)
	{
		AosSetError(rdata, "missing_sdoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 1. Determine the statemachine. This is done through the Doc Selector
	// AOSTAG_STATEMACHINE_SELECTOR.
	AosDocSelObjPtr docsel_obj = AosDocSelObj::getDocSelector();
	aos_assert_r(docsel_obj, false);
	AosXmlTagPtr stmc_doc = docsel_obj->selectDoc(
			sdoc, AOSTAG_STATEMACHINE_SELECTOR, rdata);
	if (!stmc_doc)
	{
		// Did not find the statemachine. This is an error
		AosSetErrorU(rdata, "failed_retrieve_statemachine") 
			<< sdoc->getAttrStr(AOSTAG_OBJID) << enderr;
		return false;
	}

	// Found the statemachine doc. Continue the processing.
	return createStatemachine(sdoc, stmc_doc, rdata);
}


bool 
AosStatemachine::createStatemachine(
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &stmc_doc, 
		const AosRundataPtr &rdata)
{
	// This function creates a statemachine instance based on 'sdoc' and 'stmc_doc',
	// where 'sdoc' is the smart doc. If the statemachine is not created by 
	// a smart doc, 'sdoc' is the same as 'stmc_doc'.
	aos_assert_rr(sdoc, rdata, false);
	aos_assert_rr(stmc_doc, rdata, false);

	// Making sure there is the log container in 'stmc_doc'. If not, 
	// it is an error. It will fail the operation.
	OmnString logctnr_objid = stmc_doc->getAttrStr(AOSTAG_LOG_CTNR_OBJID);
	if (logctnr_objid == "")
	{
		AosSetErrorU(rdata, "missing_logctnr_objid") 
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Retrieve the parent container in which the instance doc is supposed to be
	OmnString ctnr_objid = sdoc->getAttrStr(AOSTAG_INST_CTNR_OBJID);
	if (ctnr_objid == "")
	{
		AosSetErrorU(rdata, "missing_stmc_inst_coid")
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		AOSLOG_LEAVE(rdata);
		createLog(AOSTERM("access_denied", rdata.getPtr()), __FILE__, __LINE__, 
				sdoc, stmc_doc, 0, 0, sgEntryType_AccessDenied, rdata);
		return false;
	}

	// Determine the instance doc objid
	OmnString objid = determineInstDocObjid(stmc_doc, rdata);
	if (objid == "") 
	{
		AosSetErrorU(rdata, "failed_ret_stmcinoid01")
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		return false;
	}
	
	// Check whether the statemachine has been started
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_rr(smDocClient, rdata, false);
	AosXmlTagPtr inst_doc = smDocClient->getDocByObjid(objid, rdata);
	if (inst_doc)
	{
		// The statemachine has already been started. 
		return true;
	}

	// Check Security
	if (!AosSecurityMgrObj::getSecurityMgr()->checkCreateStatemachine(stmc_doc, rdata))
	{
		// Access denied.
		createLog(AOSTERM("access_denied", rdata.getPtr()), __FILE__, __LINE__, 
				sdoc, stmc_doc, 0, 0, sgEntryType_AccessDenied, rdata);
		return false;
	}

	// Retrieve the creator
	AosXmlTagPtr creator = stmc_doc->getFirstChild(AOSTAG_INSTANCEDOC_CREATOR);
	if (!creator)
	{
		// There is no creator. This is an error.
		AosSetErrorU(rdata, "missing_inst_doc_creator") 
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		createLog(AOSTERM("missing_inst_doc_creator", rdata.getPtr()), __FILE__, __LINE__, 
				sdoc, stmc_doc, 0, 0, sgEntryType_MissInstDocCreator, rdata);
		return 0;
	}

	// Check whether the container is already there
	AosXmlTagPtr ctnr = smDocClient->getDocByObjid(ctnr_objid, rdata);
	if (!ctnr)
	{
		// The container is not there yet. Check whether it should create 
		// the container.
		if (!sdoc->getAttrBool(AOSTAG_CREATE_CONTAINER, true))
		{
			// Not allowed to create the container. 
			AosSetErrorU(rdata, "missing_container") 
				<< stmc_doc->getAttrStr(AOSTAG_OBJID)
				<< ":" << ctnr_objid << enderr;
			createLog(AOSTERM("missing_container", rdata.getPtr()), __FILE__, __LINE__, 
				sdoc, stmc_doc, 0, 0, sgEntryType_MissingContainer, rdata);
			AOSLOG_LEAVE(rdata);
			return false;
		}

		bool is_public = sdoc->getAttrBool(AOSTAG_CTNR_IS_PUBLIC, false);

		// Need to create the container. Note that that contain requires a 
		// container, which may or may not have been created yet. Retrieve
		// the instance container's container objid.
		OmnString ctnr_parent_objid = sdoc->getAttrStr(AOSTAG_CTNR_PARENT_OBJID);
		OmnString ctnr_docstr = "<ctnr ";
		if (ctnr_parent_objid != "")
		{
			ctnr_docstr << AOSTAG_PARENTC << "=\"" << ctnr_parent_objid << "\" ";
		}
		ctnr_docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER 
			<< "\"" << AOSTAG_STYPE << "=\"" << AOSSTYPE_STMC_INST_CTNR
			<< "\"" << AOSTAG_SDOC_CREATOR << "=\"" << sdoc->getAttrStr(AOSTAG_OBJID) 
			<< "\"/>";
		ctnr = smDocClient->createDocSafe1(rdata, ctnr_docstr, rdata->getCid(),
				"", is_public, true, false, false, false, true);
		if (!ctnr)
		{
			// Failed creating the container. 
			createLog(AOSTERM("failed_crt_container", rdata.getPtr()), __FILE__, __LINE__, sdoc, stmc_doc, 
					0, 0, sgEntryType_FailedCreateContainer, rdata);
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	// Check whether it is to create a public or private instance doc.
	bool is_public = sdoc->getAttrBool(AOSTAG_INST_DOC_PUBLIC, false);

	// Create the doc. A statemachine instance doc is in the form:
	// 	<stmcinst
	// 		AOSTAG_OBJID="xxx"
	// 		AOSTAG_OTYPE=AOSOTYPE_STMC_INST_DOC
	// 		AOSTAG_PARENTC="xxx"
	// 		AOSTAG_STMC_OBJID="xxx"
	// 		AOSTAG_STMCTYPE="xxx">
	// 		<
	OmnString stmc_type = stmc_doc->getAttrStr(AOSTAG_STMCTYPE);
	if (stmc_type == "") stmc_type = AOSSTMCTYPE_DOC_BASED;

	OmnString docstr = "<stmcinst ";
	docstr << AOSTAG_STMC_OBJID << "=\"" << stmc_doc->getAttrStr(AOSTAG_OBJID)
		<< "\" " << AOSTAG_OBJID << "=\"" << objid 
		<< "\" " << AOSTAG_OTYPE << "=\"" << AOSOTYPE_STMC_INST_DOC
		<< "\" " << AOSTAG_STMCTYPE << "=\"" << stmc_type
		<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid;
	if (is_public) docstr << "\" " << AOSTAG_PUBLIC_DOC << "=\"true\"";

	// Create the states nodes
	AosXmlTagPtr states = stmc_doc->getFirstChild(AOSTAG_STATES);
	AosXmlTagPtr state;
	if (!states || !(state = states->getFirstChild()))
	{
		// This is not allowed.
		AosSetErrorU(rdata, "missing_states01") 
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		return false;
	}

	docstr << "<" << AOSTAG_STATES << ">";
	int guard = eMaxStates;
	OmnString dft_status = stmc_doc->getAttrStr(AOSTAG_STATE_DFT_STATUS);
	if (dft_status == "") dft_status = AOSVALUE_FUTURE;
	while (guard-- && state)
	{
		docstr << "<" << state->getTagname() << " "
			<< AOSTAG_STATUS << "=\"" << dft_status << "\"";

		// Handle state id
		OmnString sid = state->getAttrStr(AOSTAG_STATE_ID);
		if (sid != "") docstr << " " << AOSTAG_STATE_ID << "=\"" << sid << "\"";

		// Handle state name 
		OmnString sname  = state->getAttrStr(AOSTAG_STATE_NAME);
		if (sname != "") docstr << " " << AOSTAG_STATE_NAME << "=\"" << sname << "\"";

		docstr << "/>";
		state = states->getNextChild();
	}

	if (guard <= 0)
	{
		AosSetErrorU(rdata, "internal_error") 
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
	}

	docstr << "</stmcinst>";
	AosXmlTagPtr working_doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	if (!working_doc)
	{
		AosSetError(rdata, "internal_error") << enderr;
		createLog(AOSTERM("internal_error", rdata.getPtr()), __FILE__, __LINE__, 
				sdoc, stmc_doc, 0, sgEntryType_InternalLog, rdata);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Need to go over all the states again to run the actions as needed.
	AosXmlTagPtr working_states = working_doc->getFirstChild(AOSTAG_STATES);
	aos_assert_rr(working_states, rdata, false);
	AosXmlTagPtr working_state = working_states->getFirstChild();
	aos_assert_rr(working_state, rdata, false);

	state = states->getFirstChild();
	guard = eMaxStates;

	AosActionObjPtr action_obj = AosActionObj::getActionObj();
	aos_assert_r(action_obj, false);

	while (guard-- && state)
	{
		aos_assert_rr(working_state, rdata, false);

		AosXmlTagPtr actions = state->getFirstChild(AOSTAG_CREATION_ACTIONS);
		rdata->setWorkingDoc(working_state, false);
		if (actions)
		{
			action_obj->runActions(actions, rdata);
		}

		state = states->getNextChild();
		working_state = working_states->getNextChild();
	}

	if (guard <= 0)
	{
		AosSetErrorU(rdata, "internal_error") 
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
	}

	rdata->setWorkingDoc(working_doc, true);
	// Chen Ding, 11/28/2012
	// AosSmartDocObjPtr sdoc_obj = AosSmartDocObj::getSmartDocObj();
	// aos_assert_r(sdoc_obj, false);
	inst_doc = AosSmartDoc::createDoc(creator, rdata);
	if (!inst_doc)
	{
		// Failed creating the instance doc.
		createLog(AOSTERM("failed_crt_stmc_inst_doc", rdata.getPtr()), __FILE__, __LINE__, 
				sdoc, stmc_doc, 0, sgEntryType_FailedCreateInstDoc, rdata);
		return false;
	}

	// 4. Start the statemachine.
	return startStatemachine(sdoc, stmc_doc, inst_doc, rdata);
}


bool
AosStatemachine::startStatemachine(
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &stmc_doc,
		const AosXmlTagPtr &inst_doc,
		const AosRundataPtr &rdata)
{
	// This function starts the statemachine 'stmc_doc' on the instance
	// 'inst_doc'. The instance doc should have a current state. If not, 
	// it is an error. Otherwise, it checks whether the statemachine
	// has the 'start statemachine actions'. If yes, it runs those 
	// actions. After that, it runs the entering actions on the 
	// initial state. 
	//
	// This function assumes the security check was done.
	
	// 1. Select the initial state
	// 	<statemachine ...>
	// 		<AOSTAG_INIT_STATE_SEL>
	// 			<cond .../>
	// 			<cond .../>
	// 			...
	// 		</AOSTAG_INIT_STATE_SEL>
	// 		...
	// 	</statemachine>
	aos_assert_rr(stmc_doc, rdata, false);
	aos_assert_rr(inst_doc, rdata, false);

	AosXmlTagPtr init_state_sel = stmc_doc->getFirstChild(AOSTAG_INIT_STATE_SEL);
	if (!init_state_sel)
	{
		AosSetErrorU(rdata, "missing_init_state_sel") << ": " 
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		createLog(AOSTERM("missing_init_state_sel", rdata.getPtr()), __FILE__, __LINE__, 
				sdoc, stmc_doc, inst_doc, sgEntryType_MissingInitStateSelector, rdata);
		return false;
	}

	AosXmlTagPtr cond = init_state_sel->getFirstChild();
	if (!cond)
	{
		AosSetErrorU(rdata, "init_state_sel_empty")
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		createLog(AOSTERM("init_state_sel_empty", rdata.getPtr()), __FILE__, __LINE__, 
				sdoc, stmc_doc, inst_doc, sgEntryType_InitStateSelectorEmpty, rdata);
		return false;
	}

	int guard = eMaxInitSelConds;
	while (guard-- && cond)
	{
		if (AosCondition::evalCondStatic(cond, rdata))
		{
			// The condition evaluates to true. The state id is in the condition.
			OmnString state_id = cond->getAttrStr(AOSTAG_STATE_ID);
			if (state_id == "")
			{
				AosSetErrorU(rdata, "init_state_id_empty")
					<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
				createLog(AOSTERM("init_state_id_empty", rdata.getPtr()), __FILE__, __LINE__, 
						sdoc, stmc_doc, inst_doc, sgEntryType_InitStateIdEmpty, rdata);
				return false;
			}

			AosXmlTagPtr state = getState(state_id, stmc_doc, rdata);
			if (!state)
			{
				AosSetErrorU(rdata, "state_not_found")
					<< state_id << ". " 
					<< AOSTERM("statemachine", rdata.getPtr()) << ": "
					<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
				createLog(AOSTERM("state_not_found", rdata.getPtr()), __FILE__, __LINE__, 
						sdoc, stmc_doc, inst_doc, sgEventType_StateNotFound, rdata);
				return false;
			}

			createLog(AOSTERM("statemachine_started", rdata.getPtr()), __FILE__, __LINE__, 
					sdoc, stmc_doc, inst_doc, sgEntryType_StatemachineStarted, rdata);
			enteringState(0, state, stmc_doc, inst_doc, rdata);
			return true;
		}
		cond = init_state_sel->getNextChild();
	}

	if (guard == 0)
	{
		// This is serious problem
		AosSetErrorU(rdata, "internal_error") << enderr;
		createLog(AOSTERM("internal_error", rdata.getPtr()), __FILE__, __LINE__, 
				sdoc, stmc_doc, inst_doc, sgEntryType_InternalError, rdata);
		return false;
	}

	return true;
}


bool
AosStatemachine::startStatemachines(const AosRundataPtr &rdata)
{
	// This is called from SecurityMgr. It checks whether to start statemachines.
	// Starting statemachines are configured in containers through the following 
	// subtag:
	// 	<ctnr ...>
	// 		<AOSTAG_START_STATEMACHINES>
	// 			<cond ...>statemachine,statemachine,...</cond>
	// 			<cond ...>statemachine,statemachine,...</cond>
	// 			...
	// 		</AOSTAG_START_STATEMACHINES>
	// 	</ctnr>
OmnNotImplementedYet;
return false;
/*	AosXmlTagPtr ctnr = rdata->getAccessedCtnr();
	if (!ctnr) return false;
	//aos_assert_rr(ctnr, rdata, false);
	AosXmlTagPtr subtag = ctnr->getFirstChild(AOSTAG_START_STATEMACHINES);
	if (!subtag)
	{
		// No starting statemachine configuration. Do nothing.
		return true;
	}

	AosXmlTagPtr cond = subtag->getFirstChild();
	int guard = eMaxStatemachinesToStart;
	while (guard-- && cond)
	{
		// Each 'cond' is a condition
		if (AosCondition::evalCondStatic(cond, rdata))
		{
			// It evaluates true. Start the statemachines.
			startStatemachines(cond, ctnr, rdata);
		}
		cond = subtag->getNextChild();
	}

	aos_assert_rr(guard > 0, rdata, false);
	return true;
	*/
}


bool
AosStatemachine::startStatemachines(
		const AosXmlTagPtr &stmc_tag, 
		const AosXmlTagPtr &ctnr, 
		const AosRundataPtr &rdata)
{
	// This function assumes 'stmc_tag' is a tag in the following format:
	// 	<stmc_tag ...>statemachine,statemachine,...</stmc_tag>
	// It starts all the statemachines in this tag.
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_rr(smDocClient, rdata, false);

	aos_assert_rr(stmc_tag, rdata, false);
	aos_assert_rr(ctnr, rdata, false);
	OmnString statemachines = stmc_tag->getNodeText();
	if (statemachines == "")
	{
		AosSetErrorU(rdata, "missing_statemachines") 
			<< ctnr->getAttrStr(AOSTAG_OBJID) << enderr;
		return false;
	}

	OmnStrParser1 parser(statemachines, ",");
	OmnString stmc_objid;
	int guard = eMaxStatemachinesToStart;
	while (guard-- && ((stmc_objid = parser.nextWord()) != ""))
	{
		// Found a statemachine. Need to start it.
		AosXmlTagPtr stmc_doc = smDocClient->getDocByObjid(stmc_objid, rdata);
		if (!stmc_doc)
		{
			OmnAlarm << "Failed retrieving statemachine: " << stmc_objid << enderr;
		}
		else
		{
			createStatemachine(stmc_doc, stmc_doc, rdata);
		}
	}
	aos_assert_rr(guard > 0, rdata, false);
	return true;
}


bool
AosStatemachine::lookupStatemachines(const AosRundataPtr &rdata)
{
OmnNotImplementedYet;
return false;
/*	// This function is called from SecurityMgr. It looks up statemachines based
	// on the access. Note that this function is called after the starting 
	// statemachine function was called. If a statemachine needs to be started, 
	// it should have started. 
	//
	// Note that this function looks up statemachine instances.
	//
	// In most cases, statemachine instances are associated through one of the following:
	// 	1. Select statemachine by Doc (AosSelStmcByDoc)
	// 	2. Select statemachine by Ctnr (AosSelStmcByCtnr)
	// 	3. Select statemachine by User Groups (AosSelStmcByUserGroups)
	// 	4. Select statemachine by User Roles (AosSelStmcByUserRoles)
	// 	5. Select statemachine by User Domains (AosSelStmcByUserDomains)
	// Statemachine lookups are configured in containers:
	// 	<ctnr ...>
	// 		<AOSTAG_STATEMACHINE_LOOKUPS>
	// 			<cond ...>
	// 				<value_selector .../>
	// 				<value_selector .../>
	// 				...
	// 			</cond>
	// 			...
	// 		</AOSTAG_STATEMACHINE_LOOKUPS>
	// 		...
	// 	</ctnr>
	//
	// If a condition evaluates to true, it uses its doc selectors to select
	// docs. These docs are the ones associated with statemachine instances.
	// The statemachine instance objids are calculated by:
	// 		AosObjid::composeStmcInstDocObjid(docid);
	
	AosXmlTagPtr ctnr = rdata->getAccessedCtnr();
	if (!ctnr) return true;

	AosXmlTagPtr subtag = ctnr->getFirstChild(AOSTAG_STATEMACHINE_LOOKUPS);
	if (!subtag) 
	{
		// No statemachine lookups. Do nothing.
		return true;
	}

	AosXmlTagPtr cond = subtag->getFirstChild();
	int guard = eMaxStatemachinesToStart;
	while (guard-- && cond)
	{
		// Each 'cond' is a condition
		if (AosCondition::evalCondStatic(cond, rdata))
		{
			// It evaluates true. Lookup the statemachines.
			lookupStatemachines(cond, ctnr, rdata);
		}
		cond = subtag->getNextChild();
	}

	aos_assert_rr(guard > 0, rdata, false);
	return true;
	*/
}


bool
AosStatemachine::lookupStatemachines(
		const AosXmlTagPtr &cond, 
		const AosXmlTagPtr &ctnr, 
		const AosRundataPtr &rdata)
{
OmnNotImplementedYet;
return false;
/*	// 'cond' is in the following format:
	// 	<cond ...>
	// 		<value_selector .../>
	// 		<value_selector .../>
	// 		...
	// 	</cond>
	// This function runs all the value selectors. Each selector returns a 
	// value, which is an objid. For each objid, it converts to a statemachine
	// instance doc objid. It then retrieves the doc. If found, it is the
	// doc to use.
	aos_assert_rr(cond, rdata, false);
	aos_assert_rr(ctnr, rdata, false);
	AosXmlTagPtr selectors = cond->getFirstChild();
	if (!selectors)
	{
		AosSetErrorU(rdata, "missing_stmc_selectors") 
			<< ctnr->getAttrStr(AOSTAG_OBJID) << enderr;
		return false;
	}

	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_rr(smDocClient, rdata, false);

	int guard = eMaxCondsToLookup;
	AosXmlTagPtr selector = selectors->getFirstChild();
	while (guard-- && selector)
	{
		// Found a statemachine. Need to start it.
		AosValueRslt value;
		bool rslt = AosValueSel::getValueStatic(value, selector, rdata);
		OmnString objid = value.getValueStr("", rslt);
		if (rslt && objid != "")
		{
			// Found an object. Convert the objid.
			u64 did = smDocClient->getDocidByObjid(objid, rdata);
			if (!did)
			{
				AosSetErrorU(rdata, "internal_error") << objid << enderr;
				return false;
			}

			OmnString inst_objid = AosObjid::composeStmcInstDocObjid(did);
			AosXmlTagPtr inst_doc = smDocClient->getDocByObjid(inst_objid, rdata);
			if (inst_doc)
			{
				// Found a statemachine instance. Appended it to 'rdata'.
				AosXmlTagPtr stmc_doc = getStatemachine(inst_doc, rdata);
				if (!stmc_doc)
				{
					AosSetError(rdata, "failed_ret_statemachine") << ": " 
						<< inst_doc->getAttrStr(AOSTAG_OBJID);
					OmnAlarm << rdata->getErrmsg() << enderr;
				}
				else
				{
					rdata->addStatemachine(stmc_doc, inst_doc);
				}
			}
		}
		selector = selectors->getNextChild();
	}
	aos_assert_rr(guard > 0, rdata, false);
	return true;
	*/
}


bool
AosStatemachine::enteringState(
		const AosXmlTagPtr &from_state,
		const AosXmlTagPtr &to_state,
		const AosXmlTagPtr &stmc_doc,
		const AosXmlTagPtr &inst_doc,
		const AosRundataPtr &rdata)
{
	// This function is called when a state is entered. It runs the
	// entering actions, if any.
	aos_assert_rr(to_state, rdata, false);
	AosXmlTagPtr actions = to_state->getFirstChild(AOSTAG_ENTER_ACTIONS);
	if (actions) 
	{
		runActions(actions, AOSVALUE_ACTSRC_ENTERACTIONS, 
				to_state, stmc_doc, inst_doc, rdata);
	}

	// Check whether the state wants to transit to another state
	// upon entering
	OmnString next_state_id = to_state->getAttrStr(AOSTAG_TRANSIT_ON_ENTER);
	if (next_state_id != "")
	{
		// It does need to transit to another state
		AosXmlTagPtr next_state = getState(next_state_id, stmc_doc, rdata);
		if (next_state)
		{
			transitState(to_state, next_state, stmc_doc, inst_doc, rdata);
		}
	}
	return true;
}


bool
AosStatemachine::runActions(
		const AosXmlTagPtr &actions,
		const OmnString &action_source,
		const AosXmlTagPtr &state,
		const AosXmlTagPtr &stmc_doc,
		const AosXmlTagPtr &inst_doc,
		const AosRundataPtr &rdata)
{
	// This function runs all the actions in 'actions'.
	// 	<actions ...>
	// 		<action .../>
	// 		<action .../>
	// 		...
	// 	</actions>
	aos_assert_rr(stmc_doc, rdata, false);
	aos_assert_rr(inst_doc, rdata, false);

	if (!actions) return true;
	AosXmlTagPtr action = actions->getFirstChild();
	if (!action) return true;
	int guard = eMaxActions;
	OmnString logctnr_objid = stmc_doc->getAttrStr(AOSTAG_LOG_CTNR_OBJID);
	aos_assert_rr(logctnr_objid != "", rdata, false);
	AosActionObjPtr action_obj = AosActionObj::getActionObj();
	aos_assert_r(action_obj, false);
	while (guard-- && action)
	{
		bool rslt = action_obj->runAction(action, rdata);
		if (rslt)
		{
			logAction(AOSTERM("action_executed", rdata.getPtr()), __FILE__, __LINE__, 
					stmc_doc, inst_doc, sgEntryType_ActionExecuted, 
					action_source, state,
					action_obj->getActionId(action), 
					rdata->getArg1(AOSARG_ACTION_EXECUTION_LOG), 
					rdata);
		}
		else
		{
			logAction(AOSTERM("failed_action", rdata.getPtr()), __FILE__, __LINE__, 
					stmc_doc, inst_doc, sgEntryType_FailedAction, 
					action_source, state,
					action_obj->getActionId(action), 
					rdata->getArg1(AOSARG_ACTION_EXECUTION_LOG), 
					rdata);
		}
		action = actions->getNextChild();
	}

	if (guard <= 0)
	{
		// This is a serious problem
		AosSetErrorU(rdata, "internal_error") << enderr;
	}
	return true;
}


bool
AosStatemachine::checkOperationPriv(
		const AosXmlTagPtr &stmc_doc,
		const AosXmlTagPtr &inst_doc,
		const AosRundataPtr &rdata)
{
	// This function checks wehther the operation 'opr_id' is allowed.
	// 'stmt_doc' is the statemachine doc and 'inst_doc' is the instance
	// doc of the statemachine.
	// The instance doc should tell its current state. If not, it is 
	// an error. Otherwise, it retrieves the state from the statemachine.
	// The state should have a map that tells which operation is allowed
	// or not.
	aos_assert_rr(inst_doc, rdata, false);
	aos_assert_rr(stmc_doc, rdata, false);
	AosXmlTagPtr crt_state = getCrtState(stmc_doc, inst_doc, rdata);
	aos_assert_rr(crt_state, rdata, false);

	// A state is in the following format:
	// 	<state ...>
	// 		<AOSTAG_ALLOWED_OPERATIONS ...>		// These are conditions
	// 			<cond .../>
	// 			<cond .../>
	// 			...
	// 		</AOSTAG_ALLOWED_OPERATIONS>
	// 		...
	// 	</state>
	AosXmlTagPtr conds = crt_state->getFirstChild(AOSTAG_ALLOWED_OPERATIONS);
	if (!conds)
	{
		// No operation-based controls. Allow it. 
		return true;
	}

	AosXmlTagPtr cond = conds->getFirstChild();
	int guard = eMaxConds;
	while (guard-- && cond)
	{
		if (AosCondition::evalCondStatic(cond, rdata))
		{
			// It is allowed. 
			createLog(AOSTERM("access_granted", rdata.getPtr()), __FILE__, __LINE__, 
				stmc_doc, inst_doc, 0, sgEntryType_AccessGranted, rdata);
			return true;
		}
	}

	createLog(AOSTERM("access_denied", rdata.getPtr()), __FILE__, __LINE__, 
				stmc_doc, inst_doc, crt_state, sgEntryType_AccessDenied, rdata);
	return false;
}


AosXmlTagPtr
AosStatemachine::getCrtState(
		const AosXmlTagPtr &stmc_doc,
		const AosXmlTagPtr &inst_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(stmc_doc, rdata, 0);
	aos_assert_rr(inst_doc, rdata, 0);

	// 1. Retrieve the current state.
	OmnString crt_state = inst_doc->getAttrStr(AOSTAG_CURRENT_STATE);
	if (crt_state == "")
	{
		AosSetErrorU(rdata, "missing_crt_state") 
			<< inst_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		return 0;
	}

	return getState(crt_state, stmc_doc, rdata);
}


AosXmlTagPtr
AosStatemachine::getState(
		const OmnString &state_id,
		const AosXmlTagPtr &stmc_doc,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr states = stmc_doc->getFirstChild(AOSTAG_STATES);
	if (!states)
	{
		AosSetErrorU(rdata, "missing_states_tag") 
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		return 0;
	}

	AosXmlTagPtr state = states->getFirstChild(state_id);
	if (!state)
	{
		AosSetErrorU(rdata, "missing_state")
			<< state_id << ". " 
			<< AOSTERM("statemachine", rdata.getPtr()) << ": "  
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		return 0;
	}

	return state;
}


bool
AosStatemachine::transitState(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
OmnNotImplementedYet;
return false;
/*	// This function transits state. The inputs are from 'rdata'. It does the
	// following:
	// 1. Run the 'from' state actions.
	// 2. Run the edge actions.
	// 3. Run the 'to' state actions.
	// 4. Log the state transition
	if (!sdoc)
	{
		AosSetErrorU(rdata, "missing_sdoc") << enderr;
		AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 1. Retrieve the statemachine
	AosXmlTagPtr stmc_doc = rdata->getCrtStatemachine();
	if (!stmc_doc)
	{
		AosSetErrorU(rdata, "missing_statemachine") << enderr;
		return false;
	}

	// 2. Retrieve the statemachine instance doc 
	AosXmlTagPtr inst_doc = rdata->getCrtStmcInstDoc();
	if (!inst_doc)
	{
		AosSetErrorU(rdata, "missing_stmc_inst_doc") << enderr;
		return false;
	}

	// 3. Retrieve the from state
	OmnString from_state_id = rdata->getArg1(AOSARG_FROM_STATE);
	if (from_state_id == "")
	{
		AosSetErrorU(rdata, "missing_from_state") << enderr;
		return false;
	}

	// 4. Retrieve the to state
	OmnString to_state_id = rdata->getArg1(AOSARG_TO_STATE);
	if (to_state_id == "")
	{
		AosSetErrorU(rdata, "missing_to_state") << enderr;
		return false;
	}

	AosXmlTagPtr from_state = getState(from_state_id, stmc_doc, rdata);
	if (!from_state)
	{
		AosSetErrorU(rdata, "missing_from_state") << from_state_id
			<< ":" << to_state_id << enderr;
		createLog(AOSTERM("missing_from_state", rdata), __FILE__, __LINE__, 
				sdoc, stmc_doc, inst_doc, 0, sgEntryType_MissingFromState, rdata);
		return false;
	}

	AosXmlTagPtr to_state = getState(to_state_id, stmc_doc, rdata);
	if (!to_state)
	{
		AosSetErrorU(rdata, "missing_to_state") << from_state_id
			<< ":" << to_state_id << enderr;
		createLog(AOSTERM("missing_to_state", rdata), __FILE__, __LINE__, 
				sdoc, stmc_doc, inst_doc, 0, sgEntryType_MissingToState, rdata);
		return false;
	}

	return transitState(from_state, to_state, stmc_doc, inst_doc, rdata);
	*/
}


bool
AosStatemachine::transitState(
		const AosXmlTagPtr &from_state, 
		const AosXmlTagPtr &to_state, 
		const AosXmlTagPtr &stmc_doc,
		const AosXmlTagPtr &inst_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(stmc_doc, rdata, false);
	aos_assert_rr(inst_doc, rdata, false);
	aos_assert_rr(from_state, rdata, false);
	aos_assert_rr(to_state, rdata, false);

	runActions(from_state, AOSTERM("exit_actions", rdata.getPtr()), 
			from_state, stmc_doc, inst_doc, rdata);
	runEdgeActions(from_state, to_state, stmc_doc, inst_doc, rdata);
	enteringState(from_state, to_state, stmc_doc, inst_doc, rdata);	
	logStateTransit(AOSTERM("state_transit", rdata.getPtr()), __FILE__, __LINE__, 
				stmc_doc, inst_doc, from_state, to_state, 
				sgEntryType_StateTransit, rdata);

	if (isStatemachineFinished(inst_doc, rdata))
	{
		// The statemachine finished.
		statemachineFinished(stmc_doc, inst_doc, rdata);
	}

	return true;
}


bool
AosStatemachine::statemachineFinished(
		const AosXmlTagPtr &stmc_doc, 
		const AosXmlTagPtr &inst_doc, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(inst_doc, rdata, false);
	createLog(AOSTERM("stmc_finished", rdata.getPtr()), __FILE__, __LINE__, 
				stmc_doc, inst_doc, 0, sgEntryType_StatemachineFinished, rdata);
	setStatemachineFinishedFlag(inst_doc);
	return true;
}


bool
AosStatemachine::runActions(
		const AosXmlTagPtr &state, 
		const OmnString &action_source,
		const AosXmlTagPtr &stmc_doc, 
		const AosXmlTagPtr &inst_doc, 
		const OmnString &tagname,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(state, rdata, false);
	aos_assert_rr(tagname != "", rdata, false);
	AosXmlTagPtr actions = state->getFirstChild(tagname);
	if (!actions) return true;
	return runActions(actions, action_source, state, stmc_doc, inst_doc, rdata);
}


bool
AosStatemachine::runEdgeActions(
		const AosXmlTagPtr &from_state, 
		const AosXmlTagPtr &to_state, 
		const AosXmlTagPtr &stmc_doc, 
		const AosXmlTagPtr &inst_doc, 
		const AosRundataPtr &rdata)
{
	// Each state has an Edge Action subtag: AOSTAG_EDGE_ACTIONS:
	// 	<state ...>
	// 		<AOSTAG_EDGE_ACTIONS>
	// 			<to_state_id ...>
	// 				<action .../>
	// 				<action .../>
	// 				...
	// 			</to_state_id>
	// 			<to_state_id ...>
	// 				<action .../>
	// 				<action .../>
	// 				...
	// 			</to_state_id>
	//			...
	// 		</AOSTAG_EDGE_ACTIONS>
	// 	</state>
	aos_assert_rr(from_state, rdata, false);
	aos_assert_rr(to_state, rdata, false);
	OmnString to_state_id = to_state->getAttrStr(AOSTAG_STATE_ID);
	aos_assert_rr(to_state_id != "", rdata, false);

	// 1. Retrieve the edge action subtag: AOSTAG_EDGE_ACTIONS
	AosXmlTagPtr edge_actions = from_state->getFirstChild(AOSTAG_EDGE_ACTIONS);
	if (!edge_actions) return true;

	// 2. Retrieve the corresponding edge: 'to_state_id'
	AosXmlTagPtr to_actions = edge_actions->getFirstChild(to_state_id);
	if (!to_actions) return true;

	// 3. Run actions
	runActions(to_actions, AOSTERM("edge_actions", rdata.getPtr()), from_state, stmc_doc, inst_doc, rdata);
	return true;
}


bool
AosStatemachine::stopStatemachine(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function stops a statemachine. It is called from the action.
	if (!sdoc)
	{
		AosSetError(rdata, "missing_sdoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 1. Determine the statemachine. This is done through the Doc Selector
	// AOSTAG_STATEMACHINE_SELECTOR.
	AosDocSelObjPtr docsel_obj = AosDocSelObj::getDocSelector();
	aos_assert_r(docsel_obj, false);
	AosXmlTagPtr stmc_doc = docsel_obj->selectDoc(
			sdoc, AOSTAG_STATEMACHINE_SELECTOR, rdata);
	if (!stmc_doc)
	{
		AosSetErrorU(rdata, "failed_retrieve_statemachine") 
			<< sdoc->getAttrStr(AOSTAG_OBJID) << enderr;
		return false;
	}

	// 2. Retrieve the instance doc
	AosXmlTagPtr inst_doc = docsel_obj->selectDoc(sdoc, 
			AOSTAG_INSTANCEDOC_SEL, rdata);
	if (!inst_doc)
	{
		// The instance doc is not there. It is possible that the statemachine
		// did not start at all, or it may be an error.
		AosSetErrorU(rdata, "failed_ret_stmc_inst_doc") 
			<< sdoc->getAttrStr(AOSTAG_OBJID) << enderr;
		return false;
	}

	// 3. Check Security
	if (!AosSecurityMgrObj::getSecurityMgr()->checkCreateStatemachine(stmc_doc, rdata))
	{
		// Access denied.
		createLog(AOSTERM("stop_stmc_denied", rdata.getPtr()), __FILE__, __LINE__, 
				sdoc, stmc_doc, inst_doc, sgEntryType_StopStatemachineDenied, rdata);
		return false;
	}

	// 4. Retrieve the reason
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, AOSTAG_REASON_VAL_SELECTOR, rdata);
	OmnString reason;
	if (!rslt)
	{
		reason = AOSTERM("unknown_from_sdoc", rdata.getPtr());
	}
	else
	{
		reason = value.getStr();
	}

	return stopStatemachine(reason, stmc_doc, inst_doc, rdata);
}


bool
AosStatemachine::stopStatemachine(
		const OmnString &reason,
		const AosXmlTagPtr &stmc_doc, 
		const AosXmlTagPtr &inst_doc,
		const AosRundataPtr &rdata)
{
	// This function stops the statemachine 'inst_doc'. It does the following:
	// 1. Run the stop statemachine actions, if any
	// 2. Set the instance doc status.
	// 3. Log the event
	aos_assert_rr(stmc_doc, rdata, false);
	aos_assert_rr(inst_doc, rdata, false);

	// Run stop actions
	AosXmlTagPtr crt_state = getCrtState(stmc_doc, inst_doc, rdata);
	AosXmlTagPtr actions = stmc_doc->getFirstChild(AOSTAG_STMC_STOP_ACTIONS);
	if (actions)
	{
		runActions(actions, AOSTERM("stmc_stop_actions", rdata.getPtr()), 
				crt_state, stmc_doc, inst_doc, rdata);
	}
	logStatemachineStop(AOSTERM("stmc_stopped", rdata.getPtr()), __FILE__, __LINE__, 0, 
			stmc_doc, inst_doc, crt_state, sgEntryType_StopStatemachine, 
			reason, rdata);
	stopInstance(stmc_doc, inst_doc, rdata);
	return true;
}


bool
AosStatemachine::stopInstance(
		const AosXmlTagPtr &stmc_doc, 
		const AosXmlTagPtr &inst_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(inst_doc, rdata, false);
	setInstDocStopFlag(inst_doc, rdata);
	return true;
}


bool
AosStatemachine::checkOperation(bool &denied, const AosRundataPtr &rdata)
{
OmnNotImplementedYet;
return false;
/*	// This function is called by AosSecurityMgr. It checks whether the
	// access is associated with statemachines. If no, it returns true
	// and 'denied' is set to false. Otherwise, it checks
	// whether the statemachines allow the operation. If yes, it returns
	// true and 'denied' is set to false. Otherwise, it returns true and
	// sets 'denied' to true.
	AOSLOG_ENTER_R(rdata, false);
	denied = false;
	vector<AosXmlTagPtr> &stmc_docs = rdata->getStatemachines();
	vector<AosXmlTagPtr> &inst_docs = rdata->getStmcInstDocs();
	aos_assert_rr(stmc_docs.size() == inst_docs.size(), rdata, false);
	if (inst_docs.size() <= 0) 
	{
		// No statemachines are associated with the operation. 
		// Do nothing.
		AOSLOG_LEAVE(rdata);
		return true;
	}

	// Normally there shall be at most one statemachines, but in the 
	// current implementations, we consider supporting multiple 
	// statemachines. If there are multiple statemachines, accesses
	// are denied if one of the statemachines denied the accesses.
	for (u32 i=0; i<inst_docs.size(); i++)
	{
		if (!checkOperationPriv(stmc_docs[i], inst_docs[i], rdata))
		{
			// It is rejected. 
			denied = true;
			AOSLOG_LEAVE(rdata);
			return true;
		}
	}

	// It is allowed.
	denied = false;
	AOSLOG_LEAVE(rdata);
	return true;
	*/
}


AosXmlTagPtr
AosStatemachine::getStatemachine(
		const AosXmlTagPtr &inst_doc,
		const AosRundataPtr &rdata)
{
	// This function retrieves the statemachine doc, which is stored in the
	// attribute AOSTAG_STMC_OBJID:
	aos_assert_rr(inst_doc, rdata, 0);
	OmnString stmc_objid = inst_doc->getAttrStr(AOSTAG_STMC_OBJID);
	aos_assert_rr(stmc_objid != "", rdata, 0);

	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_rr(smDocClient, rdata, 0);
	return smDocClient->getDocByObjid(stmc_objid, rdata);
}


bool 
AosStatemachine::createLogBody(
		OmnString &log,
		OmnString &logctnr_objid,
		const OmnString &contents, 
		const OmnString &fname,
		const int line,
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &stmc,
		const AosXmlTagPtr &inst,
		const AosXmlTagPtr &state,
		const OmnString &entry_type,
		const OmnString &attrs,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(stmc, rdata, false);
	logctnr_objid = stmc->getAttrStr(AOSTAG_LOG_CTNR_OBJID);
	aos_assert_rr(logctnr_objid != "", rdata, false);
	log = "<stmclog ";

	// Attributes: 
	// 		AOSTAG_OTYPE
	// 		AOSTAG_FILEPOS
	// 		AOSTAG_STMC_OBJID
	// 		AOSTAG_PARENTC
	// 		AOSTAG_ENTRY_TYPE
	log << AOSTAG_OTYPE << "=\"" << AOSOTYPE_LOG 
		<< "\" " << AOSTAG_FILEPOS << "=\"" << AosFilePosEncoder::getCode(fname, line)
		<< "\" " << AOSTAG_STMC_OBJID << "=\"" << stmc->getAttrStr(AOSTAG_OBJID) 
		<< "\" " << AOSTAG_ENTRY_TYPE << "=\"" << entry_type 
		<< "\" " << AOSTAG_PARENTC << "=\"" << logctnr_objid;

	// Handle Attribute: AOSTAG_STMC_INST_OBJID
	addLogAttr(log, inst, AOSTAG_OBJID, AOSTAG_STMC_INST_OBJID);

	// Handle Attribute: AOSTAG_SMARTDOC_OBJID 
	addLogAttr(log, sdoc, AOSTAG_OBJID, AOSTAG_SMARTDOC_OBJID);

	if (state)
	{
		log << " " << AOSTAG_STATENAME << "=\"" << state->getTagname() << "\"";
	}

	if (attrs != "") log << " " << attrs;

	// Add 'contents'
	log << "/>"
		<< "<" << AOSTAG_CONTENTS << "><![CDATA[" 
		<< contents << "]]></" << AOSTAG_CONTENTS << ">";
	return true;
}


OmnString
AosStatemachine::determineInstDocObjid(
		const AosXmlTagPtr &stmc_doc,
		const AosRundataPtr &rdata)
{
	// Each statemachine has a subtag:
	// 	<stmc ...>
	// 		<AOSTAG_INSTDOC_SELECTOR .../>	// a value selector
	// 		...
	// 	</stmc>
	// This subtag is used to select the objid of the doc that is 
	// associated with a statemachine. The objid is then converted to 
	// the objid of the statemachine instance.
	AOSLOG_ENTER_R(rdata, "");
	aos_assert_rr(stmc_doc, rdata, "");

	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, stmc_doc, AOSTAG_INSTDOC_SELECTOR, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_ret_stmcdoc01")
			<< stmc_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		createLog(AOSTERM("missing_stmc_type", rdata.getPtr()), __FILE__, __LINE__, 0, stmc_doc, 0, 
				sgEntryType_MissingStmcType, rdata);
		AOSLOG_LEAVE(rdata);
		return "";
	}

	// Convert the objid
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_rr(smDocClient, rdata, "");
	OmnString ss = value.getStr();
	u64 did = smDocClient->getDocidByObjid(ss, rdata);
	aos_assert_rr(did, rdata, "");
	return AosObjid::composeStmcInstDocObjid(did);
}


bool
AosStatemachine::sendLog(
		const OmnString &pctr_objid, 
		const AosXmlTagPtr &stmc_doc,
		const OmnString &log, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(stmc_doc, rdata, false);
	if (!smLogClient) smLogClient = AosSeLogClientObj::getLogClient();
	aos_assert_rr(smLogClient, rdata, false);
	return smLogClient->addLog(rdata, pctr_objid, 
			stmc_doc->getAttrStr(AOSTAG_STMC_LOGNAME), log);
}
