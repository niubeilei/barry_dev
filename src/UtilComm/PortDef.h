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
#ifndef Omn_UtilComm_PortDef_h
#define Omn_UtilComm_PortDef_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/IpAddr.h"




class AosPortDef : public OmnRCObject
{
	OmnDefineRCObject;
private:
	enum
	{
		eMaxPortRanges = 200
	};

	int		mLows[eMaxPortRanges];
	int		mHighs[eMaxPortRanges];
	int		mNumRanges;
	int		mCrtIdx;

public:
	AosPortDef();
	~AosPortDef() {}

	bool	config(const OmnString &def, const int dft_low, const int dft_high);
	void	reset() {mCrtIdx = 0;}
	void	nextRange(int &low, int &high);
	int		getNumRanges() const {return mNumRanges;}
	bool	hasMore() const {return mCrtIdx < mNumRanges;}

private:
	bool	config(const OmnString &def);
};

#endif
