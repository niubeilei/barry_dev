////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tcp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NativeAPI_Tcp_h
#define Aos_NativeAPI_Tcp_h


#include "NativeAPI/UtilComm/AddrDef.h"
#include "NativeAPI/UtilComm/PortDef.h"
#include "NativeAPI/UtilComm/CommTypes.h"
#include "NativeAPI/UtilComm/ConnBuff.h"

#include "NativeAPI/Porting/Socket.h"
#include "NativeAPI/Util/IpAddr.h"

#include "NativeAPI/Util/SPtr.h"
#include "NativeAPI/Rslt.h"



class OmnMutex;


class OmnTcp //: public virtual OmnRCObject
{
public:
	//
	// add by lxx 08/24/2003 2003-0112
	//
	enum
	{
		eTwoByteMaxLen = 64000,
		eMaxTcpLength = 30000
	};

	//
	// Sock Type we support
	//
	enum
	{
		eTypeTcp = 0, 
		eTypeUnix,
	};

protected:
    int                 mSock;
	int					mSockType;
	OmnString			mUnixPath;

	OmnString			mName;
	OmnMutexPtr			mLock;

	OmnIpAddr			mLocalIpAddr;
	int					mLocalCrtPort;
	int					mLocalStartPort;

	OmnIpAddr			mRemoteIpAddr;
	int					mRemoteCrtPort;
	int					mRemoteStartPort;

	int					mRemoteNumPorts;
	int					mLocalNumPorts;
	AosTcpLengthType	mLengthType;

	bool				mIsBlocking;
	//
	// The copy constructor and assignment operator should not be used.
	//
	OmnTcp(const OmnTcp& OmnTcp);
	OmnTcp& operator = (const OmnTcp& tcp);

public:
	OmnTcp(const OmnIpAddr &localIpAddr,
		   const int localPort,
		   const int numPorts,
		   const OmnString &name,
		   const AosTcpLengthType lt);

	OmnTcp(AosAddrDef &addrDef, 
		   AosPortDef &ports, 
		   const OmnString &name, 
		   const AosTcpLengthType length_type);

	OmnTcp(const OmnString &uPath,
		   const OmnString &name,
		   const AosTcpLengthType lt);
		   
	OmnTcp(const OmnString &name,
		   const OmnIpAddr &remoteIpAddr,
		   const int remotePort,
		   const int numPorts,
		   const AosTcpLengthType lt);

	OmnTcp(const OmnIpAddr &remoteIpAddr,
		   const int remotePort,
		   const int remoteNumPorts,
		   const OmnIpAddr &localIpAddr,
		   const int localPort,
		   const int localNumPorts,
		   const OmnString &name,
		   const AosTcpLengthType lt);

//	OmnTcp(const OmnXmlItemPtr &conf);
	virtual ~OmnTcp();


	OmnString			getReadError() const;
	OmnString			getWriteError() const;

	//
	// Writes
	//
	/*
	virtual OmnRslt		writeTo(const char *data, const int len);

	virtual OmnRslt     writeTo(const char *data,
                      		const int length,
                      		const OmnIpAddr &recvIpAddr,
                      		const int recvPort);
    */


	virtual OmnRslt		connect(OmnString &) = 0;
	virtual OmnRslt		closeConn() = 0;
	virtual bool		isConnGood() const;
	virtual OmnString	getName() const {return mName;}
	bool 				hasPacketToRead() const;

	void				setSock(const int sock) {mSock = sock;}
	int					getSock() const {return mSock;}

	OmnIpAddr			getLocalIpAddr() const {return mLocalIpAddr;}
	int					getLocalPort() const {return mLocalCrtPort;}
	int					getLocalStartPort() const {return mLocalStartPort;}

	OmnIpAddr			getRemoteIpAddr() const {return mRemoteIpAddr;}
	int					getRemotePort() const {return mRemoteCrtPort;}
	int					getRemoteStartPort() const {return mRemoteStartPort;}


	void				setLocalIpAddr(const OmnIpAddr localIpAddr) {mLocalIpAddr = localIpAddr;}
	void				setLocalPort(const int localCrtPort) {mLocalCrtPort= localCrtPort;}
	void				setLocalStartPort(const int localStartPort) {mLocalStartPort= localStartPort;}

	void				setRemoteIpAddr(const OmnIpAddr remoteIpAddr) {mRemoteIpAddr = remoteIpAddr;}
	void				setRemotePort(const int remoteCrtPort) {mRemoteCrtPort = remoteCrtPort;}
	void				setRemoteStartPort(const int remoteStartPort) {mRemoteStartPort = remoteStartPort;}

	int					checkEvent(fd_set *fdMask);

	int					readLengthIndicator();
	OmnRslt				writeToSock(OmnIoBuff* msgStr, int msgLen);

	//
	// Chen Ding, 08/26/2005
	//
	void				setLengthType(const AosTcpLengthType l) {mLengthType = l;}
	void				setBlockingType(const bool blocking){mIsBlocking = blocking;}
	bool				getBlockingType() { return mIsBlocking; }
	virtual bool 		smartSend(const char *data, const int len);
	virtual bool 		smartSend(const char *, const int, const char *, const int);
	bool				smartSend(const OmnString &data)
	 					{return smartSend(data.data(), data.length());}
	int		smartRead(OmnConnBuffPtr &buff);
	OmnRslt 			readFromSock(char *data, int &bytesToRead);

private:
	void				handleWriteError(const OmnErrId::E errcode);

	// Chen Ding, 2013/01/09
	bool writeToSockNonBlock(OmnIoBuff *buff, const int num_buffs);

public:
	// Chen Ding, 06/11/2012
	static void setIgnoredPorts(const OmnString &ss);
};
#endif
