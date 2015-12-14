////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpServer_h 
#define Omn_UtilComm_TcpServer_h 


#include "Porting/Socket.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/ValList.h"
#include "Util/SPtr.h"
#include "Util/Array10.h"
#include "Util/IpAddr.h"
#include "UtilComm/Tcp.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/Ptrs.h"
#include "Util/RCObjImp.h"



class OmnTcpServer : public OmnTcp,
					 public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnVList<OmnTcpClientPtr>		mConnections;
	OmnMutexPtr						mLock;
	fd_set 							mReadFds; 		  
	fd_set							mWorkingFds;
	int 							mFdscnt;
	OmnTcpListenerPtr				mListener;
	OmnTcpMsgReaderPtr				mReader;
	OmnThreadPtr					mThread;

	bool								mConnFlag;
public:
	OmnTcpServer(const OmnIpAddr &localIpAddr,
				 const int localPort,
				 const int numPorts,
				 const OmnString &name,
				 const AosTcpLengthType lt = eAosTLT_LengthIndicator);
	OmnTcpServer(const OmnString &uPath,
				 const OmnString &name,
				 const AosTcpLengthType lt = eAosTLT_LengthIndicator);
	~OmnTcpServer();
   
	OmnRslt		connect(OmnString &err); 
	OmnRslt		closeConn();
	OmnString	getConnInfo() const;
	OmnString	getName() const {return mName;}
	bool		anyToRead();

	void		startReading();
	void		stopReading();
	void		setListener(const OmnTcpListenerPtr &listener);
	void			setReader(const OmnTcpMsgReaderPtr &reader);

	bool		sendTo(const OmnMsgPtr &msg);

	OmnRslt		writeTo(const char *data, const int length, const int sock);
	OmnRslt		writeTo(const char *data, const int length, const OmnTcpClientPtr client);
	OmnRslt		writeTo(OmnString str, const OmnTcpClientPtr client);

	void		readFromNew(OmnConnBuffPtr &buf, OmnTcpClientPtr &conn);
//	OmnRslt		readFrom(char *data, 
//						 int &length, 
//						 OmnIpAddr &ipAddr, 
//						 int &port, 
//						 int &sock);

	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 
	void 		setReadStyleCallBack(int (*callback)(int, void *));
	void 		clearReadStyleCallBack();

	bool		getClientByRemoteAddr(
					const OmnIpAddr addr, 
					const int port, 
					OmnTcpClientPtr & client);

	//
	//	If conn flag  off, close all the current conns , and refuse to connect 
	//	other client, else run normally
	//
	void			setConnFlag(const bool connFlag);
	
	virtual bool 		postProc();

private:
	bool 				delConn(const int conn_id);
	OmnTcpClientPtr		getClient(const int sockId);
	OmnRslt				removeConn(const int sockId);
	bool				initialize();
	bool 				waitOnEvent(OmnTcpClientPtr &client,bool &timeout);
	OmnRslt				closeConn(const OmnTcpClientPtr &client);
	OmnTcpClientPtr		getConn(const int theSock);
	bool 				getConnEvent(OmnTcpClientPtr &client);
	OmnTcpClientPtr		acceptNewConn();
	bool 				addConn(const OmnTcpClientPtr &client);
	void				checkConns();
};
#endif

