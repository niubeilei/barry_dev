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
//   DocStore API from DocPkg to Cube
//
// Modification History:
// 2014/11/24 Created by Chen Ding
// 2015/03/14 Copied from Jimo.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoRaftAPI_h
#define Aos_JimoAPI_JimoRaftAPI_h

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Util/String.h"

//class AosRaftAPIObj;
namespace Jimo
{
extern bool raftSendMsgSync(
						AosRundata *rdata,
						u32	endPointId,
						const AosBuffPtr &buff);

extern bool raftSendMsgAsync(
						AosRundata *rdata,
						u32	endPointId,
						const AosBuffPtr &buff);

extern bool raftSendMsgAsyncNoResp(
						AosRundata *rdata,
						u32 endPointId,
						const AosBuffPtr &buff);

};
#endif


