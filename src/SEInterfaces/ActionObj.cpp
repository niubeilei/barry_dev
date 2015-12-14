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
// 01/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ActionObj.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/BuffData.h"
#include "SEInterfaces/DLLActionObj.h"
#include "SEInterfaces/TaskObj.h"
#include "XmlUtil/XmlTag.h"

AosActionObjPtr AosActionObj::smAction;
static AosActionObjPtr	sgActions[AosActionType::eMax+1];
static OmnMutex 		sgLock;


AosActionObj::AosActionObj(const u32 version)
:
AosJimo(AosJimoType::eAction, version),
mType(AosActionType::eJimoAction)
{
}


bool
AosActionObj::runActions(
		const AosXmlTagPtr &actions, 
		const OmnString &tagname,
		const AosRundataPtr &rdata)
{
	// This function runs all the actions defined in 'actions', 
	// which is in the form: 
	// 	<doc ...>
	// 		<name>
	// 			<action .../>
	// 			<action .../>
	// 		</name>
	// 		...
	// 	</doc>
	if (tagname == "") return runActions(actions, rdata);

	AosXmlTagPtr aa = actions->getFirstChild(tagname);
	if (!aa) return true;
	return runActions(aa, rdata);
}


bool
AosActionObj::runActions(
		const AosXmlTagPtr &actions, 
		const AosRundataPtr &rdata)
{
	// This function runs all the actions defined in 'actions', 
	// which is in the form: 
	// 	<actions>
	// 		<action .../>
	// 		<action .../>
	// 		...
	// 	</actions>
	if (!actions)
	{
		AosSetErrorUser(rdata, "internal_error") << actions->toString() << enderr;
		return false;
	}

	AosXmlTagPtr action = actions->getFirstChild();
	if (!action)
	{
		// Chen Ding, 10/04/2012
		// It is possible no actions to run. 
		// AosSetErrorU(rdata, "no_action_defined") << ": " << actions->toString();
		// return false;
		return true;
	}

	AosActionObjPtr act;
	while (action)
	{
		// Chen Ding, 05/29/2012
		// OmnString actionid = action->getAttrStr(AOSTAG_ZKY_TYPE);
		// if (actionid == "") actionid = action->getAttrStr("type");
		// act = getAction(actionid, rdata);
		act = getAction(action, rdata);
		if (!act) return false;
		if (!act->run(action ,rdata))	
		{
			return false;
		}
		action = actions->getNextChild();
	}

	rdata->setOk();
	return true;
}


bool
AosActionObj::runAction(
		const AosXmlTagPtr &action_def, 
		const AosRundataPtr &rdata)
{
	if (!action_def)
	{
		AosSetErrorU(rdata, "missing_action_definitions");
		return false;
	}

	// Chen Ding, 05/29/2012
	AosActionObjPtr act = getAction(action_def, rdata);
	if (!act) return false;
	return act->run(action_def, rdata);
}


// Chen Ding, 2013/02/07
bool
AosActionObj::runAction(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &action_def, 
		const AosBuffPtr &buff)
{
	if (!action_def)
	{
		AosSetErrorU(rdata, "missing_action_definitions");
		return false;
	}

	AosActionObjPtr act = getAction(action_def, rdata);
	aos_assert_rr(act, rdata, false);
	return act->run(buff, rdata);
}



bool
AosActionObj::registerAction(const AosActionObjPtr &action, const OmnString &name)
{
	AosActionType::E type = action->getType();
	if (type <= AosActionType::eInvalid || type >= AosActionType::eMax)
	{
		OmnAlarm << "Invalid action id: " << type << enderr;
		return false;
	}

	sgLock.lock();
	if (sgActions[type])
	{
		sgLock.unlock();
		OmnAlarm << "Action already registered: " << name << ":" << type << enderr;
		return false;
	}

	sgActions[type] = action;
	OmnString errmsg;
	bool rslt = AosActionType::addName(name, type, errmsg);
	sgLock.unlock();
	if (!rslt)
	{
		OmnAlarm << "Failed add name: " << errmsg << enderr;
	}
	return rslt;
}


AosActionObjPtr
AosActionObj::getAction(
		const AosActionType::E the_type, 
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosActionType::E type = the_type;
	if (!AosActionType::isValid(type))
	{
		// Backward compatibility
		type = AosActionType::toEnum(def->getAttrStr("type"));
		if (!AosActionType::isValid(type))
		{
			// Check whether it is an DLL action
			if (def)
			{
				AosDLLObjPtr dllobj = AosCreateDLLObj(rdata.getPtr(), def);
				if (dllobj)
				{
					AosDLLActionObjPtr action = dllobj->convertToAction(rdata);
					if (action) return action.getPtr();
				}
			}
		}

		AosSetErrorU(rdata, "unrecognized_action") << ": " << (int)type; 
		return 0;
	}

	sgLock.lock();
	AosActionObjPtr act;
	act = sgActions[type];
	sgLock.unlock();

	if (!act)
	{
		AosSetErrorUser(rdata, "action_not_found")
			<< AOSDICTERM("action_type", rdata.getPtr()) << ": " << AosActionType::toString(type)
			<< ". " << AOSDICTERM("configuration", rdata.getPtr()) 
			<< ": " << def->toString() << enderr;
		return 0;
	}

	return act->clone(def, rdata);
}


OmnString 
AosActionObj::getActionId(const AosXmlTagPtr &action)
{
	if (action) return action->getAttrStr(AOSTAG_ZKY_TYPE);
	return "";
}


bool
AosActionObj::runAction(
		const AosRundataPtr &rdata, 
		const OmnString &act_def)
{
	AosXmlTagPtr act_xml = AosStr2Xml(rdata.getPtr(), act_def AosMemoryCheckerArgs); 
	aos_assert_rr(act_xml, rdata, false);
	return runAction(act_xml, rdata);
}
	

/*
bool
AosActionObj::checkConfigStatic(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf,
		set<OmnString> &data_col_ids)
{
	// Ketty 2013/12/26
	aos_assert_r(conf, 0);

	OmnString actionid = def->getAttrStr(AOSTAG_ZKY_TYPE);
	if (actionid == "") actionid = def->getAttrStr(AOSTAG_TYPE);
	AosActionType::E type = AosActionType::toEnum(actionid);
	
	if (!AosActionType::isValid(type))
	{
		AosSetEntityError(rdata, "task_action_type_invalid",
			conf->getTagname(), "")
			<< conf << enderr;
		return false;
	}
	
	sgLock.lock();
	AosActionObjPtr action;
	action = sgActions[type];
	sgLock.unlock();

	if (!action)
	{
		AosSetErrorUser(rdata, "action_not_found")
			<< AOSDICTERM("action_type", rdata) << ": " << AosActionType::toString(type)
			<< ". " << AOSDICTERM("configuration", rdata) 
			<< ": " << def->toString() << enderr;
		return false;
	}

	return action->checkConfig(rdata, conf, data_col_ids);	
}
*/

bool
AosActionObj::createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
{
	return true;
}
