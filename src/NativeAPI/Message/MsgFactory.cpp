////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MsgFactory.cpp
// Description:
//	This class is used to create messages. This is a singleton class.
//	It maintains an array of Message Creator. Each message should 
//  register with this factory in order for the factory to create 
//  instances for it.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Message/MsgFactory.h"

#include "Debug/Debug.h"
#include "Message/Msg.h"
#include "Message/MsgCreator.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/SerialFrom.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(OmnMsgFactorySingleton,
                 OmnMsgFactory,
                 OmnMsgFactorySelf,
                "OmnMsgFactory");

OmnMsgPtr OmnMsgFactory::mMsgCreator[OmnMsgId::eLastValidEntry];

OmnMsgFactory::OmnMsgFactory()
{
}


OmnMsgFactory::~OmnMsgFactory()
{
}


OmnMsgPtr
OmnMsgFactory::createMsg(const OmnMsgId::E msgId)
{
	OmnCheckAReturn(msgId > 0, OmnString("Msg ID is negative: ") << msgId, 0);
	OmnCheckAReturn(msgId < OmnMsgId::eLastValidEntry, OmnString("Msg Id too big: ") << msgId, 0);
	OmnCheckAReturn(!mMsgCreator[msgId].isNull(), "Msg Creator is null", 0);

	return mMsgCreator[msgId]->clone();
}


OmnMsgPtr	
OmnMsgFactory::createMsg(const OmnConnBuffPtr &buff)
{
	// 
	// This function creates a message using the buffer 'buff'. It first
	// pops up the message ID (assuming it is a message). This factory
	// maintains an array of OmnMsgCreator. If it is a valid message id,
	// 
	OmnSerialFrom s(buff);
	OmnMsgId::E msgId = s.popMsgId();

	OmnCheckAReturn(msgId > 0, OmnString("Msg ID is negative: ") << msgId, 0);
	OmnCheckAReturn(msgId < OmnMsgId::eLastValidEntry, OmnString("Msg Id too big: ") << msgId, 0);

	// 
	// Check whether the creator is there. 
	//
	if (!mMsgCreator[msgId])
	{
		OmnAlarmProgError << "Message creator is null: " 
			<< OmnMsgId::toStr(msgId) << enderr;
		return 0;
	}

	try
	{
		OmnMsgPtr msg = mMsgCreator[msgId]->clone();
		if (msg->serializeFrom(s))
		{
			// 
			// Msg created successfully
			//
			return msg;
		}

		// 
		// Serialization failed
		//
		OmnAlarm << "Failed to create message: " << msgId << enderr;
		return 0;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to create message: " 
			<< e.getErrmsg() << enderr;
		return 0;
	}

	return 0;
}


bool			
OmnMsgFactory::start()
{
	return true;
}


bool			
OmnMsgFactory::stop()
{
	return true;
}


bool
OmnMsgFactory::config(const AosXmlTagPtr &def)
{
	return true;
}


bool	
OmnMsgFactory::registerMsgCreator(OmnMsg *creator)
{
	OmnCheckAReturn(creator, "Creator is null", false);
	
	OmnMsgId::E msgId = creator->getMsgId();
	OmnCheckAReturn(msgId >= OmnMsgId::eInvalidMsgId && 
		msgId <= OmnMsgId::eLastValidEntry, 
		OmnString("Invalid message ID: ") << msgId, false);

	if (!mMsgCreator[msgId])
	{
		mMsgCreator[msgId] = creator;
	}
	return true;
}

