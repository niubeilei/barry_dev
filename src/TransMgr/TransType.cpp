////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TransType.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TransMgr/Trans.h"



OmnString
OmnTransType::toStr(const OmnTransType::E e)
{
	switch (e)
	{
	case eFirstValidEntry:
		 return "FirstValidEntry";

	case eUnknown:
		 return "Unknown";

	case eNullTrans:
		 return "NullTrans";

	case eInfobusTrans:
		 return "InfobusTrans";

	case eStreamTrans:
		 return "StreamTrans";

	case eCreateDataTrans:
		 return "CreateDataTrans";

	case eDeleteDataTrans:
		 return "DeleteDataTrans";

	case eRepliStreamTrans:
		 return "RepliStreamTrans";

	case eRetrieveDataTrans:
		 return "RetrieveDataTrans";

	case eScvsCreateLdeTrans:
		 return "ScvsCreateLdeTrans";
	
	case eSeLogTrans:
		 return "SeLogTrans";

	case eLastValidEntry:
		 return "LastValidEntry";

	default:
		 return OmnString("Unrecognized: ") << e;
	}

	return "Incorrect";
}

