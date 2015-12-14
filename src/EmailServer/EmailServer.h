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
//
// Modification History:
// 05/31/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_EmailServer_h
#define AOS_EmailServer_h

#include "EmailServer/Ptrs.h"
#include "Util/String.h"
#include "TransServer/TransProc.h"
#include "Rundata/Ptrs.h"

class AosEmailServer: public AosTransProc
{
	OmnDefineRCObject;

private:
	AosPop3ServerPtr                 mPop3Svr;
	AosSmtpServerPtr				 mSmtpSvr;
public:
	AosEmailServer(const AosXmlTagPtr &config);
	~AosEmailServer();
	
	virtual bool proc(
			const AosXmlTagPtr &data,
			const AosRundataPtr &rdata);

	void setTransServer(AosTransServerPtr &transserver){};
	
};
#endif

