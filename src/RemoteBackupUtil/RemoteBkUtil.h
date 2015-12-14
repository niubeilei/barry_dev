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
// 02/16/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RemoteBackupUtil_RemoteBkUtil_h
#define Aos_RemoteBackupUtil_RemoteBkUtil_h

class AosRemoteBkUtil
{
public:
	static void decomposeTransid(const u64 &transid, u32 &clientid, u32 &seqno)
	{
		clientid = (transid >> 32);
		seqno = transid;
	}
};
#endif



