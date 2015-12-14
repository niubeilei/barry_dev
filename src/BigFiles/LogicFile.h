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
#ifndef Omn_BigFile_LogicFile_h
#define Omn_BigFile_LogicFile_h	

#include "BigFile/AtomicFile.h"

class AosLogicFile: public AosBigFile
{
	OmnDefineRCObject;

private:
	u64		mFileId;

public:
	AosLogicFile(const int phyid, const u64 fileid);
	~AosLogicFile();

	virtual u64 getFileLength() const;

};

#endif

