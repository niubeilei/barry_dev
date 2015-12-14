////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Resp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_Resp_h
#define Omn_Message_Resp_h

#include "Debug/ErrId.h"
#include "Message/Msg.h"
#include "Message/Ptrs.h"
#include "Util/RCObjImp.h"


class OmnResp : public OmnMsg
{
	OmnDefineRCObject;

protected:
	enum 
	{
		eRespCode, 
		eErrmsg
	};

	OmnErrId::E		mRespCode;
	OmnString		mErrmsg;	

public:
	OmnResp();
	OmnResp(const OmnReqPtr &req);
	OmnResp(const OmnIpAddr &senderIpAddr,
		    const int senderPort,
		    const OmnIpAddr &receiverIpAddr,
		    const int receiverPort);
	virtual ~OmnResp();

	virtual bool		isReq() const {return false;}
	virtual bool		isResp() const {return true;}

    virtual OmnString   toString() const = 0;
    virtual OmnRslt     serializeFrom(OmnSerialFrom &s) = 0;
    virtual OmnRslt     serializeTo(OmnSerialTo &s) const = 0;
    
	void clone(const OmnRespPtr &rhs) const
	{
		rhs->mRespCode = mRespCode;
		rhs->mErrmsg = mErrmsg;
	}

	bool			isPositive() const {return mRespCode == OmnErrId::e200Ok;}
	OmnErrId::E		getRespCode() const {return mRespCode;}
	OmnString		getErrmsg() const {return mErrmsg;}

	void	setRespCode(const OmnErrId::E c) {mRespCode = c;}
	void	setErrmsg(const OmnString &errmsg) {mErrmsg = errmsg;}
};
#endif
