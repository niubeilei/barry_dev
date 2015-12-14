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
// 2013/03/30 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_LogTrans_LogTrans_h
#define Aos_LogTrans_LogTrans_h

#include "TransUtil/CubicTrans.h"

class AosLogTrans : public AosCubicTrans
{

public:
	AosLogTrans(
		const AosTransType::E type,
		const bool regflag);
	AosLogTrans(
		const AosTransType::E type,
		const u64 &pctr_docid,
		const bool need_save,
		const bool need_resp);
	~AosLogTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone() = 0;
	virtual bool proc();

	virtual bool procLog() = 0;

};
#endif

