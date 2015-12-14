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
// 2013/04/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BigFile/LogicFile.h"


AosLogicFile::AosLogicFile(const int phyid, const u64 fileid)
:
AosAtomicFile(phyid),
mFileId(fileid)
{
}


AosLogicFile::~AosLogicFile()
{
}

