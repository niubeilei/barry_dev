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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_ProcEcg_h
#define Omn_ReqProc_ProcEcg_h	

#include "ReqProc/ReqProc.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"

class AosProcEcg: public AosSeRequestProc
{
	//Important: This class shoud not have any member data, it's not thead safe. 
public:
	AosProcEcg(const bool);
	~AosProcEcg() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
private:
	bool readEcg(const AosRundataPtr &rdata, OmnString path, OmnString &ecg);
	bool readPres(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc);
	bool readResp(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc);
	bool readTmp(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc);
	bool readRr(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc);
	bool readPersonDetails(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc);
	bool sendFailedMsgToFront(const AosXmlTagPtr usr_doc, const AosRundataPtr &rdata);
};
#endif

