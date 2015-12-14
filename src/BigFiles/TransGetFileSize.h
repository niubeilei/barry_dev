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
// 2013/04/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BigFile_TransGetFileSize_h
#define Aos_BigFile_TransGetFileSize_h

#include "TransUtil/TaskTrans.h"

class AosTransGetFileSize : virtual public AosTaskTrans
{
private:

public:
	AosTransGetFileSize(const bool regflag);
	AosTransGetFileSize(const u64 &fileid);
	AosTransGetFileSize(const OmnString &fname);
	~AosTransGetFileSize();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

private:
	AosXmlTagPtr 	getDoc();

};
#endif

