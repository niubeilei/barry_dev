////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//	This is the database for MySQL.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataServer_Ptrs_h
#define Omn_DataServer_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(OmnMgcpEndpoint, OmnMgcpEndpointPtr);

class OmnDataMgr;
class OmnMgcpEptRepli;
class OmnMgcpConnRepli;

typedef OmnSPtr<OmnDataMgr>			OmnDataMgrPtr;
typedef OmnSPtr<OmnMgcpEptRepli>	OmnMgcpEptRepliPtr;
typedef OmnSPtr<OmnMgcpConnRepli>	OmnMgcpConnRepliPtr;


#endif
