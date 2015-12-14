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
// 02/27/2012 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_DbVarNames_h
#define Omn_SEUtil_DbVarNames_h

#include "Util/String.h"


#define AOSDBVAR_SOBJTRANSMGR_SEQNO			"zkdb_001"
#define AOSDBVAR_SOBJTRANSMGR_VDIR			"zkdb_002"
#define AOSDBVAR_SOBJTRANSMGR_IILID			"zkdb_003"

class AosDbVarNames
{
private:
	static char 	mConnChar;

public:
	static OmnString composeTransMgrSeqnoVarname(const u64 &virtual_dir)
	{
		OmnString name = AOSDBVAR_SOBJTRANSMGR_SEQNO;
		name << mConnChar << virtual_dir;
		return name;
	}

	static OmnString composeTransMgrVirtualDirVarname(const OmnString &objid)
	{
		OmnString name = AOSDBVAR_SOBJTRANSMGR_VDIR;
		name << mConnChar << objid;
		return name;
	}

	static OmnString composeTransMgrIILIDVarname(const u64 &virtual_dir)
	{
		OmnString name = AOSDBVAR_SOBJTRANSMGR_IILID;
		name << mConnChar << virtual_dir;
		return name;
	}

};
#endif

