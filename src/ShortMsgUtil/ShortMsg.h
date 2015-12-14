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
// 06/21/2011: Created by  Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ShortMsgUtil_ShortMsg_h
#define Aos_ShortMsgUtil_ShortMsg_h

struct AosShortMsg
{
	OmnString	mServerCenterNumber;
	OmnString   mSenderNumber;
	OmnString   mRcvTime;
	OmnString   mMsg;
	OmnString   mGsmPhone;

	OmnString toString()
	{
		OmnString ss;
		ss << "Server Center Number: " << mServerCenterNumber 
			<< "; Sender Number: " << mSenderNumber 
			<< "; Receive Message Time : " << mRcvTime
			<< "; Receive Message : " << mMsg;
		return ss;
	};
	
	void setServerCenterNumber(const OmnString &number)
	{
		mServerCenterNumber = number;
	};
	
	void setSenderNumber(const OmnString &number)
	{
		mSenderNumber = number;
	};
	
	void setRcvTime(const OmnString &time)
	{
		mRcvTime = time;
	};
	
	void setMsg(const OmnString &msg)
	{
		mMsg = msg;
	};
	void setGsmPhone(const OmnString &phone)
	{
		mGsmPhone = phone;
	};
};
#endif

