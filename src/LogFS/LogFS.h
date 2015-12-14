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
// 2014/09/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_LogFS_LogFSObj_h
#define Aos_LogFS_LogFSObj_h

#include "SEInterfaces/LogFSObj.h"
#include "SEInterfaces/Ptrs.h"


class AosLogFS : public AosLogFSObj
{
private:
	AosFmtSvrObj *		mFmtMgr;

public:

protected:
	bool syncChanges(AosRundata *rdata, 
						const char *data, 
						const int len);

	bool fileTooBig() const {return mCrtSize > mMaxSize;}

	virtual bool createNextLogFile(AosRundata *rdata);
};
#endif

