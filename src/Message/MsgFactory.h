////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MsgFactory.h
// Description:
//	This class is used to create messages. This is a singleton class.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_MsgFactory_h
#define Omn_Message_MsgFactory_h

#include "Message/Ptrs.h"
#include "Message/MsgId.h"
//#include "Message/MsgFctryIntf.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"

#ifndef OmnRegisterMsg
#define OmnRegisterMsg(msgId, creator) OmnMsgFactorySelf->registerMsgCreator(msgId, creator)
#endif

OmnDefineSingletonClass(OmnMsgFactorySingleton,
                		OmnMsgFactory, 
						OmnMsgFactorySelf,
						OmnSingletonObjId::eMsgFactory, 
						"MsgFactory");

class OmnMsgFactory 
{
private:
	static OmnMsgPtr	mMsgCreator[OmnMsgId::eLastValidEntry];

public:
	OmnMsgFactory();
	virtual ~OmnMsgFactory();

	OmnMsgPtr	createMsg(const OmnConnBuffPtr &buff);
	OmnMsgPtr	createMsg(const OmnMsgId::E msgId);

	static bool	registerMsgCreator(OmnMsg *creator);

	// 
	// Singleton class interface
	//
	static OmnMsgFactory *	getSelf();
	virtual bool			start();
	virtual bool			stop();
	virtual bool			config(const AosXmlTagPtr &def);
};
#endif
