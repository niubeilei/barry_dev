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
// 08/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_TaskTransObj_h
#define Aos_SEInterfaces_TaskTransObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskTransType.h"
#include "SEInterfaces/TaskTransCreatorObj.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosTaskTransObj : virtual public OmnRCObject
{
public:
	enum Status
	{
		eStatus_Invalid,

		eStatus_Idle,
		eStatus_Active,
		eStatus_ProcessFinished,
		eStatus_FirstPhase,
		eStatus_SecondPhaseFinished,
		eStatus_Committed,

		eStatus_Max
	};

private:
	static AosTaskTransCreatorObjPtr		smCreator;

protected:
	AosTaskTransType::E		mTransType;

public:

	static OmnString statusToStr(const Status s);
	static void setCreator(const AosTaskTransCreatorObjPtr &d) {smCreator = d;}
	static bool registerTaskTrans(const AosTaskTransObjPtr &task);
	static AosTaskTransCreatorObjPtr getCreator() {return smCreator;}
	static AosTaskTransObjPtr createTaskTrans(
					const AosJobObjPtr &job,
					const AosXmlTagPtr &tag, 
					const AosRundataPtr &rdata);
	static AosTaskTransObjPtr serializeFromStatic(
					const AosBuffPtr &buff, 
					const AosRundataPtr &rdata);
};
#endif
