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
#ifndef Aos_AdminTrans_AdminStartProcessTrans_h
#define Aos_AdminTrans_AdminStartProcessTrans_h

#include "TransUtil/AdminTrans.h"

class AosAdminStartProcessTrans : virtual public AosAdminTrans
{

private:
	OmnString		mArgs;
	AosXmlTagPtr	mClusterConfig;
	AosXmlTagPtr	mNormConfig;

public:
	AosAdminStartProcessTrans(const bool regflag);
	AosAdminStartProcessTrans(
			const OmnString &args,
			const AosXmlTagPtr &cluster_config,
			const AosXmlTagPtr &norm_config,
			const int svr_id,
			const bool need_save,
			const bool need_resp);
	~AosAdminStartProcessTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

