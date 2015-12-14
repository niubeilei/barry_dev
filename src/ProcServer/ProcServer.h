////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ProcServer_ProcServer_h
#define AOS_ProcServer_ProcServer_h

#include "XmlUtil/XmlTag.h"
#include "ProcServer/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "UtilComm/TcpListener.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include <vector>
#include <deque>
#include <map>

using namespace std;

struct machine 
{
	OmnString mMachineName;
	OmnString mMachineIp;
	int 	  mMachinePort;
	OmnString mMachineDoc;
	~ machine()
	{
	}

};
class AosProcServer : virtual public OmnRCObject,
					  virtual public OmnTcpListener
{
	OmnDefineRCObject;


private:
	vector<machine>		mMachines;

	AosRundataPtr		mRundata;
	OmnTcpServerPtr     mServer;
	OmnMutexPtr        	mLock;
	u32             	mTransId;
	OmnString			mSeServerIp;
	int					mSeServerPort;
	bool				mIsSeServer;
	map<OmnString, deque<OmnTcpClientPtr> > mIdleConnsMap;

private:
	OmnTcpClientPtr getConn(const OmnString &addr, const int &port);
	void returnConn(const OmnTcpClientPtr &conn, const OmnString &addr);

public:
	bool	
	isSeServer(){return mIsSeServer;}
	bool 
	updateDocOnServer(OmnString &ssid, OmnString &docstr);

	bool
	login(
		const OmnString &username,
		const OmnString &passwd,
		const OmnString &ctnr_name,
		OmnString &ssid,
		u64 &userid,
		AosXmlTagPtr &userdoc,
		const OmnString &siteid,
		const OmnString &cid = "");

	bool
	sendToServer(
		const OmnString &addr,
		const int &port,
		const OmnString &send_req,
		OmnString &resp,
		const AosRundataPtr &rdata);
	bool
	modifyDocOnServer(const AosXmlTagPtr &doc);
	bool
	retrieveDocByObjid(const OmnString &objid, AosXmlTagPtr &doc);

	AosXmlTagPtr
	retrieveDoc(
		const OmnString &siteid, 
		const OmnString &ssid, 
		const OmnString &objid,
		OmnString &resp);

	AosProcServer(const AosXmlTagPtr &config);
	~AosProcServer();
	virtual void 	connCreated(const OmnTcpClientPtr &conn){}
	virtual void 	connClosed(const OmnTcpClientPtr &conn){}
	virtual void    msgRecved(
						const OmnConnBuffPtr &buff,
			        	const OmnTcpClientPtr &conn);
	virtual OmnString   getTcpListenerName() const {return "ProcConn";};
	bool	sendToProcService(
				const OmnString &req,
				const OmnString &machine_name, OmnString &resp);
	void	sendResp(
				const AosRundataPtr &rdata,
				const OmnTcpClientPtr conn);
	void	sendResp(
				const OmnTcpClientPtr conn,
				const AosXmlRc errcode,
				const OmnString &errmsg,
				const OmnString &contents);
	vector<machine>	getMachines(){return mMachines;}
	bool	setMachines(vector<machine> machines){mMachines = machines; return true;}

};
#endif
