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
#ifndef Aos_Statemachine_Statemachine_h
#define Aos_Statemachine_Statemachine_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "Statemachine/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosSecReq;

class AosStatemachine : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxConds = 50,
		eMaxActions = 50,
		eMaxStatemachinesToStart = 50,
		eMaxCondsToLookup = 50,
		eMaxInitSelConds = 50,
		eMaxStates = 100,
		eMaxTransitStates = 50
	};

	static AosDocClientObjPtr	smDocClient;
	static AosSeLogClientObjPtr	smLogClient;

	// In the current implementations, there shall be no instances for this class.
	AosStatemachine();
	~AosStatemachine();

public:
	static bool startStatemachines(const AosRundataPtr &rdata);
	static bool lookupStatemachines(const AosRundataPtr &rdata);
	static bool checkOperation(bool &denied, const AosRundataPtr &rdata);
	static bool procEvent(
			const OmnString &trigger,
			const AosXmlTagPtr &olddoc,
			const AosXmlTagPtr &newdoc,
			bool &cont_proc,
			const AosRundataPtr &rdata);
	static bool startStatemachineBySdoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	static bool stopStatemachine(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	static bool transitState(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	static bool transitState(
			const AosXmlTagPtr &from_state, 
			const AosXmlTagPtr &to_state, 
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);

	static bool checkOperationPriv(
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);

	static bool procEvent(
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			bool &continue_proc,
			const AosRundataPtr &rdata);

	static bool createStatemachine(
			const AosXmlTagPtr &sdoc, 
			const AosXmlTagPtr &stmc_doc, 
			const AosRundataPtr &rdata);
//	static bool startStatemachine(
//			const OmnString &sdoc_objid,
//			const OmnString &ctnr_objid,
//			const AosXmlTagPtr &stmc_doc,
//			const AosXmlTagPtr &inst_doc,
//			const AosRundataPtr &rdata);
	static bool startStatemachines(
			const AosXmlTagPtr &cond, 
			const AosXmlTagPtr &ctnr, 
			const AosRundataPtr &rdata);
	static bool startStatemachine(
			const AosXmlTagPtr &sdoc,
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);

	static bool lookupStatemachines(
			const AosXmlTagPtr &cond, 
			const AosXmlTagPtr &ctnr, 
			const AosRundataPtr &rdata);
	static bool enteringState(
			const AosXmlTagPtr &from_state,
			const AosXmlTagPtr &to_state,
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);
	static bool runActions(
			const AosXmlTagPtr &state, 
			const OmnString &action_source,
			const AosXmlTagPtr &stmc_doc, 
			const AosXmlTagPtr &inst_doc, 
			const OmnString &tagname,
			const AosRundataPtr &rdata);
	static bool runActions(
			const AosXmlTagPtr &actions,
			const OmnString &source,
			const AosXmlTagPtr &state,
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);
	static bool checkOperationAccess(
			const AosXmlTagPtr &stmc_doc, 
			const AosXmlTagPtr &inst_doc, 
			const AosRundataPtr &rdata);
	static AosXmlTagPtr getCrtState(
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);
	static AosXmlTagPtr getState(
			const OmnString &state_id,
			const AosXmlTagPtr &stmc_doc,
			const AosRundataPtr &rdata);
	inline static void setStatemachineFinishedFlag(
			const AosXmlTagPtr &inst_doc) 
	{
		inst_doc->setAttr(AOSTAG_STMC_FINISHED_FLAG, "true");
	}
	inline static bool isStatemachineFinished(
			const AosXmlTagPtr &inst_doc, 
			const AosRundataPtr &rdata)
	{
		aos_assert_rr(inst_doc, rdata, true);
		return inst_doc->getAttrBool(AOSTAG_STMC_FINISHED_FLAG, false);
	}
	static bool runStateExitActions(
			const OmnString &from_state, 
			const OmnString &to_state, 
			const AosXmlTagPtr &stmc_doc, 
			const AosXmlTagPtr &inst_doc, 
			const AosRundataPtr &rdata);
	static bool runStateEnterActions(
			const OmnString &from_state, 
			const OmnString &to_state, 
			const AosXmlTagPtr &stmc_doc, 
			const AosXmlTagPtr &inst_doc, 
			const AosRundataPtr &rdata);
	static bool runEdgeActions(
			const OmnString &from_state, 
			const OmnString &to_state, 
			const AosXmlTagPtr &stmc_doc, 
			const AosXmlTagPtr &inst_doc, 
			const AosRundataPtr &rdata);
	static bool stopStatemachine(
			const OmnString &reason,
			const AosXmlTagPtr &stmc_doc, 
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);
	static bool runStopActions(
			const AosXmlTagPtr &stmc_doc, 
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);
	static bool stopInstance(
			const AosXmlTagPtr &stmc_doc, 
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);
	static bool isStatemachineFinished(
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);
	static bool statemachineFinished(
			const AosXmlTagPtr &stmc_doc, 
			const AosXmlTagPtr &inst_doc, 
			const AosRundataPtr &rdata);
	static AosXmlTagPtr getStatemachine(
			const AosXmlTagPtr &inst_doc,
			const AosRundataPtr &rdata);
	static OmnString determineInstDocObjid(
			const AosXmlTagPtr &stmc_doc,
			const AosRundataPtr &rdata);
	static bool createLogBody(
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
			const AosRundataPtr &rdata);
	inline static bool createLog(
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
		OmnString log, logctnr_objid;
		if (createLogBody(log, logctnr_objid, contents, fname, line, 
					sdoc, stmc, inst, state, entry_type, attrs, rdata))
		{
			return false;
		}
		log << "</stmclog>";
		return sendLog(logctnr_objid, stmc, log, rdata);
	}
	inline static bool logStatemachineStop(
			const OmnString &contents, 
			const OmnString &fname,
			const int line,
			const AosXmlTagPtr &sdoc,
			const AosXmlTagPtr &stmc,
			const AosXmlTagPtr &inst,
			const AosXmlTagPtr &state,
			const OmnString &entry_type,
			const OmnString &reason,
			const AosRundataPtr &rdata)
	{
		OmnString log, logctnr_objid;
		if (createLogBody(log, logctnr_objid, contents, fname, line, 
					sdoc, stmc, inst, state, entry_type, "", rdata))
		{
			return false;
		}

		log << "<" << AOSTAG_STOP_REASON << "><![CDATA["
		    << reason << "]]></" << AOSTAG_STOP_REASON << ">"
			<< "</stmclog>";
		return sendLog(logctnr_objid, stmc, log, rdata);
	}
	inline static bool logStateTransit(
			const OmnString &contents, 
			const OmnString &fname,
			const int line,
			const AosXmlTagPtr &stmc,
			const AosXmlTagPtr &inst,
			const AosXmlTagPtr &from_state,
			const AosXmlTagPtr &to_state,
			const OmnString &entry_type,
			const AosRundataPtr &rdata)
	{
		OmnString log, logctnr_objid;
		OmnString attrs;
		if (from_state)
		{
			OmnString from_id = from_state->getAttrStr(AOSTAG_STATE_ID);
			if (from_id != "")
			{
				attrs << AOSTAG_FROM_STATE_ID << "=\"" << from_id << "\"";
			}
		}
		if (to_state)
		{
			OmnString to_id = to_state->getAttrStr(AOSTAG_STATE_ID);
			if (to_id != "")
			{
				attrs << AOSTAG_TO_STATE_ID << "=\"" << to_id << "\"";
			}
		}

		if (createLogBody(log, logctnr_objid, contents, fname, line, 0, 
					stmc, inst, to_state, entry_type, attrs, rdata))
		{
			return false;
		}
		log << "</stmclog>";
		return sendLog(logctnr_objid, stmc, log, rdata);
	}
	inline static bool createLog(
			const OmnString &contents, 
			const OmnString &fname,
			const int line,
			const AosXmlTagPtr &sdoc,
			const AosXmlTagPtr &stmc,
			const AosXmlTagPtr &inst,
			const AosXmlTagPtr &state,
			const OmnString &entry_type,
			const AosRundataPtr &rdata)
	{
		OmnString log, logctnr_objid;
		if (createLogBody(log, logctnr_objid, contents, fname, line, sdoc, 
					stmc, inst, state, entry_type, "", rdata))
		{
			return false;
		}
		log << "</stmclog>";
		return sendLog(logctnr_objid, stmc, log, rdata);
	}
	inline static bool createLog(
			const OmnString &contents, 
			const OmnString &fname,
			const int line,
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			const AosXmlTagPtr &state,
			const OmnString &entry_type,
			const AosRundataPtr &rdata)
	{
		return createLog(contents, fname, line, 0, stmc_doc, inst_doc, state, entry_type, rdata);
	}
	inline static bool logOperation(
			const OmnString &contents, 
			const OmnString &fname,
			const int line,
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			const OmnString &entry_type,
			const AosXmlTagPtr &state,
			const OmnString &opr_id,
			const AosRundataPtr &rdata)
	{
		OmnString log, logctnr_objid;

		OmnString attrs;
		if (opr_id != "")
		{
			attrs = AOSTAG_OPERATION;
			attrs << "=\"" << opr_id << "\"";
		}

		if (!createLogBody(log, logctnr_objid, contents, fname, line, 0, 
					stmc_doc, inst_doc, state, entry_type, attrs, rdata))
		{
			return false;
		}
		return sendLog(logctnr_objid, stmc_doc, log, rdata);
	}
	inline static bool logAction(
			const OmnString &contents, 
			const OmnString &fname,
			const int line,
			const AosXmlTagPtr &stmc_doc,
			const AosXmlTagPtr &inst_doc,
			const OmnString &entry_type,
			const OmnString &action_source,
			const AosXmlTagPtr &state,
			const OmnString &action_id,
			const OmnString &action_log,
			const AosRundataPtr &rdata)
	{
		OmnString log, logctnr_objid;

		OmnString attrs;
		if (action_source != "")
		{
			attrs = AOSTAG_ACTION_SOURCE;
			attrs << "=\"" << action_source << "\"";
		}

		if (!createLogBody(log, logctnr_objid, contents, fname, line, 0, 
					stmc_doc, inst_doc, state, entry_type, attrs, rdata))
		{
			return false;
		}

		log << "<" << AOSTAG_ACTION << " " << AOSTAG_ACTION_ID
		    << "=\"" << action_id << "><![CDATA["
		    << action_log << "]]></" << AOSTAG_ACTION << ">"
			<< "</stmclog>";
		return sendLog(logctnr_objid, stmc_doc, log, rdata);
	}
	inline static bool addLogAttr(
			OmnString &log, 
			const AosXmlTagPtr &doc, 
			const OmnString &from_aname,
			const OmnString &to_aname)
	{
		if (!doc) return true;
		OmnString value = doc->getAttrStr(from_aname);
		if (value == "") return true;
		log << " " << to_aname << "=\"" << value << "\"";
		return true;
	}
	static bool sendLog(
			const OmnString &pctr_objid,
			const AosXmlTagPtr &stmc_doc,
			const OmnString &log, 
			const AosRundataPtr &rdata);
	static inline bool setInstDocStopFlag(
			const AosXmlTagPtr &inst_doc, 
			const AosRundataPtr &rdata)
	{
		aos_assert_rr(inst_doc, rdata, false);
		inst_doc->setAttr(AOSTAG_STMC_STOPPED_FLAG, "true");
		return true;
	}
	static bool runEdgeActions(
			const AosXmlTagPtr &from_state, 
			const AosXmlTagPtr &to_state, 
			const AosXmlTagPtr &stmc_doc, 
			const AosXmlTagPtr &inst_doc, 
			const AosRundataPtr &rdata);
	inline static OmnString getStateid(const AosXmlTagPtr &state)
	{
		aos_assert_r(state, "");
		return state->getAttrStr(AOSTAG_STATE_ID);
	}
};
#endif


