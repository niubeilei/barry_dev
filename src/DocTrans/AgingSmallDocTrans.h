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
// 11/20/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_AgingSmallDocTrans_h
#define Aos_DocTrans_AgingSmallDocTrans_h

#include "TransUtil/DocTrans.h"

class AosAgingSmallDocTrans : virtual public AosDocTrans
{

private:
	u64					mDocid;

public:
	AosAgingSmallDocTrans(const bool regflag);
	AosAgingSmallDocTrans(
			const u64 docid,
			const bool need_save,
			const bool need_resp);
	~AosAgingSmallDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

