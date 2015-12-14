////////////////////////////////////////////////////////////////////////////
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
// 08/16/2010: Created by James
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_SEModules_OnlineUser_h
#define Aos_SEModules_OnlineUser_h

#include "Proggie/ProggieUtil/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/SeXmlParser.h"
#include <queue>

#include "Thread/Ptrs.h"                                                                                            
#include "Thread/CondVar.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"

using namespace std;

class AosOnlineUser : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDefaultWaitSec = 20
	};

	OmnMutexPtr			mLock;
	OmnCondVarPtr   	mCondVar;
	queue<OmnString> 	mMsgs;
	int 				mWaitTimeSec;

public:
	AosOnlineUser();
	~AosOnlineUser();
	bool readMsg(OmnString &msg, bool &istimeout);
	bool sendMsg(const OmnString &msg);

};
#endif

