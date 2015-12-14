////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamerTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilCommTester_StreamerTester_h
#define Omn_UtilCommTester_StreamerTester_h

#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/BasicTypes.h"
#include "UtilComm/StreamRequester.h"
#include "UtilComm/Ptrs.h"


class OmnTestMgr;

class OmnStreamerTester : public OmnTestPkg, 
						  public OmnStreamRequester
{
	OmnDefineRCObject;

private:
	uint			mCrtSessionId;
	uint			mTransId;
	OmnStreamerPtr	mStreamer;

public:
	OmnStreamerTester() {mName = "OmnStreamerTester";}
	~OmnStreamerTester() {}

	// 
	// StreamerRequester interface
	//
/*
	virtual void	sendComplete(const OmnRespCode::E respCode,
								 const OmnString &errmsg, 
								 const uint sessionId, 
								 const uint transId);
	virtual void	streamRecved(const uint sessionId, 
								 const OmnStringPtr &buff, 
								 const OmnIpAddr &senderAddr,
								 const int senderPort);
	virtual OmnString	getStreamRequesterName() const {return "StreamerTester";}
*/

	virtual bool		start();
};
#endif

