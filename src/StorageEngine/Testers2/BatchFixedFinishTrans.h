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
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_BatchFixedFinishTrans_h
#define Aos_DocTrans_BatchFixedFinishTrans_h

#include "TransUtil/TaskTrans.h"


class AosBatchFixedFinishTrans : virtual public AosTaskTrans
{
	int	 		mPhy;
	int	 		mDataPhy;
	bool		mCreate;

public:
	AosBatchFixedFinishTrans(const bool regflag);
	AosBatchFixedFinishTrans(
			const int phy,
			const int phy_data,
			const bool create,
			const bool need_save,
			const bool need_resp);
	~AosBatchFixedFinishTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif
