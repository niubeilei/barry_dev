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
//
// Modification History:
// 01/09/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NativeAPI_SEClient_h
#define Aos_NativeAPI_SEClient_h

#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"

#include "NativeAPI/TcpClient.h"

#include <deque>
using namespace std;

class OmnStrParser1;


class AosSEClient : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxFields = 100
	};

private:
	OmnIpAddr		mRemoteAddr;
	int				mRemotePort;
	u32				mSiteid;
	OmnString		mAppname;
	OmnString		mUsername;
	OmnString		mQueryid;
	bool			mIsGood;
	//OmnTcpClientPtr	mConn;
	//OmnMutexPtr		mLock;
	deque<OmnTcpClientPtr> 	mIdleConns;

public:
	AosSEClient();
	AosSEClient(const OmnIpAddr &raddr, const int rport);
	AosSEClient(const OmnIpAddr &raddr, 
			const int rport, 
			const u32 siteid, 
			const OmnString &appname, 
			const OmnString &username);
	~AosSEClient();

	bool	procRequest(
				const u32 siteid, 
				const OmnString &appname, 
				const OmnString &uname,
				const OmnString &sqlquery, 
				OmnString &resp,
				OmnString &errmsg);

private:
	bool	addObj(
				OmnString &req,
				const OmnString &stmt, 
				OmnString &resp,
				OmnString &errmsg);

	bool	sql(OmnString &req,
				const OmnString &stmt, 
				OmnString &resp,
				OmnString &errmsg);
	bool	updateObj(
				OmnString &req,
				const OmnString &stmt, 
				OmnString &errmsg);
	bool	deleteObj(
				OmnString &req,
				const OmnString &stmt, 
				OmnString &errmsg);
	bool 	parseSelect(
				OmnString &req,
				OmnStrParser1 &parser, 
				OmnString &errmsg);
	bool	parseConds(
				OmnStrParser1 &parser, 
				OmnString &conds, 
				OmnString &startidx, 
				OmnString &psize,
				OmnString &order,
				OmnString &errmsg);
	bool	parseTerm(
				const OmnString &lhs,
				const OmnString &opr,
				OmnString &conds, 
				OmnStrParser1 &parser, 
				OmnString &errmsg);
	bool	parseInrange(
				const OmnString &lhs,
				OmnString &conds, 
				OmnStrParser1 &parser, 
				OmnString &errmsg);

	OmnTcpClientPtr getConn();
	void returnConn(const OmnTcpClientPtr &);

};
#endif

