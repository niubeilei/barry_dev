////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Msg.h
// Description:
//	This is an interface. All messages in this environment derive
//  from this class.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Message_Msg_h
#define Omn_Message_Msg_h

#include "aosUtil/Types.h"
#include "Debug/ErrId.h"
#include "Debug/Rslt.h"
#include "Logger/Ptrs.h"
#include "Message/MsgId.h"
#include "Message/Ptrs.h"
#include "Message/ModuleId.h"
#include "Network/NetEtyType.h"
#include "Porting/TimeOfDay.h"
#include "TransMgr/Ptrs.h"
#include "Util/BasicTypes.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"


class OmnSerialFrom;
class OmnSerialTo;

#ifndef OmnRegisterMsgFactory
#define OmnRegisterMsgFactory(className) \
	static sgMsgCreatorInitializer25423435 = \
			className::registerMsg();
#endif

#ifndef OmnDeclareMsg
#define OmnDeclareMsg bool mMsgCreatorInitializer25423435;
#endif

#ifndef OmnMsgSendReason
#define OmnMsgSendReason(m, x) m->mFile = __FILE__; m->mLine = __LINE__; m->mReason = x;
#endif

#ifndef OmnMsgSendReason1
#define OmnMsgSendReason1(m, y, z, x) m->mFile = y; m->mLine = z; m->mReason = x;
#endif

#ifndef OmnDisplayMsgSendReason
#define OmnDisplayMsgSendReason    "\n    File:     " << mFile \
	                            << "\n    Line:     " << mLine \
								<< "\n    Reason:   " << mReason
#endif

#ifndef OmnPopMsgSendReason
#define OmnPopMsgSendReason s >> mFile; \
							s >> mLine; \
							s >> mReason;
#endif

#ifndef OmnPushMsgSendReason
#define OmnPushMsgSendReason s << mFile; \
							s << mLine; \
							s << mReason;
#endif

class OmnMsg : virtual public OmnRCObject
{
private:
	uint			mSeqno;		// Unique to all messages. Not transported to 
								// outside the program (i.e., not in the serialization
								// functions.

protected:
	enum
	{
		eTransId, 
		eModuleId,
		eRecverDn,
		eSenderId,
		eFlag,
		eFile,
		eLine,
		eReason,

		eExpectResp = 0x01
	};

	int64_t			mTransId;
	OmnTransPtr		mTrans;
	int				mModuleId;		// Used to identify responsible module
	OmnIpAddr		mSenderAddr;
	int				mSenderPort;
	OmnIpAddr		mRecverAddr;
	int				mRecverPort;
	OmnString		mRecverDn;
	int				mSenderId;
	unsigned int	mFlag;

public:
	OmnString		mFile;
	int				mLine;
	OmnString		mReason;

public:
	OmnMsg();
	OmnMsg(const OmnIpAddr &senderAddr,
		   const int senderPort,
		   const OmnIpAddr &receiverAddr,
		   const int receiverPort);
	OmnMsg(const OmnIpAddr &recvAddr, const int recvPort);
	virtual ~OmnMsg();


	// 
	// Pure virtual functions
	//
	virtual OmnMsgPtr		createInst(OmnSerialFrom &s) = 0;
	virtual OmnMsgId::E		getMsgId() const = 0;
	virtual OmnMsgId::E		getSubtype() const = 0;
	virtual OmnString		toString() const = 0;
	virtual OmnRslt			serializeFrom(OmnSerialFrom &s) = 0;
	virtual OmnRslt			serializeTo(OmnSerialTo &s) const = 0;
	virtual OmnMsgPtr		clone() const = 0;
	virtual bool			isReq() const = 0;
	virtual bool			isResp() const = 0;
	
	OmnString getMsgName() const 
	{
		return OmnMsgId::toStr(getMsgId());
	}

	uint	getSeqno() const {return mSeqno;}

	void	setModuleId(const OmnModuleId::E m) {mModuleId = m;}
	OmnModuleId::E	getModuleId() const {return (OmnModuleId::E)mModuleId;}

	void		setTrans(const OmnTransPtr &trans);
	OmnTransPtr	getTrans() const;

	// static void	setMsgLogger(const OmnMsgLoggerPtr &logger);
	bool		logMsg();
	void		clone(const OmnMsgPtr &msg) const;
	void		setSenderId(const int senderId) {mSenderId = senderId;}
	void		setRecvDn(const OmnString &dn) {mRecverDn = dn;}
	OmnRslt		send();

	bool		expectResponse() const {return mFlag & eExpectResp;}
	void		setExpectRespFlag(const bool f)
	{
		if (f)
		{
			mFlag |= eExpectResp;
		}
		else
		{
			mFlag &= ~eExpectResp;
		}
	}

	int64_t			getTransId() const {return mTransId;}
	void			setTransId(const int64_t &t) {mTransId = t;}

	OmnString		getRecvDn() const {return mRecverDn;}
	bool			isRecvAddrValid() const {return mRecverAddr.isValid() && mRecverPort > 0;}

	void setSenderAddr(const OmnIpAddr &addr, const int port)
	{
		mSenderAddr = addr;
		mSenderPort = port;
	}

	void setRecverAddr(const OmnIpAddr &addr, const int port)
	{
		mRecverAddr = addr;
		mRecverPort = port;
	}

	void toggleAddrFrom(const OmnMsgPtr &msg)
	{
		mSenderAddr = msg->mRecverAddr;
		mSenderPort = msg->mRecverPort;
		mRecverAddr = msg->mSenderAddr;
		mRecverPort = msg->mSenderPort;
	}

	OmnIpAddr	getSenderAddr() const {return mSenderAddr;}
	OmnIpAddr	getRecverAddr() const {return mRecverAddr;}
	int			getSenderPort() const {return mSenderPort;}
	int			getRecverPort() const {return mRecverPort;}
	void		setRecverPort(const int port) {mRecverPort = port;}
	void		reset();
};
#endif
