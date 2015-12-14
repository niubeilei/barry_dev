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
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Config_ConfigMgr_h
#define Omn_Config_ConfigMgr_h

#include "Debug/Rslt.h"
#include "Config/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/HashUtil.h"
#include <vector>
using namespace std;

class AosConfigMgr
{
private:
	typedef hash_map<const OmnString, AosConfigHandlerPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosConfigHandlerPtr, Omn_Str_hash, compare_str>::iterator mapitr_t;
	enum
	{
		eInvalid = 0,

		eDocServerPort = 20,
		eIILMgrPort = 25,

		eMax
	};

	static int		smBasePort;
	static map_t	smMap;
	static bool		smInited;
public:
	AosConfigMgr();

	static int getDocServerPort() 	{return smBasePort + eDocServerPort;}
	static int getIILMgrPort() 		{return smBasePort + eIILMgrPort;}
	static bool registerHandler(const AosConfigHandlerPtr &handler);

private:
	static bool addHandler(const AosConfigHandlerPtr &handler);
};
#endif

