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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 02/22/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MsgProc_MsgProc_h
#define AOS_MsgProc_MsgProc_h

#include "MsgProc/MsgProcTypes.h"
#include "MsgProc/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosMsgProc : public OmnRCObject
{
public:
	AosMsgProcType::E	mType;

public:
	AosMsgProc(
			const OmnString &name,
			const AosMsgProcType::E type, 
			const bool flag);

	virtual bool msgReceived(const OmnString &appid, 
							 const AosXmlTagPtr &msg, 
							 const AosRundataPtr &rdatga) = 0; 

	static bool initAllProcs();
	static bool config(const AosXmlTagPtr &config);
	static AosMsgProcPtr getProc(const OmnString &name);

private:
	bool registerMsgProc(const AosMsgProcPtr &access, const OmnString &name);
};
#endif

