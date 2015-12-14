////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 02/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEModules_FrontEndSvr_h
#define AOS_SEModules_FrontEndSvr_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "UtilComm/Ptrs.h"
#include <vector>
using namespace std;


OmnDefineSingletonClass(AosFrontEndSvrSingleton,
						AosFrontEndSvr,
						AosFrontEndSvrSelf,
						OmnSingletonObjId::eFrontEndSvr,
						"FrontEndSvr");


class AosFrontEndSvr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:

private:
	struct ServerInfo
	{
		int server_id;
		OmnString ip;
		int port;
		OmnTcpClientPtr conn;

		ServerInfo()
		:
		server_id(-1),
		ip(0),
		port(-1)
		{
		}

		ServerInfo(const int sid, const OmnString &p, const int t)
		:
		server_id(sid),
		ip(p),
		port(t)
		{
		}
	};
			
	int					mServerId;
	vector<ServerInfo>	mServerInfo;

public:
	AosFrontEndSvr();
	~AosFrontEndSvr();

    // Singleton class interface
    static AosFrontEndSvr*  	getSelf();
    virtual bool      			start();
    virtual bool        		stop();
    virtual bool				config(const AosXmlTagPtr &def);

	int getServerId() const {return mServerId;}
	bool checkSsid(const OmnString &ssid);
};
#endif

