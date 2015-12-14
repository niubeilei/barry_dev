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
// 08/13/2011	by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DbQuery_DbListener_h
#define AOS_DbQuery_DbListener_h

class AosDbListener : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	virtual bool dbNotify(const AosEvents &events, 
					const u32 notify_id,
					const OmnString &source_objid,
					const OmnString &triggered_objid, 
					const AosRundataPtr &rdata) = 0;
};
#endif
