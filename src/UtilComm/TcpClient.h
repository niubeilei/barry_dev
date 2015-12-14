////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpClient.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_TcpClient_h
#define Omn_UtilComm_TcpClient_h

#include "UtilComm/Tcp.h"
#include "Util/SPtr.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommTypes.h"
#include "XmlParser/Ptrs.h"

class OmnTcpClient : public OmnTcp
{
	OmnDefineRCObject;

public:	
	enum ConnStatus
	{
		IDLE,
		READ_FAILED,
		WRITE_FAILED,
		GOOD,
		CLOSED,
		CREATING,
		REESTABLISHING
	};

	enum
	{
		eReconnIntervalTimerInSeconds = 2
	};

protected:
	int				mReconnTrialTimer;
	ConnStatus		mConnStatus;
	int				mPortUsed;
	void *			mUserData;
	int				mClientId;
//	int				(*mReadStyleCallBack)(int, OmnConnBuffPtr &);
	OmnTcpMsgReaderPtr	mMsgReader;
	OmnConnBuffPtr	mConnBuff;

	int 			mLengthIndicatorPos;
	unsigned char 	mLength[4];
	
	// The copy constructor and assignment operator should not be used.
	//
	OmnTcpClient(const OmnTcpClient &rhs);
	OmnTcpClient & operator = (const OmnTcpClient &rhs);

public:
	OmnTcpClient(const OmnIpAddr &remoteIpAddr,
			     const int remotePort,
				 const int numPort,
				 const AosTcpLengthType lt);
	OmnTcpClient(const OmnString &name,
				 const OmnIpAddr &remoteIpAddr,
			     const int remotePort,
				 const int numPort,
				 const AosTcpLengthType lt = eAosTLT_LengthIndicator);
	OmnTcpClient(const OmnIpAddr &remoteIpAddr,
		   const int remotePort,
		   const int remoteNumPorts,
		   const OmnIpAddr &localIpAddr,
		   const int localPort,
		   const int localNumPorts,
		   const OmnString &name,
		   const AosTcpLengthType lt = eAosTLT_LengthIndicator);

	OmnTcpClient(const OmnString &name,
		   const OmnString &uPath,
		   const AosTcpLengthType lt = eAosTLT_LengthIndicator);

    virtual ~OmnTcpClient();

    OmnRslt			connect(OmnString &err);
	OmnRslt			closeConn();
	OmnString		toString() const;

	OmnRslt			sendMsgTo(const OmnMsgPtr &msg);

	virtual int		smartRead(OmnConnBuffPtr &buff);
	// virtual bool	smartSend(const char *data, const int len);
	// bool			smartSend(const OmnString &data)
	// 				{return smartSend(data.data(), data.length());}
	// OmnRslt			readFromSock(char *data, int &length);
	bool			readFrom1(OmnConnBuffPtr &buff, 
							  bool &connBroken,
							  bool &isEndOfFile, 
							  bool appendBuff = false);
	bool			readFrom(OmnConnBuffPtr &buff, const int sec, 
						bool &timeout, bool &connBroken,bool appendBuff = false);
	bool			readMsg(OmnConnBuffPtr &buff, const int sec, 
						bool &timeout, bool &connBroken, const bool);

	void *			getUserData() const {return mUserData;}
	void			setUserData(void *d) {mUserData = d;}
	void			setClientId(const int i) {mClientId = i;}
	int				getClientId() const {return mClientId;}
	OmnConnBuffPtr	getDataBuff();

	void			setMsgReader(const OmnTcpMsgReaderPtr	msgReader);

	bool			truncateBuff(const int len);

	bool 			isReadReady()
					{
						if (mIsBlocking)
						{
							return true;
						}
						else
						{
							return mLengthIndicatorPos == 4 && mConnBuff->getBufferLength() == mConnBuff->getDataLength();
						}
					}
	bool			clearBuffedData();
	
protected:
	OmnRslt			tryToConnect(const bool createSock, OmnString &err);
	int 			readLengthIndicator();
	int 			readBody();
	int 			getDataLength();
};
#endif
