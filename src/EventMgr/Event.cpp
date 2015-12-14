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
// 10/25/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "EventMgr/Event.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Actions/SdocAction.h"
#include "EventMgr/EventTypes.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "Statemachine/Statemachine.h"
#include "XmlUtil/XmlTag.h"


static AosDocClientObjPtr	sgDocClient;

AosEvent::AosEvent()
{
}


AosEvent::~AosEvent()
{
}


bool
AosEvent::checkEvent1(
		const AosXmlTagPtr &olddoc, 
		const AosXmlTagPtr &newdoc, 
		const OmnString &trigger,
		const AosRundataPtr &rdata)
{
	// This function checks whether it triggers events. Events are 
	// defined by the doc's container.
	if (!sgDocClient) sgDocClient = AosDocClientObj::getDocClient();
	aos_assert_rr(sgDocClient, rdata, false);

	if (sgDocClient->isRepairing()) return true;;

	// Chen Ding, 01/02/2012
	// Check the statemachines first. Note that statemachines may
	// want to stop the processing. 
	bool cont_proc = true;

	// Ice 2013/6/3
	//bool rslt = AosStatemachine::procEvent(trigger, olddoc, newdoc, cont_proc, rdata);
	bool rslt = true;
	aos_assert_rr(rslt, rdata, false);
	if (!cont_proc)
	{
		// The statemachines stopped the processing.
		return true;
	}

	aos_assert_rr(newdoc, rdata, false);
	OmnString ctnr_objid = newdoc->getAttrStr(AOSTAG_PARENTC);
	// Linda, 2012/12/26
	if (trigger == AOSEVENTTRIGGER_LOG_CREATED)
	{
		ctnr_objid = newdoc->getAttrStr(AOSTAG_LOG_CTNROBJID);
	}
	if (ctnr_objid == "") 
	{
		// There is only one doc that does not have parent container.
		if (newdoc->getAttrStr(AOSTAG_OBJID) == AosObjid::composeSysRootAcctObjid(
					rdata->getSiteid()))
		{
			rdata->setOk();
			return true;
		}
		
		AosSetErrorU(rdata, "missing_parent_ctnr")
			<< newdoc->getAttrStr(AOSTAG_OBJID) << enderr;
		return false;
	}

	AosXmlTagPtr ctnr = sgDocClient->getDocByObjid(ctnr_objid, rdata);
	if (!ctnr) 
	{
		if (ctnr_objid == AOSOBJIDPRE_ROOTCTNR) return true;

		AosSetErrorU(rdata, "missing_parent_ctnr") 
			<< newdoc->getAttrStr(AOSTAG_OBJID) << enderr;
		return false;
	}

	AosXmlTagPtr events = ctnr->getFirstChild(AOSTAG_EVENTDEFS);
	if (!events)
	{
		// No events are defined. Do nothing.
		return true;
	}

	OmnString sdoc_objid = events->getAttrStr(AOSTAG_SDOC_OBJIDS);
	if (sdoc_objid != "")
	{
		events = sgDocClient->getDocByObjid(sdoc_objid, rdata);
		if (!events)
		{
			AosSetErrorU(rdata, "failed_ret_sdoc") << sdoc_objid << enderr;
			return false;
		}
	}

	AosXmlTagPtr triggered_events = events->getFirstChild(trigger);
	if (!triggered_events) return true;

	// Found the event def, which is in the following format:
	// 	<zky_events>
	// 		<trigger sdoc_objids="xxx,xxx,...">
	// 			<event sdoc_objids="xxx,xxx,...">
	// 				<event_def>
	// 					<input type="xxx" .../>
	// 					<input type="xxx" .../>
	// 					...
	// 				</event_def>
	// 				<actions>
	// 					<action .../>
	// 					<action .../>
	// 					...
	// 				</actions>
	// 			</event>
	// 			...
	// 		</trigger>
	// 		...
	// 	</zky_events>
	AosXmlTagPtr triggered_event = triggered_events->getFirstChild();
	int guard = eMaxEvents;
	while (guard-- && triggered_event)
	{
		procEvent(olddoc, newdoc, trigger, triggered_event, rdata);
		triggered_event = triggered_events->getNextChild();
	}
	aos_assert_rr(guard, rdata, false);
	return true;
}


bool
AosEvent::procEvent(
		const AosXmlTagPtr &olddoc, 
		const AosXmlTagPtr &newdoc, 
		const OmnString &trigger,
		const AosXmlTagPtr &event_xml,
		const AosRundataPtr &rdata)
{
	// 'event_xml' is in the following format:
	// 		<event>
	// 			<event_def>
	// 				<input type="xxx" .../>
	// 				<input type="xxx" .../>
	// 				...
	// 			</event_def>
	// 			<actions>
	// 				<action .../>
	// 				<action .../>
	// 				...
	// 			</actions>
	// 		</event>
	if (!sgDocClient) sgDocClient = AosDocClientObj::getDocClient();
	aos_assert_rr(sgDocClient, rdata, false);
	if (sgDocClient->isRepairing()) return true;;

	aos_assert_rr(newdoc, rdata, false);
	aos_assert_rr(event_xml, rdata, false);
	
	AosXmlTagPtr event_def = event_xml->getFirstChild("event_def");
	if (!event_def || tryToFireEvent(event_def, olddoc, newdoc, trigger, rdata))
	{
		// Need to run actions
		//AosXmlTagPtr actions = event_xml->getFirstChild("actions");
		//if (!actions) return true;

		// Run actions 'actions'.
		// rdata->setDocByVar(olddoc, AOSARG_OLD_DOC);
		// rdata->setDocByVar(newdoc, AOSARG_NEW_DOC);

		OmnString objids = event_def->getNodeText();
		rdata->setEvent(trigger);
		AosSmartDocObjPtr sdoc_obj = AosSmartDocObj::getSmartDocObj();
		aos_assert_r(sdoc_obj, false);
		sdoc_obj->runSmartdocs(objids, rdata);
		// bool rslt = AosSdocAction::runActions(actions, rdata);
		// if (!rslt)
		// {
		// 	AosSetError(rdata, AosErrmsgId::eFailedToDoAction);
		// 	OmnAlarm << rdata->getErrmsg() << enderr;
		// 	return false;
		// }
		return true;
	}

	return true;
}


bool
AosEvent::tryToFireEvent(
		const AosXmlTagPtr &event_def, 
		const AosXmlTagPtr &olddoc,
		const AosXmlTagPtr &newdoc,
		const OmnString &trigger,
		const AosRundataPtr &rdata)
{
	// This function determines whether to trigger an event.
	// <event_def>
	// 	 <input type="xxx" attr="xxx">
	// 	 	<before>xxx</before>
	// 	 	<after>xxx</after>
	// 	 </input>
	// 	 <input type="xxx" .../>
	// 	 ...
	// </event_def>
	aos_assert_rr(event_def, rdata, false);
	AosXmlTagPtr input = event_def->getFirstChild();
	while (input)
	{
		OmnString type = input->getAttrStr("type");
		switch (AosEventType::toEnum(type))
		{
		case AosEventType::eDoc:
			 {
				// This means the event is fired if and only if its
				// old value is the same as defined in 'input' and
				// its new value is the same as defined in 'input'.
				// If 'input' does not have 'before_value', it does
				// not check the old value. 
				OmnString attrname = input->getAttrStr(AOSTAG_ANAME);
				aos_assert_rr(attrname != "", rdata, false);

				AosXmlTagPtr before_tag = input->getFirstChild("before");
				if (before_tag)
				{
					// There is the 'before_tag'. If its value is not empty,
					// it means the olddoc must be the same value as the
					// one in the 'before_tag'.
					OmnString before = input->getNodeText();
					if (olddoc)
					{
						if (before != olddoc->getAttrStr(attrname)) return false;
					}
					else
					{
						if (before != "") return false;
					}
				}

				OmnString after = input->xpathQuery("after/_#text");
				aos_assert_rr(newdoc, rdata, false);
				return (newdoc->getAttrStr(attrname) == after);
			 }
			 break;
		
		case AosEventType::eSystem:
			 OmnNotImplementedYet;
			 break;

		case AosEventType::eTimer:
			 return (input->getAttrStr("timer_id") == trigger);

		default:
			OmnAlarm << "Undefined type" << enderr;
			return false;
		}
		input = event_def->getNextChild();
	}
	return true;
}


/*
AosEvent::E
AosEvent::toEnum(const OmnString &type)
{
	aos_assert_r(type.length() > 3, eInvalid);
	const char *str = type.data();
	switch (str[0])
	{
	case 'd':
		if (type == AOSINPUTTYPE_DOC) return eDoc;
		break;

	case 'o':
		if (type == AOSINPUTTYPE_OPERATION) return eOperation;
		break;

	case 's':
		if (type == AOSINPUTTYPE_SYSTEM) return eSystem;
		break;

	case 't':
		if (type == AOSINPUTTYPE_TIMER) return eTimer;
		break;

	default:
		OmnAlarm << "Invlid type: " << type << enderr;
		return eInvalid;
	}
	return eInvalid;
}
*/
