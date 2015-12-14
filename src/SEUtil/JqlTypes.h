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
// 02/11/2014	Copued from JqlStament.h by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_JqlTypes_h
#define Omn_SEUtil_JqlTypes_h

#include "Util/String.h"



struct JQLTypes
{
	enum OpType
	{
		eOpInvalid,

		eCreate,
		eShow,
		eShowStatus,
		eDescribe,
		eDrop,
		eClear,
		eDump,
		eRun,
		eStart,
		eStop,
		eList,
		eUse,
		eUpdate,
		eWait,
		eRestart,
		eAlter,

		eOpMaxEntry
	};

	enum DocType
	{
		eDocInvalid,

		eTableDoc,
		eIndexDoc,
		eSchemaDoc,
		eDatasetDoc,
		eJobDoc,
		eTaskDoc,
		eDataProcDoc,
		eDataScannerDoc,
		eDataConnectorDoc,
		eDataRecordDoc,
		eDataFieldDoc,
		eDatabaseDoc,
		eMap,
		eVirtualField,
		eServiceDoc,
		eStatisticsDoc,
		eSequenceDoc,
		eJimoLogicDoc,
		eUserDoc,
		eDict,

		eDocMax
	};

	static std::string toStr(JQLTypes::OpType opr)
	{
		switch(opr)
		{
			case eCreate:
				return "create";
			case eShow:
				return "show";
			case eDescribe:
				return "describe";
			case eDrop:
				return "drop";
			case eClear:
				return "clear";
			case eDump:
				return "dump";
			case eRun:
				return "run";
			case eStart:
				return "start";
			case eStop:
				return "stop";
			case eList:
				return "list";
			case eUse:
				return "use";
			case eUpdate:
				return "update";
			case eRestart:
				return "restart";
			default:
				return "";
		}
		return "";
	}
};


#endif
