////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamerServerTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilCommTester_StreamerServerTester_h
#define Omn_UtilCommTester_StreamerServerTester_h

#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/BasicTypes.h"
#include "UtilComm/StreamRequester.h"
#include "UtilComm/Ptrs.h"



class OmnTestMgr;

class OmnStreamerServerTester : public OmnTestPkg, 
						  public OmnStreamRequester
{
	OmnDefineRCObject;

private:
	uint			mCrtSessionId;
	OmnStreamerPtr	mStreamer;

public:
	OmnStreamerServerTester() {mName = "OmnStreamerServerTester";}
	~OmnStreamerServerTester() {}

/*	// 
	// StreamerRequester interface
	//
	virtual void	sendComplete(const OmnRespCode::E respCode,
								 const OmnString &errmsg, 
								 const uint sessionId, 
								 const uint transId);
	virtual void	streamRecved(const uint sessionId, 
								 const OmnStringPtr &buff, 
								 const OmnIpAddr &senderAddr,
								 const int senderPort);
	virtual OmnString	getStreamRequesterName() const {return "StreamTester";}
*/

	virtual bool		start();
};
#endif

