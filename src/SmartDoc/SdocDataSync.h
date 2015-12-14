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
// 03/03/2012	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocDataSync_h
#define Aos_SmartDoc_SdocDataSync_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <vector>

class AosSdocDataSync: public AosSmartDoc
{
	enum
	{
		eReConnTime = 60,
		eMaxTries = 10,
		eSyncNum = 50
	};

public:
	AosSdocDataSync(const bool flag);
	~AosSdocDataSync();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocDataSync(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	bool	procEachCtnr(
				const OmnTcpClientPtr &conn, 
				const OmnString &attrname,
				const OmnString &ctnr, 
				const u32 &eSyncNum, 
				const OmnString &lastSyncTime,
				const AosRundataPtr &rdata,
				const OmnString &ctnrname);

	bool	procModified(
				const OmnTcpClientPtr &conn, 
				const AosXmlTagPtr &contents,
				const AosRundataPtr &rdata);

	bool    procModifyAccr(
			const AosXmlTagPtr &contents,
			const vector<OmnString> &old_docids,
			const vector<OmnString> &new_docids,
			const AosRundataPtr &rdata);


	bool	send(const OmnTcpClientPtr &conn,
				const OmnString &req, 
				AosXmlTagPtr &root,
				AosXmlTagPtr &rootChild,
				const AosRundataPtr &rdata);

	OmnString	createSendMsg(const OmnString &attrname,
					const OmnString &ctnr,
					const u32 &eSyncNum,
					const OmnString &lastSyncTime,
					const u32 startIdx,
					const AosRundataPtr &rdata,
					const OmnString &ctnrname);

	bool	getServerCrtTime(
				const OmnTcpClientPtr &conn,
				OmnString &server_crt_time,
				const AosRundataPtr	&rdata);

	bool	checkCtnr(const OmnString &ctnr, const AosRundataPtr &rdata);

	bool	retrieveCtnr(const OmnTcpClientPtr &conn,
				const OmnString &ctnr,
				const AosRundataPtr &rdata);
	
};
#endif

