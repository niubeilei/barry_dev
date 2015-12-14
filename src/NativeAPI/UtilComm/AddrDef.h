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
// 03/23/2009	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NativeAPI_UtilComm_AddrDef_h
#define Aos_NativeAPI_UtilComm_AddrDef_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/IpAddr.h"




class AosAddrDef : public OmnRCObject
{
	OmnDefineRCObject;
private:
	enum
	{
		eMaxAddrs = 50
	};

	OmnIpAddr	mAddrs[eMaxAddrs];
	int			mNumAddrs;
	int			mCrtIdx;

public:
	AosAddrDef();
	~AosAddrDef() {}

	bool 		config(const OmnString &def, const OmnString &dft);
	bool 		config(const OmnString &def);
	void		reset() {mCrtIdx = 0;}
	OmnIpAddr	nextAddr();
	bool		hasMore() const {return mCrtIdx < mNumAddrs;}
	int			getNumAddrs() const {return mNumAddrs;}
};

#endif
