////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ProxyComm.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_ProxyComm_h
#define Omn_UtilComm_ProxyComm_h

#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Comm.h"



class OmnProxyComm : public OmnComm
{
private:
	OmnIpAddr			mLocalIpAddr;
	int					mLocalPort;
	OmnRCObjImp			mRCObj;
	int					mNIID;

	OmnString			mLastReadErrMsg;
	OmnErrId::E			mLastReadErrId;
	OmnString			mLastWriteErrMsg;
	OmnErrId::E			mLastWriteErrId;

	//
	// Do not use the following
	//
	OmnProxyComm(const OmnProxyComm &rhs);
	OmnProxyComm & operator = (const OmnProxyComm &rhs);

public:
	OmnProxyComm(const OmnString &name,
			   const int niid,
			   const OmnIpAddr &localIpAddr,
			   const int localPort);
	virtual ~OmnProxyComm();

	//
	// Writes
	//
	virtual bool		readFrom(OmnMsgPtr &msg, 
							const int timerSec = -1,
							const int timeruSec = 0);
	virtual OmnRslt		sendTo(const OmnMsgPtr &msg);
	virtual bool		isConnGood() const;
	virtual bool		connect();
	virtual OmnString	toString() const;
	virtual bool		closeConn();
	virtual int			getNIID() const {return mNIID;}

	int		getProxyPort();
	bool	returnProxyPort(const int port);
	bool	openProxyPort(const int port);
	bool	closeProxyPort(const int port);

	virtual OmnString	getLastReadErrMsg() const {return mLastReadErrMsg;}
	virtual OmnErrId::E	getLastReadErrId() const {return mLastReadErrId;}
	virtual OmnString	getLastWriteErrMsg() const {return mLastWriteErrMsg;}
	virtual OmnErrId::E	getLastWriteErrId() const {return mLastWriteErrId;}

	virtual OmnIpAddr	getLocalIpAddr() const {return mLocalIpAddr;}
	virtual int			getLocalPort() const {return mLocalPort;}

	virtual void		addRef() {mRCObj.addRef();}
	virtual int		removeRef(const bool delFlag) 
					{
						int result = mRCObj.removeRef();
						if (result <= 0 && delFlag) OmnDelete this;
						return result;
					}
};
#endif