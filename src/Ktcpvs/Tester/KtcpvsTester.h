////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KtcpvsTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Ktcpvs_Tester_KtcpvsTester_h
#define Omn_Ktcpvs_Tester_KtcpvsTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"

class AosKtcpvsTester : public OmnTestPkg
{
private:
	static int 		mRepeat;
	static int		mNumConns;
	static int		mContentLen;

public:
	AosKtcpvsTester()
	{
		mName = "AosKtcpvsTester";
	}
	~AosKtcpvsTester() {}

	virtual bool		start();
    void    trafficGenFinished(OmnVList<AosTcpTrafficGenClientPtr> &clients);	

	static void setRepeat(const int r) {mRepeat = r;}
	static void setConns(const int c) {mNumConns = c;}
	static void setLen(const int l) {mContentLen = l;}

private:
	bool	processGenerator();
	bool	processBouncer();
};
#endif

