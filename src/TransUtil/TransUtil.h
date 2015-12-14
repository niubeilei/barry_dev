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
// 07/01/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransUtil_TransUtil_h
#define Aos_TransUtil_TransUtil_h

#include "AppMgr/App.h"
#include "UtilData/ModuleId.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define  AosTransLog {\
	ostringstream os; \
	os << hex << "<0x" << pthread_self() << dec << ":" << __FILE__ <<  ":" << __LINE__ << "> " 


#define endlog \
	std::endl;\
	AosTransUtil::log(os);\
}

using namespace std;

class AosTransUtil
{

public:
	enum	MsgType
	{
		eSendTrans				= 1,
		eCltListener,
		eHeartBeat,

		eError,
		//eStopping,
		eConnError,
		eAck,
		eResp,
		//eNotify,
		eFinishTrans,

		eCheckSvrStatus,
	
	};

	enum
	{
		// eSendTrans + send_seq + clientId + clientIdCheck.
		eBuffTransOff = sizeof(u32) + sizeof(u64) + sizeof(u32) + sizeof(int),
	};

	static ofstream mLogFile;
public:

	static void log(const ostringstream &data)
	{
		if (!mLogFile.is_open())
		{
			OmnString fname = OmnApp::getTransDir();
			fname << "/trans_log.txt"; 
			mLogFile.open(fname.data(), ios::app|ios::out);
		}
		mLogFile << data.str(); 
		mLogFile.flush();
		
		//cout << data.str();
		//cout.flush();
	}

	// Chen Ding, 2013/03/02
	bool createMsg(
				OmnString &msg, 
				const u32 send_cubeid,
				const u32 recv_cubeid,
				const AosModuleId::E send_modid,
				const AosModuleId::E recv_modid,
				const OmnString &msg_id, 
				const bool is_req);

};
#endif
