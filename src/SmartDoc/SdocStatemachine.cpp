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
// 2011/05/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocStatemachine.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Conds/Condition.h"
#include "Debug/Error.h"
#include "SEInterfaces/DocClientObj.h"
#include "DocSelector/DocSelector.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "DocServer/DocSvr.h"
#include "SeLogClient/SeLogClient.h"
#include "SearchEngine/DocServerCb.h"
#include "Security/Session.h"
#include "Util/StrSplit.h"
#include "XmlUtil/AccessRcd.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/WebRequest.h"

AosSdocStatemachine::AosSdocStatemachine(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_STATEMACHINE, AosSdocId::eStatemachine, flag)
{
}


AosSdocStatemachine::~AosSdocStatemachine()
{
}


bool
AosSdocStatemachine::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function runs a statemachine. 
	// Each statemachine has an instance data that records the current state
	// of the statemachine. The state instance data is stored in a doc's 
	// access record. 
	//
	// When this function is called, it retrieves the current state of the 
	// statemachine. It then checks whether it should transit the state to 
	// a new state. 
	AosAccessRcdPtr arcd = getAccessRecord(sdoc, rdata);
	if (!arcd)
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << ". Failed retrieving record!" << enderr;
		return false;
	}

	AosXmlTagPtr log_template = sdoc->getFirstChild(AOSTAG_STATE_LOG_TPLT);
	OmnString log_container = sdoc->getAttrStr(AOSTAG_LOG_CONTAINER);
	OmnString logname = sdoc->getAttrStr(AOSTAG_LOG_NAME);
	if (log_container == "")
	{
		log_container = arcd->getParentContainerObjid();
		if (log_container == "")
		{
			AosSetError(rdata, AosErrmsgId::eInternalError);
			OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
			return false;
		}
	}

	// Get the current state
	OmnString statename = arcd->getStatemachineState();
	if (statename == "") statename = AOSSTATE_START;

	// Get the state from 'sdoc'.
	AosXmlTagPtr states = sdoc->getFirstChild("states");
	if (!states)
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << ". Missing states! " << enderr;
		return false;
	}

	AosXmlTagPtr state_def = states->getFirstChild(statename);
	if (!state_def)
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << ". Unrecognized state: " << statename << enderr;
		return false;
	}
	
	AosXmlTagPtr events = state_def->getFirstChild("events");
	if (!events)
	{
		// Event not recognized by this state machine.
		createErrorLog(log_container, logname, statename, "", "", 
				"Events not found", arcd->getDocid(), sdoc, rdata);
		return true;	
	}

	// Determine the event.
	AosXmlTagPtr event_def;
	OmnString event = checkEvent(sdoc, event_def, rdata);
	if (event == "")
	{
		// Nothing happened. Do nothing.
		rdata->setOk();
		return true;
	}

	// Determine whether it will trigger state transits.
	// 'state_def' should be in the form:
	// 	<state ...>
	// 		<event .../>
	// 		<event .../>
	// 		...
	// 	</state>

	AosXmlTagPtr event_branch = events->getFirstChild(event);
	if (!event_branch)
	{
		// Event not recognized by this state machine.
		createErrorLog(log_container, logname, statename, "", event, 
				"Event not found", arcd->getDocid(), sdoc, rdata);
		return true;
	}

	// An event branch is found. This should trigger the state transition.
	OmnString next_statename = event_branch->getAttrStr(AOSTAG_NEXT_STATE);
	if (next_statename == "")
	{
		// This is an error
		AosSetError(rdata, AosErrmsgId::eMissingNextState);
		OmnAlarm << rdata->getErrmsg() << ". Missing next state" << enderr;
		createErrorLog(log_container, logname, statename, "", event, 
				"Missing next state", arcd->getDocid(), sdoc, rdata);
		return true;
	}

	AosXmlTagPtr next_state = states->getFirstChild(next_statename);
	if (!next_state)
	{
		AosSetError(rdata, AosErrmsgId::eMissingNextState);
		OmnAlarm << rdata->getErrmsg() << ". Missing next state: " << next_statename << enderr;
		createErrorLog(log_container, logname, statename, next_statename, event,
				"Failed retrieve next state", arcd->getDocid(), sdoc, rdata);
		return true;
	}

	// Run the state exit actions for the current state.
	// Chen Ding, 2013/01/27
	/*
	vector<AosXmlTagPtr> exist_logs = rdata->getLogs();
	bool rslt = AosSdocAction::runActions(state_def, AOSTAG_EXIT_ACTIONS, rdata);
	vector<AosXmlTagPtr> logs = rdata->getLogs();
	if (logs.size() > 0)
	{
		createActionLogs(statename, next_statename, event, log_container, logname, 
				arcd->getDocid(), logs, rdata);
	}

	// Run the edge actions
	rslt = AosSdocAction::runActions(event_branch, AOSTAG_TRANSIT_ACTIONS, rdata);
	logs = rdata->getLogs();
	rdata->clearLogs();
	if (logs.size() > 0)
	{
		createActionLogs(statename, next_statename, event, log_container, logname, 
				arcd->getDocid(), logs, rdata);
	}

	// Run the entering actions
	rslt = AosSdocAction::runActions(next_state, AOSTAG_ENTER_ACTIONS, rdata);
	logs = rdata->getLogs();
	rdata->clearLogs();
	if (logs.size() > 0)
	{
		createActionLogs(statename, next_statename, event, log_container, logname, 
				arcd->getDocid(), logs, rdata);
	}

	// Chen Ding, 2013/01/27
	// rdata->setLogs(exist_logs);

	// Create the state transit log
	bool log_added = false;
	if (log_template)
	{
		rdata->setArg1(AOSARG_FROM_STATE, statename);
		rdata->setArg1(AOSARG_TO_STATE, next_statename);
		rdata->setArg1(AOSARG_EVENT, event);

		// it must be createlog ,but the log is not work ,
		// so temporary createdoc
		AosSmartDoc::runSmartdoc(log_template, rdata);
		AosXmlTagPtr log = rdata->getCreatedDoc();
	//	bool rslt = AosDocClientObj::getDocClient()->createDocSafe3(rdata,
	//	          log, rdata->getCid(), "", true, true, false,
	//	          false, false, true, true);
		bool rslt = AosDocClientObj::getDocClient()->createDoc1(rdata, NULL, rdata->getCid(), log,
				       false, 0, 0, false);
		if (rslt)
		{
			log_added = true;
		}

		//if (log)
		//{
		//	AosSeLogClient::getSelf()->addLog(log_container, logname, log, rdata);
		//	log_added = true;
		//}
	}

	if (!log_added)
	{
		OmnString logstr = "<log ";
		logstr << AOSTAG_FROM_STATE << "=\"" << statename << "\" "
			<< AOSTAG_TO_STATE << "=\"" << next_statename << "\" "
			<< AOSTAG_EVENT << "=\"" << event << "\" "
			<< AOSTAG_STATEMACHINE_ID << "=\"" << arcd->getDocid() << "\"/>";
		AosSeLogClient::getSelf()->addLog(log_container, logname, logstr, rdata);
	}

	rslt = arcd->modifyStatemachineState(next_statename, rdata);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


AosAccessRcdPtr 
AosSdocStatemachine::getAccessRecord(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// Each statemachine instance is stored in a doc's access record. 
	// This function retrieves the access record. It assumes the following:
	// 	<sdoc ...>
	// 		<managed_doc .../>
	// 		...
	// 	</sdoc>
	//
	// where '<managed_doc .../>' is a doc selector. It selects one and only
	// one doc that is responsible for managing the statemachine. If not
	// found, it is an error.
	AosXmlTagPtr doc = AosRunDocSelector(rdata, sdoc, AOSTAG_MANAGED_DOC);
	if (!doc)
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << ". Missing doc: " << sdoc->toString() << enderr;
		return 0;
	}

	return AosDocClientObj::getDocClient()->getAccessRecord(doc->getAttrU64(AOSTAG_DOCID, 0), true, rdata);
}


OmnString 
AosSdocStatemachine::checkEvent(
		const AosXmlTagPtr &sdoc, 
		AosXmlTagPtr &event_def, 
		const AosRundataPtr &rdata)
{
	// This function determines whether it should trigger an event. 
	// Events are defined by 'sdoc' as:
	// 		<state ...>
	// 			<events ...>
	// 				<event_def  ...>
	// 					<conds>
	// 						<cond .../>
	// 						<cond .../>
	//						...
	//					</conds>
	// 				<event_def  .../>
	// 				...
	// 			</events>
	// 		</state>
	aos_assert_rr(sdoc, rdata, "");
	AosXmlTagPtr events = sdoc->getFirstChild("events");
	aos_assert_rr(events, rdata, "");
	event_def = events->getFirstChild();
	while (event_def)
	{
		if (AosCondition::evalCondStatic(event_def, "conds", rdata))
		{
			// Matched an event. 
			OmnString event = event_def->getAttrStr(AOSTAG_EVENTNAME);
			if (event == "")
			{
				// This is an error
				AosSetError(rdata, AosErrmsgId::eStatemachineSyntaxError);
				OmnAlarm << rdata->getErrmsg() << ". state: " 
					<< events->toString() << enderr;
				return "";
			}
			return event;
		}
		event_def = events->getNextChild();
	}

	// Did not trigger any event.
	return "";
}


bool
AosSdocStatemachine::createErrorLog(
		const OmnString log_container, 
		const OmnString &logname,
		const OmnString &fromstate, 
		const OmnString &tostate,
		const OmnString &event, 
		const OmnString &errmsg,
		const u64 &docid,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	OmnString logstr = "<log ";
	if (event != "")
	{
		logstr << AOSTAG_EVENT << "=\"" << event << "\" ";
	}

	if (fromstate != "")
	{
		logstr << AOSTAG_FROM_STATE << "=\"" << fromstate << "\" ";
	}

	if (tostate != "")
	{
		logstr << AOSTAG_TO_STATE << "=\"" << tostate << "\" ";
	}

	logstr << AOSTAG_LOGENTRY_TYPE << "=\"error\" "
		<< AOSTAG_STATEMACHINE_ID << "=\"" << docid << "\">"
		<< "<![CDATA[" << errmsg << "]]></log>";
	AosAddLog(rdata, log_container, logname, logstr);
	return true;
}

bool
AosSdocStatemachine::createActionLogs(
		const OmnString &fromstate,
		const OmnString &tostate,
		const OmnString &event,
		const OmnString &log_container, 
		const OmnString &logname,
		const u64 &docid,
		const vector<AosXmlTagPtr> &logs, 
		const AosRundataPtr &rdata)
{
	OmnString logstr = "<log ";
	if (event != "")
	{
		logstr << AOSTAG_EVENT << "=\"" << event << "\" ";
	}

	if (fromstate != "")
	{
		logstr << AOSTAG_FROM_STATE << "=\"" << fromstate << "\" ";
	}

	if (tostate != "")
	{
		logstr << AOSTAG_TO_STATE << "=\"" << tostate << "\" ";
	}

	logstr << AOSTAG_LOGENTRY_TYPE << "=\"actions\" "
		<< AOSTAG_STATEMACHINE_ID << "=\"" << docid << "\">"
		<< "<actions>";
	
	for (u32 i=0; i<logs.size(); i++)
	{
		logstr << logs[i]->toString();
	}
	logstr << "</actions></log>";
	AosAddLog(rdata, log_container, logname, logstr);
	return true;	
}
	
