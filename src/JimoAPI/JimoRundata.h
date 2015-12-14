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
// 2014/12/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoRundata_h
#define Aos_JimoAPI_JimoRundata_h

#include "Rundata/Rundata.h"

#include <string>
using namespace std;


class AosJimoRundata
{
	u32		mSiteid;
	u64		mUserid;
	string	mAppname;
	string	mSsid;
	string	mCid;

public:
	AosJimoRundata(
		 const u32 siteid, 
		 const u64 userid,
		 const string &app_name, 
		 const string &ssid,
		 const string &cid);
	~AosJimoRundata();

	AosRundataPtr convert();	
};

#endif
