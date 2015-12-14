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
// 03/20/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_LogTrans_RetrieveLogsTrans_h
#define Aos_LogTrans_RetrieveLogsTrans_h

#include "TransUtil/CubicTrans.h"

class AosRetrieveLogsTrans : virtual public AosCubicTrans
{

private:
	AosXmlTagPtr 	mReq;

public:
	AosRetrieveLogsTrans(const bool regflag);
	AosRetrieveLogsTrans(
		const AosXmlTagPtr &request,
		const u64 docid,
		const bool need_save,
		const bool need_resp);
	~AosRetrieveLogsTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

