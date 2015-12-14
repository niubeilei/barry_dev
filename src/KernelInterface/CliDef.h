////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliDef.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelInterface_CliDef_h
#define Omn_KernelInterface_CliDef_h

#include "Util/String.h"

struct OmnCliDef
{
	enum
	{
		eMaxWordsPerCli = 10
	};

	OmnString		mWords[eMaxWordsPerCli];
	OmnString		mHelper[eMaxWordsPerCli];
};


class OmnCliDefMgr
{
private:
	int			mNumClis;
	OmnCliDef *	mClis;

public:
	OmnCliDefMgr() {}
	virtual ~OmnCliDefMgr() {}

	bool	help(const OmnString &cmd, OmnString &modifiedCmd);
};
#endif

