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
#ifndef Omn_ReqProc_SendToMsgService_h
#define Omn_ReqProc_SendToMsgService_h	

#include "ReqProc/ReqProc.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"

class AosSendToMsgService: public AosSeRequestProc
{
	//Important: This class shoud not have any member data, it's not thead safe. 
public:
	AosSendToMsgService(const bool);
	~AosSendToMsgService() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
private:
bool sendSuccessMsg(OmnString &userid, OmnString &yuyue_id, const OmnString &objidnmae, OmnString &objidvalue, const OmnString &device, const AosRundataPtr &rdata);
bool sendFailedMsg(OmnString &userid, const OmnString &device, const AosRundataPtr &rdata);
bool procBatch(AosXmlTagPtr &request, AosXmlTagPtr &objdef, const OmnString &ToS, const OmnString &method, const AosRundataPtr &rdata);
AosXmlTagPtr getUserAccount(OmnString &peopleid, const AosRundataPtr &rdata);
bool batch(AosXmlTagPtr &request, AosXmlTagPtr &account, AosXmlTagPtr &entrydoc, const OmnString &ToS, const AosRundataPtr &rdata);
bool reBatch(AosXmlTagPtr &request, AosXmlTagPtr &account, AosXmlTagPtr &entrydoc, const OmnString &ToS, const AosRundataPtr &rdata);
OmnString saveOneBmd(AosXmlTagPtr &request, AosXmlTagPtr &bmddoc, const AosRundataPtr &rdata);
OmnString saveOneIOI(AosXmlTagPtr &request, AosXmlTagPtr &ioidoc, const AosRundataPtr &rdata);
bool modifyYuyueDoc(AosXmlTagPtr &yuyuedoc, OmnString &objid, const OmnString &ToS, const AosRundataPtr &rdata);
AosXmlTagPtr createYuyueDoc(AosXmlTagPtr &account, AosXmlTagPtr &entrydoc, const OmnString &ToS, const AosRundataPtr &rdata);
AosXmlTagPtr getGarbage(OmnString &container, const AosRundataPtr &rdata);
bool addAttributes(AosXmlTagPtr &entry, AosXmlTagPtr &account, const OmnString &ToS, const AosRundataPtr &rdata);
bool updateAccountByIOI(AosXmlTagPtr &entrydoc, AosXmlTagPtr &account, const AosRundataPtr &rdata);
bool procSingleIOI(AosXmlTagPtr &root, const AosRundataPtr &rdata);
};
#endif

