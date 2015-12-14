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
// 01/03/2010: Copy by Brian 
////////////////////////////////////////////////////////////////////////////// 
#ifndef Aos_ShortMsgUtil_GsmModem_h 
#define Aos_ShortMsgUtil_GsmModem_h

#include "Rundata/Rundata.h" 
#include "ShortMsgUtil/CommOpr.h" 
#include "ShortMsgUtil/Ptrs.h"
#include "ShortMsgUtil/ShortMsgUtil.h" 
#include "ShortMsgUtil/ShortMsg.h" 
#include "ShortMsgUtil/SmsProc.h" 
#include "Util/String.h" 

class AosGsmModem : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:

	int							mTryToRead;
    int 						mSerialPort; 
    int							mDelayTime; 
    OmnString					mPhone;
	OmnString					mAreaCode;
	OmnString					mPduSvrPhone;
	AosSmsProcPtr				mProc;

public:
	AosGsmModem(const AosXmlTagPtr &config);
	~AosGsmModem();

	bool 			openModem(const int &port);  
	
	bool 			sendMessage(
					AosUCharPtr_t pUserCode,
					AosUCharPtr_t pMessage,
					const AosRundataPtr &rdata);  
	
	OmnString       getLocalNum(){return mPhone;}
	void 			closeModem(); 

private:
	bool			init(const AosXmlTagPtr &config);

	bool			convertToPdu(
					AosUCharPtr_t szTextHEX,
					const int &length, 
					AosUCharPtr_t szUserCodePDU,
					AosSmsReqPtr &sReq,
					const AosRundataPtr &rdata);

	bool			readFromPort();	
};
#endif
