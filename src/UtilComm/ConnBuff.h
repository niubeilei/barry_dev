////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ConnBuff.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_ConnBuff_h
#define Omn_UtilComm_ConnBuff_h

#include "Message/MsgId.h"
#include "Network/NetEtyType.h"
#include "Porting/IPv6.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"


class OmnMemPool;

class OmnConnBuff : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum 
	{
		eDefaultBuffSize = 6000,
		eMaxBuffSize = 2000000000,		// 2G
		eIncrSize = 500,
		
		eLastSegment = 0x01,		// The buff is the last segment
		eMsgFlag	 = 0x02			// The buff is a message.
	};

private:
	static OmnMemPool *	csBufferPool;

	OmnIpAddr			mLocalAddr;
	int					mLocalPort;
	OmnIpAddr			mRemoteAddr;
	int					mRemotePort;

	//char 				mDefault[eDefaultBuffSize];
	char *				mDynamic;
	char *				mDataPtr;
	uint				mDataLength;
	uint				mBuffLength;

	OmnCommPtr			mComm;
	OmnTcpClientPtr		mConn;
	uint				mSessionId;
	uint				mSegId;
	uint				mFlags;

	// Chen Ding, 04/13/2010
	OmnConnBuffPtr		mPrev;
	OmnConnBuffPtr		mNext;

public:
	OmnConnBuff();
	OmnConnBuff(const uint length);
	OmnConnBuff(const char *data, const uint length, const bool);
	OmnConnBuff(const OmnConnBuff &rhs);
	OmnConnBuff & operator = (const OmnConnBuff &rhs);
	~OmnConnBuff();

	char *			getData() {return mDataPtr;}
	int				getDataLength() const {return mDataLength;}
	void			setDataLength(const int data_len) {mDataLength = data_len;}
	char *			getBuffer() {return mDataPtr;}
	int				getBufferLength() const {return mBuffLength;}
	OmnString		getString() const;

	OmnIpAddr		getLocalAddr() const {return mLocalAddr;}
	int				getLocalPort() const {return mLocalPort;}
	OmnIpAddr		getRemoteAddr() const {return mRemoteAddr;}
	int				getRemotePort() const {return mRemotePort;}
	uint			getSessionId() const {return mSessionId;}
	uint			getSegId() const {return mSegId;}
	bool			isLastSeg() const {return mFlags & eLastSegment;}

	// Chen Ding, 04/13/2010
	OmnCommPtr		getComm();
	OmnConnBuffPtr	getPrev() const {return mPrev;}
	OmnConnBuffPtr	getNext() const {return mNext;}
	void			setPrev(const OmnConnBuffPtr &p) {mPrev = p;}
	void			setNext(const OmnConnBuffPtr &n) {mNext = n;}

	bool			setDataLength(const uint len);
	static bool		attachSenderAddr(char *data, uint &length,
						const OmnIpAddr &addr, const int port);
	bool			attachSenderAddr();
	bool			restoreSenderAddr();
	void			setSessionId(const uint s) {mSessionId = s;}
	void			setSegId(const uint s) {mSegId = s;}
	void			setComm(const OmnCommPtr &c);
	bool			append(const OmnConnBuffPtr &buff);
	bool			append(const OmnConnBuff &buff);
	void			append(const char *data, const int length);

	void			setAddr(const OmnIpAddr &localIp, const int localPort,
							const OmnIpAddr &remoteIp, const int remotePort)
					{
						mLocalAddr = localIp;
						mLocalPort = localPort;
						mRemoteAddr = remoteIp;
						mRemotePort = remotePort;
					}
	
	void			setRemoteAddr(const OmnIpAddr &remoteIp, const int remotePort)
					{
						mRemoteAddr = remoteIp;
						mRemotePort = remotePort;
					}

	void			resetRemoteAddr()
					{
						mRemotePort = -1;
					}

	void			set(const char *data, 
						const int length, 
						const OmnConnBuffPtr &);
	void dumpHex(OmnString &str);

	OmnConnBuff & operator << (const int value);
	bool	set(const char *data, const uint length);

	OmnRslt		prepareToSend();
	OmnRslt		prepareToProcRecv();
	void		setLastSegment(const bool b) 
	{
		if (b)
		{
			mFlags |= eLastSegment;
		}
		else
		{
			mFlags &= ~eLastSegment;
		}
	}

	bool		isMsg() const {return (mFlags & eMsgFlag) != 0;}
	void		setMsgFlag(const bool b)
	{
		if (b)
		{
			mFlags |= eMsgFlag;
		}
		else
		{
			mFlags &= ~eMsgFlag;
		}
	}

	bool		removeHeader(const int len);

	// Chen Ding, 05/06/2009
	void	setConn(const OmnTcpClientPtr &conn);
	OmnTcpClientPtr	getConn() const;
	bool	determineMemory(const uint newLength);

	// Chen Ding, 10/27/2010
	bool expandMemory()
	{
		return determineMemory(mBuffLength + eDefaultBuffSize);
	}

private:
};
#endif

