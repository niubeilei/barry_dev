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
//	Whenever a program generates an error, it should use this class to
//  record the error. It is more than just an error message.    
//
// Modification History:
// 2015/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Debug_ErrorMsg_h
#define Omn_Debug_ErrorMsg_h

#include "Util/BasicTypes.h"
#include "Util/String.h"
#include "Debug/ErrId.h"
#include "Message/Ptrs.h"

class AosErrMsg
{
private:
	OmnString		mFile;
	int				mLine;
	u32				mErrSeqno;
	u64				mTimestamp;
	OmnString		mContents;

public:
	AosErrMsg();
	AosErrMsg(const OmnString &msgid);
	~AosErrMsg();

	AosErrMsg & field(const OmnString &name, const u64 value);	
	AosErrMsg & field(const OmnString &name, const OmnString &value);	
	OmnString   getErrmsg() const {return mContents;}
};
#endif
