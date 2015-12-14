////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TransType.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TransMgr_TransType_h
#define Omn_TransMgr_TransType_h


class OmnTransType
{
public:
	enum E
	{
		eFirstValidEntry,

		eUnknown,

		eNullTrans,
		eInfobusTrans,
		eStreamTrans,

		eCreateDataTrans,
		eDeleteDataTrans,
		eRepliStreamTrans,
		eRetrans,
		eRetrieveDataTrans,

		eScvsCreateLdeTrans,
		eScvsStreamTrans,

		eStreamRecvTrans,
		eStreamSendTrans,
		eSeLogTrans,

		eLastValidEntry
	};

	static OmnString toStr(const E e);
};
#endif

