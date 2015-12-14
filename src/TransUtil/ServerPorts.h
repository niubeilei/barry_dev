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
// 06/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransUtil_ServerPorts_h
#define Aos_TransUtil_ServerPorts_h


class AosServerPorts 
{
public:
	enum E
	{
		eInvalidPort = 0,

		ePubServerPort 		= 20,
		eDocServerPort 		= 25,
		eBigIILServerPort 	= 30,
		eIILServerPort 		= 35,
		eStatServerPort 	= 40,
		eMsgServerPort 		= 45,
		eShortMsgServerPort = 50,
		eTaskServerPort 	= 55,
		eSqlServerPort 		= 60,
		eEmailServerPort 	= 65,
		eLogServerPort 		= 70,

		eMaxPort
	};

private:
	static int		smBasePort;

public:
	static int getPubServerPort() 		{return smBasePort + ePubServerPort;}
	static int getDocServerPort() 		{return smBasePort + eDocServerPort;}
	static int getIILServerPort() 		{return smBasePort + eIILMgrPort;}
	static int getBigIILServerPort()	{return smBasePort + eBigIILMgrPort;}
	static int getLogServerPort()		{return smBasePort + eLogServerPort;}
	static int getStatServerPort()		{return smBasePort + eStatServerPort;}
	static int getMsgServerPort()		{return smBasePort + eMsgServerPort;}
	static int getShortMsgServerPort()	{return smBasePort + eShortMsgServerPort;}
	static int getTaskServerPort()		{return smBasePort + eTaskServerPort;}
	static int getSqlServerPort()		{return smBasePort + eSqlServerPort;}
	static int getEmailServerPort()		{return smBasePort + eEmailServerPort;}
};
#endif

