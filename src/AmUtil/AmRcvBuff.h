////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AmRcvBuff.h
// Description:
//   
//
// Modification History:
// 2007-04-10: Created by CHK
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmUtil_AmRcvBuff_h
#define Aos_AmUtil_AmRcvBuff_h

#include "AmUtil/AmTrans.h"
#include "AmUtil/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"

class OmnConnBuff;

class AosAmRcvBuff : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnTcpClientPtr 	mConn;	// remote tcp client information
	OmnConnBuffPtr 		mBuff;	// Local buffer

	// 
	// No one should use these two
	//
	AosAmRcvBuff(const AosAmRcvBuff &rhs);
	AosAmRcvBuff & operator = (const AosAmRcvBuff &rhs);

public:
	AosAmRcvBuff(const OmnTcpClientPtr &conn);
	AosAmRcvBuff(const OmnTcpClientPtr &conn, const OmnConnBuffPtr &buff);
	~AosAmRcvBuff();

	OmnTcpClientPtr getConn() const;

	bool			append(const OmnConnBuff &buff);
	bool			append(const OmnConnBuffPtr &buff);
	AosAmTransPtr	getTrans();
	int				getHashKey();
	int				hasSameObjId(const AosAmRcvBuffPtr &obj);
};
#endif // Aos_AmUtil_AmRcvBuff_h

