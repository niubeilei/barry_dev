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
// Modification History:
// 2013/03/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysConsole_ConsoleProc_h
#define Aos_SysConsole_ConsoleProc_h

// #include "CloudCube/MsgProc.h"
#include "SysConsole/Ptrs.h"
#include "Util/HashUtil.h"

class AosConsoleProc : virtual public OmnRCObject
{
private:
	typedef hash_map<const OmnString, AosConsoleProcPtr, 
			Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosConsoleProcPtr, 
			Omn_Str_hash, compare_str>::iterator mapitr_t;

	static map_t		smProcs;

protected:
	OmnString	mInfoId;

public:
	AosConsoleProc(const OmnString &info_id, 
					const bool regflag);

	virtual bool procReq(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &entry,
					OmnString &resp, 
					bool &finished) = 0;

	static bool init();
	static AosConsoleProcPtr getProc(const OmnString &name);
};
#endif

