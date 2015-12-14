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
// 12/26/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_JobTrans_CreateSizeIdByDocTrans_h
#define Aos_JobTrans_CreateSizeIdByDocTrans_h

#include "TransUtil/TaskTrans.h"

class AosCreateSizeIdByDocTrans : virtual public AosTaskTrans
{
private:
	int				mSize;
	AosXmlTagPtr	mRecordDoc;
public:
	AosCreateSizeIdByDocTrans(const bool regflag);
	AosCreateSizeIdByDocTrans(
			const int size,
			const AosXmlTagPtr &record,
			const int svr_id);
	~AosCreateSizeIdByDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif
#endif
