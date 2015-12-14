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
// 05/31/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransClient_Tester_TransSvrProcLocal_h
#define AOS_TransClient_Tester_TransSvrProcLocal_h

#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/Ptrs.h"
#include "TransServer/TransProc.h"
#include "TransUtil/Ptrs.h"

class AosTransSvrProcLocal: public AosTransProc
{
	OmnDefineRCObject;
public:
	AosTransSvrProcLocal();
	~AosTransSvrProcLocal() {};
	
	virtual bool proc(
			        const AosXmlTransPtr &trans,
					const AosXmlTagPtr &trans_doc,
					const AosRundataPtr &rdata);
	virtual void setTransServer(const AosTransServerPtr &transserver){};
	virtual bool proc(const AosTinyTransPtr &trans, const AosRundataPtr &rdata){ return true;};

private:
};
#endif

